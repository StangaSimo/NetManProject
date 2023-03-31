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
#include <netinet/tcp.h>
#include <netinet/udp.h>

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

void dummyProcesssPacket(u_char *_deviceId, const struct pcap_pkthdr *h, const u_char *p) {
    struct ether_header ehdr;
    //u_short eth_type, vlan_id;
    struct ip ip;
    struct tcphdr tcp;
    struct udphdr udp;
    //struct in_addr inaddr;
    memcpy(&ehdr, p, sizeof(struct ether_header));
    memcpy(&ip, p+sizeof(ehdr), sizeof(struct ip));
    u_short eth_type = ntohs(ehdr.ether_type);
    if(eth_type == 0x0800) {
        printf("SrcIP: %-15s" ,intoa(ntohl(ip.ip_src.s_addr)));
        printf(" | DstIP: %-15s",intoa(ntohl(ip.ip_dst.s_addr)));
        printf(" | P: %-5s", proto2str(ip.ip_p));
        if (ip.ip_p == IPPROTO_TCP) {
            memcpy(&tcp, p+sizeof(ehdr)+sizeof(ip), sizeof(struct tcphdr));
            printf(" | SrcP: %-10u", tcp.th_sport);
            printf(" | DstP: %-10u", tcp.th_dport);
        } else if (ip.ip_p == IPPROTO_UDP) {
            memcpy(&udp, p+sizeof(ehdr)+sizeof(ip), sizeof(struct udphdr));
            printf(" | SrcP: %-10u", udp.uh_sport);
            printf(" | DstP: %-10u", udp.uh_dport);
        }
    } else{
        printf("dk");
    }
    printf("\n");
}

/* *************************************** */

//struct sockaddr_in* getSubnetMask(char* device) {
void getBroadCast(char* device) {
    struct ifaddrs *ifaddr, *ifa;
    char *interface_name = "eth0"; // inserire il nome della scheda di rete
    struct sockaddr_in *sa;
    char subnet_mask[INET_ADDRSTRLEN];
    unsigned char subnet[sizeof(struct in6_addr)];

    if (getifaddrs(&ifaddr) == -1) {perror("getifaddrs");exit(EXIT_FAILURE);}

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (strcmp(ifa->ifa_name, device) == 0 && ifa->ifa_addr->sa_family == AF_INET) {
            sa = (struct sockaddr_in *) ifa->ifa_netmask;
            inet_ntop(AF_INET, &(sa->sin_addr), subnet_mask, INET_ADDRSTRLEN);
            break;
        }
    }

    printf("Subnet mask of %s: %s --- %u\n", interface_name, subnet_mask, sa->sin_addr.s_addr);
    int s = inet_pton(AF_INET,subnet_mask, subnet);
    if (s <= 0) {
        if (s == 0)
            fprintf(stderr, "Not in presentation format");
        else
            perror("inet_pton");
        exit(EXIT_FAILURE);
    }
    freeifaddrs(ifaddr);
    //return sa;
}

/* *************************************** */

int main(int argc, char* argv[]) {
    char *device = NULL; 
    //char *bpfFilter = NULL;
    u_char c;
    char errbuf[PCAP_ERRBUF_SIZE];
    int promisc, snaplen = DEFAULT_SNAPLEN;
    struct sockaddr_in* subnet_mask;

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
    //return 0; 

    /* hardcode: promisc=1, to_ms=500 */
    promisc = 1;
      
    if((pd = pcap_open_live(device, snaplen,promisc, 500, errbuf)) == NULL) {
        printf("pcap_open_live: %s\n", errbuf); 
        return(-1);
    }
    signal(SIGINT, sigproc);
    signal(SIGTERM, sigproc);

    pcap_loop(pd, -1, dummyProcesssPacket, NULL);

    pcap_close(pd);
    return(0);
}
