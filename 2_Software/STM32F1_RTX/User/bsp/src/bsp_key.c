/*
*********************************************************************************************************
*
*   模块名称 : 独立按键驱动模块
*   文件名称 : bsp_key.c
*   版    本 : V1.0
*   说    明 : 扫描独立按键，具有软件滤波机制，具有按键FIFO。可以检测如下事件：
*               (1) 按键按下
*               (2) 按键弹起
*               (3) 长按键
*               (4) 长按时自动连发
*
*   修改记录 :
*       版本号  日期        作者     说明
*       V1.0    2013-02-01 armfly  正式发布
*       V1.1    2013-06-29 armfly  增加1个读指针，用于bsp_Idle() 函数读取系统控制组合键（截屏）
*                                  增加 K1 K2 组合键 和 K2 K3 组合键，用于系统控制
*       V1.2    2015-08-08 armfly  K1，K2，K3独立按键进行排他判断，修改  IsKeyDown1()等函数
*
*   Copyright (C), 2015-2016, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"


/*
    KeyBoard For PC
*/

/* 按键口对应的RCC时钟 */
#define RCC_ALL_KEY     (RCC_APB2Periph_GPIOB)

#define GPIO_PORT_ROW      GPIOB
#define GPIO_PORT_COL       GPIOB

#define GPIO_PORT_ROW1    GPIOB
#define GPIO_PIN_ROW1      GPIO_Pin_0

#define GPIO_PORT_ROW2    GPIOB
#define GPIO_PIN_ROW2      GPIO_Pin_1

#define GPIO_PORT_ROW3    GPIOB
#define GPIO_PIN_ROW3      GPIO_Pin_2

#define GPIO_PORT_ROW4    GPIOB
#define GPIO_PIN_ROW4      GPIO_Pin_3

#define GPIO_PORT_ROW5    GPIOB
#define GPIO_PIN_ROW5      GPIO_Pin_4

#define GPIO_PORT_ROW6    GPIOB
#define GPIO_PIN_ROW6      GPIO_Pin_5

#define GPIO_PORT_ROW7    GPIOB
#define GPIO_PIN_ROW7      GPIO_Pin_6

#define GPIO_PORT_ROW8    GPIOB
#define GPIO_PIN_ROW8      GPIO_Pin_7


#define GPIO_PORT_COL1    GPIOB
#define GPIO_PIN_COL1      GPIO_Pin_8

#define GPIO_PORT_COL2    GPIOB
#define GPIO_PIN_COL2      GPIO_Pin_9

#define GPIO_PORT_COL3    GPIOB
#define GPIO_PIN_COL3      GPIO_Pin_10

#define GPIO_PORT_COL4    GPIOB
#define GPIO_PIN_COL4      GPIO_Pin_11

#define GPIO_PORT_COL5    GPIOB
#define GPIO_PIN_COL5      GPIO_Pin_12

#define GPIO_PORT_COL6    GPIOB
#define GPIO_PIN_COL6      GPIO_Pin_13

#define GPIO_PORT_COL7    GPIOB
#define GPIO_PIN_COL7      GPIO_Pin_14

#define GPIO_PORT_COL8    GPIOB
#define GPIO_PIN_COL8      GPIO_Pin_15



static KEY_T s_tBtn[KEY_COUNT];
static KEY_FIFO_T s_tKey;       /* 按键FIFO变量,结构体 */

static void bsp_InitKeyVar(void);
static void bsp_InitKeyHard(void);
static void bsp_DetectKey(uint8_t i);


/*
*********************************************************************************************************
*   函 数 名: bsp_InitKey
*   功能说明: 初始化按键. 该函数被 bsp_Init() 调用。
*   形    参:  无
*   返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitKey(void)
{
    bsp_InitKeyVar();       /* 初始化按键变量 */
    bsp_InitKeyHard();      /* 初始化按键硬件 */
}

/*
*********************************************************************************************************
*   函 数 名: bsp_PutKey
*   功能说明: 将1个键值压入按键FIFO缓冲区。可用于模拟一个按键。
*   形    参:  _KeyCode : 按键代码
*   返 回 值: 无
*********************************************************************************************************
*/
void bsp_PutKey(uint8_t _KeyCode)
{
    s_tKey.Buf[s_tKey.Write] = _KeyCode;

    if(++s_tKey.Write  >= KEY_FIFO_SIZE)
    {
        s_tKey.Write = 0;
    }
}

/*
*********************************************************************************************************
*   函 数 名: bsp_GetKey
*   功能说明: 从按键FIFO缓冲区读取一个键值。
*   形    参:  无
*   返 回 值: 按键代码
*********************************************************************************************************
*/
uint8_t bsp_GetKey(void)
{
    uint8_t ret;

    if(s_tKey.Read == s_tKey.Write)
    {
        return KEY_NONE;
    }
    else
    {
        ret = s_tKey.Buf[s_tKey.Read];

        if(++s_tKey.Read >= KEY_FIFO_SIZE)
        {
            s_tKey.Read = 0;
        }
        return ret;
    }
}

