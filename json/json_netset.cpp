
#include <stdio.h> 
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <glib.h>
#include <netdb.h>
#include <fcntl.h>
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

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>

#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "config.h"
#include "parse_json.h"
#include "json_op.h"
#include "json_netset.h"
#include "get_info_from_files.h"
#include "thread_object.h"

using namespace rapidjson;

 char *  getroutes(char *net_name);	



#define ETHER_ADDR_LEN    6
#define UP    1
#define DOWN    0



int get_mac_addr(char *ifname, char *mac)
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
    ifr.ifr_addr.sa_family = AF_INET;    
    strncpy(ifr.ifr_name, (const char *)ifname, IFNAMSIZ - 1 );

    if ( (rtn = ioctl(fd, SIOCGIFHWADDR, &ifr) ) == 0 )
        memcpy(    mac, (unsigned char *)ifr.ifr_hwaddr.sa_data, 6);
    close(fd);
    return rtn;
}





/*
 * Convert Ethernet address string representation to binary data
 * @param    a    string in xx:xx:xx:xx:xx:xx notation
 * @param    e    binary data
 * @return    TRUE if conversion was successful and FALSE otherwise
 */
int
ether_atoe(const char *a, unsigned char *e)
{
    char *c = (char *) a;
    int i = 0;

    memset(e, 0, ETHER_ADDR_LEN);
    for (;;) {
        e[i++] = (unsigned char) strtoul(c, &c, 16);
        if (!*c++ || i == ETHER_ADDR_LEN)
            break;
    }
    return (i == ETHER_ADDR_LEN);
}


/*
 * Convert Ethernet address binary data to string representation
 * @param    e    binary data
 * @param    a    string in xx:xx:xx:xx:xx:xx notation
 * @return    a
 */
char *
ether_etoa(const unsigned char *e, char *a)
{
    char *c = a;
    int i;

    for (i = 0; i < ETHER_ADDR_LEN; i++) {
        if (i)
            *c++ = ':';
        c += sprintf(c, "%02X", e[i] & 0xff);
    }
    return a;
}

int    json_encode_netset(string &return_string);




 int     json_netset::json_parse(const char *   json_string ,shared_ptr<thread_object>  ptr_thread_object ,  string &return_string)
{

     LOG_INFO<<"json_parse_heartbeat";
	int ret;

	struct netinfo *Ptrnetinfo = (struct netinfo *)malloc(sizeof(struct netinfo));
	if (Ptrnetinfo == NULL){
		return  NULL;
	}

	memset(Ptrnetinfo,0,sizeof(struct netinfo ));
	
	  Document d;
	  d.Parse(json_string);
	   string  ordertype = d["ordertype"].GetString();

	 cout <<	json_op_name <<endl;

	 if ( d.HasMember("net_name") ) {

	 Ptrnetinfo->net_name= d["net_name"].GetString();
	cout <<Ptrnetinfo->net_name<<endl;
	
	 }


	 if ( d.HasMember("net_ipaddr") ) {

	Ptrnetinfo->net_ipaddr = d["net_ipaddr"].GetString();
	cout <<Ptrnetinfo->net_ipaddr<<endl;
	 }



	 if ( d.HasMember("net_mask") ) {

	 Ptrnetinfo->net_mask = d["net_mask"].GetString();
	cout <<Ptrnetinfo->net_mask<<endl;
	 }

	 if ( d.HasMember("net_gateway") ) {

	 Ptrnetinfo->net_gateway = d["net_gateway"].GetString();
	cout <<Ptrnetinfo->net_gateway<<endl;
	 }

	 if ( d.HasMember("hw_addr") ) {

	 Ptrnetinfo->hw_addr = d["hw_addr"].GetString();
	cout <<Ptrnetinfo->hw_addr<<endl;
	 }

	if (Ptrnetinfo->net_name!=NULL){	
		net_print(Ptrnetinfo);
		net_config_write_file(Ptrnetinfo);
		ret =netset(Ptrnetinfo);	

	}

	 json_encode_netset(return_string);
	cout <<return_string<<endl;

	 free(Ptrnetinfo);

	return 0;
}





json_netset::json_netset()
{
	json_op_name = "netset";
	//cout <<"json_netset"<<endl;	
	LOG_INFO<<"json_netset";
}

json_netset::~json_netset()
{
	LOG_INFO<<"~json_netset";
}


void json_netset_init(map <string,shared_ptr<json_op> >  &json_op_map)
{	
	
	shared_ptr<json_op> ptrjson_netset (new json_netset());
	RegisterJsonOp(ptrjson_netset,json_op_map);
}


