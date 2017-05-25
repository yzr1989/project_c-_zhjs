#ifndef 	_STING2HEX_H_
#define	_STING2HEX_H_

#ifdef __cplusplus
extern "C" {
#endif
void print_hex_dump_bytes( const void *buf, size_t len);
void print_hex_dump(const void *buf, size_t len);
void hex_dump_to_buffer(const void *buf, size_t len,
			 char *linebuf, size_t linebuflen);

int hex2bin_trim(char  *dst, char  *src, size_t count);
int hex2bin(uint8_t *dst, const char *src, size_t count);
int hex2bin_trim_len(char  *dst, const  char  *src, size_t count);


#ifdef __cplusplus
}
#endif 


#endif   /*_STING2HEX_H_*/
