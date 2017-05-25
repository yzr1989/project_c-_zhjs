#ifndef  _PARSE_BIT_FIELD_H_
#define _PARSE_BIT_FIELD_H_

#include <stdint.h>

//int  parse_bit_field(char *src , int *hd_bit_offset);
vector<string>    parse_bit_field( string   strTag  );
//int parse_router_list(char *src , uint16_t   *list,  int  list_num);
vector<string>    parse_router_list( string  strTag );
#endif   /*_PARSE_BIT_FIELD_H_*/
