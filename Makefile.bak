
#CROSS_COMPILE =  arm-linux-
AS		= ccache $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= ccache $(CROSS_COMPILE)gcc
CPP		= ccache $(CROSS_COMPILE)g++
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm

STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump

export AS LD CC CPP AR NM
export STRIP OBJCOPY OBJDUMP

#CFLAGS := -Wall -Werror -O2   -g -std=gnu99 -D_GNU_SOURCE 
CFLAGS := -Wall  -Werror  -g  -D_GNU_SOURCE  -ffunction-sections -fdata-sections


CFLAGS  += -I$(PWD)/include
CFLAGS  += -I$(PWD)/parse/mysql
CFLAGS  += -I$(PWD)/protocol  
CFLAGS  += -I$(PWD)/protocol/ds_bus  
CFLAGS  += -I$(PWD)/protocol/modbus
CFLAGS  += -I$(PWD)/protocol/modbus_tcp
CFLAGS  += -I$(PWD)/protocol/modbus/src
CFLAGS  += -I$(PWD)/protocol/dlt645  
CFLAGS  += -I$(PWD)/protocol/dlt645_router 
CFLAGS  += -I$(PWD)/protocol/modbus_router 
CFLAGS  += -I$(PWD)/protocol/ds_bus_router
CFLAGS  += -I$(PWD)/protocol/xinke_relay
CFLAGS  += -I$(PWD)/parse/mysql
CFLAGS  += -I$(PWD)/hardtype_op
CFLAGS  += -I$(PWD)/
CFLAGS  += -I$(PWD)/parse
CFLAGS  += -I$(PWD)/parse/common
CFLAGS  += -I$(PWD)/json
CFLAGS  += -I/usr/include/mysql++/
CFLAGS  += -I/usr/include/mysql/
#CFLAGS  += -I/usr/include/modbus
CFLAGS  += -I$(PWD)/iniparser


CFLAGS  += `pkg-config --cflags --libs glib-2.0`
#CFLAGS  += `pkg-config --cflags --libs glibmm-2.4`

#LDFLAGS := `pkg-config --cflags --libs glib-2.0`   -L /usr/lib/mysql  -lm  -lrt  -lpthread -pthread -lmodbus   -ljson  -llog4cxx  -lmysqlclient -lmysqlpp    -Wl,-Map,test.map 
LDFLAGS := `pkg-config --cflags --libs glib-2.0`   -L /usr/lib/mysql  -lm  -lrt  -lpthread -pthread -lmodbus    -lmysqlclient -lmysqlpp    -Wl,-Map,test.map 
#LDFLAGS += `pkg-config --cflags --libs glibmm-2.4`

export CFLAGS LDFLAGS

TOPDIR := $(shell pwd)
export TOPDIR

TARGET := zhjs


obj-y += main.o
obj-y += thread_air_control.o
obj-y += run_as_daemon.o
obj-y += utils.o
obj-y += get_config_value.o
obj-y += pthread_op.o
obj-y += thread_server_udp.o
obj-y += protocol/
obj-y += json/
obj-y += parse/
obj-y += hardtype_op/
obj-y += serial_op.o
obj-y += sting2hex.o
obj-y += thread_object.o
obj-y += parse_comharddevice.o
obj-y += iniparser/
obj-y += muduo/

all : 
	make -C ./ -f $(TOPDIR)/Makefile.build
	$(CPP) -lboost_date_time -lboost_thread -lboost_system   $(LDFLAGS) -o $(TARGET)_s  built-in.o 
	$(STRIP)   $(TARGET)_s  -o $(TARGET) 

clean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")	
	rm -f $(shell find -name "*.bak")
	rm -f $(shell find -name "*.map.swp")
	rm -f $(shell find -name "*.txt")
	rm -f $(TARGET)

distclean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	rm -f $(shell find -name "*.bak")
	rm -f $(TARGET)
	
line:
	wc -l `find -name '*.c'`
	wc -l `find -name '*.h'`	
	wc -l `find -name '*.cpp'`	