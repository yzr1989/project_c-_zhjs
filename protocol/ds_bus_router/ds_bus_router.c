#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
	   
#include "ds_bus_router.h"


static uint16_t crc16(uint8_t *buffer, uint16_t buffer_length);

/* Internal use */
#define MSG_LENGTH_UNDEFINED -1

#define add_len_fileNo_header   3

/* Table of CRC values for high-order byte */
static const uint8_t table_crc_hi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/* Table of CRC values for low-order byte */
static const uint8_t table_crc_lo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

ds_bus_router_t* ds_bus_router_new( const char *device,
                         int baud, char parity, int data_bit,
                         int stop_bit)
{
	ds_bus_router_t *ctx;
	ctx = malloc(sizeof(ds_bus_router_t));
	if (ctx == NULL){
		printf("malloc ds_bus_router_t error\n");
		return NULL;
	}


	ctx->slave = -1;
	ctx->s = -1;

	ctx->debug = FALSE;
	ctx->error_recovery = DS_BUS_ROUTER_ERROR_RECOVERY_NONE;

	ctx->response_timeout.tv_sec = 0;
	ctx->response_timeout.tv_usec = _RESPONSE_TIMEOUT;

	ctx->byte_timeout.tv_sec = 0;
	ctx->byte_timeout.tv_usec = _BYTE_TIMEOUT;
	

	if (strlen(device) == 0){
		printf("The device string is empty\n");
		return NULL;
	}
	
	ctx->device  = device;
	 ctx->baud = baud;
	 
     if (parity == 'N' || parity == 'E' || parity == 'O') {
        ctx->parity = parity;
     } else {
     
        errno = EINVAL;
        return NULL;
     }	

	ctx->data_bit = data_bit;
	ctx->stop_bit = stop_bit;


	ctx->header_length=_ds_bus_router_HEADER_LENGTH;
	ctx->checksum_length=_ds_bus_router_CHECKSUM_LENGTH;
	ctx->max_adu_length=ds_bus_router_MAX_ADU_LENGTH;

	 return ctx;
	 
}



int ds_bus_router_set_slave(ds_bus_router_t *ctx, int slave)
{
        /* Broadcast address is 0 (MODBUS_BROADCAST_ADDRESS) */
    if (slave >= 0 && slave <= 247) {
        ctx->slave = slave;
    }  else {
        errno = EINVAL;
        return -1;
    }

    return 0;
}

int ds_bus_router_set_slave_For_File_Record(ds_bus_router_t *ctx, int slave)
{
        /* Broadcast address is 0 (MODBUS_BROADCAST_ADDRESS) */
 
        ctx->slave = slave;


    return 0;
}


#if 0

int ds_bus_router_set_serial_no(ds_bus_router_t *ctx, uint8_t *serial_no)
{
        /* Broadcast address is 0 (MODBUS_BROADCAST_ADDRESS) */

        memcpy(ctx->serial_no,serial_no,sizeof(serial_no));


    return 0;
}


#endif 


