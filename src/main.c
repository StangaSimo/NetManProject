#include <pcap/pcap.h>
#include <signal.h>
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#define ALARM_SLEEP 1
#define DEFAULT_SNAPLEN 256
#define hash_DIM 256

#include <stdlib.h>
#include <stddef.h>

pcap_t *pd;
int verbose = 0;
struct pcap_stat pcapStats;

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

// https://github.com/RoaringBitmap/CRoaring
// https://github.com/Mashpoe/c-hashmap#proper-usage-of-keys

// static struct timeval startTime;
// unsigned long long numPkts = 0, numBytes = 0;

#include <ifaddrs.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include "roaring.c"
#include "map.c"
#include <ncurses.h>

struct sockaddr_in broadcastIP;
struct sockaddr_in allbroadcastIP;
struct sockaddr_in minMultiIP;
struct sockaddr_in maxMultiIP;
struct sockaddr_in intraIP;
struct sockaddr_in myIP;

// roaring_bitmap_t *bitmap_BH;
roaring_bitmap_t *bitmap_src;
hashmap *hash_BH;

typedef struct
{
    int src;
    int dst;
    struct timeval time_src;
    struct timeval time_dst;
    roaring_bitmap_t *bitmap;
} DATA;

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

    /* Convert the string to lowercase */
    retStr = (char *)(cp + 1);

    return (retStr);
}

/* ************************************ */

static char buf[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"];
char *intoa(unsigned int addr)
{
    return (__intoa(addr, buf, sizeof(buf)));
}

/* ******************************** */

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

/* ******************************** */

bool print_bh_src(uint32_t value, void *parap)
{
    printf("ip: %s\n",intoa(ntohl(value)));
    return true; 
}


void print_hash_entry(void *key, size_t ksize, uintptr_t d, void *usr)
{
    printw("dentro: %s\n", intoa(ntohl(*(__uint32_t*)key)));
    DATA *data = (DATA *)d;
    // possibile bh o certo bh

    //l'unico caso da evitare è solo src = 1 e dst = 0
    if (!(data->src && !data->dst))
    {
        long delta = data->time_dst.tv_sec - data->time_src.tv_sec;
        printw("delta dst %ld, delta src %ld, delta %ld\n", data->time_dst.tv_sec, data->time_src.tv_sec, delta);
        if (delta > 60)
        {
            // certo bh se sono passati piu di 60 secondi
            printw("è un bh: %s, non ha tx da %ld\n", intoa(ntohl(*(in_addr_t*)key)), delta);
            uint32_t counter = 0;
            roaring_iterate(data->bitmap,print_bh_src, &counter);
        }
        else
        {
            // secondi e diciamo che è un possibile blackhole??
            if (delta > 5)
            {
                printw("possibile bh: %s, non ha tx da %ld\n", intoa(ntohl(*(in_addr_t*)key)),delta);
            }
            // per dire che è tornato a funzionare??
        }
        //TODO: manca è uscito dal blackhole
    }
}

/* ******************************** */

void optimize(roaring_bitmap_t *bitmpap)
{
    uint32_t expectedsizebasic = roaring_bitmap_portable_size_in_bytes(bitmpap);
    roaring_bitmap_run_optimize(bitmpap);
    uint32_t expectedsizerun = roaring_bitmap_portable_size_in_bytes(bitmpap);
    printf("size before run optimize %d bytes, and after %d bytes\n", expectedsizebasic, expectedsizerun);
}

/* ******************************** */

int c=0; 

void print_stats()
{
    
    //hashmap_iterate(hash_BH, (hashmap_callback)print_hash_entry, NULL);
    clear();
    hashmap_iterate(hash_BH, print_hash_entry, NULL);
    printw("itero %d volte\n",c++);
    refresh();
    // uint32_t c = roaring_bitmap_get_cardinality(bitmap_BH);
    // printf("black Hole totali: %u\n",c);
    // itero i blackhole
    // uint32_t counter = 0;
    // roaring_iterate(bitmap_BH, print_BH, &counter);

    // TODO: esempio per iterare sulla hashmap
    //  define our callback with the correct parameters
    // void print_entry(void* key, size_t ksize, uintptr_t value, void* usr)
    //{
    //	// prints the entry's key and value
    //	// assumes the key is a null-terminated string
    //	printf("Entry \"%s\": %i\n", key, value);
    // }
    //
    //// print the key and value of each entry
    
    // optimize(bitmap_BH);
}

/* ******************************** */

void my_sigalarm(int sig)
{
    print_stats();
    alarm(ALARM_SLEEP);
    signal(SIGALRM, my_sigalarm);
}

/* *************************************** */

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

/* *************************************** */

/*long delta_time(struct timeval *now, struct timeval *before)
{
    time_t delta_seconds;
    time_t delta_microseconds;
    delta_seconds = now->tv_sec - before->tv_sec;

    delta_microseconds = now->tv_usec - before->tv_usec;
    if (delta_microseconds < 0)
    {
        delta_microseconds += 1000000;
        --delta_seconds;
    }
    return ((delta_seconds * 1000000) + delta_microseconds);
}
*/

/* *************************************** */

// struct sockaddr_in* getSubnetMask(char* device) {
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
    printf("SubnetMask:%s localIP:%s broadcastIP: %s\n", subnet_mask, ip, broad);
    freeifaddrs(ifaddr);
}

