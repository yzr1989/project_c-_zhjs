#ifndef   _SERIAL_OP_H_
#define  _SERIAL_OP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <termios.h>


typedef struct serial_op {
	const char *device_name;
	int fd;
	int baud;
	int data_bit;
	int stop_bit;
	char parity;
	struct termios old_tios;
	
} serial_op,*serial_opPtr;


int open_serial(serial_opPtr PTserial_op);
//void serial_close(serial_opPtr PTserial_op);
void close_serial(serial_opPtr PTserial_op);

#ifdef __cplusplus
}
#endif


#endif  /*_SERIAL_OP_H_*/