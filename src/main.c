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

// static struct timeval startTime;
// unsigned long long numPkts = 0, numBytes = 0;

#include <ifaddrs.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include "roaring.c"
#include "map.c"

struct sockaddr_in broadcastIP;
struct sockaddr_in allbroadcastIP;
struct sockaddr_in minMultiIP;
struct sockaddr_in maxMultiIP;
struct sockaddr_in intraIP;
struct sockaddr_in myIP;

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

void print_stats()
{
    // TODO
    //  usare uint32_t cardinality = roaring_bitmap_get_cardinality(r1);

    // da usare per comprimere la bitmap
    // uint32_t expectedsizebasic = roaring_bitmap_portable_size_in_bytes(r1);
    // roaring_bitmap_run_optimize(r1);
    // uint32_t expectedsizerun = roaring_bitmap_portable_size_in_bytes(r1);
    // printf("size before run optimize %d bytes, and after %d bytes\n",expectedsizebasic, expectedsizerun);

    // esempio per iterare sulla hashmap
    //  define our callback with the correct parameters
    // void print_entry(void* key, size_t ksize, uintptr_t value, void* usr)
    //{
    //	// prints the entry's key and value
    //	// assumes the key is a null-terminated string
    //	printf("Entry \"%s\": %i\n", key, value);
    // }
    //
    //// print the key and value of each entry
    // hashmap_iterate(m, print_entry, NULL);
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

long delta_time(struct timeval *now, struct timeval *before)
{
    time_t delta_seconds;
    time_t delta_microseconds;
    delta_seconds = now->tv_sec - before->tv_sec;
    delta_microseconds = now->tv_usec - before->tv_usec;
    if (delta_microseconds < 0)
    {
        delta_microseconds += 1000000; /* 1e6 */
        --delta_seconds;
    }
    return ((delta_seconds * 1000000) + delta_microseconds);
}

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
            // printf("SrcIP: %-15s", intoa(ntohl(ip.ip_src.s_addr)));
            // printf(" | DstIP: %-15s", intoa(ntohl(ip.ip_dst.s_addr)));
            // printf(" | Proto: %-5s\n", proto2str(ip.ip_p));

            // SRC check
            if (roaring_bitmap_contains(bitmap_BH, ip.ip_src.s_addr))
            {
                // tolgo src da tutto
                roaring_bitmap_remove(bitmap_BH, ip.ip_src.s_addr);
                uintptr_t r;
                hashmap_get(hash_BH, &ip.ip_src.s_addr, sizeof(ip.ip_src.s_addr), &r);
                roaring_bitmap_free((roaring_bitmap_t *)r);
                hashmap_remove(hash_BH, &ip.ip_src.s_addr, sizeof(ip.ip_src.s_addr));
            }

            // DST check
            if (roaring_bitmap_contains(bitmap_BH, ip.ip_dst.s_addr))
            {
                // mettere controllo e aggiunta del src
                uintptr_t r;
                hashmap_get(hash_BH, &ip.ip_dst.s_addr, sizeof(ip.ip_src.s_addr), &r);
                if (!roaring_bitmap_contains((roaring_bitmap_t *)r, ip.ip_src.s_addr))
                    roaring_bitmap_add((roaring_bitmap_t *)r, ip.ip_src.s_addr);
            }
            else
            {
                // aggiungo alla bitmap e alla hash dst associandoli la sua bitmap degli src con il nuovo src
                roaring_bitmap_add(bitmap_BH, ip.ip_dst.s_addr);
                roaring_bitmap_t *src = roaring_bitmap_create();
                roaring_bitmap_add(src, ip.ip_src.s_addr);
                hashmap_set(hash_BH, &ip.ip_dst.s_addr, sizeof(ip.ip_src.s_addr),(uintptr_t)(void*)src);
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
    char *device = NULL;
    u_char c;
    char errbuf[PCAP_ERRBUF_SIZE];
    int promisc, snaplen = DEFAULT_SNAPLEN;

    // bitmap create
    bitmap_BH = roaring_bitmap_create();
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
    roaring_bitmap_free(bitmap_BH);
    // free hash
    pcap_close(pd);
    return (0);
}