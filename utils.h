#ifndef _MY_UTILS_H_
#define  _MY_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <sys/syscall.h>  

#define gettid() syscall(__NR_gettid)  

int bcd_code_l2int(uint32_t bcd_code);
int bcd_code_h2int(uint32_t bcd_code);
int bcd_code_2int(uint32_t bcd_code);

void DebugBacktrace(int signal_no);
void  print_help(char *argv[]);
void sigterm_handler(int arg) ;

//char *print_binary_16(uint16_t bin_value,char   *bin_string);
char *print_binary_16(uint16_t bin_value,char   *bin_string , int len);
int hex2bin_trim(char  *dst, char  *src, size_t count);
void hex_dump_to_buffer(const void *buf, size_t len,
			 char *linebuf, size_t linebuflen);

int hex2bin_trim_len(char  *dst, const  char  *src, size_t count);

#ifdef __cplusplus
}
#endif

#endif /*_MY_UTILS_H_*/