/*
*********************************************************************************************************
*   函 数 名: bsp_GetKey2
*   功能说明: 从按键FIFO缓冲区读取一个键值。独立的读指针。
*   形    参:  无
*   返 回 值: 按键代码
*********************************************************************************************************
*/
uint8_t bsp_GetKey2(void)
{
    uint8_t ret;

    if(s_tKey.Read2 == s_tKey.Write)
    {
        return KEY_NONE;
    }
    else
    {
        ret = s_tKey.Buf[s_tKey.Read2];

        if(++s_tKey.Read2 >= KEY_FIFO_SIZE)
        {
            s_tKey.Read2 = 0;
        }
        return ret;
    }
}

/*
*********************************************************************************************************
*   函 数 名: bsp_GetKeyState
*   功能说明: 读取按键的状态
*   形    参:  _ucKeyID : 按键ID，从0开始
*   返 回 值: 1 表示按下， 0 表示未按下
*********************************************************************************************************
*/
uint8_t bsp_GetKeyState(KEY_ID_E _ucKeyID)
{
    return s_tBtn[_ucKeyID].State;
}

/*
*********************************************************************************************************
*   函 数 名: bsp_SetKeyParam
*   功能说明: 设置按键参数
*   形    参：_ucKeyID : 按键ID，从0开始
*           _LongTime : 长按事件时间
*            _RepeatSpeed : 连发速度
*   返 回 值: 无
*********************************************************************************************************
*/
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed)
{
    s_tBtn[_ucKeyID].LongTime = _LongTime;          /* 长按时间 0 表示不检测长按键事件 */
    s_tBtn[_ucKeyID].RepeatSpeed = _RepeatSpeed;            /* 按键连发的速度，0表示不支持连发 */
    s_tBtn[_ucKeyID].RepeatCount = 0;                       /* 连发计数器 */
}


/*
*********************************************************************************************************
*   函 数 名: bsp_ClearKey
*   功能说明: 清空按键FIFO缓冲区
*   形    参：无
*   返 回 值: 按键代码
*********************************************************************************************************
*/
void bsp_ClearKey(void)
{
    s_tKey.Read = s_tKey.Write;
}

