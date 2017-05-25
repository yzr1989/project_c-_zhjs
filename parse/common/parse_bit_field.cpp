#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <string>  
#include <iostream>        
#include <boost/format.hpp>  
#include <boost/tokenizer.hpp>  
#include <boost/algorithm/string.hpp>  

#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>





//#define OVECCOUNT 100 /* should be a multiple of 3 */
//#define EBUFLEN 128
//#define BUFLEN 1024

using namespace std;	
using namespace  boost;  

#if 0

int  parse_bit_field(char *src , int *hd_bit_offset)
{
	
	int a ,b,data;

	sscanf(src,"[%d:%d]",&a,&b);		  

	if (a !=b){
		data = pow(2,a)-pow(2,b);
	}
	if (a==b){
	 	data = pow(2,a);
	}

	*hd_bit_offset = b; 
	
	printf("bit field =%x\n",data);

        return  data;
}


 #endif 


vector<string>    parse_bit_field( string   strTag  )
{
	
	 boost::char_separator<char> sep(",");  
	typedef boost::tokenizer<boost::char_separator<char> >  
	CustonTokenizer;  
	CustonTokenizer tok(strTag,sep);  
        vector<string> vecSegTag;  
        for(CustonTokenizer::iterator beg=tok.begin(); beg!=tok.end();++beg)  
        {  
            vecSegTag.push_back(*beg);  
        }  

	return vecSegTag;
}


 


vector<string>    parse_router_list( string   strTag  )
{
	
	 boost::char_separator<char> sep(",");  
	typedef boost::tokenizer<boost::char_separator<char> >  
	CustonTokenizer;  
	CustonTokenizer tok(strTag,sep);  
        vector<string> vecSegTag;  
        for(CustonTokenizer::iterator beg=tok.begin(); beg!=tok.end();++beg)  
        {  
            vecSegTag.push_back(*beg);  
        }  

	return vecSegTag;
}
