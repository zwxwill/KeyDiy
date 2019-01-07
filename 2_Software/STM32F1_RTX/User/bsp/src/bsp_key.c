/*
*********************************************************************************************************
*
*   ģ������ : ������������ģ��
*   �ļ����� : bsp_key.c
*   ��    �� : V1.0
*   ˵    �� : ɨ�������������������˲����ƣ����а���FIFO�����Լ�������¼���
*               (1) ��������
*               (2) ��������
*               (3) ������
*               (4) ����ʱ�Զ�����
*
*   �޸ļ�¼ :
*       �汾��  ����        ����     ˵��
*       V1.0    2013-02-01 armfly  ��ʽ����
*       V1.1    2013-06-29 armfly  ����1����ָ�룬����bsp_Idle() ������ȡϵͳ������ϼ���������
*                                  ���� K1 K2 ��ϼ� �� K2 K3 ��ϼ�������ϵͳ����
*       V1.2    2015-08-08 armfly  K1��K2��K3�����������������жϣ��޸�  IsKeyDown1()�Ⱥ���
*
*   Copyright (C), 2015-2016, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"


/*
    KeyBoard For PC
*/

/* �����ڶ�Ӧ��RCCʱ�� */
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
static KEY_FIFO_T s_tKey;       /* ����FIFO����,�ṹ�� */

static void bsp_InitKeyVar(void);
static void bsp_InitKeyHard(void);
static void bsp_DetectKey(uint8_t i);


/*
*********************************************************************************************************
*   �� �� ��: bsp_InitKey
*   ����˵��: ��ʼ������. �ú����� bsp_Init() ���á�
*   ��    ��:  ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitKey(void)
{
    bsp_InitKeyVar();       /* ��ʼ���������� */
    bsp_InitKeyHard();      /* ��ʼ������Ӳ�� */
}

/*
*********************************************************************************************************
*   �� �� ��: bsp_PutKey
*   ����˵��: ��1����ֵѹ�밴��FIFO��������������ģ��һ��������
*   ��    ��:  _KeyCode : ��������
*   �� �� ֵ: ��
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
*   �� �� ��: bsp_GetKey
*   ����˵��: �Ӱ���FIFO��������ȡһ����ֵ��
*   ��    ��:  ��
*   �� �� ֵ: ��������
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
*   �� �� ��: bsp_GetKey2
*   ����˵��: �Ӱ���FIFO��������ȡһ����ֵ�������Ķ�ָ�롣
*   ��    ��:  ��
*   �� �� ֵ: ��������
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
*   �� �� ��: bsp_GetKeyState
*   ����˵��: ��ȡ������״̬
*   ��    ��:  _ucKeyID : ����ID����0��ʼ
*   �� �� ֵ: 1 ��ʾ���£� 0 ��ʾδ����
*********************************************************************************************************
*/
uint8_t bsp_GetKeyState(KEY_ID_E _ucKeyID)
{
    return s_tBtn[_ucKeyID].State;
}

/*
*********************************************************************************************************
*   �� �� ��: bsp_SetKeyParam
*   ����˵��: ���ð�������
*   ��    �Σ�_ucKeyID : ����ID����0��ʼ
*           _LongTime : �����¼�ʱ��
*            _RepeatSpeed : �����ٶ�
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed)
{
    s_tBtn[_ucKeyID].LongTime = _LongTime;          /* ����ʱ�� 0 ��ʾ����ⳤ�����¼� */
    s_tBtn[_ucKeyID].RepeatSpeed = _RepeatSpeed;            /* �����������ٶȣ�0��ʾ��֧������ */
    s_tBtn[_ucKeyID].RepeatCount = 0;                       /* ���������� */
}


/*
*********************************************************************************************************
*   �� �� ��: bsp_ClearKey
*   ����˵��: ��հ���FIFO������
*   ��    �Σ���
*   �� �� ֵ: ��������
*********************************************************************************************************
*/
void bsp_ClearKey(void)
{
    s_tKey.Read = s_tKey.Write;
}