int ds_bus_router_connect(ds_bus_router_t *ctx)
{
	struct termios tios;
	speed_t speed;
	 
	if (ctx->debug) {
		printf("Opening %s at %d bauds (%c, %d, %d)\n",
		   ctx->device, ctx->baud, ctx->parity,
		   ctx->data_bit, ctx->stop_bit);
	}


 /* The O_NOCTTY flag tells UNIX that this program doesn't want
       to be the "controlling terminal" for that port. If you
       don't specify this then any input (such as keyboard abort
       signals and so forth) will affect your process

       Timeouts are ignored in canonical input mode or when the
       NDELAY option is set on the file via open or fcntl */
    ctx->s = open(ctx->device, O_RDWR | O_NOCTTY | O_NDELAY | O_EXCL);
    if (ctx->s == -1) {
        fprintf(stderr, "ERROR Can't open the device %s (%s)\n",
                ctx->device, strerror(errno));
        return -1;
    }

    /* Save */
    tcgetattr(ctx->s, &(ctx->old_tios));

    memset(&tios, 0, sizeof(struct termios));

  /* C_ISPEED     Input baud (new interface)
       C_OSPEED     Output baud (new interface)
    */
    switch (ctx->baud) {
    case 110:
        speed = B110;
        break;
    case 300:
        speed = B300;
        break;
    case 600:
        speed = B600;
        break;
    case 1200:
        speed = B1200;
        break;
    case 2400:
        speed = B2400;
        break;
    case 4800:
        speed = B4800;
        break;
    case 9600:
        speed = B9600;
        break;
    case 19200:
        speed = B19200;
        break;
    case 38400:
        speed = B38400;
        break;
    case 57600:
        speed = B57600;
        break;
    case 115200:
        speed = B115200;
        break;
    default:
        speed = B9600;
        if (ctx->debug) {
            fprintf(stderr,
                    "WARNING Unknown baud rate %d for %s (B9600 used)\n",
                    ctx->baud, ctx->device);
        }
    }
	


    /* Set the baud rate */
    if ((cfsetispeed(&tios, speed) < 0) ||
        (cfsetospeed(&tios, speed) < 0)) {
        close(ctx->s);
        ctx->s = -1;
        return -1;
    }

    /* C_CFLAG      Control options
       CLOCAL       Local line - do not change "owner" of port
       CREAD        Enable receiver
    */
    tios.c_cflag |= (CREAD | CLOCAL);
    /* CSIZE, HUPCL, CRTSCTS (hardware flow control) */

    /* Set data bits (5, 6, 7, 8 bits)
       CSIZE        Bit mask for data bits
    */
    tios.c_cflag &= ~CSIZE;
    switch (ctx->data_bit) {
    case 5:
        tios.c_cflag |= CS5;
        break;
    case 6:
        tios.c_cflag |= CS6;
        break;
    case 7:
        tios.c_cflag |= CS7;
        break;
    case 8:
    default:
        tios.c_cflag |= CS8;
        break;
    }

    /* Stop bit (1 or 2) */
    if (ctx->stop_bit == 1)
        tios.c_cflag &=~ CSTOPB;
    else /* 2 */
        tios.c_cflag |= CSTOPB;

    /* PARENB       Enable parity bit
       PARODD       Use odd parity instead of even */
    if (ctx->parity == 'N') {
        /* None */
        tios.c_cflag &=~ PARENB;
    } else if (ctx->parity == 'E') {
        /* Even */
        tios.c_cflag |= PARENB;
        tios.c_cflag &=~ PARODD;
    } else {
        /* Odd */
        tios.c_cflag |= PARENB;
        tios.c_cflag |= PARODD;
    }

    /* Read the man page of termios if you need more information. */

    /* This field isn't used on POSIX systems
       tios.c_line = 0;
    */

    /* C_LFLAG      Line options

       ISIG Enable SIGINTR, SIGSUSP, SIGDSUSP, and SIGQUIT signals
       ICANON       Enable canonical input (else raw)
       XCASE        Map uppercase \lowercase (obsolete)
       ECHO Enable echoing of input characters
       ECHOE        Echo erase character as BS-SP-BS
       ECHOK        Echo NL after kill character
       ECHONL       Echo NL
       NOFLSH       Disable flushing of input buffers after
       interrupt or quit characters
       IEXTEN       Enable extended functions
       ECHOCTL      Echo control characters as ^char and delete as ~?
       ECHOPRT      Echo erased character as character erased
       ECHOKE       BS-SP-BS entire line on line kill
       FLUSHO       Output being flushed
       PENDIN       Retype pending input at next read or input char
       TOSTOP       Send SIGTTOU for background output

       Canonical input is line-oriented. Input characters are put
       into a buffer which can be edited interactively by the user
       until a CR (carriage return) or LF (line feed) character is
       received.

       Raw input is unprocessed. Input characters are passed
       through exactly as they are received, when they are
       received. Generally you'll deselect the ICANON, ECHO,
       ECHOE, and ISIG options when using raw input
    */

    /* Raw input */
    tios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    /* C_IFLAG      Input options

       Constant     Description
       INPCK        Enable parity check
       IGNPAR       Ignore parity errors
       PARMRK       Mark parity errors
       ISTRIP       Strip parity bits
       IXON Enable software flow control (outgoing)
       IXOFF        Enable software flow control (incoming)
       IXANY        Allow any character to start flow again
       IGNBRK       Ignore break condition
       BRKINT       Send a SIGINT when a break condition is detected
       INLCR        Map NL to CR
       IGNCR        Ignore CR
       ICRNL        Map CR to NL
       IUCLC        Map uppercase to lowercase
       IMAXBEL      Echo BEL on input line too long
    */
    if (ctx->parity == 'N') {
        /* None */
        tios.c_iflag &= ~INPCK;
    } else {
        tios.c_iflag |= INPCK;
    }

    /* Software flow control is disabled */
    tios.c_iflag &= ~(IXON | IXOFF | IXANY);

    /* C_OFLAG      Output options
       OPOST        Postprocess output (not set = raw output)
       ONLCR        Map NL to CR-NL

       ONCLR ant others needs OPOST to be enabled
    */

    /* Raw ouput */
    tios.c_oflag &=~ OPOST;

    /* C_CC         Control characters
       VMIN         Minimum number of characters to read
       VTIME        Time to wait for data (tenths of seconds)

       UNIX serial interface drivers provide the ability to
       specify character and packet timeouts. Two elements of the
       c_cc array are used for timeouts: VMIN and VTIME. Timeouts
       are ignored in canonical input mode or when the NDELAY
       option is set on the file via open or fcntl.

       VMIN specifies the minimum number of characters to read. If
       it is set to 0, then the VTIME value specifies the time to
       wait for every character read. Note that this does not mean
       that a read call for N bytes will wait for N characters to
       come in. Rather, the timeout will apply to the first
       character and the read call will return the number of
       characters immediately available (up to the number you
       request).

       If VMIN is non-zero, VTIME specifies the time to wait for
       the first character read. If a character is read within the
       time given, any read will block (wait) until all VMIN
       characters are read. That is, once the first character is
       read, the serial interface driver expects to receive an
       entire packet of characters (VMIN bytes total). If no
       character is read within the time allowed, then the call to
       read returns 0. This method allows you to tell the serial
       driver you need exactly N bytes and any read call will
       return 0 or N bytes. However, the timeout only applies to
       the first character read, so if for some reason the driver
       misses one character inside the N byte packet then the read
       call could block forever waiting for additional input
       characters.

       VTIME specifies the amount of time to wait for incoming
       characters in tenths of seconds. If VTIME is set to 0 (the
       default), reads will block (wait) indefinitely unless the
       NDELAY option is set on the port with open or fcntl.
    */
    /* Unused because we use open with the NDELAY option */
    tios.c_cc[VMIN] = 0;
    tios.c_cc[VTIME] = 0;

    if (tcsetattr(ctx->s, TCSANOW, &tios) < 0) {
        close(ctx->s);
        ctx->s = -1;
        return -1;
    }

return 0;
 
}

