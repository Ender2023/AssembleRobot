/************************************************************
*@file:		dev_rgblcd.h
*@brief:	Һ������ͷ�ļ�
*@author:	Pan ZhiKang
*@date:		2022-10-14
************************************************************/

#ifndef __BSP_RGBLCD_H__
#define __BSP_RGBLCD_H__

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include "system.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************
*					��������ǰ��(��Ҫ�޸�)
************************************************************/
#define LCD_SOFT_SPI_CTRL		0
#define LCD_HARD_SPI_CTRL		1

#define LCD_SOFT_CS_CTRL		0
#define LCD_HARD_CS_CTRL		1

#define LCD_UNUSE_DMA			0
#define LCD_USE_DMA				1

#define LCD_DMA_BUF_MAX			65535
/************************************************************
*				����LCD��Ļ�ĳ���������
************************************************************/
#define LCD_W					128
#define LCD_H					128

/************************************************************
*		����LCD��Ļ�Ľӿ�����Ϊ(��/Ӳ)������ģʽ
************************************************************/
#define LCD_USE_SPI_CTRL_MODE	LCD_HARD_SPI_CTRL

/************************************************************
*		����LCD��Ļר�õ���ʱ����(��ֲ���޸ĸô�)
************************************************************/
#define LCD_Delay_ms(x)			delay_ms(x)
#define LCD_Delay_us(x)			delay_us(x)

#define LCD_NOP();	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
					__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
					__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
					__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();

/***********************************************************/
#if 	LCD_USE_SPI_CTRL_MODE == LCD_SOFT_SPI_CTRL
/************************************************************
*		[SOFT]����LCD��Ļ��MCU�Ľӿ�GPIO(��ֲ���޸ĸô�)
************************************************************/
#define LCD_SCL_PORT			GPIOB
#define LCD_SCL_PIN				GPIO_Pin_13

#define LCD_SDA_PORT			GPIOB							//SDA=MOSI(�˴�����MISO,�ʼ�ΪSDA)
#define LCD_SDA_PIN				GPIO_Pin_15

#define LCD_RES_PORT			GPIOC
#define LCD_RES_PIN				GPIO_Pin_7

#define LCD_DC_PORT				GPIOC
#define LCD_DC_PIN				GPIO_Pin_9

#define LCD_CS_PORT				GPIOC
#define LCD_CS_PIN				GPIO_Pin_10

#define LCD_BLK_PORT			GPIOC
#define LCD_BLK_PIN				GPIO_Pin_11

/************************************************************
*		[SOFT]����MCU����LCD��GPIO�ӿڵľ���ʵ��(��ֲ���޸ĸô�)
************************************************************/
#define LCD_SCL_0()             GPIO_ResetBits(LCD_SCL_PORT,LCD_SCL_PIN)	//SCL=SCLK
#define LCD_SCL_1()             GPIO_SetBits(LCD_SCL_PORT,LCD_SCL_PIN)

#define LCD_SDA_0()             GPIO_ResetBits(LCD_SDA_PORT,LCD_SDA_PIN)	//SDA=MOSI
#define LCD_SDA_1()             GPIO_SetBits(LCD_SDA_PORT,LCD_SDA_PIN)

#define LCD_RES_0()             GPIO_ResetBits(LCD_RES_PORT,LCD_RES_PIN)	//RES
#define LCD_RES_1()             GPIO_SetBits(LCD_RES_PORT,LCD_RES_PIN)

#define LCD_DC_0()              GPIO_ResetBits(LCD_DC_PORT,LCD_DC_PIN)		//DC
#define LCD_DC_1()              GPIO_SetBits(LCD_DC_PORT,LCD_DC_PIN)

#define LCD_CS_0()              GPIO_ResetBits(LCD_CS_PORT,LCD_CS_PIN)		//CS
#define LCD_CS_1()              GPIO_SetBits(LCD_CS_PORT,LCD_CS_PIN)

