#include "dma_fun.h"
#include <string.h>
#include "stm32f1xx_hal.h"


#define DMA_NEW_RX_BYTE(IDX)    (0x00 == ((dmaR_Conf.recBuf[(IDX)]) >> 8))
#define DMA_GET_RX_BYTE(IDX)    (dmaR_Conf.recBuf[(IDX)] & 0xff)
#define DMA_CLR_RX_BYTE(IDX)    (dmaR_Conf.recBuf[(IDX)] = 0xffff)
#define HAL_USART_DMA_IDLE      (10)

#define HAL_USART_DMA_FULL      (RECE_BUF_MAX_SIZE - 16)

static dmaRec_cfg dmaR_Conf;
static sendData_Callback dmaSendData_cb;
static USART_TypeDef* hDMAUsart = 0;



//��ʼ��dmaRec_cfg�ṹ������rx Buffer
uint8_t* usartDMA_Init(sendData_Callback sendCb, USART_TypeDef* hUsart) 
{
    memset(dmaR_Conf.recBuf, 0xff, RECE_BUF_MAX_SIZE << 1);
    dmaR_Conf.rxHead = 0;
    dmaR_Conf.rxEnd = 0;
    dmaR_Conf.rxTimeoutTick = 0;
    dmaR_Conf.rxTimeoutCount = 0;
    dmaR_Conf.txIdx[0] = 0;
    dmaR_Conf.txIdx[1] = 0;
    dmaR_Conf.txSel = 0;
    dmaR_Conf.txTimeoutCountValid = 0;
    dmaR_Conf.txDMApending = 0;
    dmaSendData_cb = sendCb;
    hDMAUsart = hUsart;
    return (uint8_t*)dmaR_Conf.recBuf;
}

//�ӽ��ջ����������ȡָ�����ȵ����ݣ����ͷ�ռ�ÿռ�
//����ֵΪ��ȡ���ĳ���,�ֽ�
uint16_t usartDMA_Read(uint8_t *buf, uint16_t len)
{
    uint16_t count;
    for (count = 0; count < len; count++) {
        //���ͷ��������Ч
        if (!DMA_NEW_RX_BYTE(dmaR_Conf.rxHead)) 
            break;
        //��ͷ�����ݴ��buf
        *buf++ = DMA_GET_RX_BYTE(dmaR_Conf.rxHead);
        //���������
        DMA_CLR_RX_BYTE(dmaR_Conf.rxHead);
        if (++(dmaR_Conf.rxHead) >= RECE_BUF_MAX_SIZE)
            dmaR_Conf.rxHead = 0;
    }
    return count;
}

//��Ҫ���͵����ݿ�����ָ����������
uint16_t usartDMA_Write(uint8_t *buf, uint16_t len)
{
    uint16_t count;
    uint8_t txSel;
    uint8_t txIdx;
    
    if (len + dmaR_Conf.txIdx[txSel] > SEND_BUF_MAX_SIZE)
        return 0;
    txSel = dmaR_Conf.txSel;
    txIdx = dmaR_Conf.txIdx[txSel];
    
    for (count = 0; count < len; count++) {
        dmaR_Conf.sendBuf[txSel][txIdx++] = buf[count];
    }
    
    if (txSel != dmaR_Conf.txSel) {
        txSel = dmaR_Conf.txSel;
        txIdx = dmaR_Conf.txIdx[txSel];
        for (count = 0; count < len; count++) {
            dmaR_Conf.sendBuf[txSel][txIdx++] = buf[count];
        }
    }
    
    if (dmaR_Conf.txIdx[(txSel ^ 1)] == 0)
        dmaR_Conf.txDMApending = 1;
    
    return count;
}


//���㻺�������ݸ���
uint16_t usartDMA_Avail(void)
{
    uint16_t count = 0;
    if (DMA_NEW_RX_BYTE(dmaR_Conf.rxHead))// ?? ����!��
    {
        for (uint16_t idx = 0; idx < RECE_BUF_MAX_SIZE; idx++) {
            if (DMA_NEW_RX_BYTE(idx)) {
                count++;
            }
        }
    }
    
    return count;
}

static uint16_t findTail(void)
{
    uint16_t idx = dmaR_Conf.rxHead;
    
    do {
        if (!DMA_NEW_RX_BYTE(idx))
            break;
        if (++idx >= RECE_BUF_MAX_SIZE)
            idx = 0;
    }while (idx != dmaR_Conf.rxHead);
    return idx;
}


//��ѯ���ջ��������ݳ���
uint8_t usartDMA_Poll_RecAvail(void)
{
    uint16_t count = 0;
    uint16_t evt = 0;
    if (DMA_NEW_RX_BYTE(dmaR_Conf.rxHead)) {
        uint16_t tail = findTail();
        
        if (dmaR_Conf.rxEnd != tail) {
            dmaR_Conf.rxEnd = tail;
            if (dmaR_Conf.rxTimeoutTick == 0)
                dmaR_Conf.rxTimeoutCount = HAL_GetTick();
            dmaR_Conf.rxTimeoutTick = HAL_USART_DMA_IDLE;
        }
        
        else if (dmaR_Conf.rxTimeoutTick) {
            uint32_t tick = HAL_GetTick();
            uint32_t delta = tick >= dmaR_Conf.rxTimeoutTick?
                            (tick - dmaR_Conf.rxTimeoutTick):
                            (tick + (UINT32_MAX - dmaR_Conf.rxTimeoutTick));
            if (dmaR_Conf.rxTimeoutTick > delta) {
                dmaR_Conf.rxTimeoutTick -= delta;
                dmaR_Conf.rxTimeoutCount = tick;
            }
            else 
                dmaR_Conf.rxTimeoutTick = 0;
        }
        count = usartDMA_Avail();
    }
    else 
        dmaR_Conf.rxTimeoutTick = 0;
    if (count >= HAL_USART_DMA_FULL)
        evt = HAL_USART_RX_FULL;
    else if (count && !dmaR_Conf.rxTimeoutTick)
        evt = HAL_USART_RX_TIMEOUT;
    
    if (dmaR_Conf.txTimeoutCountValid) {
        uint32_t decr = HAL_GetTick() - dmaR_Conf.txTimeoutCount;
        if (decr > dmaR_Conf.txTimeoutTick)
            dmaR_Conf.txTimeoutCountValid = 0;
    }
    if (dmaR_Conf.txDMApending && !dmaR_Conf.txTimeoutCountValid) {
        dmaR_Conf.txDMApending = 0;
        if (dmaSendData_cb)
            dmaSendData_cb(dmaR_Conf.sendBuf[dmaR_Conf.txSel],
                           dmaR_Conf.txIdx[dmaR_Conf.txSel]);
        dmaR_Conf.txSel ^= 1;
    }
    
    return evt;
}

//��д�������ݻص�����
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == hDMAUsart) {
        dmaR_Conf.txIdx[(dmaR_Conf.txSel ^ 1)] = 0;
        dmaR_Conf.txTimeoutCount = HAL_GetTick();
        dmaR_Conf.txTimeoutCountValid = 1;
        
        if (dmaR_Conf.txIdx[(dmaR_Conf.txSel)])
            dmaR_Conf.txDMApending = 1;
    }
}