/* Builds a RTU request header */
static int _ds_bus_router_build_request_basis_File_Record(ds_bus_router_t *ctx, int function,uint8_t *req,
							File_Record *File_Record_array[], int File_Record_num,ds_router_list  *ptr_ds_router_list,
								int *router_header_num)
{

   int  i =0;
   int  num =0;

  int  router_num = ptr_ds_router_list->router_num;

   req[num++] = ptr_ds_router_list->address;
   req[num++] = ptr_ds_router_list->function_no;
   req[num++] = ptr_ds_router_list->recvicer_address;
   req[num++] = ptr_ds_router_list->router_num;

 for (i=0;i<router_num;i++){
	 req[num++] =  ptr_ds_router_list->router_list[i];
 }
    printf("num=%d\n",num);
   *router_header_num = num;
    req[num++] = 0xfb;
    req[num++] = function;
 //   req[num++] = 2;
   req[num++] = 1; 
    req[num++] = ctx->slave ;
   // req[num++] = 0x00;
	
   // num= 5;
   printf("num=%d\n",num);

/*
	for (int i=0;i<File_Record_num;i++){
		printf("File_Record_array[%d]->file_no=%d\n",i,File_Record_array[i]->file_no);
		printf("File_Record_array[%d]->register_address=%d\n",i,File_Record_array[i]->register_address);
		printf("File_Record_array[%d]->register_num=%d\n",i,File_Record_array[i]->register_num);
	}  	
	
*/


	
	for (i = 0 ;i<File_Record_num;i++){
		req[num++] = 6;
		req[num++] = File_Record_array[i]->file_no& 0x00ff;     //11
		req[num++] = File_Record_array[i]->file_no >> 8;	   //12

		req[num++] = File_Record_array[i]->register_address & 0x00ff;   //13
		req[num++] = File_Record_array[i]->register_address  >> 8;    //14

		req[num++] = File_Record_array[i]->register_num & 0x00ff;   //15
		req[num++] = File_Record_array[i]->register_num  >> 8;      //16
		
	}  

    return num;
}


/* Builds a RTU request header */
static int _ds_bus_router_build_request_basis_File_Record_write_send(ds_bus_router_t *ctx, int function,uint8_t *req,
							File_Record *File_Record_array[] , int File_Record_num,ds_router_list  *ptr_ds_router_list,
								int *router_header_num)
{ 
   int  num =0;
   int  i =0;
    int  router_num = ptr_ds_router_list->router_num;

   req[num++] = ptr_ds_router_list->address;
   req[num++] = ptr_ds_router_list->function_no;
   req[num++] = ptr_ds_router_list->recvicer_address;
   req[num++] = ptr_ds_router_list->router_num;	

 for (i=0;i<router_num;i++){
	 req[num++] =  ptr_ds_router_list->router_list[i];
 }
   
   *router_header_num = num;
   
    printf("num=%d\n",num);
	
    req[num++] = 0xfb;
    req[num++] = function;
  //  req[num++] = 2;
    req[num++] = 1;	
    req[num++] = ctx->slave ;
//    req[num++] = 0xff;
	
     printf("num=%d\n",num);

    return num;
}




int _ds_bus_router_send_msg_pre(uint8_t *req, int req_length)
{
    uint16_t crc = crc16(req, req_length);
    req[req_length++] = crc >> 8;	
    req[req_length++] = crc & 0x00FF;
	
    return req_length;
}


int _ds_bus_router_send_msg_prefix(uint8_t *req, int req_length,int router_header_num)
{

  	

    uint16_t crc = crc16(req+router_header_num, req_length-router_header_num);
    printf("crc=0x%x\n",crc);
    req[req_length++] = crc >> 8;	
    req[req_length++] = crc & 0x00FF;

   	

   crc = crc16(req, req_length);	
       printf("crc=0x%x\n",crc);
    req[req_length++] = crc >> 8;	
    req[req_length++] = crc & 0x00FF; 
	
    return req_length;
}



ssize_t _ds_bus_router_send(ds_bus_router_t *ctx, const uint8_t *req, int req_length)
{

    return write(ctx->s, req, req_length);

}


void ds_bus_router_close(ds_bus_router_t *ctx)
{
    /* Closes the file descriptor in RTU mode */
    tcsetattr(ctx->s, TCSANOW, &(ctx->old_tios));
    close(ctx->s);

}

void ds_bus_router_free(ds_bus_router_t *ctx)
{
    if (ctx == NULL)
        return;

   
    free(ctx);
}
	

/* Sends a request/response */
static int send_msg(ds_bus_router_t *ctx, uint8_t *msg, int msg_length,int router_header_num)
{
    int rc;
    int i;

    msg_length = _ds_bus_router_send_msg_prefix(msg, msg_length,router_header_num);

    if (ctx->debug) {
        for (i = 0; i < msg_length; i++)
            printf("[%.2X]", msg[i]);
        printf("\n");
    }

    /* In recovery mode, the write command will be issued until to be
       successful! Disabled by default. */
    do {
        rc = _ds_bus_router_send(ctx, msg, msg_length);
        if (rc == -1) {
            _error_print(ctx, NULL);
            if (ctx->error_recovery & DS_BUS_ROUTER_ERROR_RECOVERY_LINK) {
                int saved_errno = errno;

                if ((errno == EBADF || errno == ECONNRESET || errno == EPIPE)) {
                    ds_bus_router_close(ctx);
                    ds_bus_router_connect(ctx);
                } else {
                    _sleep_and_flush(ctx);
                }
                errno = saved_errno;
            }
        }
    } while ((ctx->error_recovery & DS_BUS_ROUTER_ERROR_RECOVERY_LINK) &&
             rc == -1);

    if (rc > 0 && rc != msg_length) {
        errno = EMBBADDATA;
        return -1;
    }

    return rc;
}

