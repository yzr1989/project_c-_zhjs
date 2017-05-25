#ifndef _DS_BUS_H_
#define  _DS_BUS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <termios.h>
#include <stdint.h>


#define DSBUS_DEBUG

 typedef    struct  _File_Record{
	uint16_t file_no;
	uint16_t register_address;
	uint16_t register_num;
	uint8_t   *data;	
}File_Record ,* File_Record_Ptr;

typedef  struct _ds_bus {
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
    const char *device;
    int baud;
    /* Data bit */
    uint8_t data_bit;
    /* Stop bit */
    uint8_t stop_bit;
    /* Parity: 'N', 'O', 'E' */
    char parity;	
   struct termios old_tios;	
}    ds_bus_t ,  *ds_bus_ptr;


#define DS_BUS_MAX_READ_BITS              2000
#define DS_BUS_MAX_WRITE_BITS             1968

#define DS_BUS_MAX_READ_REGISTERS          3000
#define DS_BUS_MAX_WRITE_REGISTERS         3000
#define DS_BUS_MAX_RW_WRITE_REGISTERS      3000


typedef enum
{
    DS_BUS_ERROR_RECOVERY_NONE          = 0,
    DS_BUS_ERROR_RECOVERY_LINK          = (1<<1),
    DS_BUS_ERROR_RECOVERY_PROTOCOL      = (1<<2),
} ds_bus_error_recovery_mode;


#define _RESPONSE_TIMEOUT    500000
#define _BYTE_TIMEOUT        500000


/* Max between RTU and TCP max adu length (so TCP) */
#define MAX_MESSAGE_LENGTH 	1000


/* Function codes */




#define _FC_READ_SEND_File_Record   104
#define _FC_READ_RESPONSE_File_Record   105
#define _FC_WRITE_SEND_File_Record   106
#define _FC_WRITE_RESPONSE_File_Record   107
#define _FC_IR_WRITE_SEND_File_Record   110

#define _DS_BUS_HEADER_LENGTH      1
#define _DS_BUS_PRESET_REQ_LENGTH  15
#define _DS_BUS_PRESET_RSP_LENGTH  2




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
    _STEP_FUNCTION,
    _STEP_FILE,		
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
#define DS_BUS_ENOBASE 112345678

/* Protocol exceptions */
enum {
    DS_BUS_EXCEPTION_ILLEGAL_FUNCTION = 0x01,
    DS_BUS_EXCEPTION_ILLEGAL_DATA_ADDRESS,
    DS_BUS_EXCEPTION_ILLEGAL_DATA_VALUE,
    DS_BUS_EXCEPTION_SLAVE_OR_SERVER_FAILURE,
    DS_BUS_EXCEPTION_ACKNOWLEDGE,
    DS_BUS_EXCEPTION_SLAVE_OR_SERVER_BUSY,
    DS_BUS_EXCEPTION_NEGATIVE_ACKNOWLEDGE,
    DS_BUS_EXCEPTION_MEMORY_PARITY,
    DS_BUS_EXCEPTION_NOT_DEFINED,
    DS_BUS_EXCEPTION_GATEWAY_PATH,
    DS_BUS_EXCEPTION_GATEWAY_TARGET,
    DS_BUS_EXCEPTION_MAX
};

#define EMBXILFUN  (DS_BUS_ENOBASE + DS_BUS_EXCEPTION_ILLEGAL_FUNCTION)
#define EMBXILADD  (DS_BUS_ENOBASE + DS_BUS_EXCEPTION_ILLEGAL_DATA_ADDRESS)
#define EMBXILVAL  (DS_BUS_ENOBASE + DS_BUS_EXCEPTION_ILLEGAL_DATA_VALUE)
#define EMBXSFAIL  (DS_BUS_ENOBASE + DS_BUS_EXCEPTION_SLAVE_OR_SERVER_FAILURE)
#define EMBXACK    (DS_BUS_ENOBASE + DS_BUS_EXCEPTION_ACKNOWLEDGE)
#define EMBXSBUSY  (DS_BUS_ENOBASE + DS_BUS_EXCEPTION_SLAVE_OR_SERVER_BUSY)
#define EMBXNACK   (DS_BUS_ENOBASE + DS_BUS_EXCEPTION_NEGATIVE_ACKNOWLEDGE)
#define EMBXMEMPAR (DS_BUS_ENOBASE + DS_BUS_EXCEPTION_MEMORY_PARITY)
#define EMBXGPATH  (DS_BUS_ENOBASE + DS_BUS_EXCEPTION_GATEWAY_PATH)
#define EMBXGTAR   (DS_BUS_ENOBASE + DS_BUS_EXCEPTION_GATEWAY_TARGET)

/* Native libmodbus error codes */
#define EMBBADCRC  (EMBXGTAR + 1)
#define EMBBADDATA (EMBXGTAR + 2)
#define EMBBADEXC  (EMBXGTAR + 3)
#define EMBUNKEXC  (EMBXGTAR + 4)
#define EMBMDATA   (EMBXGTAR + 5)


#define _MIN_REQ_LENGTH 256


#define    DS_BUS_MAX_ADU_LENGTH  256
#define 	_DS_BUS_HEADER_LENGTH      1

#define 	_DS_BUS_PRESET_RSP_LENGTH  2
#define   _DS_BUS_CHECKSUM_LENGTH    2

 void ds_bus_free(ds_bus_t *ctx);
void _error_print(ds_bus_t *ctx, const char *context);
int _ds_sleep_and_flush(ds_bus_t *ctx);
ds_bus_t* ds_bus_new(const  char *device,
                         int baud, char parity, int data_bit,
                         int stop_bit);

int ds_bus_set_slave(ds_bus_t *ctx, int slave);
int ds_bus_connect(ds_bus_t *ctx);

void ds_bus_close(ds_bus_t *ctx);
int ds_bus_read_registers(ds_bus_t *ctx, int addr,uint8_t *serial_no, int nb, uint8_t *dest);

void ds_bus_set_debug(ds_bus_t *ctx, int boolean);
int ds_bus_set_slave_For_File_Record(ds_bus_t *ctx, int slave);
int ds_bus_File_Record_write_send(ds_bus_t *ctx ,File_Record  *File_Record_array[] , int File_Record_num );

int ds_bus_File_Record_read_send(ds_bus_t *ctx ,File_Record  *File_Record_array[] , int File_Record_num );

void _ds_error_print(ds_bus_t *ctx, const char *context);
const char *ds_bus_strerror(int errnum);
int ds_bus_set_error_recovery(ds_bus_t *ctx,
                              ds_bus_error_recovery_mode error_recovery);

void ds_bus_get_response_timeout(ds_bus_t *ctx, struct timeval *timeout);
void ds_bus_set_response_timeout(ds_bus_t *ctx, const struct timeval *timeout);
void ds_bus_get_byte_timeout(ds_bus_t *ctx, struct timeval *timeout);
void ds_bus_set_byte_timeout(ds_bus_t *ctx, const struct timeval *timeout);

int ds_bus_File_Record_IR_write_send(ds_bus_t *ctx ,File_Record  *File_Record_array[] , int File_Record_num );


#ifdef __cplusplus
}
#endif

#endif   /*_DS_BUS_H_*/
