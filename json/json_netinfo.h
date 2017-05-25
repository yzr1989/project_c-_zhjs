#ifndef	_JSON_NETINFO_H_ 
#define 	_JSON_NETINFO_H_

#include <boost/noncopyable.hpp>

struct net_info{
	char *net_name;
	char *net_ipaddr;
	char *net_hwaddr;
	char *net_mask;
	char *net_gateway;
	char *rx_bytes;
	char *tx_bytes;
};

struct user_net_device_stats {
	unsigned long long rx_packets;	/* total packets received       */
	unsigned long long tx_packets;	/* total packets transmitted    */
	unsigned long long rx_bytes;	/* total bytes received         */
	unsigned long long tx_bytes;	/* total bytes transmitted      */
	unsigned long rx_errors;	/* bad packets received         */
	unsigned long tx_errors;	/* packet transmit problems     */
	unsigned long rx_dropped;	/* no space in linux buffers    */
	unsigned long tx_dropped;	/* no space available in linux  */
	unsigned long rx_multicast;	/* multicast packets received   */
	unsigned long rx_compressed;
	unsigned long tx_compressed;
	unsigned long collisions;

	/* detailed rx_errors: */
	unsigned long rx_length_errors;
	unsigned long rx_over_errors;	/* receiver ring buff overflow  */
	unsigned long rx_crc_errors;	/* recved pkt with crc error    */
	unsigned long rx_frame_errors;	/* recv'd frame alignment error */
	unsigned long rx_fifo_errors;	/* recv'r fifo overrun          */
	unsigned long rx_missed_errors;	/* receiver missed packet     */
	/* detailed tx_errors */
	unsigned long tx_aborted_errors;
	unsigned long tx_carrier_errors;
	unsigned long tx_fifo_errors;
	unsigned long tx_heartbeat_errors;
	unsigned long tx_window_errors;
};

 char *  getroutes(char *net_name);	
 int    get_net_info(struct net_info *PT_net_info[]  ,int *num);
 int  get_net_bytes_by_name(struct net_info *ptr_net_info);
 void printf_net_byte(struct user_net_device_stats  *ptr_user_net_device_stats,struct net_info *ptr_net_info);
 void get_dev_fields(char *bp,struct user_net_device_stats  *ptr_user_net_device_stats, int procnetdev_vsn);
 char *get_name(char *name, char *p);
 char*  skip_whitespace(const char *s);
 int procnetdev_version(char *buf);

class json_op;

class  json_netinfo :  public  json_op,boost::noncopyable 
{



public :
	json_netinfo();
	~json_netinfo();
	int     json_parse(const char *   json_string, shared_ptr<thread_object>  ptr_thread_object , string &return_string);
	
	 int   json_encode( string &return_json_string);
};



#endif   /*_JSON_NETINFO_H_*/