int _ds_bus_router_select(ds_bus_router_t *ctx, fd_set *rfds,
                       struct timeval *tv, int length_to_read)
{
    int s_rc;

    while ((s_rc = select(ctx->s+1, rfds, NULL, NULL, tv)) == -1) {
        if (errno == EINTR) {
            if (ctx->debug) {
                fprintf(stderr, "A non blocked signal was caught\n");
            }
            /* Necessary after an error */
            FD_ZERO(rfds);
            FD_SET(ctx->s, rfds);
        } else {
            return -1;
        }
    }

    if (s_rc == 0) {
        /* Timeout */
        errno = ETIMEDOUT;
        return -1;
    }


    return s_rc;
}




int _ds_bus_router_flush(ds_bus_router_t *ctx)
{

    return tcflush(ctx->s, TCIOFLUSH);

}


/* Computes the length to read after the function received */
static uint8_t compute_file_length_after_function(int function,
                                                  msg_type_t msg_type)
{
    int length;
	printf("function=0x%0x\n",function);

	if (function&0x80){
		printf("error  happened \n");
		return -1;
	}

	
    if (msg_type == MSG_INDICATION) {
      
            /* _FC_READ_EXCEPTION_STATUS, _FC_REPORT_SLAVE_ID */
            length = 0;
        
    } else {
        /* MSG_CONFIRMATION */
        switch (function) {	
	case _FC_READ_RESPONSE_File_Record:	
	     length = 2;
		break;
	case _FC_WRITE_RESPONSE_File_Record:	
	     length = 2;
		break;
	case _FC_WRITE_RESPONSE_IR_File_Record:	
	     length = 2;	
		 break;
        default:
            length = 1;
		break;
        }
    }




    return length;
}




/* Computes the length to read after the meta information (address, count, etc) */
static int compute_data_length_after_file(ds_bus_router_t *ctx, uint8_t *msg,
                                          msg_type_t msg_type,int file_order, int compute_data_length_after_file_sum)
{
    int function = msg[ctx->header_length];
    int  offset= ctx->header_length +3;
    int length;

    printf("function=0x%x\n",function);
	printf("offset=%d\n",offset);
    if (msg_type == MSG_INDICATION) {
        switch (function) {
       
        default:
            length = 0;
        }
    } else {
        /* MSG_CONFIRMATION */
     if( function == _FC_READ_RESPONSE_File_Record) {

		
	printf("file_order=%d\n,sum=%d\n",file_order,compute_data_length_after_file_sum);
	printf("msg[%d]=0x%x\n",offset + 7*file_order+5+compute_data_length_after_file_sum,msg[offset + 7*file_order+5+compute_data_length_after_file_sum]);	  //6
	printf("msg[%d]=0x%x\n",offset + 7*file_order+6+compute_data_length_after_file_sum,msg[offset + 7*file_order+6+compute_data_length_after_file_sum]);    //7

		 
	   length = (msg[offset + 7*file_order+6+compute_data_length_after_file_sum] << 8 | msg[offset + 7*file_order+5+compute_data_length_after_file_sum]);	
	   compute_data_length_after_file_sum = compute_data_length_after_file_sum+length;
	     printf("length=%d\n",length);	
		   printf("sum=%d\n",compute_data_length_after_file_sum);	
        }  else if( function == _FC_WRITE_RESPONSE_File_Record) {
	   length = 2;
	     printf("length=%d\n",length);		 
        }	else  if (function == _FC_WRITE_RESPONSE_IR_File_Record) {
        //  跳转包的crc不需要接受
	    length = 2;
		 printf("length=%d\n",length);	
	   }else {
	    length = 0;
	   }
    }
   printf("length=%d\n",length);	

    return length;
}



/* Computes the length to read after the meta information (address, count, etc) */
static int compute_meta_length_after_file(ds_bus_router_t *ctx, uint8_t *msg,
                                          msg_type_t msg_type)
{
    int function = msg[ctx->header_length];
    int length;
    printf("function=0x%x\n",function);
    if (msg_type == MSG_INDICATION) {
        switch (function) {
        
        default:
            length = 0;
        }
    } else {
        /* MSG_CONFIRMATION */
       if( function == _FC_READ_RESPONSE_File_Record) {
	    length = 7;	
	     printf("length=%d\n",length);		 
        }  else if( function == _FC_WRITE_RESPONSE_File_Record) {
	   length = 7;
	     printf("length=%d\n",length);		 
        }	else if( function == _FC_WRITE_RESPONSE_IR_File_Record) {
	   length = 7;
	     printf("length=%d\n",length);		 
        }	else {
	    length = 0;
	   }
    }
   printf("length=%d\n",length);	

    return length;
}


ssize_t _ds_bus_router_recv(ds_bus_router_t *ctx, uint8_t *rsp, int rsp_length)
{

    return read(ctx->s, rsp, rsp_length);

}


 static uint16_t crc16(uint8_t *buffer, uint16_t buffer_length)
{
    uint8_t crc_hi = 0xFF; /* high CRC byte initialized */
    uint8_t crc_lo = 0xFF; /* low CRC byte initialized */
    unsigned int i; /* will index into CRC lookup */

    /* pass through message buffer */
    while (buffer_length--) {
        i = crc_hi ^ *buffer++; /* calculate the CRC  */
        crc_hi = crc_lo ^ table_crc_hi[i];
        crc_lo = table_crc_lo[i];
    }

    return (crc_hi << 8 | crc_lo);
}

/* The check_crc16 function shall return the message length if the CRC is
   valid. Otherwise it shall return -1 and set errno to EMBADCRC. */
