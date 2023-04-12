#include <pcap/pcap.h>
#include <signal.h>
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#define ALARM_SLEEP 3
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

//https://github.com/RoaringBitmap/CRoaring
//https://github.com/Mashpoe/c-hashmap#proper-usage-of-keys

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

//roaring_bitmap_t *bitmap_BH;
roaring_bitmap_t *bitmap_src;
hashmap *hash_BH;

typedef struct{
   int src;
   int dst;   
   struct timeval time_src;
   struct timeval time_dst;
   roaring_bitmap_t *bitmap;
}DATA;
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

//bool print_BH(uint32_t value, void *param) {
//    printf("blackhole: %s\n",intoa(ntohl(value)));
//    return true;  // iterate till the end
//}

void blackhole(void* key,roaring_bitmap_t *bitmap){
     printf("Blackhole %s\n", intoa(ntohl(key)));
     printf("sorgenti: ");
      //uint32_t counter = 0;
     uint32_t value;
     roaring_uint32_iterator_t *i = roaring_create_iterator(bitmap);
     while (i->has_value) {
         roaring_move_uint32_iterator_equalorlarger(i,value);
         printf(" %s,", intoa(ntohl(value)));
         }
        printf("\n");
        roaring_free_uint32_iterator(i);
}


void print_entry(void* key, size_t ksize, DATA *data, void* usr){
    // prints the entry's key and value

    if(data->src && data->dst){
       if((data->time_dst.tv_sec-data->time_src.tv_sec)>60){
           blackhole((void*)key,data->bitmap);
       }
    }
    if(data->dst && !(data->src)){
        if((data->time_src.tv_sec) > 30){
            blackhole((void*)key,data->bitmap);
        }
    }

    }

/* ******************************** */

void optimize(roaring_bitmap_t *bitmpap) 
{
    uint32_t expectedsizebasic = roaring_bitmap_portable_size_in_bytes(bitmpap);
    roaring_bitmap_run_optimize(bitmpap);
    uint32_t expectedsizerun = roaring_bitmap_portable_size_in_bytes(bitmpap);
    printf("size before run optimize %d bytes, and after %d bytes\n",expectedsizebasic, expectedsizerun); 
}

/* ******************************** */

void print_stats()
{
   // uint32_t c = roaring_bitmap_get_cardinality(bitmap_BH);
  //  printf("black Hole totali: %u\n",c);
    hashmap_iterate(hash_BH, print_entry, NULL);
    //itero i blackhole
    //uint32_t counter = 0;
    //roaring_iterate(bitmap_BH, print_BH, &counter);

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
    // hashmap_iterate(m, print_entry, NULL);

    //optimize(bitmap_BH);
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
            // printf("SrcIP: %-15s", intoa(ntohl(ip.ip_src.s_addr)));
            // printf(" | DstIP: %-15s", intoa(ntohl(ip.ip_dst.s_addr)));
            // printf(" | Proto: %-5s\n", proto2str(ip.ip_p));

            //bitmap degli src 
            // SRC check
            uintptr_t r;
            if (hashmap_get(hash_BH, &ip.ip_src.s_addr, sizeof(ip.ip_src.s_addr), &r))
            {
                // tolgo src da tutto  
               DATA *r = (DATA *)r;
               if(!(r->src))
                 r->src=1; 

               r->time_src = h->ts;
            }

            else
            {
                DATA *src_data;
                src_data = malloc(sizeof(DATA));
                src_data->src=1;
                src_data->dst=0;
                src_data->time_src = h->ts;
                src_data->bitmap = roaring_bitmap_create();
                
                hashmap_set(hash_BH, &ip.ip_src.s_addr, sizeof(ip.ip_src.s_addr),(uintptr_t)(void*)src_data);               
            }

             if (hashmap_get(hash_BH, &ip.ip_dst.s_addr, sizeof(ip.ip_dst.s_addr), &r))
             {
               // t1 *a = (t1 *)arg;
                DATA *r = (DATA *)r;
               if(!(r->dst))
               {
                  r->dst=1;
               }

                r->time_dst = h->ts;               

               if (!(roaring_bitmap_contains(r->bitmap, ip.ip_src.s_addr)))
               {               
                    roaring_bitmap_add(r->bitmap, ip.ip_src.s_addr);
               }

             }

             else
             {
                DATA *dst_data = malloc(sizeof(DATA));
                dst_data->dst=1;
                dst_data->src=0;
                dst_data->time_src = h->ts;
                dst_data->bitmap = roaring_bitmap_create();
                
                hashmap_set(hash_BH, &ip.ip_dst.s_addr, sizeof(ip.ip_src.s_addr),(uintptr_t)(void*)dst_data);   
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
    hashmap_free(hash_BH);
    pcap_close(pd);
    return (0);
}