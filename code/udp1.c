/**
 *UDP row-socket
 *"Test"
 * */
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

int main(void)
{
	int fd = 0;
	char buf[256] = {0};
	struct sockaddr_ll dest;//sockaddr_ll:
	int destlen = 0;
	int ret = 0;
	struct ifreq ifstruct;//ifreq:
	/*ETH_P_ALL:0x0003 every packet*/
	fd = socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL));//protocol family
	if(fd < 0)
	{
		printf("ERR:socket was failed.\n");
		return -1;
	}

	memset((char *)&dest, 0x00,sizeof(dest));
	destlen = sizeof(dest);
	strcpy(ifstruct.ifr_name, "eth0");//get host ip 
	/*SIOCGIFINDEX:interface index */
	ioctl(fd, SIOCGIFINDEX,&ifstruct);//control .io
	dest.sll_ifindex = ifstruct.ifr_ifindex;//interface type

	/*AF_PACKET:packet direct to the applicition not the core*/
	dest.sll_family = AF_PACKET;//address family
	/*in*/
	unsigned char  my_mac[] = {0x00,0x50,0x56,0xC0,0x00,0x08};
	memcpy(dest.sll_addr,my_mac,6);
	dest.sll_halen = ETH_ALEN;//mac address length

	memset (buf,0x00,sizeof(buf));
	//int j = 0;
	/*eth hrad*/
	/*dst mac*/
	buf[0] = 0x00;
	buf[1] = 0x0C;
	buf[2] = 0x29;
	buf[3] = 0x17;
	buf[4] = 0x48;
	buf[5] = 0x1E;
	/*src mac*/
	buf[6] = 0x00;
	buf[7] = 0x50;
	buf[8] = 0x56;
	buf[9] = 0xC0;
	buf[10] = 0x00;
	buf[11] = 0x08;
	/*eth_type :ip*/
	buf[12] = 0x08;
	buf[13] = 0x00;
	/*ip head*/
	buf[14] = 0x45;//version:4;IHL:5*4 = 20
	buf[15] = 0x00;//DS
	buf[16] = 0x00;
	buf[17] = 0x20;//total length:32 = 20 + 8 + 4
	buf[18] = 0x15;
	buf[19] = 0x62;//identification 
	buf[20] = 0x00;//flag: 000XMD
	buf[21] = 0x00;//flgment offset:13  8
	buf[22] = 0xff;//TTL
	buf[23] = 0x11;//protocol :udp
	buf[24] = 0x7c;
	buf[25] = 0x3d;//header checksum
	/*source ip address*/
	buf[26] = 0xac;
	buf[27] = 0x10;
	buf[28] = 0xe9;
	buf[29] = 0x01;
	/*destination broad */
	buf[30] = 0xac;
	buf[31] = 0x10;
	buf[32] = 0xe9;
	buf[33] = 0x64;
	/*udp head*/
	buf[34] = 0x13;
	buf[35] = 0x29;//source port
	buf[36] = 0x13;
	buf[37] = 0x29;//des port
	buf[38] = 0x00;
	buf[39] = 0x0c;//length :12 = 8 + 4
	buf[40] = 0xe7;
	buf[41] = 0x88;//checksum
	/*data:Test*/
	buf[42] = 0x54;
	buf[43] = 0x65;
	buf[44] = 0x73;
	buf[45] = 0x74;

	int ii = 0;
	while(1)
	{
		/*send data to the link layer*/
		ret = sendto(fd,buf,46,0,(struct sockaddr *)&dest,destlen);
		if(ret < 0)
		{
			perror("error");
		}
		printf("%d\n",ii);
		ii++;
		sleep(1);
	}
	close(fd);
	printf("finished!\n");
	return 0;
}
