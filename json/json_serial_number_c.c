#include <stdio.h> 
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <glib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "config.h"


int   json_encode_write_serial_number(const char *string_serial_number)
{


	GKeyFile* config = g_key_file_new();
	
  	gsize length = 0;

	g_key_file_load_from_file(config,CONFIG_SYS_FILE_NAME,G_KEY_FILE_KEEP_COMMENTS|G_KEY_FILE_KEEP_TRANSLATIONS,NULL);
	g_key_file_set_value(config,"system_config","serial_number",string_serial_number);

	 gchar* content = g_key_file_to_data(config,&length,NULL);
	 FILE* fp = fopen(CONFIG_SYS_FILE_NAME,"w");
    	if(fp == NULL) {		
		 printf("fopen error\n");
		  return -1;
    	}	 
      	fwrite(content,1,length,fp);
      	fclose(fp);

	 g_key_file_free(config);

	return 0;

	  
}

