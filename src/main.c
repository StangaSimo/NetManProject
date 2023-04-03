#include <pcap/pcap.h> 
#include <signal.h>
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>

#define DEFAULT_SNAPLEN 256

pcap_t  *pd;
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
//#include <netinet/ip6.h>
#include <net/ethernet.h>     /* the L2 protocols */

//static struct timeval startTime;
unsigned long long numPkts = 0, numBytes = 0;

#include <ifaddrs.h>
#include <search.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

struct sockaddr_in broadcastIP;
struct sockaddr_in minMultiIP;
struct sockaddr_in maxMultiIP;

/*************************************************/

char* __intoa(unsigned int addr, char* buf, u_short bufLen) {
    char *cp, *retStr;
    u_int byte;
    int n;

    cp = &buf[bufLen];
    *--cp = '\0';

    n = 4;
    do {
        byte = addr & 0xff;
        *--cp = byte % 10 + '0';
        byte /= 10;
        if (byte > 0) {
            *--cp = byte % 10 + '0';
            byte /= 10;
            if (byte > 0)
                *--cp = byte + '0';
        }
        *--cp = '.';
        addr >>= 8;
    } while (--n > 0);

    /* Convert the string to lowercase */
    retStr = (char*)(cp+1);

    return(retStr);
}

/* ************************************ */

static char buf[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"];
char* intoa(unsigned int addr) {
    return(__intoa(addr, buf, sizeof(buf)));
}
/* ******************************** */

void sigproc(int sig) {
    static int called = 0;
    fprintf(stderr, "Leaving...\n");
    if (called) return; else called = 1;
    pcap_breakloop(pd);
}

/* *************************************** */

char* proto2str(u_short proto) {
    static char protoName[8];

    switch(proto) {
        case IPPROTO_TCP:  return("TCP");
        case IPPROTO_UDP:  return("UDP");
        case IPPROTO_ICMP: return("ICMP");
        default:
               snprintf(protoName, sizeof(protoName), "%d", proto);
               return(protoName);
    }
}

/* *************************************** */

ENTRY s, d, *sp, *dp;

typedef struct data{
    int src;
    int dsc;
    struct timeval t;
}DATA;

long delta_time (struct timeval * now, struct timeval * before) {
  time_t delta_seconds;
  time_t delta_microseconds;
  delta_seconds      = now -> tv_sec  - before -> tv_sec;
  delta_microseconds = now -> tv_usec - before -> tv_usec;
  if(delta_microseconds < 0) {
    delta_microseconds += 1000000;  /* 1e6 */
    -- delta_seconds;
  }
  return((delta_seconds * 1000000) + delta_microseconds);
}

void dummyProcesssPacket(u_char *_deviceId, const struct pcap_pkthdr *h, const u_char *p) {
  struct ether_header ehdr;
  struct ip ip;
  struct tcphdr tcp;
  struct udphdr udp;

  DATA *ipaddr;

  memcpy(&ehdr, p, sizeof(struct ether_header));
  memcpy(&ip, p + sizeof(ehdr), sizeof(struct ip));
  u_short eth_type = ntohs(ehdr.ether_type);

  if (eth_type == 0x0800) {
    printf("SrcIP: %-15s", intoa(ntohl(ip.ip_src.s_addr)));
    printf(" | DstIP: %-15s", intoa(ntohl(ip.ip_dst.s_addr)));
    printf(" | P: %-5s", proto2str(ip.ip_p));
    if (ip.ip_p == IPPROTO_TCP) {
        memcpy(&tcp, p + sizeof(ehdr) + sizeof(ip), sizeof(struct tcphdr));
        printf(" | SrcP: %-10u", tcp.th_sport);
        printf(" | DstP: %-10u", tcp.th_dport);
    } else if (ip.ip_p == IPPROTO_UDP) {
        memcpy(&udp, p + sizeof(ehdr) + sizeof(ip), sizeof(struct udphdr));
        printf(" | SrcP: %-10u", udp.uh_sport);
        printf(" | DstP: %-10u", udp.uh_dport);
    }

    s.key = intoa(ntohl(ip.ip_src.s_addr));
    sp = hsearch(s, FIND);
    if (sp == NULL) {
        ipaddr = malloc(sizeof(DATA));
        ipaddr->src = 1;
        ipaddr->dsc = 0;
        s.data = (void *)ipaddr;
        hsearch(s, ENTER);
        printf("inserito src\n");
    } else {
        DATA *a = sp->data;
        a->src = 1;
        printf("già presente src\n");
    }

    d.key = intoa(ntohl(ip.ip_dst.s_addr));
    dp = hsearch(s, FIND);
    if (dp == NULL) {
        ipaddr = malloc(sizeof(DATA));
        ipaddr->src = 0;
        ipaddr->dsc = 1;
        ipaddr->t = h->ts;
        // ipaddr->t = &startTime;
        d.data = (void *)ipaddr;
        printf("ts prima %lu\n", ipaddr->t.tv_usec);
        dp = hsearch(s, ENTER);
        printf("inserito dsc\n");
    } else {
        DATA *a = dp->data;

        a->dsc = 1;
        if (!(a->src)) {
            printf("ts dopo %lu\n", a->t.tv_usec);
        }
        printf("già presente dsc\n");
    }
  } else {
    printf("dk");
  }
  printf("\n");
}

/* *************************************** */

//struct sockaddr_in* getSubnetMask(char* device) {
void getBroadCast(char* device) {
    struct ifaddrs *ifaddr, *ifa;
    struct sockaddr_in *sa = NULL;
    struct sockaddr_in *su = NULL;
    char subnet_mask[INET_ADDRSTRLEN];
    char ip[INET_ADDRSTRLEN];
    char broad[INET_ADDRSTRLEN];

    if (getifaddrs(&ifaddr) == -1) {perror("getifaddrs");exit(EXIT_FAILURE);}
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (strcmp(ifa->ifa_name, device) == 0 && ifa->ifa_addr->sa_family == AF_INET) {
            sa = (struct sockaddr_in *) ifa->ifa_netmask;
            su = (struct sockaddr_in *) ifa->ifa_addr;
            inet_ntop(AF_INET, &(sa->sin_addr), subnet_mask, INET_ADDRSTRLEN);
            inet_ntop(AF_INET, &(su->sin_addr), ip, INET_ADDRSTRLEN);
            break;
        }
    }

    broadcastIP.sin_addr.s_addr = su->sin_addr.s_addr | ~(sa->sin_addr.s_addr);
    inet_ntop(AF_INET, &(broadcastIP.sin_addr), broad, INET_ADDRSTRLEN);
    printf("SubnetMask:%s localIP:%s broadcastIP: %s\n", subnet_mask, ip, broad);
    freeifaddrs(ifaddr);
}

