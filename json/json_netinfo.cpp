#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <linux/route.h>
#include <linux/sockios.h>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <string>

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>


#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"


#include "config.h"
#include "parse_json.h"
#include "json_op.h"
#include "json_netinfo.h"
#include "get_info_from_files.h"
#include "thread_object.h"

using namespace std;
using namespace boost;
using namespace rapidjson;


#define _PATH_PROCNET_DEV               "/proc/net/dev"

static const char * ss_fmt[] = {"%n%llu%lu%lu%lu%lu%n%n%n%llu%lu%lu%lu%lu%lu",
"%llu%llu%lu%lu%lu%lu%n%n%llu%llu%lu%lu%lu%lu%lu",
"%llu%llu%lu%lu%lu%lu%lu%lu%llu%llu%lu%lu%lu%lu%lu%lu"};


char *   getroutes(char *net_name)
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


  int  procnetdev_version(char *buf)
{
	if (strstr(buf, "compressed"))
		return 2;
	if (strstr(buf, "bytes"))
		return 1;
	return 0;
}



  char*   skip_whitespace(const char *s)
{
	/* In POSIX/C locale (the only locale we care about: do we REALLY want
	 * to allow Unicode whitespace in, say, .conf files? nuts!)
	 * isspace is only these chars: "\t\n\v\f\r" and space.
	 * "\t\n\v\f\r" happen to have ASCII codes 9,10,11,12,13.
	 * Use that.
	 */
	while (*s == ' ' || (unsigned char)(*s - 9) <= (13 - 9))
		s++;

	return (char *) s;
}




  char *  get_name(char *name, char *p)
{
	/* Extract <name> from nul-terminated p where p matches
	 * <name>: after leading whitespace.
	 * If match is not made, set name empty and return unchanged p
	 */
	char *nameend;
	char *namestart = skip_whitespace(p);

	nameend = namestart;
	while (*nameend && *nameend != ':' && !isspace(*nameend))
		nameend++;
	if (*nameend == ':') {
		if ((nameend - namestart) < IFNAMSIZ) {
			memcpy(name, namestart, nameend - namestart);
			name[nameend - namestart] = '\0';
			p = nameend;
		} else {
			/* Interface name too large */
			name[0] = '\0';
		}
	} else {
		/* trailing ':' not found - return empty */
		name[0] = '\0';
	}
	return p + 1;
}


   void  get_dev_fields(char *bp,struct user_net_device_stats  *ptr_user_net_device_stats, int procnetdev_vsn)
{
	
	sscanf(bp, ss_fmt[procnetdev_vsn],
		   &( ptr_user_net_device_stats->rx_bytes), /* missing for 0 */
		   &( ptr_user_net_device_stats->rx_packets),
		   &( ptr_user_net_device_stats->rx_errors),
		   &( ptr_user_net_device_stats->rx_dropped),
		   &( ptr_user_net_device_stats->rx_fifo_errors),
		   &( ptr_user_net_device_stats->rx_frame_errors),
		   &( ptr_user_net_device_stats->rx_compressed), /* missing for <= 1 */
		   &( ptr_user_net_device_stats->rx_multicast), /* missing for <= 1 */
		   &( ptr_user_net_device_stats->tx_bytes), /* missing for 0 */
		   &( ptr_user_net_device_stats->tx_packets),
		   &( ptr_user_net_device_stats->tx_errors),
		   &( ptr_user_net_device_stats->tx_dropped),
		   &( ptr_user_net_device_stats->tx_fifo_errors),
		   &( ptr_user_net_device_stats->collisions),
		   &( ptr_user_net_device_stats->tx_carrier_errors),
		   &( ptr_user_net_device_stats->tx_compressed) /* missing for <= 1 */
		   );

	if (procnetdev_vsn <= 1) {
		if (procnetdev_vsn == 0) {
			ptr_user_net_device_stats->rx_bytes = 0;
			ptr_user_net_device_stats->tx_bytes = 0;
		}
		ptr_user_net_device_stats->rx_multicast = 0;
		ptr_user_net_device_stats->rx_compressed = 0;
		ptr_user_net_device_stats->tx_compressed = 0;
	}
	
}

  void  printf_net_byte(struct user_net_device_stats  *ptr_user_net_device_stats,struct net_info *ptr_net_info)
{
	char rx_buffer[512];
	char tx_buffer[512];
	printf("rx_bytes=%llu,tx_bytes=%llu\n",ptr_user_net_device_stats->rx_bytes,ptr_user_net_device_stats->tx_bytes);

	sprintf(rx_buffer,"%llu",ptr_user_net_device_stats->rx_bytes);
	ptr_net_info->rx_bytes =strdup(rx_buffer);
	
	sprintf(tx_buffer,"%llu",ptr_user_net_device_stats->tx_bytes);
	ptr_net_info->tx_bytes =strdup(tx_buffer);	
	
}




  int   get_net_bytes_by_name(struct net_info *ptr_net_info)
{
	char *target = ptr_net_info->net_name;
	char buf[512];
	FILE *fh;
	struct user_net_device_stats  *ptr_user_net_device_stats;
	int  procnetdev_vsn;
	ptr_user_net_device_stats =  (struct user_net_device_stats  *) malloc(sizeof(struct user_net_device_stats  ));
	
	fh = fopen(_PATH_PROCNET_DEV,"r");
	if (fh == NULL){
		printf("can't open %s\n",_PATH_PROCNET_DEV);
		return -1;
	}

	
	fgets(buf, sizeof buf, fh);	/* eat line */
	fgets(buf, sizeof buf, fh);

	procnetdev_vsn = procnetdev_version(buf);
	printf("procnetdev_vsn=%d\n",procnetdev_vsn);		
//	err = 0;
	while (fgets(buf, sizeof buf, fh)) {
		char *s, name[128];
	
		s = get_name(name, buf);
	
		get_dev_fields(s, ptr_user_net_device_stats, procnetdev_vsn);

		if (target && !strcmp(target, name))
			break;
	}



	printf_net_byte(ptr_user_net_device_stats,ptr_net_info);	


	free(ptr_user_net_device_stats);

	return 0;
}


 int    get_net_info(struct net_info *PT_net_info[]  ,int *num)
{
	

	int len;
	int length =0;

	char*address;
	int sockfd;

	int ret =0;
	struct ifconf ifc;        //接口列表
	struct ifreq  buf[16];    //接口数据
	struct ifreq		*ifr;
	struct sockaddr_in	*sinptr;
	char hw_buffer[100];
	u_int8_t hd[6];
	char		*ptr;
	char addrstr[INET_ADDRSTRLEN];

	ifc.ifc_len = sizeof(buf);
	ifc.ifc_req = (struct ifreq *) buf;
	int i=0;	
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
	printf("length=%d\n",length);
	*num = length;
	
	ptr = (char *)buf;
		
	for (i=0; i<length;i++) {
		ifr = (struct ifreq *) ptr;
		len = sizeof(struct sockaddr);
		PT_net_info[i] =(struct net_info *) malloc(  sizeof( struct net_info));
		ptr += sizeof(ifr->ifr_name) + len;	/* for next one in buffer */

		switch (ifr->ifr_addr.sa_family) {
			case AF_INET:
			sinptr = (struct sockaddr_in *) &ifr->ifr_addr;
			address = (char *) inet_ntop(AF_INET, &sinptr->sin_addr, addrstr, sizeof(addrstr));
			printf("AF_INET:%s\t%s\n", (char *)ifr->ifr_name,address );

			PT_net_info[i]->net_name =strdup( ifr->ifr_name);			
			
			PT_net_info[i]->net_ipaddr =strdup( address);
			printf("PT_net_info[i].net_ipaddr=%s\n",PT_net_info[i]->net_ipaddr);
			//hw_addr 
			if(ioctl(sockfd, SIOCGIFHWADDR, ifr) == -1)
   			 	printf("hwaddr error.\n"), exit(0);
		
	    		memcpy( hd, ifr->ifr_hwaddr.sa_data, sizeof(hd));
				printf("HWaddr: %02X:%02X:%02X:%02X:%02X:%02X\n", hd[0], hd[1], hd[2], hd[3], hd[4], hd[5]);	
			

			sprintf(hw_buffer,"%02X:%02X:%02X:%02X:%02X:%02X",hd[0], hd[1], hd[2], hd[3], hd[4], hd[5]);
			printf("hw_buffer=%s\n",hw_buffer);	


			PT_net_info[i]->net_hwaddr  =strdup( hw_buffer);
			printf("PT_net_info[i].net_hwaddr=%s\n",PT_net_info[i]->net_hwaddr);	

			//net_mask 
			if(ioctl(sockfd, SIOCGIFNETMASK, ifr) == -1)
				printf("SIOCGIFNETMASK error.\n"), exit(0);
			sinptr = (struct sockaddr_in *) &ifr->ifr_netmask;
			address =  (char *)inet_ntop(AF_INET, &sinptr->sin_addr, addrstr, sizeof(addrstr));
			
			PT_net_info[i]->net_mask= strdup(address);
			printf("PT_net_info[i].net_netmask=%s\n",PT_net_info[i]->net_mask);

			//net_gateway
			
			PT_net_info[i]->net_gateway=  getroutes(PT_net_info[i]->net_name);
			printf("PT_net_info[i].net_gateway=%s\n",PT_net_info[i]->net_gateway);			

			
			get_net_bytes_by_name(PT_net_info[i]);

			printf("PT_net_info[i].net_gateway=%s\n",PT_net_info[i]->net_gateway);		
			printf("PT_net_info[i].net_gateway=%s\n",PT_net_info[i]->net_gateway);			
			
			break;

		default:
			printf("unkown %s\n", ifr->ifr_name);
			break;
		}


		}

  close (sockfd);

	

  return 0;
}

