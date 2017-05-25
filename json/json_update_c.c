
#include <stdio.h>
#include <glib.h>



int update_prog_watchdog_name(const  char * progname)
{

	GKeyFile* config = g_key_file_new();
	
  	gsize length = 0;	
	g_key_file_load_from_file(config,CONFIG_SYS_FILE_NAME,G_KEY_FILE_KEEP_COMMENTS|G_KEY_FILE_KEEP_TRANSLATIONS,NULL);


	
	g_key_file_set_value(config,"watchdog","prog_name",progname);
	
	

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