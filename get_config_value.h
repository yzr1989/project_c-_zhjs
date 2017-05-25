#ifndef _GET_CONFIG_VALUE_H_
#define _GET_CONFIG_VALUE_H_

#include <string>
using namespace std;


int GetProfileString(   string  profile,   string   AppName,   string  KeyName,   string & KeyVal );
int GetProfileInteger(   string  profile,   string   AppName,   string  KeyName,   int  & KeyVal );





#endif   /*_GET_CONFIG_VALUE_H_ */
