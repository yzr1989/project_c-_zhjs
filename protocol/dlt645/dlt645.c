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
	   
#include "dlt645.h"


/* Internal use */
#define MSG_LENGTH_UNDEFINED -1

static void _error_print(dlt645_t *ctx, const char *context);
static int _sleep_and_flush(dlt645_t *ctx);


dlt645_t* dlt645_new( const char *device,
                         int baud, char parity, int data_bit,
                         int stop_bit)
{
	dlt645_t *ctx;
	ctx = malloc(sizeof(dlt645_t));
	if (ctx == NULL){
		printf("malloc dlt645_t error\n");
		return NULL;
	}

	memset(ctx->slave,0,6);
	
	ctx->s = -1;

	ctx->debug = FALSE;
	ctx->error_recovery = dlt645_ERROR_RECOVERY_NONE;

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


	ctx->header_length=_dlt645_HEADER_LENGTH;
	ctx->checksum_length=_dlt645_CHECKSUM_LENGTH;
	ctx->max_adu_length=dlt645_MAX_ADU_LENGTH;

	 return ctx;
	 
}



int dlt645_set_slave(dlt645_t *ctx, uint8_t  *slave)
{
        /* Broadcast address is 0 (MODBUS_BROADCAST_ADDRESS) */

    // ctx->slave = slave;

	if (ctx->slave == NULL){
		printf("ctx->slave is NULL\n");
		return -1;
	}

	if (slave == NULL){
		printf("slave is NULL\n");
		return -1;
	}
	

		
     	memcpy(ctx->slave,slave,6);

    return 0;
}


#if 0

int dlt645_set_serial_no(dlt645_t *ctx, uint8_t *serial_no)
{
        /* Broadcast address is 0 (MODBUS_BROADCAST_ADDRESS) */

        memcpy(ctx->serial_no,serial_no,sizeof(serial_no));


    return 0;
}

#endif 

int dlt645_connect(dlt645_t *ctx)
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
static int _dlt645_build_request_basis(dlt645_t *ctx,uint8_t *req, int header_num)
{

   int  i =0;
   int  num =0;

  for (i=0;i< header_num;i++){
	  req[num++] = 0xFE;
  }   
   	 req[num++] = 0x68;

	for (i=0;i<6;i++){
		printf("slave[%d]=%d \t",i,ctx->slave[i]);
		 req[num++]  = ctx->slave[i];
	}

	 req[num++] = 0x68;
   //	req[num++] = 0x11; 	
 
	


    return num;
}


/* Builds a RTU request header */





int _dlt645_send_msg_pre(uint8_t *req, int req_length,int header_num)
{

	int i=0;
	int sum =0;


	for (i=header_num;i<req_length;i++){
		//printf("req[%d]=0x%x\n",i,req[i]);
		sum = sum+ req[i];

	}

	//printf("sum=%d\n",sum);
	
	
    req[req_length++] =sum&0xff;	
    req[req_length++] =0x16;
    
 

    return req_length;
}


ssize_t _dlt645_send(dlt645_t *ctx, const uint8_t *req, int req_length)
{

    return write(ctx->s, req, req_length);

}


void dlt645_close(dlt645_t *ctx)
{
    /* Closes the file descriptor in RTU mode */
    tcsetattr(ctx->s, TCSANOW, &(ctx->old_tios));
    close(ctx->s);

}

void dlt645_free(dlt645_t *ctx)
{
    if (ctx == NULL)
        return;

   
    free(ctx);
}
	

/* Sends a request/response */
static int send_msg(dlt645_t *ctx, uint8_t *msg, int msg_length,int header_num)
{
    int rc;
    int i;

    msg_length = _dlt645_send_msg_pre(msg, msg_length, header_num);

    if (ctx->debug) {
        for (i = 0; i < msg_length; i++)
            printf("%.2X ", msg[i]);
        printf("\n");
    }

    /* In recovery mode, the write command will be issued until to be
       successful! Disabled by default. */
    do {
        rc = _dlt645_send(ctx, msg, msg_length);
        if (rc == -1) {
            _error_print(ctx, NULL);
            if (ctx->error_recovery & dlt645_ERROR_RECOVERY_LINK) {
                int saved_errno = errno;

                if ((errno == EBADF || errno == ECONNRESET || errno == EPIPE)) {
                    dlt645_close(ctx);
                    dlt645_connect(ctx);
                } else {
                    _sleep_and_flush(ctx);
                }
                errno = saved_errno;
            }
        }
    } while ((ctx->error_recovery & dlt645_ERROR_RECOVERY_LINK) &&
             rc == -1);

    if (rc > 0 && rc != msg_length) {
        errno = EMBBADDATA;
        return -1;
    }

    return rc;
}

