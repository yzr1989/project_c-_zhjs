#ifndef   _GET_INFO_FROM_FILES_H_
#define _GET_INFO_FROM_FILES_H_


#ifdef __cplusplus
extern "C" {
#endif

struct netinfo{
	const char *net_name;
	const char *net_ipaddr;
	const char *net_mask;
	const char *net_gateway;
	const char *hw_addr ;
	
};

int net_print(struct netinfo *Ptrnetinfo);
 int net_config_write_file(struct netinfo *Ptrnetinfo );
 int netset(struct netinfo *Ptrnetinfo);
int net_info_set_from_file();

#ifdef __cplusplus
}
#endif


#endif  /*_GET_INFO_FROM_FILES_H_*/