int _ds_bus_router_rtu_check_integrity(ds_bus_router_t *ctx, uint8_t *msg,
                                const int msg_length)
{
    uint16_t crc_calculated;
    uint16_t crc_received;

    crc_calculated = crc16(msg, msg_length - 2);
    crc_received = (msg[msg_length - 2] << 8) | msg[msg_length - 1];

    /* Check CRC of msg */
    if (crc_calculated == crc_received) {
        return msg_length;
    } else {
        if (ctx->debug) {
            fprintf(stderr, "ERROR CRC received %0X != CRC calculated %0X\n",
                    crc_received, crc_calculated);
        }
        if (ctx->error_recovery & DS_BUS_ROUTER_ERROR_RECOVERY_PROTOCOL) {
            _ds_bus_router_flush(ctx);
        }
        errno = EMBBADCRC;
        return -1;
    }
}



/* Waits a response from a modbus server or a request from a modbus client.
   This function blocks if there is no replies (3 timeouts).

   The function shall return the number of received characters and the received
   message in an array of uint8_t if successful. Otherwise it shall return -1
   and errno is set to one of the values defined below:
   - ECONNRESET
   - EMBBADDATA
   - EMBUNKEXC
   - ETIMEDOUT
   - read() or recv() error codes
*/

static int receive_msg(ds_bus_router_t *ctx, uint8_t *msg, msg_type_t msg_type, int File_Record_num,int req_length, int router_header_skip_num)
{
    int rc;
    fd_set rfds;
    struct timeval tv;
    struct timeval *p_tv;
    int length_to_read;
    int msg_length = 0;
    _step_t step;
    int file_order =0;
    int flag_read_is_ok = 0;
    int j =0;
   int compute_data_length_after_file_sum =0;
    if (ctx->debug) {
        if (msg_type == MSG_INDICATION) {
            printf("Waiting for a indication...\n");
        } else {
            printf("Waiting for a confirmation...\n");
        }
    }
    
    /* Add a file descriptor to the set */
    FD_ZERO(&rfds);
    FD_SET(ctx->s, &rfds);

    /* We need to analyse the message step by step.  At the first step, we want
     * to reach the function code because all packets contain this
     * information. */
//    step = _STEP_FUNCTION;
    step = _STEP_ROUTER_HEADER;
   // length_to_read = ctx->header_length + 1;
   length_to_read = router_header_skip_num;
    printf("length_to_read=%d\n",length_to_read );	
    if (msg_type == MSG_INDICATION) {
        /* Wait for a message, we don't know when the message will be
         * received */
        p_tv = NULL;
    } else {
        tv.tv_sec = ctx->response_timeout.tv_sec;
        tv.tv_usec = ctx->response_timeout.tv_usec;
        p_tv = &tv;
    }
 printf("length_to_read=%d\n",length_to_read );	
    while ( (length_to_read != 0) || (!flag_read_is_ok)) {
	if (length_to_read != 0){
		
        rc = _ds_bus_router_select(ctx, &rfds, p_tv, length_to_read);
		
        if (rc == -1) {
            _error_print(ctx, "select");
            if (ctx->error_recovery & DS_BUS_ROUTER_ERROR_RECOVERY_LINK) {
                int saved_errno = errno;

                if (errno == ETIMEDOUT) {
                    _sleep_and_flush(ctx);
                } else if (errno == EBADF) {
                    ds_bus_router_close(ctx);
                   ds_bus_router_connect(ctx);
                }
                errno = saved_errno;
            }
            return -1;
        }
		
	 printf("num %d length_to_read=%d\n",j++,length_to_read );	
        rc = _ds_bus_router_recv(ctx, msg + msg_length, length_to_read);
        if (rc == 0) {
            errno = ECONNRESET;
            rc = -1;
        }

        if (rc == -1) {
            _error_print(ctx, "read");
            if ((ctx->error_recovery & DS_BUS_ROUTER_ERROR_RECOVERY_LINK) &&
                (errno == ECONNRESET || errno == ECONNREFUSED ||
                 errno == EBADF)) {
                int saved_errno = errno;
                ds_bus_router_close(ctx);
                ds_bus_router_connect(ctx);
                /* Could be removed by previous calls */
                errno = saved_errno;
            }
            return -1;
        }

	 printf("rc=%d\n",rc );
	
        /* Display the hex code of each character received */
        if (ctx->debug) {
            int i;
            for (i=0; i < rc; i++)
                printf("<%.2X>", msg[msg_length + i]);
        }
	puts("\n");
        /* Sums bytes received */
        msg_length += rc;
        /* Computes remaining bytes */
        length_to_read -= rc;
	}
	 printf("length_to_read=%d\n",length_to_read );
        if (length_to_read == 0) {	

	printf("function=0x%0x\n",msg[ctx->header_length]);
	if (msg[ctx->header_length]&0x80){
		printf("error  happened \n");
		return -1;
	}	
	
            switch (step) {

	  case   _STEP_ROUTER_HEADER	:
	  	
		length_to_read = ctx->header_length + 1;
		 printf("step=%d,length_to_read=%d\n",_STEP_ROUTER_HEADER,length_to_read);
    		step = _STEP_FUNCTION;
		msg_length = 0;
		break;	
				
            case _STEP_FUNCTION:
                /* Function code position */
		 printf("step=%d\n",_STEP_FUNCTION);		
                length_to_read = compute_file_length_after_function(
                    msg[ctx->header_length],
                    msg_type);					
                if (length_to_read != 0) {
                    step = _STEP_FILE;
                    break;
                } 
	      case 	_STEP_FILE:
		  	 printf("step=%d\n",_STEP_FILE);
	   	      length_to_read = compute_meta_length_after_file(
                    ctx, msg, msg_type);		     
			step = _STEP_META;
		
	   	break;
            case _STEP_META:
		printf("step=%d\n",_STEP_META);
                length_to_read = compute_data_length_after_file(
                    ctx, msg, msg_type,file_order,compute_data_length_after_file_sum);
                if ((msg_length + length_to_read) > ctx->max_adu_length) {
                    errno = EMBBADDATA;
                    _error_print(ctx, "too many data");
                    return -1;
                }
                step = _STEP_DATA;
                break;
	      case _STEP_DATA:	
		  printf("step=%d\n",_STEP_DATA);	
		  	if (++file_order <File_Record_num){			
			    printf("file_order=%d\n",file_order);
			     
			      step = _STEP_FILE;
			}else  {
			    step = _STEP_CRC;
			}
		break; 
		case _STEP_CRC:
		 printf("step=%d\n",_STEP_CRC)	;
		if (msg[ctx->header_length] ==_FC_WRITE_RESPONSE_IR_File_Record  ){
			length_to_read =4;
		}else {
			length_to_read =2;
		}		
		
		flag_read_is_ok = 1;
		step = _STEP_OK;
		break;
            default:
                break;
            }

        }

        if (length_to_read > 0 && ctx->byte_timeout.tv_sec != -1) {
            /* If there is no character in the buffer, the allowed timeout
               interval between two consecutive bytes is defined by
               byte_timeout */
            tv.tv_sec = ctx->byte_timeout.tv_sec;
            tv.tv_usec = ctx->byte_timeout.tv_usec;
            p_tv = &tv;
        }
    }

    if (ctx->debug)
        printf("\n");

    return _ds_bus_router_rtu_check_integrity(ctx, msg, msg_length);
}

