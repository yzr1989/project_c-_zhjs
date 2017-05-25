#ifndef  _CONTEXTMODEL_RES_H_
#define _CONTEXTMODEL_RES_H_

#include <string>

using namespace std;

class contextmodel_res{
public :

	int modelid;
	string weekcycle;
	bool  enabled;
	contextmodel_res();
	contextmodel_res(int modelid , string weekcycle );
	~contextmodel_res();
	

	
};



#endif   /*_CONTEXTMODEL_RES_H_*/