/* *************************************** */


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
        if (ip.ip_p == IPPROTO_TCP)
        {
            // time_dst = tempo ultimo pacchetto rx
            // time_src = tempo ultimo pacchetto tx,

            // in caso l'host abbia solo traffico rx,
            // time_src = tempo primo pacchetto rx

            uintptr_t r;
            if (hashmap_get(hash_BH, &ip.ip_src.s_addr, sizeof(ip.ip_src.s_addr), &r))
            {
                // SRC presente
                DATA *data = (DATA *)r;

                if (!(data->src))
                {
                    // l'host era solo DST
                    data->src = 1;
                }

                // update il tempo dell'ultimo pacchetto
                data->time_src = h->ts;
            }
            else
            {
                // SRC non presente
                DATA *src_data;
                src_data = malloc(sizeof(DATA));
                src_data->src = 1;
                src_data->dst = 0;
                src_data->time_src = h->ts;
                src_data->bitmap = roaring_bitmap_create();
                printf("inserisco src %s\n",intoa(ntohl(ip.ip_src.s_addr)));
                in_addr_t* s_addr = malloc(sizeof(in_addr_t));
                memcpy(s_addr, &ip.ip_src.s_addr, sizeof(in_addr_t));
                hashmap_set(hash_BH, s_addr, sizeof(in_addr_t), (uintptr_t)(void *)src_data);
            }

            if (hashmap_get(hash_BH, &ip.ip_dst.s_addr, sizeof(ip.ip_dst.s_addr), &r))
            {
                // DST Presente
                DATA *data = (DATA *)r;
                if (!(data->dst))
                {
                    // l'host era solo SRC
                    data->dst = 1;
                }

                // update tempo ultimo pacchetto dst
                data->time_dst = h->ts;

                // si aggiunge src alla bitmap
                if (!(roaring_bitmap_contains(data->bitmap, ip.ip_src.s_addr)))
                {
                    roaring_bitmap_add(data->bitmap, ip.ip_src.s_addr);
                }
            }
            else
            {
                // DST non presente
                DATA *dst_data = malloc(sizeof(DATA));
                dst_data->dst = 1;
                dst_data->src = 0;
                // usiamo time_src per salvarci il primo pacchetto che arriva se siamo solo dst
                dst_data->time_src = h->ts;
                dst_data->time_dst = h->ts;
                dst_data->bitmap = roaring_bitmap_create();
                roaring_bitmap_add(dst_data->bitmap, ip.ip_src.s_addr);
                printf("inserisco dest %s\n",intoa(ntohl(ip.ip_dst.s_addr)));
                in_addr_t* s_addr = malloc(sizeof(in_addr_t));
                memcpy(s_addr, &ip.ip_dst.s_addr, sizeof(in_addr_t));
                hashmap_set(hash_BH, s_addr, sizeof(in_addr_t), (uintptr_t)(void *)dst_data);
            }
        }
    }
    else
    {
        // printf("don't know");
    }
}

/* *************************************** */

int main(int argc, char *argv[])
{
    initscr();
    char *device = NULL;
    u_char c;
    char errbuf[PCAP_ERRBUF_SIZE];
    int promisc, snaplen = DEFAULT_SNAPLEN;

    // bitmap create
    // bitmap_BH = roaring_bitmap_create();
    bitmap_src = roaring_bitmap_create();
    // hash create
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
    /* hardcode: promisc=1, to_ms=500 */
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

    // free bitmap
    // roaring_bitmap_free(bitmap_BH);
    roaring_bitmap_free(bitmap_src);
    // free hash
    // TODO: free the key
    hashmap_free(hash_BH);
    pcap_close(pd);
    endwin();
    return (0);
}