#define LCD_BLK_0()             GPIO_ResetBits(LCD_BLK_PORT,LCD_BLK_PIN)	//BLK
#define LCD_BLK_1()             GPIO_SetBits(LCD_BLK_PORT,LCD_BLK_PIN)

/***********************************************************/
#elif	LCD_USE_SPI_CTRL_MODE == LCD_HARD_SPI_CTRL
/************************************************************
*		[HARD]�Ƿ�ʹ��DMA������ΪӲ����������
************************************************************/

#define LCD_USE_DMA_CTRL_MODE	LCD_USE_DMA

/***********************************************************/
#if LCD_USE_DMA_CTRL_MODE == LCD_USE_DMA

#define LCD_DMA2SPI_ENABLE();	DMA1_Channel5->CCR |= DMA_CCR1_EN;
#define LCD_DMA2SPI_DISABLE();	DMA1_Channel5->CCR &= (uint16_t)(~DMA_CCR1_EN);
#define LCD_DMA2SPI_SEND();		SPI2->CR2 |= SPI_I2S_DMAReq_Tx;

#endif

/************************************************************
*		[HARD]����LCD��Ļ��MCU�Ľӿ�GPIO
************************************************************/
#define LCD_RES_PORT			GPIOC
#define LCD_RES_PIN				GPIO_Pin_7

#define LCD_DC_PORT				GPIOC
#define LCD_DC_PIN				GPIO_Pin_9

#define LCD_CS_PORT				GPIOC
#define LCD_CS_PIN				GPIO_Pin_10

#define LCD_BLK_PORT			GPIOC
#define LCD_BLK_PIN				GPIO_Pin_11

/************************************************************
*		[HARD]����MCU����LCD��GPIO�ӿڵľ���ʵ��
************************************************************/
#define LCD_RES_0()             LCD_RES_PORT->BSRR=LCD_RES_PIN<<16
#define LCD_RES_1()             LCD_RES_PORT->BSRR=LCD_RES_PIN
                                
#define LCD_DC_0()              LCD_DC_PORT->BSRR=LCD_DC_PIN<<16
#define LCD_DC_1()              LCD_DC_PORT->BSRR=LCD_DC_PIN

#define LCD_CS_0()              LCD_CS_PORT->BSRR=LCD_CS_PIN<<16
#define LCD_CS_1()              LCD_CS_PORT->BSRR=LCD_CS_PIN

#define LCD_BLK_0()             LCD_BLK_PORT->BSRR=LCD_BLK_PIN<<16
#define LCD_BLK_1()             LCD_BLK_PORT->BSRR=LCD_BLK_PIN

#endif

/************************************************************
*					����LCD���������С
************************************************************/
#define LCD_Printf_BufferSize	512U

#if LCD_USE_DMA_CTRL_MODE	== LCD_USE_DMA

#define LCD_DMA_BufferSize		LCD_W*2

#endif

/************************************************************
*				Ԥ��Һ����ɫ��(RGB565)
************************************************************/
#define LCD_COLOR_WHITE			0xFFFF	//��ɫ
#define LCD_COLOR_BLACK			0x0000	//��ɫ
#define LCD_COLOR_BLUE			0x001F	//��ɫ
#define LCD_COLOR_BRED			0XF81F	//ƫ����
#define LCD_COLOR_GRED			0XFFE0	//ƫ�̺�
#define LCD_COLOR_GBLUE			0X07FF	//ƫ����
#define LCD_COLOR_RED			0xF800	//��ɫ
#define LCD_COLOR_MAGENTA		0xF81F	//Ʒ��ɫ
#define LCD_COLOR_GREEN			0x07E0	//��ɫ
#define LCD_COLOR_CYAN			0x7FFF	//��ɫ
#define LCD_COLOR_YELLOW		0xFFE0	//��ɫ
#define LCD_COLOR_BROWN			0XBC40	//��ɫ
#define LCD_COLOR_BRRED			0XFC07	//�غ�ɫ
#define LCD_COLOR_GRAY			0X8430	//��ɫ
#define LCD_COLOR_DARKBLUE		0X01CF	//����ɫ
#define LCD_COLOR_LIGHTBLUE		0X7D7C	//����ɫ
#define LCD_COLOR_GRAYBLUE		0X5458	//����ɫ
#define LCD_COLOR_LIGHTGREEN	0X841F	//ǳ��ɫ
#define LCD_COLOR_LGRAY			0XC618	//ǳ��ɫ(PANNEL),���屳��ɫ
#define LCD_COLOR_LGRAYBLUE		0XA651	//ǳ����ɫ(�м����ɫ)
#define LCD_COLOR_LBBLUE		0X2B12	//ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)

