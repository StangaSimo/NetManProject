#include <pcap/pcap.h>
#include <signal.h>
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <poll.h>
#include <time.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <rrd.h>
#include "roaring.c"
#include "map.c"

#define ALARM_SLEEP 1
#define GRAPH_SLEEP 5
#define OPTIMIZE_SLEEP 6

#define DEFAULT_SNAPLEN 256
#define hash_DIM 256

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define NORMAL "\x1b[m"

struct sockaddr_in broadcastIP;
struct sockaddr_in allbroadcastIP;
struct sockaddr_in minMultiIP;
struct sockaddr_in maxMultiIP;
struct sockaddr_in intraIP;
struct sockaddr_in myIP;

typedef struct
{
    int src;
    int dst;
    long tx_packet;
    long rx_packet;
    long rx_packet_base;
    struct timeval time_src;
    struct timeval time_dst;
    roaring_bitmap_t *bitmap;
} DATA;

pcap_t *pd;
int verbose = 0;
struct pcap_stat pcapStats;

roaring_bitmap_t *bitmap_BH;
hashmap *hash_BH;

/*************************************************/

char *__intoa(unsigned int addr, char *buf, u_short bufLen)
{
    char *cp, *retStr;
    u_int byte;
    int n;
    cp = &buf[bufLen];
    *--cp = '\0';
    n = 4;
    do
    {
        byte = addr & 0xff;
        *--cp = byte % 10 + '0';
        byte /= 10;
        if (byte > 0)
        {
            *--cp = byte % 10 + '0';
            byte /= 10;
            if (byte > 0)
                *--cp = byte + '0';
        }
        *--cp = '.';
        addr >>= 8;
    } while (--n > 0);

    retStr = (char *)(cp + 1); // to lower case
    return (retStr);
}

/*************************************************/

void optimize_entry(void *key, size_t ksize, uintptr_t d, void *usr)
{
    DATA *data = (DATA *)d;
    roaring_bitmap_run_optimize(data->bitmap);
}

/*************************************************/