int _sleep_and_flush(ds_bus_router_t *ctx)
{

    /* usleep source code */
    struct timespec request, remaining;
    request.tv_sec = ctx->response_timeout.tv_sec;
    request.tv_nsec = ((long int)ctx->response_timeout.tv_usec % 1000000)
        * 1000;
    while (nanosleep(&request, &remaining) == -1 && errno == EINTR)
        request = remaining;

    return _ds_bus_router_flush(ctx);
}


/* Computes the length of the expected response */
static unsigned int compute_response_length_from_request(ds_bus_router_t *ctx, uint8_t *req,int File_Record_num,
			int router_header_num)
{
    int length = 0;
    const int offset =router_header_num+ ctx->header_length+add_len_fileNo_header;
  int func = ctx->header_length+router_header_num;
   printf("req[func]=0x%x\n",req[func]);
   int temp =0;
   int i =0;
    switch (req[func]) {

case _FC_READ_SEND_File_Record:{
	for ( i =0 ; i< File_Record_num ;i++){

	printf("req[%d]=0x%x\n",offset + 7*i+5,req[offset + 7*i+5]);	  //6
	printf("req[%d]=0x%x\n",offset + 7*i+6,req[offset + 7*i+6]);    //7


   	temp = (req[offset + 7*i+6] << 8 | req[offset + 7*i+5]);
	length = length + temp;
	printf("length=%d\n",length);	
	}
	length = 7*File_Record_num+length;
}
	break;	
case _FC_WRITE_SEND_File_Record:{
	//#warning  "fixup me after change the data length " 
   	length = 9*File_Record_num;
	
	printf("length=%d\n",length);
	}
	break;		
case  _FC_WRITE_SEND_IR_File_Record:{
	//#warning  "fixup me after change the data length " 	
   	length = 9*File_Record_num;
	
	printf("length=%d\n",length);	
	}
	break;	
    default:
        length =0;
    }

	if (req[func] == _FC_WRITE_SEND_IR_File_Record){	
	    return  length + ctx->checksum_length+offset-router_header_num+2;
	}else {
	    return  length + ctx->checksum_length+offset-router_header_num;
	}
}



static int check_confirmation(ds_bus_router_t *ctx, uint8_t *req,
                              uint8_t *rsp, int rsp_length,int File_Record_num,int router_header_num)
{
    int rc;
    int rsp_length_computed;
    const int offset = ctx->header_length;

   printf("rsp_length=%d\n",rsp_length);
	for ( int i=0;i<rsp_length;i++){
		printf("0x%x ",rsp[i]);
	}
	puts("\n");


    rsp_length_computed = compute_response_length_from_request(ctx, req,File_Record_num,router_header_num);

   printf("rsp_length_computed=%x\n",rsp_length_computed);
	
    /* Check length */
    if (rsp_length == rsp_length_computed ||
        rsp_length_computed == MSG_LENGTH_UNDEFINED) {
        int req_nb_value;
        int rsp_nb_value;
        const int function = rsp[offset];

        /* Check function code */
        if (function != req[offset+router_header_num]+1) {
            if (ctx->debug) {
                fprintf(stderr,
                        "Received function not corresponding to the request (%d != %d)\n",
                        function, req[offset]+1);
            }
            if (ctx->error_recovery & DS_BUS_ROUTER_ERROR_RECOVERY_PROTOCOL) {
                _sleep_and_flush(ctx);
            }
            errno = EMBBADDATA;
            return -1;
        }

        /* Check the number of values is corresponding to the request */
        switch (function) {

        default:
            /* 1 Write functions & others */
            req_nb_value = rsp_nb_value = 1;
        }

        if (req_nb_value == rsp_nb_value) {
            rc = rsp_nb_value;
        } else {
            if (ctx->debug) {
                fprintf(stderr,
                        "Quantity not corresponding to the request (%d != %d)\n",
                        rsp_nb_value, req_nb_value);
            }

            if (ctx->error_recovery & DS_BUS_ROUTER_ERROR_RECOVERY_PROTOCOL) {
                _sleep_and_flush(ctx);
            }

            errno = EMBBADDATA;
            rc = -1;
        }
    } else if (rsp_length == (offset + 2 + ctx->checksum_length) &&
               req[offset] == (rsp[offset] - 0x80)) {
        /* EXCEPTION CODE RECEIVED */

        int exception_code = rsp[offset + 1];
        if (exception_code < ds_bus_router_EXCEPTION_MAX) {
            errno = ds_bus_router_ENOBASE + exception_code;
        } else {
            errno = EMBBADEXC;
        }
        _error_print(ctx, NULL);
        rc = -1;
    } else {
        if (ctx->debug) {
            fprintf(stderr,
                    "Message length not corresponding to the computed length (%d != %d)\n",
                    rsp_length, rsp_length_computed);
        }
        if (ctx->error_recovery & DS_BUS_ROUTER_ERROR_RECOVERY_PROTOCOL) {
            _sleep_and_flush(ctx);
        }
        errno = EMBBADDATA;
        rc = -1;
    }

    return rc;
}

