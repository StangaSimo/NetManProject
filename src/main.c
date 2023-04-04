#include <pcap/pcap.h>
#include <signal.h>
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

#define DEFAULT_SNAPLEN 256

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
// #include <netinet/ip6.h>
#include <net/ethernet.h> /* the L2 protocols */

// static struct timeval startTime;
unsigned long long numPkts = 0, numBytes = 0;

#include <ifaddrs.h>
#include <search.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

#define hashipDIM 100

struct sockaddr_in broadcastIP;
struct sockaddr_in allbroadcastIP;
struct sockaddr_in minMultiIP;
struct sockaddr_in maxMultiIP;
struct sockaddr_in intraIP;

ENTRY s, d, *sp, *dp;

typedef struct e
{
    in_addr_t ip;
} entry_ip;

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

typedef struct data
{
    int src;
    int dsc;
    struct timeval t;
    entry_ip haship[hashipDIM];
} DATA;

void insert_ip(in_addr_t ip, entry_ip table[])
{
    int i = ip % hashipDIM;
    table[i].ip = ip;
}

int is_present(in_addr_t ip, entry_ip table[])
{
    int i = ip % hashipDIM;
    return (table[i].ip == ip);
}

char *get_ip(entry_ip table[])
{
    char *a = malloc(1000 * sizeof(char));
    for (int i = 0; i < hashipDIM; i++)
    {
        if (table[i].ip != 0)
        {
            strcat(a, intoa(ntohl(table[i].ip)));
            strcat(a, " ");
        }
    }
    return a;
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

void dummyProcesssPacket(u_char *_deviceId, const struct pcap_pkthdr *h, const u_char *p)
{
    struct ether_header ehdr;
    struct ip ip;
    // struct tcphdr tcp;
    // struct udphdr udp;

    DATA *ipaddr;

    memcpy(&ehdr, p, sizeof(struct ether_header));
    memcpy(&ip, p + sizeof(ehdr), sizeof(struct ip));
    u_short eth_type = ntohs(ehdr.ether_type);

    if (eth_type == 0x0800)
    {
        // Check Multicast/Broadcast/allBroadcastIP
        if ((ntohl(ip.ip_dst.s_addr) != ntohl(broadcastIP.sin_addr.s_addr)) &&
            (ntohl(ip.ip_dst.s_addr) != ntohl(intraIP.sin_addr.s_addr)) &&
            (ntohl(ip.ip_dst.s_addr) != ntohl(allbroadcastIP.sin_addr.s_addr)) &&
            (ntohl(ip.ip_dst.s_addr) < ntohl(minMultiIP.sin_addr.s_addr) || ntohl(ip.ip_dst.s_addr) > ntohl(maxMultiIP.sin_addr.s_addr)))
        {
            printf("SrcIP: %-15s", intoa(ntohl(ip.ip_src.s_addr)));
            printf(" | DstIP: %-15s", intoa(ntohl(ip.ip_dst.s_addr)));
            printf(" | Proto: %-5s\n", proto2str(ip.ip_p));

            // if (ip.ip_p == IPPROTO_TCP)
            //{
            //     memcpy(&tcp, p + sizeof(ehdr) + sizeof(ip), sizeof(struct tcphdr));
            //     printf(" | SrcP: %-10u", tcp.th_sport);
            //     printf(" | DstP: %-10u\n", tcp.th_dport);
            // }
            // else if (ip.ip_p == IPPROTO_UDP)
            //{
            //     memcpy(&udp, p + sizeof(ehdr) + sizeof(ip), sizeof(struct udphdr));
            //     printf(" | SrcP: %-10u", udp.uh_sport);
            //     printf(" | DstP: %-10u\n", udp.uh_dport);
            // }

            // IP SRC
            s.key = intoa(ntohl(ip.ip_src.s_addr));
            sp = hsearch(s, FIND);
            if (sp == NULL)
            {
                ipaddr = malloc(sizeof(DATA));
                ipaddr->src = 1;
                ipaddr->dsc = 0;
                s.data = (void *)ipaddr;
                hsearch(s, ENTER);
                // printf("inserito src\n");
            }
            else
            {
                DATA *a = sp->data;
                a->src = 1;
                // printf("già presente src\n");
            }

            // IP DST
            d.key = intoa(ntohl(ip.ip_dst.s_addr));
            dp = hsearch(d, FIND);
            if (dp == NULL)
            {
                //Prima volta
                ipaddr = malloc(sizeof(DATA));
                ipaddr->src = 0;
                ipaddr->dsc = 1;
                ipaddr->t = h->ts;
                for (int i = 0; i < hashipDIM; i++)
                {
                    ipaddr->haship[i].ip = 0;
                }
                insert_ip(ip.ip_src.s_addr, ipaddr->haship);
                d.data = (void *)ipaddr;
                // printf("TS PRIMA %d\n", ipaddr->t.tv_usec);
                dp = hsearch(d, ENTER);
                // printf("inserito dsc\n");
            }
            else
            {
                //caso c'è già
                DATA *a = dp->data;
                //si mette src nella hash se non c'è già
                if (!is_present(ip.ip_src.s_addr, a->haship))
                    insert_ip(ip.ip_src.s_addr, a->haship);
                a->dsc = 1;

                // Possibile BlackHole
                if (!(a->src))
                {
                    struct timeval j = h->ts;
                    printf("dal primo pacchetto: %ld microseconds\n", delta_time(&j, &a->t));
                    if (delta_time(&j, &a->t) > 2000000)
                    {
                        // BLACKHOLE
                        char *ips = get_ip(a->haship);
                        printf("%s è un possibile black hole e i possibili host malevoli sono: %s\n", intoa(ntohl(ip.ip_dst.s_addr)), ips);
                        free(ips);
                    }
                }
                // printf("già presente dsc\n");
            }
        }
        else
        {
            // printf("Mutlicast\n");
        }
    }
    else
    {
        // printf("don't know");
    }
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

    broadcastIP.sin_addr.s_addr = su->sin_addr.s_addr | ~(sa->sin_addr.s_addr);
    // printf("broadcast PRIMA: %u\n", broadcastIP.sin_addr.s_addr);
    // printf("broadcast con ntohl: %u\n", ntohl(broadcastIP.sin_addr.s_addr));
    // printf("MIO: %s\n", intoa(ntohl(broadcastIP.sin_addr.s_addr)));
    inet_ntop(AF_INET, &(broadcastIP.sin_addr), broad, INET_ADDRSTRLEN);
    printf("SubnetMask:%s localIP:%s broadcastIP: %s\n", subnet_mask, ip, broad);
    freeifaddrs(ifaddr);
}

/* *************************************** */

int main(int argc, char *argv[])
{
    char *device = NULL;
    u_char c;
    size_t n = 10000;
    char errbuf[PCAP_ERRBUF_SIZE];
    int promisc, snaplen = DEFAULT_SNAPLEN;

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

    // entry_ip prova[hashipDIM] = {0};
    // insert_ip(maxMultiIP.sin_addr.s_addr,prova);
    // insert_ip(minMultiIP.sin_addr.s_addr,prova);
    // char* ips = get_ip(prova);
    // printf("prova lista: %s\n",ips);
    // free(ips);
    // return 0;
    //   min multicasdt 3758096384
    //   broad cast 11000000101010000111111111111111 4286556352
    //   192.168.127.255 11000000101010000111111111111111
    //   255      127      168      192
    //   11111111 01111111 10101000 11000000  = E' al CONTRARIOOOOOOOOO
    //   00000000 00000000 00000000 11111111
    //  struct sockaddr_in prova;
    //  inet_pton(AF_INET, "224.0.0.251", &prova.sin_addr);
    //  printf("myIP %u\nminmulti %u\nmaxmulti %u\nbroadcast %u\n", prova.sin_addr.s_addr, ntohl(minMultiIP.sin_addr.s_addr), ntohl(maxMultiIP.sin_addr.s_addr), broadcastIP.sin_addr.s_addr);
    //  if (ntohl(prova.sin_addr.s_addr) < ntohl(minMultiIP.sin_addr.s_addr) || ntohl(prova.sin_addr.s_addr) > ntohl(maxMultiIP.sin_addr.s_addr))
    //{
    //     printf("NON MUTLI\n");
    // }
    //  else
    //{
    //     printf("MUTLI\n");
    // }
    //  return 0;

    /* hardcode: promisc=1, to_ms=500 */
    promisc = 1;

    if ((pd = pcap_open_live(device, snaplen, promisc, 500, errbuf)) == NULL)
    {
        printf("pcap_open_live: %s\n", errbuf);
        return (-1);
    }
    signal(SIGINT, sigproc);
    signal(SIGTERM, sigproc);

    hcreate(n);
    pcap_loop(pd, -1, dummyProcesssPacket, NULL);
    hdestroy();

    pcap_close(pd);
    return (0);
}