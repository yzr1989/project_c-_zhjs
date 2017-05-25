#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/sockios.h>
#include <net/route.h>
#include <net/if.h>
#include <net/if_arp.h>

#include "config.h"

#include "get_info_from_files.h"


#define ETHER_ADDR_LEN    6
#define UP    1
#define DOWN    0


int net_print(struct netinfo *Ptrnetinfo)
{
	printf("net_name=%s\n",Ptrnetinfo->net_name);
	printf("net_ipaddr=%s\n",Ptrnetinfo->net_ipaddr);
	printf("net_mask=%s\n",Ptrnetinfo->net_mask);
	printf("net_gateway=%s\n",Ptrnetinfo->net_gateway);
	printf("hw_addr=%s\n",Ptrnetinfo->hw_addr);	

	return 0;
	
}

int net_info_set_from_file()
{
	int ret ;
	GKeyFile* config = g_key_file_new();
	
//	 gchar* serial_number_buffer = NULL;

	struct netinfo *Ptrnetinfo = malloc(sizeof(struct netinfo));
	if (Ptrnetinfo == NULL){
		printf("malloc netinfo error\n");
		return -1;

	}
	
//	long ip;
	int len;
	int length =0;

	const char*address;
	int sockfd;

	
	struct ifconf ifc;        //接口列表
	struct ifreq  buf[16];    //接口数据
	struct ifreq		*ifr;
	struct sockaddr_in	*sinptr;
	//char hw_buffer[100];
	//u_int8_t hd[6];
	char		*ptr;
	char addrstr[INET_ADDRSTRLEN];
	//ip = -1;
	ifc.ifc_len = sizeof(buf);
	ifc.ifc_req = (struct ifreq *) buf;
	int i=0;

	 g_key_file_load_from_file(config,CONFIG_SYS_FILE_NAME,G_KEY_FILE_KEEP_COMMENTS|G_KEY_FILE_KEEP_TRANSLATIONS,NULL);

	//net_ipaddr = g_key_file_get_value(config,net_name,"net_ipaddr",NULL);	

	sockfd = socket(AF_INET,SOCK_DGRAM,0);
    	if (sockfd <0){
		printf("socket error\n");
		return -1;	
 	}

	ret = ioctl (sockfd,SIOCGIFCONF,&ifc);
	if (ret <0){
		printf("ioctl SIOCGIFCONF error\n");
		return -1;
	}	

	length = ifc.ifc_len/sizeof(struct ifreq);

	
	ptr = (char *)buf;
		
	for (i=0; i<length;i++) {
		ifr = (struct ifreq *) ptr;
		len = sizeof(struct sockaddr);
		
		ptr += sizeof(ifr->ifr_name) + len;	/* for next one in buffer */

		switch (ifr->ifr_addr.sa_family) {
			case AF_INET:
			sinptr = (struct sockaddr_in *) &ifr->ifr_addr;
			address =  inet_ntop(AF_INET, &sinptr->sin_addr, addrstr, sizeof(addrstr));
			printf("AF_INET:%s\t%s\n", (char *)ifr->ifr_name,address );
			Ptrnetinfo->net_name =(char *)ifr->ifr_name;
			

	 	Ptrnetinfo->net_ipaddr = g_key_file_get_value(config,Ptrnetinfo->net_name,"net_ipaddr",NULL);
		Ptrnetinfo->net_mask = g_key_file_get_value(config,Ptrnetinfo->net_name,"net_mask",NULL);
		Ptrnetinfo->net_gateway = g_key_file_get_value(config,Ptrnetinfo->net_name,"net_gateway",NULL);
		Ptrnetinfo->hw_addr = g_key_file_get_value(config,Ptrnetinfo->net_name,"hw_addr",NULL);
		net_print(Ptrnetinfo);
		ret =netset(Ptrnetinfo);

	
			break;

		default:
			printf("unkown %s\n", ifr->ifr_name);
			break;
		}


		}

  close (sockfd);

	free(Ptrnetinfo);
	 g_key_file_free(config);
	 
	
	return  0;
}

int if_updown(const char *ifname, int flag)
{
    int fd, rtn;
    struct ifreq ifr;        

    if (!ifname) {
        return -1;
    }

    fd = socket(AF_INET, SOCK_DGRAM, 0 );
    if ( fd < 0 ) {
        perror("socket");
        return -1;
    }
    
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1 );

    if ( (rtn = ioctl(fd, SIOCGIFFLAGS, &ifr) ) == 0 ) {
        if ( flag == DOWN )
            ifr.ifr_flags &= ~IFF_UP;
        else if ( flag == UP ) 
            ifr.ifr_flags |= IFF_UP;
        
    }

    if ( (rtn = ioctl(fd, SIOCSIFFLAGS, &ifr) ) != 0) {
        perror("SIOCSIFFLAGS");
    }

    close(fd);

    return rtn;
}

