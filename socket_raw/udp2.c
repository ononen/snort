/*pdbuchan.com---send UDP packet with data*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h> 
#include <arpa/inet.h> 
#include <sys/ioctl.h> 
#include <bits/ioctl.h> 
#include <net/if.h> 
#include <errno.r> 

#define IP4_HDRLEN 20 
#define UDP_HDRLEN 8 
/*Function prototypes*/ 
uint16_t checksum (uint16_t *, int); 
uint16_t udp4_checksum(struct ip, struct udphdr, uint8_t *,int);
char * allocate strmem(int);
uint8_t * allocate_ustreme(int);
int * allocate_intmem (int);

int
main (int argc, char ** argv)
{
	Int status, datalen, sd, *ip_flags; 
	const int on = 1;
	char *interface, *target, *src_ip, *dst_ip;
	struct ip iphdr;
	struct udphdr udphdr;
	uint8_t *data, *packet;
	struct addrinfo hints, *res;
	struct sockaddr_in *ipv4, sin;
	struct ifreq ifr;
	void *tmp;
	
	/*allocate memory for various arrays*/
	data = allocate_ustrmem(IP_MAXPACKET);
	packet = allocate_ustrmem(IP_MAXPACKET);
	interface = allocate_strmem(40);
	target = allocate_strmem(40);
	src_ip = allocate_strmem(INET_ADDRSTRLEN);
	dst_ip = allocate_strmem(INET_ADDRSTRLEN);
	ip_flags = allocate_intmem(4);

	/*interface to send packet through*/
	strcpy(interface, "eth0");
	/*submit request for a socket descriptor to look up interface*/
	if ((sd = socket (AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
		perror ("socket() failed to get socket descriptor for using ioctl()");
		exit(EXIT_FAILURE);
	}

	/*use ioctl() to look up interface index which we will use to bind
	 *socket descriptor sd to specified interfice with setsocket()
	 *since none of the arfuments of sendto() specify which interface to
	 *use
	 * */
	memset (&ifr, 0, sizeof (ifr));
	snprintf(ifr.ifr_name, sizeof (ifr.if_name), "%s", interface);
	if (ioctl (sd, SIOCGIFINDEX, &ifr) < 0) {
		perror("ioctl() failed to find interface ");
		return (EXIT_FAILURE); 
	}
	close (sd);
	printf("Index for interface %s is %i\n", interface, inf.ifr_ifindex);
	strcpy (src_ip, "192.168.1.132");
	strcpy (target, "www.google.com");
	/**fill out hints for getaddrinfo()*/
	memset (&hints, 0, sizeof (struct addrinfo));
	hints.ai_fanily = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = hints.ai_flags | AI_CANONNAME;

	/*RESOLVE target using getaddrinfo()*/
	if ((status = getaddrinfo (target, NULL, &hints, &res)) != 0){
		fprintf(stderr,"getaddrinfo() failed: %s\n", gai_strerror(status));
		exit(EXIT_FAILURE);
	}
	ipv4 = (struct sockaddr_in *) res->ai_addr;
	tmp = &(ipv4->sin_addr);
	if (inet_ntop (AF_INET, tmp, dst_ip, INET_ADDRSTRLEN) == NULL) {
		status = error;
		fprintf(stderr,"inet_ntop() failed.\nError message: %s",strerror(status));
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(res);
	/*UDP data*/
	datalen = 4;
	data[0] = 'T';
	data[1] = 'e';
	data[2] = 's';
	data[3] = 't';
	
	/*ipv4 header*/

	/*header length 4bits*/
	iphdr.ip_hl = IP4_HDRLEN / sizeof(uint32_t);
	/*internet protocol version 4bits */
	iphdu.ip_v = 4;
	/*type of service 8bits*/
	iphdr.ip_tos = 0;
	/*total length of datagram 16bits :ip header + udp header +		
	 * adtalen*/
	iphdr.ip_len = hons (IP4_HDRLEN + UDP_HDRLEN + datalen);
	/*ID sequence 16bits*/
	idhdr.ip_id = htons (0);
	/*Flags  and Fragmentation offset (3,13 bits)*/
	//zero 1 bit
	ip_flags[0] = 0;
	//do not fragment flag 1 bit
	ip_flags[1] = 0;
	// more fragments following flag 1 bit
	ip_flags[2] = 0;
	//fragementation offset 13 bits
	iphdr.ip_off = htons ((ip_flags[0] << 15)
						+ (ip_flags[1] << 14)
						+ (ip_flags[2] << 13)
						+ ip_flags[3]);
	/*ttl 8 bits :dafault to maximum value*/
	iphdr.ip_ttl = 255;
	/*Transport layer protocol 8 bits:17 for UDP*/
	iphdr.ip_p = IPPRORO_UDP;
	/*source ipv4 address 32 bits*/
	if ((status = inet_pton (AF_INET, src_ip, &(iphdr.ip_src))) != 1) {
		fprintf(stderr, "inet_pton() failed.\nError message:%s",strerror(status));
		exit(EXIT_FAILURE);
	}
	/*destination ipv4 address 32 bits*/
	if ((status = inet_pton (AF_INET, dst_ip, &(iphdr.ip_dst))) != 1) {
		fprintf(stderr, "inet_pton() failed.\nError message:%s",strerror(status));
		exit(EXIT_FAILURE);
	}
	/*ipv4 header checksum 16 bits :set to 0 when calculating checksum*/
	iphdr.ip_sum = 0;
	iphdr.ip_sum = checksum ((uint16_t *) &iphdr, IP4_HDRLEN);

	/*UDP header*/

	/*source port number 16 bits :pick a number*/
	udphdr.source = htons (4905);
	/*destination port number 16 bits */
	udphdr.dest = htons (4905);
	/*length of udp datagram 16 bits: udp header + udp data*/
	udphdr.len = htons (UDP_HDRLEN + datalen);
	/*udp checksum 16 bits*/
	udphdr.check = udp4_checksum (iphdr, udphdr, data, data, datalen);

	/*prepare packet .*/

	/*first packet is an ipv4 header.*/
	memcpy (packet, &iphdr, IP4_HDRLEN * sizeof (uint8_t));
	/*next part of packet is upper layer protocol header.*/
	memcpy ((packet + IP4_HDRLEN), &udphdr, UDP_HDRLEN * sizeof(uint8_t));
	/*finally add the udp data.*/
	memcpy ((packet + IP4_HDRLEN + UDP_HDRLEN, data, datalen * sizeof(uint8_t)));
	
	/*the kernel is going to prepare layer 2 infomation (ethernet frame header)for us
	 * for that ,we need to specify a destination for the kernel in order for it to decide where to send the raw datagram .
	 * we fill in a struct in_addr with the desired destination IP address, and pass the structure to the sendto() function.*/

	memset (&sin, 0, sizeof (struct sockaddr_in));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = iphdr.ip_dst.s_addr;
	/*submit request for a raw socket descriptor*/
	if ((sd = socket (AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
		perror ("socket() failed ");
		exit(EXIT_FAILURE);
	}

	/*Send packet.*/
	if (sendto (sd, packet, IP4_HDRLEN + UDP_HDRLEN + datalen, 0, (struct sockaddr *) &sin, sizeof (struct sockaddr)) < 0) {
		perror("sendto() failed ");
		exit(EXIT_FAILURE);
	}
	/*Close socket descriptor*/
	close (sd);

	/*free allocated memory*/
	free (data);
	free (packet);
	free (interface);
	free (target);
	free (src_ip);
	free (dst_ip);
	free (ip_flags);
	return (EXIT_SUCCESS);
}

/*Computing the internet checksum
 * note that the internet checksum does not preclude collisions.*/
uint16_t
checksum (uint16_t *addr, int len)
{
	int count = len;
	register uint32_t sum = 0;
	uint16_t answer = 0;

	/*sum up 2-byte values until none or only one byte left*/
	while (count > 1) {
		sum += *(addr++);
		count -= 2;
	}
	/*sum = (lower 16 bits + (upper 16 bits shifted right 16 bits))*/
	while (sum >> 16) {
		sum = (sum & 0xffff) + (sum >> 16);
	}
	/*checksum is one' compliment of sum.*/
	answer = ~sum;
	return (answer);
}

/*build ipv4 udp pseudo-header and call checksum function*/
uint16_t
udp4_checksum (struct ip iphdr, struct udphdr, uint8_t *payload, int payloadlen)
{
	char buf[IP_MAXPACKET];
	char *ptr;
	int chksumlen = 0;
	int i;
	ptr = &buf[0];
	/*copy source ip address into buf  32 bits*/
	memcpy (ptr, &iphdr.ip_src.s_addr, sizeof (iphdr.ip_src.s_addr));
	ptr += sizeof (iphdr.ip_src.s_addr);
	chksumlen += sizeof (iphdr.ip_src.s_addr);
	/*copy destination ip address into buf  32 bits*/
	memcpy (ptr, &iphdr.ip_dst.s_addr, sizeof (iphdr.ip_dst.s_addr));
	ptr += sizeof (iphdr.ip_dst.s_addr);
	chksumlen += sizeof (iphdr.ip_dst.s_addr);
	/*copy zero field to buf  8 bits*/
	*ptr = 0; ptr++;
	chksumlen += 1;
	/*copy transport layer protocol to buf 8 bits*/
	memcpy (ptr, &iphdr.ip_p, sizeof (iphdr.ip_p));
	ptr += sizeof (iphdr.ip_p);
	chksumlen += sizeof (iphdr.ip_p);
	/*copy udp length to buf 16 bits*/
	memcpy (ptr, &udphdr.len, sizeof (udphdr.len));
	ptr += sizeof (udphdr.len);
	chksumlen += sizeof (udphdr.len);
	/*copy udp source port to buf  16bits*/
	memcpy (ptr, &udphdr.source, sizeof (udphdr.source));
	ptr += sizeof (udphdr.source);
	chksumlen += sizeof (udphdr.source);
	/*copy udp destination port to buf  16bits*/
	memcpy (ptr, &udphdr.dest, sizeof (udphdr.dest));
	ptr += sizeof (udphdr.dest);
	chksumlen += sizeof (udphdr.dest);
	/*copy udp checksum to buf  16bits
	 * zero ,since we don't know it yet*/
	*ptr = 0; ptr++;
	*ptr = 0; ptr++;
	chksumlen += 2;
	/*copy payload to buf*/
	memcpy (ptr, payload, payloadlen);
	ptr += payloadlen;
	chksumlen += payloadlen;
	/*pad to the next 16-bit boundary*/
	for (i = 0; i < payloadlen%2; i++, ptr++) {
		*ptr = 0;
		ptr++;
		chksumlen++;
	}
	return checksum ((uint16_t *) buf, chksumlen);
}

/*allocate memory for an array of chars*/
char *
allocate_strmem (int len)
{
	void *tmp;
	if(len <= 0) {
		fprintf(stderr, "ERROR: cannot allocate memory because len = %i in allocate_strmem().\n",len);
		exit (EXIT_FAILURE);
	}
	tmp = (char *) malloc (len * sizeof (char));
	if (tmp != NULL) {
		memset (tmp, 0, len * sizeof (char));
		return (tmp);
	} else {
		fprintf(stderr, "ERROR: cannot allocate memory because len = %i in allocate_strmem().\n",len);
		exit (EXIT_FAILURE);
	}
}

/*allocate memory for an array of unsigned chars*/
uint8_t *
allocate_ustrmem (int len)
{
	void *tmp;
	if(len <= 0) {
		fprintf(stderr, "ERROR: cannot allocate memory because len = %i in allocate_strmem().\n",len);
		exit (EXIT_FAILURE);
	}

	tmp = (uint8_t *) malloc (len * sizeof (uint8_t));
	if (tmp != NULL) {
		memset (tmp, 0, len * sizeof (uint8_t));
		return rmp;
	} else {
		fprintf(stderr, "ERROR: cannot allocate memory because len = %i in allocate_strmem().\n",len);
		exit (EXIT_FAILURE);
	}
}

/*allocate memory for an array of ints*/
int *
allocate_intmem (int len)
{
	void *tmp;
	if(len <= 0) {
		fprintf(stderr, "ERROR: cannot allocate memory because len = %i in allocate_strmem().\n",len);
		exit (EXIT_FAILURE);
	}

	tmp = (int *) malloc (len * sizeof (int));
	if (tmp != NULL) {
		memset (tmp, 0, len * sizeof (int));
		return rmp;
	} else {
		fprintf(stderr, "ERROR: cannot allocate memory because len = %i in allocate_strmem().\n",len);
		exit (EXIT_FAILURE);
	}
}