/*
	保留为了多网卡
*/





  int     json_netinfo::json_encode( string &return_json_string)
{
	
	struct net_info *PT_net_info[10];
	int num;
	int i =0;		
	get_net_info(PT_net_info,&num);
	printf("num=%d\n",num);

	for (i=0;i<num;i++){
		printf("net_name=%s\n",PT_net_info[i]->net_name);
		printf("net_ipaddr=%s\n",PT_net_info[i]->net_ipaddr);
		printf("net_hwaddr=%s\n",PT_net_info[i]->net_hwaddr);
	}	
	
	StringBuffer s;	
	
	Writer<StringBuffer> writer(s);
	writer.StartObject();  
	writer.Key("ordertype");   
	writer.String(json_op_name.c_str()); 


	for (i=0;i<num;i++){
		if (strcmp(  PT_net_info[i]->net_name ,"lo")){
		writer.Key("net_name");   
		writer.String(PT_net_info[i]->net_name); 			

		writer.Key("net_ipaddr");   
		writer.String(PT_net_info[i]->net_ipaddr); 

		writer.Key("net_mask");   
		writer.String(PT_net_info[i]->net_mask); 

		if (PT_net_info[i]->net_gateway != NULL){

		writer.Key("net_gateway");   
		writer.String(PT_net_info[i]->net_gateway); 

		}

		writer.Key("net_hwaddr");   
		writer.String(PT_net_info[i]->net_hwaddr); 

		writer.Key("ver");   
		writer.String(VER); 
		break;
		}
	
	}

	
	writer.EndObject();
	return_json_string= string(s.GetString());
	LOG_INFO<<return_json_string;



	for (i=0;i<num;i++){
		if (PT_net_info[i]->net_name != NULL){
			free(PT_net_info[i]->net_name);			
		}

		if (PT_net_info[i]->net_ipaddr != NULL){
			free(PT_net_info[i]->net_ipaddr);			
		}

		if (PT_net_info[i]->net_hwaddr != NULL){
			free(PT_net_info[i]->net_hwaddr);			
		}

		if (PT_net_info[i]->net_mask != NULL){
			free(PT_net_info[i]->net_mask);			
		}


		if (PT_net_info[i]->rx_bytes != NULL){
			free(PT_net_info[i]->rx_bytes);			
		}

		if (PT_net_info[i]->tx_bytes != NULL){
			free(PT_net_info[i]->tx_bytes);			
		}
		

		free(PT_net_info[i]);
	}		

	return 0;
	
}







