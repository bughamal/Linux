#include <stdio.h>
#include <stdlib.h>

#define	IP "192.168.11.128"
#define	GROUP_IP	"239.0.0.1"

int main(void)
{
		struct sockadaddr_in serveraddr,clientaddr;
		struct ip_mreqn group;
		int sockfd;
		char buf[1500];
		bzero(&group,sizeof(group));
		bzero(&serveraddr,sizeof(serveraddr));
		bzero(&clientaddr,sizeof(clientaddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons(8000);
		inet_pton(AF_INET,IP,&serveraddr.sin_addr.s_addr);
		clientaddr.sin_family = AF_INET;
		clientaddr.sin_port = htons(9000);
		inet_pton(AF_INET,GROUP_IP,&clientaddr.sin_addr.s_addr);
		sockfd = socket(AF_INET,SOCK_DGRAM,0);
		bind(sockfd,(struct sockaddr *)&serveraddr);
		/*服务器创建组播组*/
		/*level : IPPROTO_IP*/
		/*加入组播组optname:IP_ADD_MEMBERSHIP*/
		/*创建组播组optname:IP_MULTICAST_IF*/
		/*if_nametoindex 设备名转设备序号,设备名用ifconfig查看*/
		/*
		   struct ip_mreqn {
		   struct in_addr imr_multiaddr;  IP multicast group
		   address 
		   struct in_addr imr_address;    IP address of local
		   interface 
		   int            imr_ifindex;    interface index 
		   };

		 */
		inet_pton(AF_INET,GROUP_IP,&imr_multiaddr);	
		inet_pton(AF_INET,IP,&imr_address);
		int index = if_nametoindex("ens33");
		group.imr_ifindex = index;
		setsockopt(sockfd,IPPROTO_IP,IP_MULTICAST_IP,&group,sizeof(group));
		while((fgets(buf,sizeof(buf),stdin))!=NULL)
		{
				sendto(sockfd,buf,strlen(buf),0,(struct sockaddr*)&clientaddr,sizeof(clientaddr));
				bzero(buf,sizeof(buf));
		}
		close(sockfd);
		return 0;
}
