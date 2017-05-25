#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>
#include <iostream>
#include <string>
#include "iniparser.h"

#include "get_config_value.h"

using namespace std;




int GetProfileInteger(   string  profile,   string   AppName,   string  KeyName,   int  & KeyVal )
{
    dictionary  *   ini ;   

    ini = iniparser_load(profile.c_str());
    if (ini==NULL) {
        fprintf(stderr, "cannot parse file: %s\n", profile.c_str());
        return -1 ;
    }

   string parser = AppName+":"+KeyName;

    KeyVal = iniparser_getint(ini, parser.c_str(), (int)NULL);  

    iniparser_freedict(ini);



	return 0;
}




int GetProfileString(   string  profile,   string   AppName,   string  KeyName,   string & KeyVal )
{

    dictionary  *   ini ;


    const char  *   s ;

    ini = iniparser_load(profile.c_str());
    if (ini==NULL) {
        fprintf(stderr, "cannot parse file: %s\n", profile.c_str());
        return -1 ;
    }


	string parser = AppName+":"+KeyName;


    s = iniparser_getstring(ini, parser.c_str(), NULL);
   
    if (s != NULL){
     KeyVal = string(s);

   }
    iniparser_freedict(ini);


	return 0;
}