//"netset"



int    json_encode_netset(string &return_string)
{
	
	struct net_info *PT_net_info[10];
	int num;
	int i =0;		
	get_net_info(PT_net_info,&num);
	printf("num=%d\n",num);

	for (i=0;i<num;i++){
		printf("net_name=%s\n",PT_net_info[i]->net_name);
		printf("net_ipaddr=%s\n",PT_net_info[i]->net_ipaddr);
		printf("net_hwaddr=%s\n",PT_net_info[i]->net_hwaddr);
	}	
	
	StringBuffer s;	
	
	Writer<StringBuffer> writer(s);
	writer.StartObject();  
	writer.Key("ordertype");   
	writer.String("netset"); 


	for (i=0;i<num;i++){
		if (strcmp(  PT_net_info[i]->net_name ,"lo")){
		writer.Key("net_name");   
		writer.String(PT_net_info[i]->net_name); 			

		writer.Key("net_ipaddr");   
		writer.String(PT_net_info[i]->net_ipaddr); 

		writer.Key("net_mask");   
		writer.String(PT_net_info[i]->net_mask); 

		if (PT_net_info[i]->net_gateway != NULL){

		writer.Key("net_gateway");   
		writer.String(PT_net_info[i]->net_gateway); 

		}

		writer.Key("net_hwaddr");   
		writer.String(PT_net_info[i]->net_hwaddr); 

		writer.Key("ver");   
		writer.String(VER); 
		break;
		}
	
	}

	
	writer.EndObject();
	return_string= string(s.GetString());
	LOG_INFO<<return_string;



	for (i=0;i<num;i++){
		if (PT_net_info[i]->net_name != NULL){
			free(PT_net_info[i]->net_name);			
		}

		if (PT_net_info[i]->net_ipaddr != NULL){
			free(PT_net_info[i]->net_ipaddr);			
		}

		if (PT_net_info[i]->net_hwaddr != NULL){
			free(PT_net_info[i]->net_hwaddr);			
		}

		if (PT_net_info[i]->net_mask != NULL){
			free(PT_net_info[i]->net_mask);			
		}


		if (PT_net_info[i]->rx_bytes != NULL){
			free(PT_net_info[i]->rx_bytes);			
		}

		if (PT_net_info[i]->tx_bytes != NULL){
			free(PT_net_info[i]->tx_bytes);			
		}
		

		free(PT_net_info[i]);
	}		

	return 0;
	
}





int    json_netinfo::json_parse(const char *   json_string,shared_ptr<thread_object>  ptr_thread_object , string &return_string)
{
	

	json_encode(return_string);

	return  0;
}


json_netinfo::json_netinfo()
{
	json_op_name = "netinfo";

	LOG_INFO<<"json_netinfo";
}

json_netinfo::~json_netinfo()
{
	
}

void json_netinfo_init(map <string,shared_ptr<json_op> >  &json_op_map)
{	
	shared_ptr<json_op> ptrjson_netinfo (new json_netinfo());
	RegisterJsonOp(ptrjson_netinfo,json_op_map);
}


