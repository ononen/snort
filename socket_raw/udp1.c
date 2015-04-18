/**
 *UDP 原始帧，使用linux原始套接字发送到局域网
 *"123456789"
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

int main(void)
{
	int fd = 0;
	char buf[256] = {0};
	struct sockaddr_ll dest;//sockaddr_ll:
	int destlen = 0;
	int ret = 0;
	struct ifreq ifstruct;//ifreq:
	/*ETH_P_ALL:0x0003 every packet*/
	fd = socket(PF_PACKET,SOCK_RAW,hton(ETH_P_ALL));//protocol family
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
	dest.sll_ifindex = ifstruct.if_ifindex;//interface type

	/*AF_PACKET:packet direct to the applicition not the core*/
	dest.sll_family = AF_PACKET;//address family
	/*in*/
	unsigned char  my_mac[] = {0x08,0x00,0x27,0x93,0x68,0xe5};
	memcp(dest.sll_addr,my_mac,6);
	dest.sll_halen = ETH_ALEN;//mac address length

	memset (buf,0x00,sizeof(buf));
	//int j = 0;
	/*eth hrad*/
	/*dst mac*/
	buf[0] = 0xff;
	buf[1] = 0xff;
	buf[2] = 0xff;
	buf[3] = 0xff;
	buf[4] = 0xff;
	buf[5] = 0xff;
	/*src mac*/
	buf[6] = 0x00;
	buf[7] = 0x11;
	buf[8] = 0x22;
	buf[9] = 0x33;
	buf[10] = 0x00;
	buf[11] = 0x00;
	/*eth_type :ip*/
	buf[12] = 0x08;
	buf[13] = 0x00;
	/*ip head*/
	buf[14] = 0x45;//version:4;IHL:5*4 = 20
	buf[15] = 0x00;//DS
	buf[16] = 0x00;
	buf[17] = 0x25;//total length:37 = 20 + 8 + 9
	buf[18] = 0x00;
	buf[19] = 0x00;//identification 
	buf[20] = 0x40;//flag: 000XMD
	buf[21] = 0x00;//flgment offset:13  8
	buf[22] = 0x40;//TTL
	buf[23] = 0x11;//protocol :udp
	buf[24] = 0x76;
	buf[25] = 0x4f;//header checksum
	/*source ip address*/
	buf[26] = 0xc0;
	buf[27] = 0xa8;
	buf[28] = 0x03;
	buf[29] = 0xd1;
	/*destination broad */
	buf[30] = 0xff;
	buf[31] = 0xff;
	buf[32] = 0xff;
	buf[33] = 0xff;
	/*udp head*/
	buf[34] = 0x9e;
	buf[35] = 0xae;//source port
	buf[36] = 0x1a;
	buf[37] = 0x0a;//des port
	buf[38] = 0x00;
	buf[39] = 0x11;//length :17 = 8 + 9
	buf[40] = 0x78;
	buf[41] = 0xc5;//checksum
	/*data:123456789*/
	buf[42] = 0x31;
	buf[43] = 0x32;
	buf[44] = 0x33;
	buf[45] = 0x34;
	buf[46] = 0x35;
	buf[47] = 0x36;
	buf[48] = 0x37;
	buf[49] = 0x38;
	buf[50] = 0x39;

	int ii = 0;
	while(1)
	{
		/*send data to the link layer*/
		ret = sendto(fd,buf,51,0,(struct sockaddr *)&dest,destlen);
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
