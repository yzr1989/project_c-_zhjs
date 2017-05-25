#ifndef _dlt645_H_
#define  _dlt645_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <termios.h>
#include <stdint.h>




typedef  struct _dlt645 {
    /* Slave address */
   // uint8_t  *slave;
    uint8_t  slave[6];
    /* Socket or file descriptor */
    int s;
    int debug;
    int error_recovery;
    uint8_t    serial_no[9];
    struct timeval response_timeout;
    struct timeval byte_timeout;
    unsigned int header_length;
    unsigned int checksum_length;
    unsigned int max_adu_length;
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
}    dlt645_t ,  *dlt645_ptr;


#define dlt645_MAX_READ_BITS              2000
#define dlt645_MAX_WRITE_BITS             1968

#define dlt645_MAX_READ_REGISTERS          125
#define dlt645_MAX_WRITE_REGISTERS         123
#define dlt645_MAX_RW_WRITE_REGISTERS      121


typedef enum
{
    dlt645_ERROR_RECOVERY_NONE          = 0,
    dlt645_ERROR_RECOVERY_LINK          = (1<<1),
    dlt645_ERROR_RECOVERY_PROTOCOL      = (1<<2),
} dlt645_error_recovery_mode;


#define _RESPONSE_TIMEOUT    500000
#define _BYTE_TIMEOUT        500000
//#define _RESPONSE_TIMEOUT    1

/* Max between RTU and TCP max adu length (so TCP) */
#define MAX_MESSAGE_LENGTH 260


/* Function codes */




#define _FC_READ_SEND_File_Record   104
#define _FC_READ_RESPONSE_File_Record   105
#define _FC_WRITE_SEND_File_Record   106
#define _FC_WRITE_RESPONSE_File_Record   107



#define _dlt645_PRESET_REQ_LENGTH  15
#define _dlt645_PRESET_RSP_LENGTH  2




/*
    ---------- Request     Indication ----------
    | Client | ---------------------->| Server |
    ---------- Confirmation  Response ----------
*/

typedef enum {
    /* Request message on the server side */
    MSG_INDICATION,
    /* Request message on the client side */
    MSG_CONFIRMATION
} msg_type_t;


/* 3 steps are used to parse the query */
typedef enum {  
    _STEP_DATA,
    _STEP_SUM,
     _STEP_OK
} _step_t;



#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef OFF
#define OFF 0
#endif

#ifndef ON
#define ON 1
#endif


/* Random number to avoid errno conflicts */
#define dlt645_ENOBASE 112345678

/* Protocol exceptions */
enum {
    dlt645_EXCEPTION_ILLEGAL_FUNCTION = 0x01,
    dlt645_EXCEPTION_ILLEGAL_DATA_ADDRESS,
    dlt645_EXCEPTION_ILLEGAL_DATA_VALUE,
    dlt645_EXCEPTION_SLAVE_OR_SERVER_FAILURE,
    dlt645_EXCEPTION_ACKNOWLEDGE,
    dlt645_EXCEPTION_SLAVE_OR_SERVER_BUSY,
    dlt645_EXCEPTION_NEGATIVE_ACKNOWLEDGE,
    dlt645_EXCEPTION_MEMORY_PARITY,
    dlt645_EXCEPTION_NOT_DEFINED,
    dlt645_EXCEPTION_GATEWAY_PATH,
    dlt645_EXCEPTION_GATEWAY_TARGET,
    dlt645_EXCEPTION_MAX
};

#define EMBXILFUN  (dlt645_ENOBASE + dlt645_EXCEPTION_ILLEGAL_FUNCTION)
#define EMBXILADD  (dlt645_ENOBASE + dlt645_EXCEPTION_ILLEGAL_DATA_ADDRESS)
#define EMBXILVAL  (dlt645_ENOBASE + dlt645_EXCEPTION_ILLEGAL_DATA_VALUE)
#define EMBXSFAIL  (dlt645_ENOBASE + dlt645_EXCEPTION_SLAVE_OR_SERVER_FAILURE)
#define EMBXACK    (dlt645_ENOBASE + dlt645_EXCEPTION_ACKNOWLEDGE)
#define EMBXSBUSY  (dlt645_ENOBASE + dlt645_EXCEPTION_SLAVE_OR_SERVER_BUSY)
#define EMBXNACK   (dlt645_ENOBASE + dlt645_EXCEPTION_NEGATIVE_ACKNOWLEDGE)
#define EMBXMEMPAR (dlt645_ENOBASE + dlt645_EXCEPTION_MEMORY_PARITY)
#define EMBXGPATH  (dlt645_ENOBASE + dlt645_EXCEPTION_GATEWAY_PATH)
#define EMBXGTAR   (dlt645_ENOBASE + dlt645_EXCEPTION_GATEWAY_TARGET)

/* Native libmodbus error codes */
#define EMBBADCRC  (EMBXGTAR + 1)
#define EMBBADDATA (EMBXGTAR + 2)
#define EMBBADEXC  (EMBXGTAR + 3)
#define EMBUNKEXC  (EMBXGTAR + 4)
#define EMBMDATA   (EMBXGTAR + 5)


#define _MIN_REQ_LENGTH 256


#define    dlt645_MAX_ADU_LENGTH  256
#define 	_dlt645_HEADER_LENGTH      11

#define 	_dlt645_PRESET_RSP_LENGTH  2
#define   _dlt645_CHECKSUM_LENGTH    2

 void dlt645_free(dlt645_t *ctx);
const char *dlt645_strerror(int errnum);
dlt645_t* dlt645_new(  const    char *device,
                         int baud, char parity, int data_bit,
                         int stop_bit);

int dlt645_set_slave(dlt645_t *ctx, uint8_t  *slave);

int dlt645_connect(dlt645_t *ctx);

void dlt645_close(dlt645_t *ctx);
int dlt645_read_registers(dlt645_t *ctx, int addr,uint8_t *serial_no, int nb, uint8_t *dest);

void dlt645_set_debug(dlt645_t *ctx, int boolean);
int dlt645_set_slave_For_File_Record(dlt645_t *ctx, int slave);


int dlt645_read_send(dlt645_t *ctx,int header_num  ,int addr,int addr_num,uint8_t *data );
int dlt645_relay_on(dlt645_t *ctx,int header_num,int relay );

void dlt645_get_response_timeout(dlt645_t *ctx, struct timeval *timeout);
void dlt645_set_response_timeout(dlt645_t *ctx, const struct timeval *timeout);
void dlt645_get_byte_timeout(dlt645_t *ctx, struct timeval *timeout);
void dlt645_set_byte_timeout(dlt645_t *ctx, const struct timeval *timeout);


#ifdef __cplusplus
}
#endif

#endif   /*_dlt645_H_*/