const char *ds_bus_router_strerror(int errnum) {
    switch (errnum) {
    case EMBXILFUN:
        return "Illegal function";
    case EMBXILADD:
        return "Illegal data address";
    case EMBXILVAL:
        return "Illegal data value";
    case EMBXSFAIL:
        return "Slave device or server failure";
    case EMBXACK:
        return "Acknowledge";
    case EMBXSBUSY:
        return "Slave device or server is busy";
    case EMBXNACK:
        return "Negative acknowledge";
    case EMBXMEMPAR:
        return "Memory parity error";
    case EMBXGPATH:
        return "Gateway path unavailable";
    case EMBXGTAR:
        return "Target device failed to respond";
    case EMBBADCRC:
        return "Invalid CRC";
    case EMBBADDATA:
        return "Invalid data";
    case EMBBADEXC:
        return "Invalid exception code";
    case EMBMDATA:
        return "Too many data";
    default:
        return strerror(errnum);
    }
}


void _error_print(ds_bus_router_t *ctx, const char *context)
{
    if (ctx->debug) {
        fprintf(stderr, "ERROR %s", ds_bus_router_strerror(errno));
        if (context != NULL) {
            fprintf(stderr, ": %s\n", context);
        } else {
            fprintf(stderr, "\n");
        }
    }
}

void ds_bus_router_set_debug(ds_bus_router_t *ctx, int boolean)
{
    ctx->debug = boolean;
}





int ds_bus_router_File_Record_read_send(ds_bus_router_t *ctx ,ds_router_list  *ptr_ds_router_list,File_Record  *File_Record_array[] , int File_Record_num  )
{
    int rc;
    int req_length;
     int rsp_length;
    uint8_t req[_MIN_REQ_LENGTH];
    uint8_t rsp[MAX_MESSAGE_LENGTH];   	
    int i=0;
   int router_header_num;


     for ( i =0;i<File_Record_num;i++){

	if (File_Record_array[i]->register_num > ds_bus_router_MAX_READ_REGISTERS ) {
		  if (ctx->debug) {
            		fprintf(stderr,
                    "ERROR Too many registers requested (%d > %d)\n",
                    File_Record_array[i]->register_num, ds_bus_router_MAX_READ_REGISTERS);
        	}
        	errno = EMBMDATA;
        	return -1;	
		}
 	}

    req_length = _ds_bus_router_build_request_basis_File_Record(ctx, _FC_READ_SEND_File_Record, req,
    				File_Record_array,File_Record_num, ptr_ds_router_list,&router_header_num);
	printf("req_length=%d\n",req_length);
	printf("router_header_num=%d\n",router_header_num);
    rc = send_msg(ctx, req, req_length,router_header_num);
    if (rc > 0) {
        int offset;
        int j;
	int file_order;
	// 还有crc ，2个字节
//	#define DS_HEADER_   9 
        rc = receive_msg(ctx, rsp, MSG_CONFIRMATION,File_Record_num,req_length,router_header_num);
        if (rc == -1)
            return -1;
	rsp_length = rc;
        rc = check_confirmation(ctx, req, rsp, rc,File_Record_num,router_header_num);
        if (rc == -1)
            return -1;
	printf("rc=%d\n",rc);	 
        offset= ctx->header_length+add_len_fileNo_header ;
	printf("offset=%d\n",offset);
	int length;
	int sum =0 ;
	
	for (i=0;i<rsp_length;i++){

		printf("%x ",rsp[i]);
	
	}	

	printf("\n");
	
	for (file_order = 0 ; file_order<File_Record_num;file_order++ ){

	length = (rsp[offset + 7*file_order+6+sum] << 8 | rsp[offset + 7*file_order+5+sum]);	
	
		
	for (j = 0 ;j<File_Record_array[file_order]->register_num ;j++){
		
		File_Record_array[file_order]->data[j] =   (rsp[offset + 7*file_order+7+sum+j]); 
	}
	sum = sum+length;

		
	}
    }



    return rc;
}


