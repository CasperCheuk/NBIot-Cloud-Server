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
}ATcmd,*atcmdInfo;




typedef enum {
    MSG_IMSI,
    MSG_IMEI,
    MSG_SIGN,
    MSG_DEVID,
    MSG_MANUINFO,
    MSG_REGESTA,
    MSG_UDP,
    MSG_COAP
}msg_type;

typedef int (*NBReceMsg)(msg_type, int, char*);


typedef struct NBbc95 *NBModule;

typedef int (*NB_OpenModule)(NBModule);
typedef int (*NB_initbc95)(NBModule);
typedef int (*NB_initUdpServer)(NBModule);
typedef int (*NB_SendToUdp)(NBModule);
typedef int (*NB_RecFromUdp)(NBModule);
typedef int (*NB_BC95Main)(NBModule);

typedef struct {
    NB_OpenModule       Openbc95;
    NB_initbc95         Initbc95;
    NB_initUdpServer    InitUDPServer;
    NB_SendToUdp        SendToUdp;
    NB_RecFromUdp       RecFromUdp;
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
extern uint8_t initUDPServer_bc95(NBModule bc95);
extern uint8_t sendToUdp_bc95(NBModule bc95);
extern uint8_t recFromUdp_bc95(NBModule bc95);
extern uint8_t bc95Main(NBModule bc95);
void InitATcmd(atcmdInfo cmdinfo, const char* cmd, char* param, cmd_type property);
uint16_t formatATcmd(atcmdInfo cmdinfo);
uint8_t NBbc95SendCMD(bc95object bc95, atcmdInfo cmdinfo);




#endif

