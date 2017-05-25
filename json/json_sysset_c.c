
#include <stdio.h> 
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <pthread.h> 
#include <stdio.h>
#include <sys/un.h>
#include <glib.h>
#include <stdlib.h>
#include <unistd.h>




#include "config.h"








 int sys_config_write_file_int(const char *key_string ,int val,const char *config_file,const char *group_name)
{
	GKeyFile* config = g_key_file_new();
	
  	gsize length = 0;		

	if (val ==0){
		 g_key_file_free(config);
		return 0;
	}
	
	g_key_file_load_from_file(config,config_file,G_KEY_FILE_KEEP_COMMENTS|G_KEY_FILE_KEEP_TRANSLATIONS,NULL);
	
	g_key_file_set_integer(config,group_name,key_string,val);
	

	 gchar* content = g_key_file_to_data(config,&length,NULL);
	 FILE* fp = fopen(CONFIG_FILE_NAME,"w");
    	if(fp == NULL) {		
		 printf("fopen error\n");
		  return -1;
    	}	 
      	fwrite(content,1,length,fp);
      	fclose(fp);

	

	return 0;
}


 int sys_config_write_file_string(const char *key_string ,const char * val_string,char *config_file,char *group_name)
{
	GKeyFile* config = g_key_file_new();

  	gsize length = 0;		

	if (val_string ==NULL){
		 g_key_file_free(config);
		return 0;
	}
	
	g_key_file_load_from_file(config,config_file,G_KEY_FILE_KEEP_COMMENTS|G_KEY_FILE_KEEP_TRANSLATIONS,NULL);
	
	g_key_file_set_value(config,group_name,key_string,val_string);
	

	 gchar* content = g_key_file_to_data(config,&length,NULL);
	 FILE* fp = fopen(CONFIG_FILE_NAME,"w");
    	if(fp == NULL) {		
		 printf("fopen error\n");
		  return -1;
    	}	 
      	fwrite(content,1,length,fp);
      	fclose(fp);

	

	return 0;
}







