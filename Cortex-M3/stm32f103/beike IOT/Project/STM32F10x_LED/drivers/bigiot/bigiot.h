#ifndef _BIGIOT_H_
#define _BIGIOT_H_


extern char *DEVICEID[];
extern char *APIKEY[];
extern char *INPUTID[];

/*��CJSON������յ�����Ϣ*/
int processMessage(unsigned char i,char *msg) ;
void BigIot_app(void);
void checkin(unsigned char i);
void checkout(unsigned char i);
void update1(char *did, char *inputid, float value);
void check_status(unsigned char i);

#endif