static char buf[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"];
char *intoa(unsigned int addr) { return (__intoa(addr, buf, sizeof(buf))); }

/*************************************************/

void sigproc(int sig)
{
    static int called = 0;
    fprintf(stderr, "Leaving...\n");
    if (called)
        return;
    else
        called = 1;
    pcap_breakloop(pd);
}

/*************************************************/

int min(time_t a, time_t b)
{
    if (a < b)
        return a;
    return b;
}

/*************************************************/
/*free the hashmap entry and the bitmap*/

void free_entry(struct timeval time_dst, struct timeval time_src, void *key, DATA *data)
{  
    struct timeval time;
    gettimeofday(&time, NULL);
    if (min(time.tv_sec - time_dst.tv_sec, time.tv_sec - time_src.tv_sec) > 100)
    {   
        uintptr_t r;
        roaring_bitmap_free(data->bitmap);
        free(data);
        hashmap_remove(hash_BH, (in_addr_t *)key, sizeof(in_addr_t));
        free(key);
    }
}

/*************************************************/

void print_line_table(int c) {
    printf("|");
    switch (c)
    {
        case 0:
            printf("%s", RED);
            break;
        case 1:
            printf("%s", YELLOW);
        case 2:
            printf("%s", GREEN);
            break;
    }
    printf("⬤ ");
    printf("%s", NORMAL);
}

/*************************************************/

bool iter(uint32_t value, void* p)
{
    char rrdfile[100];
    sprintf(rrdfile, "rrd_bin/db/%s", intoa(ntohl(value)));
    char command[1000];
    sprintf(command, "rrdtool graph rrd_bin/graph/%s.png -w 1920 -h 1080 -D --start end-120s DEF:da1=%s.rrd:speed:AVERAGE LINE:da1#ff0000:'1' ", intoa(ntohl(value)), rrdfile);
    system(command);
    return true; 
}

void rd_graph() { roaring_iterate(bitmap_BH, iter, NULL); }

/*************************************************/

void rd_create(in_addr_t ip)
{
    char rrdfile[100];
    sprintf(rrdfile,"rrd_bin/db/%s.rrd",intoa(ntohl(ip)));
    int rra_step = 1;  // ogni quanto dare il valore
    unsigned long start_time = 1621314000;
    unsigned long rrd_argc = 2;
    const char** rrd_argv = calloc(sizeof(char*),2);
    rrd_argv[0] = "DS:speed:GAUGE:10:0:1000000";
    rrd_argv[1] = "RRA:AVERAGE:0.5:1:60";
    int ret = rrd_create_r(rrdfile, rra_step, start_time, rrd_argc, rrd_argv);
    if (ret != 0) {
        printf("Errore CREATE\n");
        rrd_clear_error();
        return;
    }
    free(rrd_argv);
}

/*************************************************/

long rd_update(in_addr_t ip, long p,long base)
{
    char rrdfile[100];
    long res = p-base;
    sprintf(rrdfile,"rrd_bin/db/%s.rrd",intoa(ntohl(ip)));
    unsigned long rrd_argc = 1;
    char arg[100];
    const char** rrd_argv = calloc(sizeof(char*),1);
    sprintf(arg,"N:%ld",res);
    rrd_argv[0] = arg;
    int ret = rrd_update_r(rrdfile,NULL,rrd_argc, rrd_argv); //Creazione del file RRD
    if (ret != 0) {
        printf("Errore UPDATE\n");
        rrd_clear_error();
        return 0;
    }
    free(rrd_argv);
    return p;
}

/*************************************************/

void print_hash_entry(void *key, size_t ksize, uintptr_t d, void *usr)
{
    DATA *data = (DATA *)d;
    long delta = data->time_dst.tv_sec - data->time_src.tv_sec;

    if (!(data->src && !data->dst))
    {
        if ((delta==0 && data->tx_packet==0 && data->rx_packet>10)||delta > 5) //5 second?  //Black hole
        {
            print_line_table(0);
            data->rx_packet_base = rd_update(*(in_addr_t *)key, data->rx_packet, data->rx_packet_base);
        }
        else
        {
            if (1) //(delta > 2) //probably blackhole
            {
                print_line_table(1);
                if (roaring_bitmap_contains(bitmap_BH, *(in_addr_t *)key))
                    data->rx_packet_base = rd_update(*(in_addr_t *)key, data->rx_packet, data->rx_packet_base);
                else
                {
                    roaring_bitmap_add(bitmap_BH, *(in_addr_t *)key);
                    data->rx_packet_base = data->rx_packet;
                    rd_create(*(in_addr_t *)key);
                }
            }
            else if ((roaring_bitmap_contains(bitmap_BH, *(in_addr_t *)key) && delta < 5)) // Back to send Packets
            {

                print_line_table(2);
                roaring_bitmap_remove(bitmap_BH, *(in_addr_t *)key);
                char rrdfile[100];
                sprintf(rrdfile, "rrd_bin/db/%s", intoa(ntohl(*(in_addr_t *)key)));
                remove(rrdfile);
            }
            else //normal Host 
                print_line_table(2); 
        }
        time_t d_time = data->time_dst.tv_sec + 7200;
        time_t s_time = data->time_src.tv_sec + 7200;
        struct tm *dst_time = gmtime(&d_time);
        struct tm *src_time = gmtime(&s_time);
        printf("| %-16s |", intoa(ntohl(*(__uint32_t *)key)));
        printf(" %lld.%lld.%-6lld |",(long long)dst_time->tm_hour, (long long)dst_time->tm_min, (long long)dst_time->tm_sec);
        printf(" %lld.%lld.%-6lld |",(long long)src_time->tm_hour, (long long)src_time->tm_min, (long long)src_time->tm_sec);
        printf(" %ld:%-10ld |\n", data->rx_packet, data->tx_packet);
    }

    //TODO: right? 
    //
    // free host after 300 seconds of inactivity
    //if (!(roaring_bitmap_contains(bitmap_BH, *(in_addr_t *)key)))
    //    free_entry(data->time_dst, data->time_src, key, data);
}

/*************************************************/

int c = 0;
int cont = 0;
void print_stats()
{
    printf("\n\n\n\nITER: %d\n", c++);
    printf("---------------------------------------------------------------------\n");
    printf("|  |        IP        | Last RX Time | Last TX Time | Packets RX:TX |  \n");
    hashmap_iterate(hash_BH, print_hash_entry, NULL);
    printf("---------------------------------------------------------------------\n");

    cont++;
    if ((cont % GRAPH_SLEEP) == 0) { rd_graph(); }
    
    if (cont >= OPTIMIZE_SLEEP)
    {
        hashmap_iterate(hash_BH, optimize_entry, NULL);
        cont = 0;
    }

    roaring_bitmap_run_optimize(bitmap_BH);
}

/*************************************************/

void free_hashmap(void *key, size_t ksize, uintptr_t d, void *usr)
{
    DATA *data = (DATA *)d;
    free(key);
    roaring_bitmap_free(data->bitmap);
    free(data);
}

/*************************************************/

void my_sigalarm(int sig)
{
    print_stats();
    alarm(ALARM_SLEEP);
    signal(SIGALRM, my_sigalarm);
}

/*************************************************/

char *proto2str(u_short proto)
{
    static char protoName[8];
    switch (proto)
    {
        case IPPROTO_TCP:
            return ("TCP");
        case IPPROTO_UDP:
            return ("UDP");
        case IPPROTO_ICMP:
            return ("ICMP");
        default:
            snprintf(protoName, sizeof(protoName), "%d", proto);
            return (protoName);
    }
}

/*************************************************/

void getBroadCast(char *device)
{
    struct ifaddrs *ifaddr, *ifa;
    struct sockaddr_in *sa = NULL;
    struct sockaddr_in *su = NULL;
    char subnet_mask[INET_ADDRSTRLEN];
    char ip[INET_ADDRSTRLEN];
    char broad[INET_ADDRSTRLEN];

    if (getifaddrs(&ifaddr) == -1)
    {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (strcmp(ifa->ifa_name, device) == 0 && ifa->ifa_addr->sa_family == AF_INET)
        {
            sa = (struct sockaddr_in *)ifa->ifa_netmask;
            su = (struct sockaddr_in *)ifa->ifa_addr;
            inet_ntop(AF_INET, &(sa->sin_addr), subnet_mask, INET_ADDRSTRLEN);
            inet_ntop(AF_INET, &(su->sin_addr), ip, INET_ADDRSTRLEN);
            break;
        }
    }

    inet_pton(AF_INET, ip, &myIP.sin_addr);
    broadcastIP.sin_addr.s_addr = su->sin_addr.s_addr | ~(sa->sin_addr.s_addr);
    inet_ntop(AF_INET, &(broadcastIP.sin_addr), broad, INET_ADDRSTRLEN);
    printf("\nSubnetMask:%s \nLocalIP:%s \nBroadcastIP: %s\n", subnet_mask, ip, broad);
    freeifaddrs(ifaddr);
}

/*************************************************/

void dummyProcesssPacket(u_char *_deviceId, const struct pcap_pkthdr *h, const u_char *p)
{
    struct ether_header ehdr;
    struct ip ip;
    struct tcphdr tcp;

    memcpy(&ehdr, p, sizeof(struct ether_header));
    memcpy(&ip, p + sizeof(ehdr), sizeof(struct ip));
    u_short eth_type = ntohs(ehdr.ether_type);

    if (eth_type == 0x0800)
    {
        if (ip.ip_p == IPPROTO_TCP &&
                (ntohl(ip.ip_dst.s_addr) != ntohl(broadcastIP.sin_addr.s_addr)) &&
                (ntohl(ip.ip_dst.s_addr) != ntohl(intraIP.sin_addr.s_addr)) &&
                //(ntohl(ip.ip_dst.s_addr) != ntohl(myIP.sin_addr.s_addr)) &&
                //(ntohl(ip.ip_src.s_addr) != ntohl(myIP.sin_addr.s_addr)) &&
                (ntohl(ip.ip_dst.s_addr) != ntohl(allbroadcastIP.sin_addr.s_addr)) &&
                (ntohl(ip.ip_dst.s_addr) < ntohl(minMultiIP.sin_addr.s_addr) || ntohl(ip.ip_dst.s_addr) > ntohl(maxMultiIP.sin_addr.s_addr)))
        {   
            //time_dst = last rx packet time, time_src = last tx packet time, if host have only rx traffics, time_src = last rx packet time 
            uintptr_t r;
            if (hashmap_get(hash_BH, &ip.ip_src.s_addr, sizeof(ip.ip_src.s_addr), &r)) // src ip present in the hashmap 
            {
                DATA *data = (DATA *)r;
                data->tx_packet++;
                if (!(data->src)) //host was only dst
                    data->src = 1;
                data->time_src = h->ts; //update last rx time packet
            }
            else // src ip not present in the hashmap
            {
                DATA *src_data;
                src_data = malloc(sizeof(DATA));
                src_data->src = 1;
                src_data->dst = 0;
                src_data->rx_packet = 0;
                src_data->tx_packet = 1;
                src_data->time_src = h->ts;
                src_data->bitmap = roaring_bitmap_create();
                in_addr_t *s_addr = malloc(sizeof(in_addr_t));
                memcpy(s_addr, &ip.ip_src.s_addr, sizeof(in_addr_t));
                hashmap_set(hash_BH, s_addr, sizeof(in_addr_t), (uintptr_t)(void *)src_data);
            }

            if (hashmap_get(hash_BH, &ip.ip_dst.s_addr, sizeof(ip.ip_dst.s_addr), &r)) // DST not present in the hashmap
            {
                DATA *data = (DATA *)r;
                data->rx_packet++;
                if (!(data->dst)) // was only src
                    data->dst = 1; 
                data->time_dst = h->ts;
                if (!(roaring_bitmap_contains(data->bitmap, ip.ip_src.s_addr)))
                    roaring_bitmap_add(data->bitmap, ip.ip_src.s_addr);
            }
            else // DST not present
            {
                DATA *dst_data = malloc(sizeof(DATA));
                dst_data->dst = 1;
                dst_data->src = 0;
                dst_data->rx_packet = 1;
                dst_data->tx_packet = 0;
                dst_data->time_src = h->ts;
                dst_data->time_dst = h->ts;
                dst_data->bitmap = roaring_bitmap_create();
                roaring_bitmap_add(dst_data->bitmap, ip.ip_src.s_addr);
                in_addr_t *s_addr = malloc(sizeof(in_addr_t));
                memcpy(s_addr, &ip.ip_dst.s_addr, sizeof(in_addr_t));
                hashmap_set(hash_BH, s_addr, sizeof(in_addr_t), (uintptr_t)(void *)dst_data);
            }
        }
    }
}

/*************************************************/

int main(int argc, char *argv[])
{
    char *device = NULL;
    u_char c;
    char errbuf[PCAP_ERRBUF_SIZE];
    int promisc, snaplen = DEFAULT_SNAPLEN;
    bitmap_BH = roaring_bitmap_create();
    hash_BH = hashmap_create();

    while ((c = getopt(argc, argv, "hi:l:v:f:")) != '?')
    {
        if ((c == 255) || (c == (u_char)-1))
            break;
        switch (c)
        {
            case 'i':
                device = strdup(optarg);
                break;
        }
    }

    if (geteuid() != 0)
    {
        printf("Please run this tool as superuser\n");
        return (-1);
    }

    if (device == NULL)
    {
        printf("ERROR: Missing -i\n");
        return (-1);
    }

    printf("Capturing from %s\n", device);
    getBroadCast(device);
    inet_pton(AF_INET, "224.0.0.0", &minMultiIP.sin_addr);
    inet_pton(AF_INET, "239.255.255.255", &maxMultiIP.sin_addr);
    inet_pton(AF_INET, "255.255.255.255", &allbroadcastIP.sin_addr);
    inet_pton(AF_INET, "192.168.222.11", &intraIP.sin_addr);

    promisc = 1;
    if ((pd = pcap_open_live(device, snaplen, promisc, 500, errbuf)) == NULL)
    {
        printf("pcap_open_live: %s\n", errbuf);
        return (-1);
    }

    signal(SIGINT, sigproc);
    signal(SIGTERM, sigproc);
    signal(SIGALRM, my_sigalarm);
    alarm(ALARM_SLEEP);
    pcap_loop(pd, -1, dummyProcesssPacket, NULL);
    pcap_close(pd);
    alarm(0);
    roaring_bitmap_free(bitmap_BH);
    hashmap_iterate(hash_BH, free_hashmap, NULL);
    hashmap_free(hash_BH);
    return (0);
}
