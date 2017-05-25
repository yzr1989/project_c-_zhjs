#ifndef  _JSON_SYSINFO_OP_H_
#define 	_JSON_SYSINFO_OP_H_

#ifdef __cplusplus
extern "C" {
#endif


extern unsigned long kb_main_used;
extern unsigned long kb_main_total;

void loadavg(double * av1, double * av5, double * av15);
void meminfo(void);

#ifdef __cplusplus
}
#endif


#endif   /*_JSON_SYSINFO_OP_H_*/