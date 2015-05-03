#include "checksum.h"
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
udp4_checksum (struct ip iphdr, struct udphdr udphdr, uint8_t *payload, int payloadlen)
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
		return tmp;
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
		return tmp;
	} else {
		fprintf(stderr, "ERROR: cannot allocate memory because len = %i in allocate_strmem().\n",len);
		exit (EXIT_FAILURE);
	}
}