int ds_bus_router_File_Record_write_send(ds_bus_router_t *ctx ,ds_router_list  *ptr_ds_router_list,File_Record  *File_Record_array[] , int File_Record_num)
{
    int rc;
    int i;
    int req_length;


    uint8_t req[MAX_MESSAGE_LENGTH];

     int router_header_num;

     for ( i =0;i<File_Record_num;i++){

	if (File_Record_array[i]->register_num > ds_bus_router_MAX_READ_REGISTERS ) {
		  if (ctx->debug) {
            		fprintf(stderr,
                    "ERROR Too many registers requested (%d > %d)\n",
                   File_Record_array[i]->register_num, ds_bus_router_MAX_READ_REGISTERS);
        	}
        	errno = EMBMDATA;
        	return -1;	
		}
 	}

       req_length = _ds_bus_router_build_request_basis_File_Record_write_send(ctx, _FC_WRITE_SEND_File_Record, req,File_Record_array	,File_Record_num,ptr_ds_router_list,&router_header_num);


	for (i = 0 ;i<File_Record_num;i++){
		req[req_length++] = 6;
		req[req_length++] =  File_Record_array[i]->file_no& 0x00ff;     //11
		req[req_length++] = File_Record_array[i]->file_no >> 8;	   //12

		req[req_length++] =File_Record_array[i]->register_address & 0x00ff;   //13
		req[req_length++] = File_Record_array[i]->register_address  >> 8;    //14

		req[req_length++] =File_Record_array[i]->register_num & 0x00ff;   //15
		req[req_length++] = File_Record_array[i]->register_num  >> 8;      //16
		printf("req_length=%d\n",req_length);
		printf("File_Record_array=0x%x\n",File_Record_array[i]->data[0]);
		memcpy(req+req_length,File_Record_array[i]->data,File_Record_array[i]->register_num);
		 req_length = req_length+File_Record_array[i]->register_num;
	}  	

    rc = send_msg(ctx, req, req_length,router_header_num);
    if (rc > 0) {
        uint8_t rsp[MAX_MESSAGE_LENGTH];
        rc = receive_msg(ctx, rsp, MSG_CONFIRMATION,File_Record_num,req_length,router_header_num);

//        rc = receive_msg(ctx, rsp, MSG_CONFIRMATION,File_Record_num,req_length,req_length-router_header_num+4);
        if (rc == -1)
            return -1;

        rc = check_confirmation(ctx, req, rsp, rc,File_Record_num,router_header_num);
    }

    return rc;
	
}

int ds_bus_router_set_error_recovery(ds_bus_router_t *ctx,
                              ds_bus_router_error_recovery_mode error_recovery)
{
    /* The type of modbus_error_recovery_mode is unsigned enum */
    ctx->error_recovery = (uint8_t) error_recovery;
    return 0;
}


/* Get the timeout interval used to wait for a response */
void ds_bus_router_get_response_timeout(ds_bus_router_t *ctx, struct timeval *timeout)
{
    *timeout = ctx->response_timeout;
}

void ds_bus_router_set_response_timeout(ds_bus_router_t *ctx, const struct timeval *timeout)
{
    ctx->response_timeout = *timeout;
}

/* Get the timeout interval between two consecutive bytes of a message */
void ds_bus_router_get_byte_timeout(ds_bus_router_t *ctx, struct timeval *timeout)
{
    *timeout = ctx->byte_timeout;
}

void ds_bus_router_set_byte_timeout(ds_bus_router_t *ctx, const struct timeval *timeout)
{
    ctx->byte_timeout = *timeout;
}


int ds_bus_router_File_Record_IR_write_send(ds_bus_router_t *ctx ,ds_router_list  *ptr_ds_router_list,File_Record  *File_Record_array[] , int File_Record_num)
{
    int rc;
    int i;
    int req_length;


    uint8_t req[MAX_MESSAGE_LENGTH];

	   int router_header_num;

     for ( i =0;i<File_Record_num;i++){

	if (File_Record_array[i]->register_num > ds_bus_router_MAX_READ_REGISTERS ) {
		  if (ctx->debug) {
            		fprintf(stderr,
                    "ERROR Too many registers requested (%d > %d)\n",
                   File_Record_array[i]->register_num, ds_bus_router_MAX_READ_REGISTERS);
        	}
        	errno = EMBMDATA;
        	return -1;	
		}
 	}

       req_length = _ds_bus_router_build_request_basis_File_Record_write_send(ctx, _FC_WRITE_SEND_IR_File_Record, req,File_Record_array	,File_Record_num,ptr_ds_router_list,&router_header_num);


	for (i = 0 ;i<File_Record_num;i++){
		req[req_length++] = 6;
		req[req_length++] =  File_Record_array[i]->file_no& 0x00ff;     //11
		req[req_length++] = File_Record_array[i]->file_no >> 8;	   //12

		req[req_length++] =File_Record_array[i]->register_address & 0x00ff;   //13
		req[req_length++] = File_Record_array[i]->register_address  >> 8;    //14

		req[req_length++] =File_Record_array[i]->register_num & 0x00ff;   //15
		req[req_length++] = File_Record_array[i]->register_num  >> 8;      //16
		printf("req_length=%d\n",req_length);
		printf("File_Record_array=0x%x\n",File_Record_array[i]->data[0]);
		memcpy(req+req_length,File_Record_array[i]->data,File_Record_array[i]->register_num);
		 req_length = req_length+File_Record_array[i]->register_num;
	}  	

    rc = send_msg(ctx, req, req_length,router_header_num);
    if (rc > 0) {
        uint8_t rsp[MAX_MESSAGE_LENGTH];

     //   rc = receive_msg(ctx, rsp, MSG_CONFIRMATION,File_Record_num,req_length,req_length-router_header_num+4);
       rc = receive_msg(ctx, rsp, MSG_CONFIRMATION,File_Record_num,req_length,router_header_num); 
        if (rc == -1)
            return -1;

        rc = check_confirmation(ctx, req, rsp, rc,File_Record_num,router_header_num);
    }

    return rc;
	
}




