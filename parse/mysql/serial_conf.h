#ifndef  _SERIAL_CONF_H_
#define  _SERIAL_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <termios.h>
#include <unistd.h>

typedef  struct  {

    /* Socket or file descriptor */
    int s;

    void *backend_data;
    const  char *device;
    int baud;
    /* Data bit */
    uint8_t data_bit;
    /* Stop bit */
    uint8_t stop_bit;
    /* Parity: 'N', 'O', 'E' */
    char parity;	
   struct termios old_tios;	
}    serial_conf ;


int serial_connect(serial_conf *ctx);
void serial_close(serial_conf *ctx);

#ifdef __cplusplus
}
#endif

#endif  /*_SERIAL_CONF_H_*/