int set_mac_addr(const char *ifname, char *mac)
{
    int fd, rtn;
    struct ifreq ifr;

    if( !ifname || !mac ) {
        return -1;
    }
    fd = socket(AF_INET, SOCK_DGRAM, 0 );
    if ( fd < 0 ) {
        perror("socket");
        return -1;
    }
    ifr.ifr_addr.sa_family = ARPHRD_ETHER;
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1 );
    memcpy((unsigned char *)ifr.ifr_hwaddr.sa_data, mac, 6);
    
    if ( (rtn = ioctl(fd, SIOCSIFHWADDR, &ifr) ) != 0 ){
        perror("SIOCSIFHWADDR");
    }
    close(fd);
    return rtn;
}


char *   getroutes(const char *net_name)
{
	char devname[64];
	unsigned long d, g, m;
	int flgs, ref, use, metric, mtu, win, ir;
	//struct sockaddr_in s_addr;
	//struct in_addr mask;
	char *gateway_address = NULL;
	FILE *fp = fopen("/proc/net/route","r");
	if (fp == NULL){
		printf("can't open :%d",errno);
		return  NULL;
	}

	if (fscanf(fp, "%*[^\n]\n") < 0) { /* Skip the first line. */
		printf("fscanf");
		exit(-1);
	}
	while (1) {
		int r;
		r = fscanf(fp, "%63s%lx%lx%X%d%d%d%lx%d%d%d\n",
				   devname, &d, &g, &flgs, &ref, &use, &metric, &m,
				   &mtu, &win, &ir);
		if (r != 11) {
			if ((r < 0) && feof(fp)) { /* EOF with no (nonspace) chars read. */
				break;
			}

			printf("fscanf");
			exit(-1);
		}

		if ((!strcmp(devname,net_name))&&(flgs&RTF_GATEWAY)){
			 struct in_addr	sin_addr;
			
			sin_addr.s_addr = g;
			printf("devname=%s,gw address=%s\n",devname,inet_ntoa(sin_addr));
			gateway_address = inet_ntoa(sin_addr);
		}	

		if (!(flgs & RTF_UP)) { /* Skip interfaces that are down. */
			continue;
		}



	}

	return gateway_address;
}

static void resolve_net(struct sockaddr_in *sa,sa_family_t  sa_family ,uint32_t addr)
{
	
	sa->sin_family = sa_family;
	sa->sin_addr.s_addr = addr;
	
}



int del_route_gateway( char *net_name)
{
	char *gateway_address=NULL;
	struct rtentry rt;
	int skfd;
	int ret =0;
	printf("net_name=%s\n",net_name);
	while((gateway_address =getroutes(net_name) )){
		if (gateway_address == NULL){
			break;
		}
		uint32_t  dstaddr, gwaddr;
		dstaddr = inet_addr("0.0.0.0");
		gwaddr = inet_addr(gateway_address);
		skfd = socket(AF_INET, SOCK_DGRAM, 0);
		memset(&rt, 0, sizeof(rt));
		resolve_net((struct sockaddr_in *)&(rt.rt_dst),AF_INET,dstaddr);
		resolve_net((struct sockaddr_in *)&(rt.rt_gateway),AF_INET,gwaddr);
		resolve_net((struct sockaddr_in *)&(rt.rt_genmask),AF_INET,0L);

		rt.rt_dev = net_name;
		rt.rt_flags = RTF_GATEWAY;
		
		ret = ioctl(skfd, SIOCDELRT, &rt);
		if (ret <0){
			printf("error SIOCDELRT :%d",errno);
			break;
		}
		close (skfd);
	}	
	
	return 0;
	
}