/*
*********************************************************************************************************
*   函 数 名: bsp_InitKeyHard
*   功能说明: 配置按键对应的GPIO
*   形    参:  无
*   返 回 值: 无
*********************************************************************************************************
*/
static void bsp_InitKeyHard(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_ALL_KEY, ENABLE);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   /* Push-Pull output */

    /* row */
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ROW1;
    GPIO_Init(GPIO_PORT_ROW1, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ROW2;
    GPIO_Init(GPIO_PORT_ROW2, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ROW3;
    GPIO_Init(GPIO_PORT_ROW3, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ROW4;
    GPIO_Init(GPIO_PORT_ROW4, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ROW5;
    GPIO_Init(GPIO_PORT_ROW5, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ROW6;
    GPIO_Init(GPIO_PORT_ROW6, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ROW7;
    GPIO_Init(GPIO_PORT_ROW7, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ROW8;
    GPIO_Init(GPIO_PORT_ROW8, &GPIO_InitStructure);

    /* column */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   /* Pull up input */

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_COL1;
    GPIO_Init(GPIO_PORT_COL1, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_COL2;
    GPIO_Init(GPIO_PORT_COL2, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_COL3;
    GPIO_Init(GPIO_PORT_COL3, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_COL4;
    GPIO_Init(GPIO_PORT_COL4, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_COL5;
    GPIO_Init(GPIO_PORT_COL5, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_COL6;
    GPIO_Init(GPIO_PORT_COL6, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_COL7;
    GPIO_Init(GPIO_PORT_COL7, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_COL8;
    GPIO_Init(GPIO_PORT_COL8, &GPIO_InitStructure);


}

/*
*********************************************************************************************************
*   函 数 名: bsp_InitKeyVar
*   功能说明: 初始化按键变量
*   形    参:  无
*   返 回 值: 无
*********************************************************************************************************
*/
static void bsp_InitKeyVar(void)
{
    uint8_t i;

    /* 对按键FIFO读写指针清零 */
    s_tKey.Read = 0;
    s_tKey.Write = 0;
    s_tKey.Read2 = 0;

    /* 给每个按键结构体成员变量赋一组缺省值 */
    for(i = 0; i < KEY_COUNT; i++)
    {
        s_tBtn[i].LongTime = KEY_LONG_TIME;         /* 长按时间 0 表示不检测长按键事件 */
        s_tBtn[i].Count = KEY_FILTER_TIME / 2;      /* 计数器设置为滤波时间的一半 */
        s_tBtn[i].State = 0;                            /* 按键缺省状态，0为未按下 */
        //s_tBtn[i].KeyCodeDown = 3 * i + 1;                /* 按键按下的键值代码 */
        //s_tBtn[i].KeyCodeUp   = 3 * i + 2;                /* 按键弹起的键值代码 */
        //s_tBtn[i].KeyCodeLong = 3 * i + 3;                /* 按键被持续按下的键值代码 */
        s_tBtn[i].RepeatSpeed = 0;                      /* 按键连发的速度，0表示不支持连发 */
        s_tBtn[i].RepeatCount = 0;                      /* 连发计数器 */
    }

    /* 如果需要单独更改某个按键的参数，可以在此单独重新赋值 */
    /* 比如，我们希望按键1按下超过1秒后，自动重发相同键值 */
    s_tBtn[KID_JOY_U].LongTime = 100;
    s_tBtn[KID_JOY_U].RepeatSpeed = 5;  /* 每隔50ms自动发送键值 */

    s_tBtn[KID_JOY_D].LongTime = 100;
    s_tBtn[KID_JOY_D].RepeatSpeed = 5;  /* 每隔50ms自动发送键值 */

    s_tBtn[KID_JOY_L].LongTime = 100;
    s_tBtn[KID_JOY_L].RepeatSpeed = 5;  /* 每隔50ms自动发送键值 */

    s_tBtn[KID_JOY_R].LongTime = 100;
    s_tBtn[KID_JOY_R].RepeatSpeed = 5;  /* 每隔50ms自动发送键值 */

    /* 判断按键按下的函数 */
//    s_tBtn[0].IsKeyDownFunc = IsKeyDown1;
//    s_tBtn[1].IsKeyDownFunc = IsKeyDown2;
//    s_tBtn[2].IsKeyDownFunc = IsKeyDown3;
//    s_tBtn[3].IsKeyDownFunc = IsKeyDown4;
//    s_tBtn[4].IsKeyDownFunc = IsKeyDown5;
//    s_tBtn[5].IsKeyDownFunc = IsKeyDown6;
//    s_tBtn[6].IsKeyDownFunc = IsKeyDown7;
//    s_tBtn[7].IsKeyDownFunc = IsKeyDown8;

//    /* 组合键 */
//    s_tBtn[8].IsKeyDownFunc = IsKeyDown9;
//    s_tBtn[9].IsKeyDownFunc = IsKeyDown10;
}

/*
*********************************************************************************************************
*   函 数 名: bsp_DetectKey
*   功能说明: 检测一个按键。非阻塞状态，必须被周期性的调用。
*   形    参:  按键结构变量指针
*   返 回 值: 无
*********************************************************************************************************
*/
static void bsp_DetectKey(uint8_t i)
{
    KEY_T *pBtn;

    /*
        如果没有初始化按键函数，则报错
        if (s_tBtn[i].IsKeyDownFunc == 0)
        {
            printf("Fault : DetectButton(), s_tBtn[i].IsKeyDownFunc undefine");
        }
    */

    pBtn = &s_tBtn[i];
    if(pBtn->IsKeyDownFunc())
    {
        if(pBtn->Count < KEY_FILTER_TIME)
        {
            pBtn->Count = KEY_FILTER_TIME;
        }
        else if(pBtn->Count < 2 * KEY_FILTER_TIME)
        {
            pBtn->Count++;
        }
        else
        {
            if(pBtn->State == 0)
            {
                pBtn->State = 1;

                /* 发送按钮按下的消息 */
                bsp_PutKey((uint8_t)(3 * i + 1));
            }

            if(pBtn->LongTime > 0)
            {
                if(pBtn->LongCount < pBtn->LongTime)
                {
                    /* 发送按钮持续按下的消息 */
                    if(++pBtn->LongCount == pBtn->LongTime)
                    {
                        /* 键值放入按键FIFO */
                        bsp_PutKey((uint8_t)(3 * i + 3));
                    }
                }
                else
                {
                    if(pBtn->RepeatSpeed > 0)
                    {
                        if(++pBtn->RepeatCount >= pBtn->RepeatSpeed)
                        {
                            pBtn->RepeatCount = 0;
                            /* 常按键后，每隔10ms发送1个按键 */
                            bsp_PutKey((uint8_t)(3 * i + 1));
                        }
                    }
                }
            }
        }
    }
    else
    {
        if(pBtn->Count > KEY_FILTER_TIME)
        {
            pBtn->Count = KEY_FILTER_TIME;
        }
        else if(pBtn->Count != 0)
        {
            pBtn->Count--;
        }
        else
        {
            if(pBtn->State == 1)
            {
                pBtn->State = 0;

                /* 发送按钮弹起的消息 */
                bsp_PutKey((uint8_t)(3 * i + 2));
            }
        }

        pBtn->LongCount = 0;
        pBtn->RepeatCount = 0;
    }
}



void test(void)
{
    GPIO_Write(GPIO_PORT_ROW, 0xFE);

}


/*
*********************************************************************************************************
*   函 数 名: bsp_KeyScan
*   功能说明: 扫描所有按键。非阻塞，被systick中断周期性的调用
*   形    参:  无
*   返 回 值: 无
*********************************************************************************************************
*/
void bsp_KeyScan(void)
{
    uint8_t i;

    for(i = 0; i < KEY_COUNT; i++)
    {
        bsp_DetectKey(i);
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
