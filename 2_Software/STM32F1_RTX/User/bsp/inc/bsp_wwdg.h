/*
*********************************************************************************************************
*
*   模块名称 : 窗口看门狗程序头文件
*   文件名称 : bsp_wwdg.h
*   版    本 : V1.0
*   说    明 : IWDG例程。
*   修改记录 :
*       版本号    日期          作者                 说明
*       v1.0    2012-12-23   WildFire Team  ST固件库版本 V3.5.0版本。
*
*   Copyright (C), 2012-2013, WildFire Team
*********************************************************************************************************
*/

#ifndef _BSP_WWDG_H
#define _BSP_WWDG_H

/* 初始化 */
void bsp_InitWwdg(uint8_t _ucTreg, uint8_t _ucWreg, uint32_t WWDG_Prescaler);

#endif