void getMultiCast() {
    inet_pton(AF_INET,"224.0.0.0",&minMultiIP.sin_addr);
    inet_pton(AF_INET,"239.255.255.255",&maxMultiIP.sin_addr);
}

/* *************************************** */

int main(int argc, char* argv[]) {
    char *device = NULL; 
    u_char c;
    size_t n = 10000;
    char errbuf[PCAP_ERRBUF_SIZE];
    int promisc, snaplen = DEFAULT_SNAPLEN;

    while((c = getopt(argc, argv, "hi:l:v:f:")) != '?') {
        if((c == 255) || (c == (u_char)-1)) break;
        switch(c) {
            case 'i':
                device = strdup(optarg);
                break;            
        }
    }

    if(geteuid() != 0) {
        printf("Please run this tool as superuser\n");
        return(-1);
    }

    if(device == NULL) {
        printf("ERROR: Missing -i\n");    
        return(-1);  
    }
    printf("Capturing from %s\n", device);
    getBroadCast(device);
    getMultiCast();

    /* hardcode: promisc=1, to_ms=500 */
    promisc = 1;
      
    if((pd = pcap_open_live(device, snaplen,promisc, 500, errbuf)) == NULL) {
        printf("pcap_open_live: %s\n", errbuf); 
        return(-1);
    }
    signal(SIGINT, sigproc);
    signal(SIGTERM, sigproc);

    hcreate(n);
    pcap_loop(pd, -1, dummyProcesssPacket, NULL);
    hdestroy();

    pcap_close(pd);
    return(0);
}
