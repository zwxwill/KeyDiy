/*
*********************************************************************************************************
*
*   ģ������ : LEDָʾ������ģ��
*   �ļ����� : bsp_led.c
*   ��    �� : V1.0
*   ˵    �� : ����LEDָʾ��
*
*   �޸ļ�¼ :
*       �汾��  ����        ����     ˵��
*       V1.0    2013-02-01 armfly  ��ʽ����
*
*   Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

/*
    We use 74HC595D to drive led.
*/


/* �����ڶ�Ӧ��RCCʱ�� */
#define RCC_ALL_74HC595D    (RCC_APB2Periph_GPIOA)

#define GPIO_PORT_SRST  GPIOA
#define GPIO_PIN_SRST   GPIO_Pin_4

#define GPIO_PORT_SCK    GPIOA
#define GPIO_PIN_SCK    GPIO_Pin_5

#define GPIO_PORT_SER    GPIOA
#define GPIO_PIN_SER    GPIO_Pin_6

#define GPIO_PORT_RCK    GPIOA
#define GPIO_PIN_RCK    GPIO_Pin_7

/*
*********************************************************************************************************
*   �� �� ��: bsp_InitLed
*   ����˵��: ����LEDָʾ����ص�GPIO,  �ú����� bsp_Init() ���á�
*   ��    ��:  ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitLed(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* ��GPIOʱ�� */
    RCC_APB2PeriphClockCmd(RCC_ALL_74HC595D, ENABLE);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    /* �������ģʽ */

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SRST;
    GPIO_Init(GPIO_PORT_SRST, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SCK;
    GPIO_Init(GPIO_PORT_SCK, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SER;
    GPIO_Init(GPIO_PORT_SER, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_RCK;
    GPIO_Init(GPIO_PORT_RCK, &GPIO_InitStructure);

    GPIO_ResetBits(GPIO_PORT_SRST, GPIO_PIN_SRST);
    GPIO_ResetBits(GPIO_PORT_SCK, GPIO_PIN_SCK);
    GPIO_ResetBits(GPIO_PORT_SER, GPIO_PIN_SER);
    GPIO_ResetBits(GPIO_PORT_RCK, GPIO_PIN_RCK);
}

/*
*********************************************************************************************************
*   �� �� ��: bsp_LedOn
*   ����˵��: ����ָ����LEDָʾ�ơ�
*   ��    ��:  _no : ָʾ����ţ���Χ 1 - 4
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_LedOn(uint8_t _no)
{

}

/*
*********************************************************************************************************
*   �� �� ��: bsp_LedOff
*   ����˵��: Ϩ��ָ����LEDָʾ�ơ�
*   ��    ��:  _no : ָʾ����ţ���Χ 1 - 4
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_LedOff(uint8_t _no)
{

}

/*
*********************************************************************************************************
*   �� �� ��: bsp_LedToggle
*   ����˵��: ��תָ����LEDָʾ�ơ�
*   ��    ��:  _no : ָʾ����ţ���Χ 1 - 4
*   �� �� ֵ: ��������
*********************************************************************************************************
*/
void bsp_LedToggle(uint8_t _no)
{

}

/*
*********************************************************************************************************
*   �� �� ��: bsp_IsLedOn
*   ����˵��: �ж�LEDָʾ���Ƿ��Ѿ�������
*   ��    ��:  _no : ָʾ����ţ���Χ 1 - 4
*   �� �� ֵ: 1��ʾ�Ѿ�������0��ʾδ����
*********************************************************************************************************
*/
uint8_t bsp_IsLedOn(uint8_t _no)
{


    return 0;
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
