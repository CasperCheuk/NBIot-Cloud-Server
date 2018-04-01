#ifndef __DMA_FUN_H__
#define __DMA_FUN_H__
#ifdef __cpluscplus
extern C {
#endif

#include "stm32f1xx_hal.h"
    
    
#define SEND_BUF_MAX_SIZE   256
#define RECE_BUF_MAX_SIZE   128
    
    
#define HAL_USART_RX_FULL       0X01
#define HAL_USART_RX_TIMEOUT    0X02
    
typedef struct DMA_Rece_Config {
    uint16_t recBuf[RECE_BUF_MAX_SIZE]; //��������С
    uint16_t rxHead;                    //��Ч����ͷ��
    uint16_t rxEnd;                     //���һ������ĩβ
    uint8_t  rxTimeoutTick;             //��ʱʱ��,ms
    uint32_t rxTimeoutCount;            //��ʱ������ʼ��ֵ
    
    uint8_t  sendBuf[2][SEND_BUF_MAX_SIZE];//���ͻ�����
    uint8_t  txIdx[2];
    uint8_t  txSel;
    uint8_t  txTimeoutTick;
    volatile uint32_t txTimeoutCount;
    volatile uint8_t  txTimeoutCountValid;  //txTimeoutCount�Ƿ���Ч
    uint8_t  txDMApending;              //
}dmaRec_cfg;


typedef void (*sendData_Callback)(uint8_t *buf, uint16_t len);


//��ʼ��dmaRec_cfg�ṹ������rx Buffer
extern uint8_t* usartDMA_Init(sendData_Callback, USART_TypeDef*);

//�ӽ��ջ����������ȡָ�����ȵ����ݣ����ͷ�ռ�ÿռ�
extern uint16_t usartDMA_Read(uint8_t *buf, uint16_t len);

//��Ҫ���͵����ݿ�����ָ����������
extern uint16_t usartDMA_Write(uint8_t *buf, uint16_t len);

//���㻺�������ݸ���
extern uint16_t usartDMA_Avail(void);

//�ҵ���ǰDMA���ջ��������ڲ�����λ��
static uint16_t findTail(void);

//��ѯ���ջ��������ݳ���
extern uint8_t usartDMA_Poll_RecAvail(void);

#ifdef __cpluscplus
}
#endif

#endif