int _dlt645_select(dlt645_t *ctx, fd_set *rfds,
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




int _dlt645_flush(dlt645_t *ctx)
{

    return tcflush(ctx->s, TCIOFLUSH);

}



/* Computes the length to read after the meta information (address, count, etc) */
static int compute_data_length_after_file(dlt645_t *ctx, uint8_t *msg,
                                          msg_type_t msg_type,int file_order)
{
    int length = msg[ctx->header_length-1];
   
   


    if (msg_type == MSG_INDICATION) {
	
    } else {
        /* MSG_CONFIRMATION */
	



    }
   printf("length=%d\n",length);	

    return length;
}







ssize_t _dlt645_recv(dlt645_t *ctx, uint8_t *rsp, int rsp_length)
{

    return read(ctx->s, rsp, rsp_length);

}




/* The check_crc16 function shall return the message length if the CRC is
   valid. Otherwise it shall return -1 and set errno to EMBADCRC. */
int _dlt645_rtu_check_integrity(dlt645_t *ctx, uint8_t *msg,
                                const int msg_length)
{
    uint16_t sum_calculated=0;
    uint16_t sum_received=0;
	int i=0;

	for (i=1;i<msg_length-2;i++){
	sum_calculated = sum_calculated+ msg[i];
	printf("msg[%d]=0x%x\n",i,msg[i]);
	printf("sum_calculated=0x%x\n",sum_calculated);
	}

	sum_calculated =(uint8_t) sum_calculated;
	printf("sum_calculated=0x%x\r\n",(uint8_t)sum_calculated);
	sum_received = msg[msg_length-2];
	printf("sum_received=0x%x\r\n",(uint8_t)sum_received);
	
    /* Check CRC of msg */
    if (sum_received == sum_calculated) {
        return msg_length;
    } else {
        if (ctx->debug) {
            fprintf(stderr, "ERROR CRC received %0X != CRC calculated %0X\n",
                    sum_received, sum_calculated);
        }
        if (ctx->error_recovery & dlt645_ERROR_RECOVERY_PROTOCOL) {
            _dlt645_flush(ctx);
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

static int receive_msg(dlt645_t *ctx, uint8_t *msg, msg_type_t msg_type)
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
    step = _STEP_DATA;
   
    length_to_read = ctx->header_length ;
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
		
        rc = _dlt645_select(ctx, &rfds, p_tv, length_to_read);
		
        if (rc == -1) {
            _error_print(ctx, "select");
            if (ctx->error_recovery & dlt645_ERROR_RECOVERY_LINK) {
                int saved_errno = errno;

                if (errno == ETIMEDOUT) {
                    _sleep_and_flush(ctx);
                } else if (errno == EBADF) {
                    dlt645_close(ctx);
                   dlt645_connect(ctx);
                }
                errno = saved_errno;
            }
            return -1;
        }
		
	 printf("num %d length_to_read=%d\n",j++,length_to_read );	
        rc = _dlt645_recv(ctx, msg + msg_length, length_to_read);
        if (rc == 0) {
            errno = ECONNRESET;
            rc = -1;
        }

        if (rc == -1) {
            _error_print(ctx, "read");
            if ((ctx->error_recovery & dlt645_ERROR_RECOVERY_LINK) &&
                (errno == ECONNRESET || errno == ECONNREFUSED ||
                 errno == EBADF)) {
                int saved_errno = errno;
                dlt645_close(ctx);
                dlt645_connect(ctx);
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
				
            case _STEP_DATA:
		
                length_to_read = compute_data_length_after_file(ctx, msg, msg_type,file_order);
	
    		 printf("step=%d\tlength_to_read=%d\n",_STEP_DATA,length_to_read);
                step = _STEP_SUM;
                break;

		case _STEP_SUM:
		 printf("step=%d\n",_STEP_SUM)	;
		length_to_read =2;
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

    return _dlt645_rtu_check_integrity(ctx, msg, msg_length);
}


static int _sleep_and_flush(dlt645_t *ctx)
{

    /* usleep source code */
    struct timespec request, remaining;
    request.tv_sec = ctx->response_timeout.tv_sec;
    request.tv_nsec = ((long int)ctx->response_timeout.tv_usec % 1000000)
        * 1000;
    while (nanosleep(&request, &remaining) == -1 && errno == EINTR)
        request = remaining;

    return _dlt645_flush(ctx);
}






const char *dlt645_strerror(int errnum) {
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


static void _error_print(dlt645_t *ctx, const char *context)
{
    if (ctx->debug) {
        fprintf(stderr, "ERROR %s", dlt645_strerror(errno));
        if (context != NULL) {
            fprintf(stderr, ": %s\n", context);
        } else {
            fprintf(stderr, "\n");
        }
    }
}



void dlt645_set_debug(dlt645_t *ctx, int boolean)
{
    ctx->debug = boolean;
}





int dlt645_read_send(dlt645_t *ctx,int header_num  ,int addr,int addr_num,uint8_t *data )
{
    int rc;
    int req_length;
     int rsp_length;
    uint8_t req[_MIN_REQ_LENGTH];
    uint8_t rsp[MAX_MESSAGE_LENGTH];   	
    int i=0;
	int add_fix = 0;
	int receive_length =0;
	

    req_length = _dlt645_build_request_basis(ctx, req,header_num);
	printf("req_length=%d\n",req_length);

	req[req_length++] = 0x11; 
	for (i = 0;i<addr_num;i++){
		add_fix=add_fix<<8;
		add_fix=add_fix+0x33;		
	}
		
	printf("add_fix=0x%x\r\n",add_fix);

//addr =  htonl(addr);
//printf("addr=0x%x\r\n",addr);



  // addr = addr+add_fix;
	printf("addr=0x%x\r\n",addr);

	printf("addr=0x%x\r\n",((addr)&0xff)+0x33);
	printf("addr=0x%x\r\n",((addr>>1*8)&0xff)+0x33);
	printf("addr=0x%x\r\n",(( addr>>2*8)&0xff)+0x33);
	printf("addr=0x%x\r\n",(( addr>>3*8)&0xff)+0x33);
	



	req[req_length++] = addr_num;
	req[req_length++] =( (addr)&0xff)+0x33;
	req[req_length++] =( (addr>>1*8)&0xff)+0x33;
	req[req_length++] =(( addr>>2*8)&0xff)+0x33;
	req[req_length++] =(( addr>>3*8)&0xff)+0x33;
	
    rc = send_msg(ctx, req, req_length,header_num);
    if (rc > 0) {
        int offset;
        int j;
	
        rc = receive_msg(ctx, rsp, MSG_CONFIRMATION);
        if (rc == -1)
            return -1;
	rsp_length = rc;

	printf("rc=%d\n",rc);	 
        offset= ctx->header_length-1 ;
	printf("offset=%d\n",offset);
	int length =rsp[offset] ;
	printf("length=0x%x\n",length);
	
	
	for (i=0;i<rsp_length;i++){

		printf("%x ",rsp[i]);
	
	}	

	printf("\n");
	 j=0;

	printf("respond %x \n",rsp[ctx->header_length-2]);


	for (i=ctx->header_length;i<ctx->header_length+length;i++){
		printf("%x ",rsp[i]);
	}
	printf("\n");
	for (i=ctx->header_length;i<ctx->header_length+length;i++){
		printf("%x ",rsp[i]-0x33);
		
	}
	printf("receive_length=%d\n",receive_length);
	printf("length=%d\n",length);
	for (i=ctx->header_length+addr_num;i<ctx->header_length+length;i++){
		printf("%x ",rsp[i]-0x33);
		data[j++] = rsp[i]-0x33;
	}
	printf("\n");
    }



    return rc;
}


int dlt645_write_send(dlt645_t *ctx,int header_num  ,int addr,int addr_num,uint8_t *data ,int num)
{
    int rc;
    int i;
    int req_length;
	
	int add_fix = 0;
    uint8_t req[MAX_MESSAGE_LENGTH];
	int rsp_length;
       req_length = _dlt645_build_request_basis(ctx, req,header_num);

	req[req_length++] = 0x14; 
	
	for (i = 0;i<addr_num;i++){
		add_fix=add_fix<<8;
		add_fix=add_fix+0x33;		
	}
		
	printf("add_fix=0x%x\r\n",add_fix);

//addr =  htonl(addr);
//printf("addr=0x%x\r\n",addr);



  // addr = addr+add_fix;
	printf("addr=0x%x\r\n",addr);

	printf("addr=0x%x\r\n",((addr)&0xff)+0x33);
	printf("addr=0x%x\r\n",((addr>>1*8)&0xff)+0x33);
	printf("addr=0x%x\r\n",(( addr>>2*8)&0xff)+0x33);
	printf("addr=0x%x\r\n",(( addr>>3*8)&0xff)+0x33);
	



	req[req_length++] = addr_num+num/2+8;
	req[req_length++] =( (addr)&0xff)+0x33;
	req[req_length++] =( (addr>>1*8)&0xff)+0x33;
	req[req_length++] =(( addr>>2*8)&0xff)+0x33;
	req[req_length++] =(( addr>>3*8)&0xff)+0x33;


	for (i=0;i<4;i++){
		req[req_length++] =0x33;
	}



	for (i=0;i<4;i++){
		req[req_length++] =0x63;
	}
	


	for (i=0;i<num/2;i++){
		req[req_length++] =data[i]+0x33;		
	}


	printf("req_length=%d\n",req_length);
	   
    rc = send_msg(ctx, req, req_length,header_num);
    if (rc > 0) {
        uint8_t rsp[MAX_MESSAGE_LENGTH];

        rc = receive_msg(ctx, rsp, MSG_CONFIRMATION);
        if (rc == -1)
            return -1;

       //rc = check_confirmation(ctx, req, rsp, rc);

	rsp_length = rc;
	for (i=0;i<rsp_length;i++){

		printf("%x ",rsp[i]);
	
	}
	
	printf("\n");   
    }

	

    return rc;
	
}



int dlt645_relay_on(dlt645_t *ctx,int header_num,int relay )
{

	int rc;
	int i;
	int req_length;
	
	
	uint8_t req[MAX_MESSAGE_LENGTH];
	int rsp_length;
       req_length = _dlt645_build_request_basis(ctx, req,header_num);

	req[req_length++] = 0x1c; 

	req[req_length++] = 0x10; 


	for (i=0;i<4;i++){
		req[req_length++] =0x33;
	}


	for (i=0;i<4;i++){
		req[req_length++] =0x63;
	}	

	//ÌøÕ¢
	if (relay == 0){
		req[req_length++] = 0x1a+0x33;
	}


	//ºÏÕ¢
	if (relay ==1){
		req[req_length++] = 0x1b+0x33; 
	}

	for (i=0;i<7;i++){
		req[req_length++] =0x33;
	}
	

	printf("req_length=%d\n",req_length);
	   
    rc = send_msg(ctx, req, req_length,header_num);
    if (rc > 0) {
        uint8_t rsp[MAX_MESSAGE_LENGTH];

        rc = receive_msg(ctx, rsp, MSG_CONFIRMATION);
        if (rc == -1)
            return -1;

       //rc = check_confirmation(ctx, req, rsp, rc);

	rsp_length = rc;
	for (i=0;i<rsp_length;i++){

		printf("%x ",rsp[i]);
	
	}
	
	printf("\n");   
    }

	

    return rc;
}


/* Get the timeout interval used to wait for a response */
void dlt645_get_response_timeout(dlt645_t *ctx, struct timeval *timeout)
{
    *timeout = ctx->response_timeout;
}

void dlt645_set_response_timeout(dlt645_t *ctx, const struct timeval *timeout)
{
    ctx->response_timeout = *timeout;
}

/* Get the timeout interval between two consecutive bytes of a message */
void dlt645_get_byte_timeout(dlt645_t *ctx, struct timeval *timeout)
{
    *timeout = ctx->byte_timeout;
}

void dlt645_set_byte_timeout(dlt645_t *ctx, const struct timeval *timeout)
{
    ctx->byte_timeout = *timeout;
}

