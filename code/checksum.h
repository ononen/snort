/*
#include<sys/socket.h>
#include<netinet/in.h>
#include<netinet/if_ether.h>
#include<netinet/ip.h>
#include<linux/if_packet.h>
#include<linux/if.h>
#include<linux/sockios.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
*/
/**/
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
#include <netinet/tcp.h> 
#include <arpa/inet.h> 
#include <sys/ioctl.h> 
#include <bits/ioctls.h> 
#include <net/if.h> 
#include <errno.h> 

#define IP4_HDRLEN 20 
#define TCP_HDRLEN 20 
#define UDP_HDRLEN 8 
/*Function prototypes*/ 
uint16_t checksum (uint16_t *, int); 
uint16_t udp4_checksum(struct ip, struct udphdr, uint8_t *,int);
uint16_t tcp4_checksum(struct ip, struct tcphdr, uint8_t *,int);
char * allocate_strmem(int);
uint8_t * allocate_ustrmem(int);
int * allocate_intmem (int);
