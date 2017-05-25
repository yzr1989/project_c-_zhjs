#ifndef _ds_bus_router_H_
#define  _ds_bus_router_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <termios.h>
#include <stdint.h>

 typedef    struct  _File_Record{
	uint16_t file_no;
	uint16_t register_address;
	uint16_t register_num;
	uint8_t   *data;	
}File_Record ,* File_Record_Ptr;



typedef struct  _ds_router_list{
	uint16_t  address;
	uint16_t function_no;
	uint16_t recvicer_address;
	uint16_t  router_num;
	uint16_t *router_list;
	uint16_t  router_length;
}ds_router_list, *ds_router_list_ptr;



typedef  struct _ds_bus_router_router {
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
}    ds_bus_router_t ,  *ds_bus_router_ptr;




#define ds_bus_router_MAX_READ_REGISTERS          3000
#define ds_bus_router_MAX_WRITE_REGISTERS         3000
#define ds_bus_router_MAX_RW_WRITE_REGISTERS      3000


typedef enum
{
    DS_BUS_ROUTER_ERROR_RECOVERY_NONE          = 0,
    DS_BUS_ROUTER_ERROR_RECOVERY_LINK          = (1<<1),
    DS_BUS_ROUTER_ERROR_RECOVERY_PROTOCOL      = (1<<2),
} ds_bus_router_error_recovery_mode;


#define _RESPONSE_TIMEOUT    500000
#define _BYTE_TIMEOUT        500000


/* Max between RTU and TCP max adu length (so TCP) */
#define MAX_MESSAGE_LENGTH 1000


/* Function codes */




#define _FC_READ_SEND_File_Record   104
#define _FC_READ_RESPONSE_File_Record   105
#define _FC_WRITE_SEND_File_Record   106
#define _FC_WRITE_RESPONSE_File_Record   107

#define _FC_WRITE_SEND_IR_File_Record   110
#define _FC_WRITE_RESPONSE_IR_File_Record   111

#define _ds_bus_router_HEADER_LENGTH      1
#define _ds_bus_router_PRESET_REQ_LENGTH  15
#define _ds_bus_router_PRESET_RSP_LENGTH  2




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
#define ds_bus_router_ENOBASE 112345678

/* Protocol exceptions */
enum {
    ds_bus_router_EXCEPTION_ILLEGAL_FUNCTION = 0x01,
    ds_bus_router_EXCEPTION_ILLEGAL_DATA_ADDRESS,
    ds_bus_router_EXCEPTION_ILLEGAL_DATA_VALUE,
    ds_bus_router_EXCEPTION_SLAVE_OR_SERVER_FAILURE,
    ds_bus_router_EXCEPTION_ACKNOWLEDGE,
    ds_bus_router_EXCEPTION_SLAVE_OR_SERVER_BUSY,
    ds_bus_router_EXCEPTION_NEGATIVE_ACKNOWLEDGE,
    ds_bus_router_EXCEPTION_MEMORY_PARITY,
    ds_bus_router_EXCEPTION_NOT_DEFINED,
    ds_bus_router_EXCEPTION_GATEWAY_PATH,
    ds_bus_router_EXCEPTION_GATEWAY_TARGET,
    ds_bus_router_EXCEPTION_MAX
};

#define EMBXILFUN  (ds_bus_router_ENOBASE + ds_bus_router_EXCEPTION_ILLEGAL_FUNCTION)
#define EMBXILADD  (ds_bus_router_ENOBASE + ds_bus_router_EXCEPTION_ILLEGAL_DATA_ADDRESS)
#define EMBXILVAL  (ds_bus_router_ENOBASE + ds_bus_router_EXCEPTION_ILLEGAL_DATA_VALUE)
#define EMBXSFAIL  (ds_bus_router_ENOBASE + ds_bus_router_EXCEPTION_SLAVE_OR_SERVER_FAILURE)
#define EMBXACK    (ds_bus_router_ENOBASE + ds_bus_router_EXCEPTION_ACKNOWLEDGE)
#define EMBXSBUSY  (ds_bus_router_ENOBASE + ds_bus_router_EXCEPTION_SLAVE_OR_SERVER_BUSY)
#define EMBXNACK   (ds_bus_router_ENOBASE + ds_bus_router_EXCEPTION_NEGATIVE_ACKNOWLEDGE)
#define EMBXMEMPAR (ds_bus_router_ENOBASE + ds_bus_router_EXCEPTION_MEMORY_PARITY)
#define EMBXGPATH  (ds_bus_router_ENOBASE + ds_bus_router_EXCEPTION_GATEWAY_PATH)
#define EMBXGTAR   (ds_bus_router_ENOBASE + ds_bus_router_EXCEPTION_GATEWAY_TARGET)

/* Native libmodbus error codes */
#define EMBBADCRC  (EMBXGTAR + 1)
#define EMBBADDATA (EMBXGTAR + 2)
#define EMBBADEXC  (EMBXGTAR + 3)
#define EMBUNKEXC  (EMBXGTAR + 4)
#define EMBMDATA   (EMBXGTAR + 5)


#define _MIN_REQ_LENGTH 256


#define    ds_bus_router_MAX_ADU_LENGTH  256
#define 	_ds_bus_router_HEADER_LENGTH      1

#define 	_ds_bus_router_PRESET_RSP_LENGTH  2
#define   _ds_bus_router_CHECKSUM_LENGTH    2

 void ds_bus_router_free(ds_bus_router_t *ctx);
void _error_print(ds_bus_router_t *ctx, const char *context);
int _sleep_and_flush(ds_bus_router_t *ctx);
ds_bus_router_t* ds_bus_router_new(  const  char *device,
                         int baud, char parity, int data_bit,
                         int stop_bit);

int ds_bus_router_set_slave(ds_bus_router_t *ctx, int slave);
int ds_bus_router_connect(ds_bus_router_t *ctx);

void ds_bus_router_close(ds_bus_router_t *ctx);
int ds_bus_router_read_registers(ds_bus_router_t *ctx, int addr,uint8_t *serial_no, int nb, uint8_t *dest);

void ds_bus_router_set_debug(ds_bus_router_t *ctx, int boolean);
int ds_bus_router_set_slave_For_File_Record(ds_bus_router_t *ctx, int slave);
int ds_bus_router_File_Record_write_send(ds_bus_router_t *ctx ,ds_router_list  *ptr_ds_router_list,File_Record  *File_Record_array[] , int File_Record_num);


int ds_bus_router_File_Record_read_send(ds_bus_router_t *ctx ,ds_router_list  *ptr_ds_router_list,File_Record  *File_Record_array[] , int File_Record_num  );


void ds_bus_router_set_response_timeout(ds_bus_router_t *ctx, const struct timeval *timeout);

void ds_bus_router_get_byte_timeout(ds_bus_router_t *ctx, struct timeval *timeout);

void ds_bus_router_set_byte_timeout(ds_bus_router_t *ctx, const struct timeval *timeout);

int ds_bus_router_set_error_recovery(ds_bus_router_t *ctx,
                              ds_bus_router_error_recovery_mode error_recovery);

const char *ds_bus_router_strerror(int errnum);

int ds_bus_router_File_Record_IR_write_send(ds_bus_router_t *ctx ,ds_router_list  *ptr_ds_router_list,File_Record  *File_Record_array[] , int File_Record_num);


#ifdef __cplusplus
}
#endif

#endif   /*_ds_bus_router_H_*/