int netset(struct netinfo *Ptrnetinfo)
{
//	struct sockaddr_in *addr;
	struct ifreq ifr;
	//char*address;
	int sockfd;
	//char *p;
	int ret = 0;
	uint8_t hd[6];	
	struct  sockaddr_in src;

	if (Ptrnetinfo == NULL){
		return -1;
		
	}

	printf("---netset-begin----\n");
	net_print(Ptrnetinfo);

	strcpy( ifr.ifr_name, Ptrnetinfo->net_name);
		

	

	// set hw_addr
	if (Ptrnetinfo->hw_addr != NULL){
		sscanf(Ptrnetinfo->hw_addr,"%x:%x:%x:%x:%x:%x",(unsigned int* )&hd[0],(unsigned int* )&hd[1],(unsigned int* )&hd[2],
			(unsigned int* )&hd[3],(unsigned int* )&hd[4],(unsigned int* )&hd[5]);

		for (int i =0;i<sizeof(hd);i++){
			printf("hd[%d]=0x%x\t",i,hd[i]);	
		}
		printf("\n");
	if_updown(Ptrnetinfo->net_name,DOWN);
	set_mac_addr(Ptrnetinfo->net_name,(char *)hd);
	if_updown(Ptrnetinfo->net_name,UP);

	}
		
	
	//set ip
	if (Ptrnetinfo->net_ipaddr != NULL){
		printf("----net_ipaddr-----\n");
	    	sockfd = socket(AF_INET,SOCK_DGRAM,0);	
		if (sockfd <0){
			printf("socket error:%d",errno);
			return -1;
		}	
		src.sin_family = AF_INET;
		src.sin_port = 0;
		src.sin_addr.s_addr = inet_addr(Ptrnetinfo->net_ipaddr);


		memcpy(&(ifr.ifr_netmask),&src,sizeof(struct sockaddr_in));
		//set  net addr
			ret  = ioctl( sockfd, SIOCSIFADDR, &ifr) ;
		if(ret <0 ){
				printf("SIOCSIFADDR ioctl error.\n");
		}else {
				printf("SIOCSIFADDR ioctl ok.\n");

		}
		close(sockfd);
		if_updown(Ptrnetinfo->net_name,UP);
	}
	//set  net mask
	if (Ptrnetinfo->net_mask != NULL){
	    	sockfd = socket(AF_INET,SOCK_DGRAM,0);	
		if (sockfd <0){
			printf("socket error:%d",errno);
			return -1;
		}			
		src.sin_family = AF_INET;
		src.sin_port = 0;
		src.sin_addr.s_addr = inet_addr(Ptrnetinfo->net_mask);
		memcpy(&(ifr.ifr_addr),&src,sizeof(struct sockaddr_in));
		
			ret  = ioctl( sockfd, SIOCSIFNETMASK, &ifr) ;
		if(ret <0 ){
				printf("SIOCSIFADDR ioctl error:%d\n",errno);
		}else {
				printf("SIOCSIFADDR ioctl ok.\n");

		}
		close(sockfd);
	}

	//set  net gateway 
	if (Ptrnetinfo->net_gateway != NULL){
		
		del_route_gateway((char *)Ptrnetinfo->net_name);
		struct rtentry rt;
		uint32_t  dstaddr, gwaddr;
		memset(&rt, 0, sizeof(rt));
		dstaddr = inet_addr("0.0.0.0");
		gwaddr = inet_addr(Ptrnetinfo->net_gateway);
	    	sockfd = socket(AF_INET,SOCK_DGRAM,0);	
		if (sockfd <0){
			printf("socket error:%d",errno);
			return -1;
		}
		resolve_net((struct sockaddr_in *)&(rt.rt_dst),AF_INET,dstaddr);
		resolve_net((struct sockaddr_in *)&(rt.rt_gateway),AF_INET,gwaddr);
		resolve_net((struct sockaddr_in *)&(rt.rt_genmask),AF_INET,0L);
		rt.rt_dev = (char *)(Ptrnetinfo->net_name);
		rt.rt_flags = RTF_GATEWAY;
		
		ret = ioctl(sockfd, SIOCADDRT, &rt);


		if(ret <0 ){
				printf("SIOCADDRT ioctl error:%d\n",errno);
		}else {
				printf("SIOCADDRT ioctl ok.\n");

		}
		close(sockfd);
	}	

	   printf("---netset-end----\n");
      return ret;

 
}


 int net_config_write_file(struct netinfo *Ptrnetinfo )
{
	GKeyFile* config = g_key_file_new();
	 //gchar* serial_number_buffer;
  	gsize length = 0;	
	g_key_file_load_from_file(config,CONFIG_SYS_FILE_NAME,G_KEY_FILE_KEEP_COMMENTS|G_KEY_FILE_KEEP_TRANSLATIONS,NULL);


	if (Ptrnetinfo->net_ipaddr != NULL){
		g_key_file_set_value(config,Ptrnetinfo->net_name,"net_ipaddr",Ptrnetinfo->net_ipaddr);
	}
	if (Ptrnetinfo->net_mask != NULL){
		g_key_file_set_value(config,Ptrnetinfo->net_name,"net_mask",Ptrnetinfo->net_mask);		
	}
	if(Ptrnetinfo->net_gateway != NULL){
		g_key_file_set_value(config,Ptrnetinfo->net_name,"net_gateway",Ptrnetinfo->net_gateway);	
	}

	if (Ptrnetinfo->hw_addr != NULL){
		g_key_file_set_value(config,Ptrnetinfo->net_name,"hw_addr",Ptrnetinfo->hw_addr);
	}

	 gchar* content = g_key_file_to_data(config,&length,NULL);
	 FILE* fp = fopen(CONFIG_SYS_FILE_NAME,"w");
    	if(fp == NULL) {		
		 printf("fopen error\n");
		  return -1;
    	}	 
      	fwrite(content,1,length,fp);
      	fclose(fp);

	 g_key_file_free(config);

	return 0;
}

