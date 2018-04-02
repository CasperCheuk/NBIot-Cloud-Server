#ifndef __NBBC95_H__
#define __NBBC95_H__


#include <stdint.h>
#include "usart.h"



typedef enum {
    CMD_TEST,   //��������
    CMD_READ,   //��ȡ����
    CMD_SET,    //��������
    CMD_EXCUTE  //ִ������
}cmd_type;


typedef struct{
    const char* NBcmd;      //ATָ��
    char* NBcmdParameter;   //ATָ�����
    cmd_type property;      //ָ������
    char* expectReply;      //�����õ��Ļظ�
    unsigned char cmd_try;  //�����Դ���
    unsigned char havetried;//�Ѿ��ظ����͵Ĵ���
    uint8_t repeatPeri;     //�ظ����͵�ʱ����
    uint16_t max_timeout;   //���ʱʱ��
}ATcmd;

typedef ATcmd* atcmdInfo;





typedef enum {
    MSG_NONE,
    MSG_INIT,
    MSG_IMSI,
    MSG_IMEI,
    MSG_SIGN,
    MSG_DEVID,
    MSG_MANUINFO,
    MSG_MODULEINFO,
    MSG_REGISTER,
    MSG_UDP_CREATE,
    MSG_UDP_SEND,
    MSG_UDP_RECE,
    MSG_UDP_CLOSE,
    MSG_COAP
}msg_type;



typedef enum {
    PROCESS_NONE,
    PROCESS_INIT = MSG_INIT,
    PROCESS_MODULE_INFO = MSG_MODULEINFO,
    PROCESS_REGISTER = MSG_REGISTER,
//    PROCESS_IMSI = MSG_IMSI,
//    PROCESS_IMEI = MSG_IMEI,
    RPOCESS_SIGN = MSG_SIGN,
    PROCESS_UDP_CREATE = MSG_UDP_CREATE,
    PROCESS_UDP_SEND = MSG_UDP_SEND,
    PROCESS_UDP_RECE = MSG_UDP_RECE,
    PROCESS_UDP_CLOSE = MSG_UDP_CLOSE,
    PROCESS_COAP = MSG_COAP
}NB_Process;


typedef struct BC95State {
    NB_Process  state;
    int         sub_state;
}bc95_state;

typedef int (*NBReceMsg)(msg_type, int, char*);


typedef struct NBbc95 *NBModule;

typedef uint8_t (*NB_OpenModule)(NBModule);
typedef uint8_t (*NB_initbc95)(NBModule);
typedef const char* (*NB_ModuleInfo)(NBModule);
typedef const char* (*NB_RegisterInfo)(NBModule);
typedef const char* (*NB_MISIInfo)(NBModule);
typedef uint8_t (*NB_NetSign)(NBModule);
typedef uint8_t (*NB_CreateUdpServer)(NBModule);
typedef uint8_t (*NB_SendToUdp)(NBModule, int , char*);
typedef uint8_t (*NB_RecFromUdp)(NBModule);
typedef uint8_t (*NB_CloseUdp)(NBModule);
typedef uint8_t (*NB_BC95Main)(NBModule);

typedef struct {
    NB_OpenModule       Openbc95;
    NB_initbc95         Initbc95;
    NB_ModuleInfo       getModuleInfo;
    NB_RegisterInfo     getRegisterInfo;
    NB_MISIInfo         getMISIInfo;
    NB_NetSign          isNetSign;
    NB_CreateUdpServer  CreateUDPServer;
    NB_SendToUdp        SendToUdp;
    NB_RecFromUdp       RecFromUdp;
    NB_CloseUdp         CloseUdp;
    NB_BC95Main         BC95Main;
}NBOperaFun;


//BC95���ܽṹ����
typedef struct NBbc95{
    NBOperaFun  *funcPtr;
    void        *object;//BC95 uart����ָ��
    NBReceMsg   recMsg_Callback;
}NBbc95;

//���ڽ��ջص�
typedef void (*bc95_SP_receive_callback)(char*, uint16_t);


typedef void (*SerialPortOpen)(bc95_SP_receive_callback, uint32_t);
typedef void (*SerialPortSend)(uint8_t*, uint16_t);
typedef void (*SerialPortClose)(void);
//���ڲ����ṹ�壬������ʼ��BC95����ṹ��serialfun
typedef struct {
    SerialPortOpen  serialPortOpen;
    SerialPortSend  serialPortSend;
    SerialPortClose serialPortClose;
}serialfun;


//BC95����ṹ��������ʼ��NBbc95�е�BC95 uart����ָ��
typedef struct {
    uint16_t    baundrate;
    serialfun   *SPfunTable;
}bc95Module;

typedef bc95Module *bc95object;



extern uint8_t openbc95(NBModule bc95);
extern uint8_t initbc95(NBModule bc95);
extern const char* getModuleInfo_bc95(NBModule bc95);
extern const char* getRegisterInfo_bc95(NBModule bc95);
extern uint8_t isNetSign_bc95(NBModule bc95);
extern uint8_t createUDPServer_bc95(NBModule bc95);
extern uint8_t sendToUdp_bc95(NBModule bc95, int len, char* buf);
extern uint8_t recFromUdp_bc95(NBModule bc95);
extern uint8_t closeUdp_bc95(NBModule bc95);
extern uint8_t bc95Main(NBModule bc95);
void InitATcmd(atcmdInfo cmdinfo, const char* cmd, char* param, cmd_type property);
uint16_t formatATcmd(atcmdInfo cmdinfo);





#endif

