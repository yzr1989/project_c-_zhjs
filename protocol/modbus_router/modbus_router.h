#ifndef _modbus_router_H_
#define _modbus_router_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <termios.h>
#include <stdint.h>



typedef struct  _modbus_router_list{
	uint16_t  address;
	uint16_t function_no;
	uint16_t recvicer_address;
	uint16_t  router_num;
	uint16_t *router_list;
	uint16_t  router_length;
}modbus_router_list, *modbus_router_list_ptr;


typedef  struct _modbus_router {
    /* Slave address */
    int slave;
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
}    modbus_router_t ,  *modbus_router_ptr;




#define MODBUS_ROUTER_MAX_READ_REGISTERS          125
#define MODBUS_ROUTER_MAX_WRITE_REGISTERS         123
#define MODBUS_ROUTER_MAX_RW_WRITE_REGISTERS      121


typedef enum
{
    MODBUS_ROUTER_ERROR_RECOVERY_NONE          = 0,
    MODBUS_ROUTER_ERROR_RECOVERY_LINK          = (1<<1),
    MODBUS_ROUTER_ERROR_RECOVERY_PROTOCOL      = (1<<2),
} modbus_router_error_recovery_mode;


#define _RESPONSE_TIMEOUT    500000
#define _BYTE_TIMEOUT        500000


/* Max between RTU and TCP max adu length (so TCP) */
#define MAX_MESSAGE_LENGTH   1000




/* Function codes */
#define _FC_READ_COILS                0x01
#define _FC_READ_DISCRETE_INPUTS      0x02
#define _FC_READ_HOLDING_REGISTERS    0x03
#define _FC_READ_INPUT_REGISTERS      0x04
#define _FC_WRITE_SINGLE_COIL         0x05
#define _FC_WRITE_SINGLE_REGISTER     0x06
#define _FC_READ_EXCEPTION_STATUS     0x07
#define _FC_WRITE_MULTIPLE_COILS      0x0F
#define _FC_WRITE_MULTIPLE_REGISTERS  0x10
#define _FC_REPORT_SLAVE_ID           0x11
#define _FC_WRITE_AND_READ_REGISTERS  0x17





#define _MODBUS_ROUTER_HEADER_LENGTH      1
#define _MODBUS_ROUTER_PRESET_REQ_LENGTH  15
#define _MODBUS_ROUTER_PRESET_RSP_LENGTH  2




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
    _STEP_ROUTER_HEADER,
    _STEP_FUNCTION,	
    _STEP_META,
    _STEP_DATA,
    _STEP_CRC,
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
#define MODBUS_ROUTER_ENOBASE 112345678

/* Protocol exceptions */
enum {
    MODBUS_ROUTER_EXCEPTION_ILLEGAL_FUNCTION = 0X01,
    MODBUS_ROUTER_EXCEPTION_ILLEGAL_DATA_ADDRESS,
    MODBUS_ROUTER_EXCEPTION_ILLEGAL_DATA_VALUE,
    MODBUS_ROUTER_EXCEPTION_SLAVE_OR_SERVER_FAILURE,
    MODBUS_ROUTER_EXCEPTION_ACKNOWLEDGE,
    MODBUS_ROUTER_EXCEPTION_SLAVE_OR_SERVER_BUSY,
    MODBUS_ROUTER_EXCEPTION_NEGATIVE_ACKNOWLEDGE,
    MODBUS_ROUTER_EXCEPTION_MEMORY_PARITY,
    MODBUS_ROUTER_EXCEPTION_NOT_DEFINED,
    MODBUS_ROUTER_EXCEPTION_GATEWAY_PATH,
    MODBUS_ROUTER_EXCEPTION_GATEWAY_TARGET,
    MODBUS_ROUTER_EXCEPTION_MAX
};

#define EMBXILFUN  (MODBUS_ROUTER_ENOBASE + MODBUS_ROUTER_EXCEPTION_ILLEGAL_FUNCTION)
#define EMBXILADD  (MODBUS_ROUTER_ENOBASE + MODBUS_ROUTER_EXCEPTION_ILLEGAL_DATA_ADDRESS)
#define EMBXILVAL  (MODBUS_ROUTER_ENOBASE + MODBUS_ROUTER_EXCEPTION_ILLEGAL_DATA_VALUE)
#define EMBXSFAIL  (MODBUS_ROUTER_ENOBASE + MODBUS_ROUTER_EXCEPTION_SLAVE_OR_SERVER_FAILURE)
#define EMBXACK    (MODBUS_ROUTER_ENOBASE + MODBUS_ROUTER_EXCEPTION_ACKNOWLEDGE)
#define EMBXSBUSY  (MODBUS_ROUTER_ENOBASE + MODBUS_ROUTER_EXCEPTION_SLAVE_OR_SERVER_BUSY)
#define EMBXNACK   (MODBUS_ROUTER_ENOBASE + MODBUS_ROUTER_EXCEPTION_NEGATIVE_ACKNOWLEDGE)
#define EMBXMEMPAR (MODBUS_ROUTER_ENOBASE + MODBUS_ROUTER_EXCEPTION_MEMORY_PARITY)
#define EMBXGPATH  (MODBUS_ROUTER_ENOBASE + MODBUS_ROUTER_EXCEPTION_GATEWAY_PATH)
#define EMBXGTAR   (MODBUS_ROUTER_ENOBASE + MODBUS_ROUTER_EXCEPTION_GATEWAY_TARGET)

/* Native libmodbus error codes */
#define EMBBADCRC  (EMBXGTAR + 1)
#define EMBBADDATA (EMBXGTAR + 2)
#define EMBBADEXC  (EMBXGTAR + 3)
#define EMBUNKEXC  (EMBXGTAR + 4)
#define EMBMDATA   (EMBXGTAR + 5)


#define _MIN_REQ_LENGTH 256


#define    MODBUS_ROUTER_MAX_ADU_LENGTH  256



#define   _MODBUS_ROUTER_CHECKSUM_LENGTH    2

void modbus_router_free(modbus_router_t *ctx);

modbus_router_t* modbus_router_new(const  char *device,
                         int baud, char parity, int data_bit,
                         int stop_bit);

int modbus_router_set_slave(modbus_router_t *ctx, int slave);
int modbus_router_connect(modbus_router_t *ctx);
const char *modbus_router_strerror(int errnum) ;
void modbus_router_close(modbus_router_t *ctx);
int modbus_router_read_registers(modbus_router_t *ctx,int addr,int nb, uint16_t *dest,	modbus_router_list  *ptr_modbus_router_list);
void modbus_router_set_debug(modbus_router_t *ctx, int boolean);

int modbus_router_set_slave(modbus_router_t *ctx, int slave);

int modbus_router_set_error_recovery(modbus_router_t *ctx,
                              modbus_router_error_recovery_mode error_recovery);
int modbus_router_write_registers(modbus_router_t *ctx, int addr, int nb, const uint16_t *src,
				modbus_router_list  *ptr_modbus_router_list);

void modbus_router_set_byte_timeout(modbus_router_t *ctx, const struct timeval *timeout);
void modbus_router_set_response_timeout(modbus_router_t *ctx, const struct timeval *timeout);

#ifdef __cplusplus
}
#endif


#endif   /*_modbus_router_H_*/