/************************************************************
*			ST7789�����������б�(��Ҫ�޸�!)
************************************************************/
#define LCD_CMD_INVOFF			0x20
#define LCD_CMD_INVON			0x21
#define LCD_CMD_GAMSET			0x26
#define LCD_CMD_DISPOFF			0x28
#define LCD_CMD_DISPON			0x29
#define LCD_CMD_CASET			0x2A
#define LCD_CMD_RASET			0x2B
#define LCD_CMD_RAMWR			0x2C
#define LCD_CMD_RAMRD			0x2E
#define LCD_CMD_PTLAR			0x30
#define LCD_CMD_VSCRDEF			0x33
#define LCD_CMD_TEOFF			0x34
#define LCD_CMD_TEON			0x35
#define LCD_CMD_MADCTL			0x36
#define LCD_CMD_VSCSAD			0x37
#define LCD_CMD_IDMOFF			0x38
#define LCD_CMD_IDMON			0x39
#define LCD_CMD_COLMOD			0x3A
#define LCD_CMD_WRMEMC			0x3C
#define LCD_CMD_RDMEMC			0x3E
#define LCD_CMD_STE				0x44

/************************************************************
*						���״̬�ṹ��
************************************************************/
typedef struct
{
	union
	{
		uint32_t WORD[3];

		struct
		{
			uint32_t SCAN_MODE		:2;			//Һ��ɨ�跽ʽ
			uint32_t fmt_mode		:1;			//��ʽ����ʾģʽ
			uint32_t fmt_sizey		:16;		//��ʽ���ֺŴ�С
			uint32_t RESERVED		:13;		//��һ�ֶα���λ

			uint32_t fmt_x			:16;		//��ʽ��x����
			uint32_t fmt_y			:16;		//��ʽ��y����

			uint32_t fmt_font_color	:16;		//��ʽ��������ɫ
			uint32_t fmt_bg_color	:16;		//��ʽ��������ɫ
		}BIT;
	};

}LCD_STATUS_TypeDef;

/************************************************************
*					�û�API�ӿڼ������ⲿ��������
************************************************************/
/*������*/
void LCD_Init(void);

/*�û���*/
void LCD_FillColor(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color);
void LCD_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color);
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color);
void LCD_DrawRectangle(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color);
bool LCD_ShowChar(uint16_t x,uint16_t y,uint8_t Char,uint16_t font_color,uint16_t bg_color,uint8_t sizey,bool mode);
bool LCD_ShowChinese(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);
void LCD_Set_Printfmt(uint16_t x,uint16_t y,uint16_t font_color,uint16_t bg_color,uint8_t sizey,bool mode);
void LCD_Printf(const char *fmt,...);
void LCD_ShowPicture(uint16_t x,uint16_t y,uint16_t x_dir_width,uint16_t y_dir_width,const uint8_t pic[]);

/*�ⲿ�ɷ���LCD״̬*/
extern LCD_STATUS_TypeDef	LCD_STATUS;

#ifdef __cplusplus
}
#endif

#endif

/************************************************************
*						End of File
************************************************************/