/*
*********************************************************************************************************
*   �� �� ��: bsp_InitKeyHard
*   ����˵��: ���ð�����Ӧ��GPIO
*   ��    ��:  ��
*   �� �� ֵ: ��
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
*   �� �� ��: bsp_InitKeyVar
*   ����˵��: ��ʼ����������
*   ��    ��:  ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_InitKeyVar(void)
{
    uint8_t i;

    /* �԰���FIFO��дָ������ */
    s_tKey.Read = 0;
    s_tKey.Write = 0;
    s_tKey.Read2 = 0;

    /* ��ÿ�������ṹ���Ա������һ��ȱʡֵ */
    for(i = 0; i < KEY_COUNT; i++)
    {
        s_tBtn[i].LongTime = KEY_LONG_TIME;         /* ����ʱ�� 0 ��ʾ����ⳤ�����¼� */
        s_tBtn[i].Count = KEY_FILTER_TIME / 2;      /* ����������Ϊ�˲�ʱ���һ�� */
        s_tBtn[i].State = 0;                            /* ����ȱʡ״̬��0Ϊδ���� */
        //s_tBtn[i].KeyCodeDown = 3 * i + 1;                /* �������µļ�ֵ���� */
        //s_tBtn[i].KeyCodeUp   = 3 * i + 2;                /* ��������ļ�ֵ���� */
        //s_tBtn[i].KeyCodeLong = 3 * i + 3;                /* �������������µļ�ֵ���� */
        s_tBtn[i].RepeatSpeed = 0;                      /* �����������ٶȣ�0��ʾ��֧������ */
        s_tBtn[i].RepeatCount = 0;                      /* ���������� */
    }

    /* �����Ҫ��������ĳ�������Ĳ����������ڴ˵������¸�ֵ */
    /* ���磬����ϣ������1���³���1����Զ��ط���ͬ��ֵ */
    s_tBtn[KID_JOY_U].LongTime = 100;
    s_tBtn[KID_JOY_U].RepeatSpeed = 5;  /* ÿ��50ms�Զ����ͼ�ֵ */

    s_tBtn[KID_JOY_D].LongTime = 100;
    s_tBtn[KID_JOY_D].RepeatSpeed = 5;  /* ÿ��50ms�Զ����ͼ�ֵ */

    s_tBtn[KID_JOY_L].LongTime = 100;
    s_tBtn[KID_JOY_L].RepeatSpeed = 5;  /* ÿ��50ms�Զ����ͼ�ֵ */

    s_tBtn[KID_JOY_R].LongTime = 100;
    s_tBtn[KID_JOY_R].RepeatSpeed = 5;  /* ÿ��50ms�Զ����ͼ�ֵ */

    /* �жϰ������µĺ��� */
//    s_tBtn[0].IsKeyDownFunc = IsKeyDown1;
//    s_tBtn[1].IsKeyDownFunc = IsKeyDown2;
//    s_tBtn[2].IsKeyDownFunc = IsKeyDown3;
//    s_tBtn[3].IsKeyDownFunc = IsKeyDown4;
//    s_tBtn[4].IsKeyDownFunc = IsKeyDown5;
//    s_tBtn[5].IsKeyDownFunc = IsKeyDown6;
//    s_tBtn[6].IsKeyDownFunc = IsKeyDown7;
//    s_tBtn[7].IsKeyDownFunc = IsKeyDown8;

//    /* ��ϼ� */
//    s_tBtn[8].IsKeyDownFunc = IsKeyDown9;
//    s_tBtn[9].IsKeyDownFunc = IsKeyDown10;
}

/*
*********************************************************************************************************
*   �� �� ��: bsp_DetectKey
*   ����˵��: ���һ��������������״̬�����뱻�����Եĵ��á�
*   ��    ��:  �����ṹ����ָ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_DetectKey(uint8_t i)
{
    KEY_T *pBtn;

    /*
        ���û�г�ʼ�������������򱨴�
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

                /* ���Ͱ�ť���µ���Ϣ */
                bsp_PutKey((uint8_t)(3 * i + 1));
            }

            if(pBtn->LongTime > 0)
            {
                if(pBtn->LongCount < pBtn->LongTime)
                {
                    /* ���Ͱ�ť�������µ���Ϣ */
                    if(++pBtn->LongCount == pBtn->LongTime)
                    {
                        /* ��ֵ���밴��FIFO */
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
                            /* ��������ÿ��10ms����1������ */
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

                /* ���Ͱ�ť�������Ϣ */
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
*   �� �� ��: bsp_KeyScan
*   ����˵��: ɨ�����а���������������systick�ж������Եĵ���
*   ��    ��:  ��
*   �� �� ֵ: ��
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

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
