/*
*********************************************************************************************************
*
*   模块名称 : LED指示灯驱动模块
*   文件名称 : bsp_led.c
*   版    本 : V1.0
*   说    明 : 驱动LED指示灯
*
*   修改记录 :
*       版本号  日期        作者     说明
*       V1.0    2013-02-01 armfly  正式发布
*
*   Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

/*
    We use 74HC595D to drive led.
*/


/* 按键口对应的RCC时钟 */
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
*   函 数 名: bsp_InitLed
*   功能说明: 配置LED指示灯相关的GPIO,  该函数被 bsp_Init() 调用。
*   形    参:  无
*   返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitLed(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 打开GPIO时钟 */
    RCC_APB2PeriphClockCmd(RCC_ALL_74HC595D, ENABLE);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    /* 推挽输出模式 */

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
*   函 数 名: bsp_LedOn
*   功能说明: 点亮指定的LED指示灯。
*   形    参:  _no : 指示灯序号，范围 1 - 4
*   返 回 值: 无
*********************************************************************************************************
*/
void bsp_LedOn(uint8_t _no)
{

}

/*
*********************************************************************************************************
*   函 数 名: bsp_LedOff
*   功能说明: 熄灭指定的LED指示灯。
*   形    参:  _no : 指示灯序号，范围 1 - 4
*   返 回 值: 无
*********************************************************************************************************
*/
void bsp_LedOff(uint8_t _no)
{

}

/*
*********************************************************************************************************
*   函 数 名: bsp_LedToggle
*   功能说明: 翻转指定的LED指示灯。
*   形    参:  _no : 指示灯序号，范围 1 - 4
*   返 回 值: 按键代码
*********************************************************************************************************
*/
void bsp_LedToggle(uint8_t _no)
{

}

/*
*********************************************************************************************************
*   函 数 名: bsp_IsLedOn
*   功能说明: 判断LED指示灯是否已经点亮。
*   形    参:  _no : 指示灯序号，范围 1 - 4
*   返 回 值: 1表示已经点亮，0表示未点亮
*********************************************************************************************************
*/
uint8_t bsp_IsLedOn(uint8_t _no)
{


    return 0;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
