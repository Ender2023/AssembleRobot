/************************************************************
*@file:		dev_rgblcd.c
*@brief:	Һ������Դ��
*@author:	Pan ZhiKang
*@date:		2022-10-14
************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include "bsp_rgblcd.h"
#include "lcd_Fontlib.h"
#include "bsp_delay.h"

LCD_STATUS_TypeDef	LCD_STATUS={.WORD[0]=0};
static char LCD_Printf_Buffer[LCD_Printf_BufferSize];

#if LCD_USE_DMA_CTRL_MODE	== LCD_USE_DMA
static uint8_t LCD_OSD_Buffer[LCD_DMA_BufferSize];
#endif

#if	LCD_USE_SPI_CTRL_MODE == LCD_SOFT_SPI_CTRL
/************************************************************
*@brief:	[SOFT]LCD��Ļ��������(��ֲ���޸�)
************************************************************/
static void LCD_PIN_CONFIG(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = LCD_RES_PIN|LCD_DC_PIN|LCD_CS_PIN|LCD_BLK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = LCD_SCL_PIN|LCD_SDA_PIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*ȷ�������ŵ��ϵ�״̬*/
	GPIO_ResetBits(GPIOC,LCD_RES_PIN|LCD_DC_PIN|LCD_CS_PIN|LCD_BLK_PIN);
	GPIO_ResetBits(GPIOB,LCD_SCL_PIN|LCD_SDA_PIN);
}

/************************************************************
*@brief:	[SOFT]SPIЭ��㴮��д���ʽ
*@param:	[I]BYTE:	һ�ֽڴ�С������
*@retval:	NONE
*@note:		MSB
************************************************************/
static void LCD_SPI_SEND_DATA(uint8_t BYTE)
{
	uint8_t Bit_ctr;

	LCD_CS_0();
	for(Bit_ctr=0;Bit_ctr<8;Bit_ctr++)
	{
		LCD_SCL_0();
		(BYTE & 0x80) ? LCD_SDA_1() : LCD_SDA_0();
		LCD_SCL_1();
		BYTE<<=1;
	}
	LCD_CS_1();
}

#elif LCD_USE_SPI_CTRL_MODE == LCD_HARD_SPI_CTRL

/************************************************************
*@brief:	[HARD]LCD��Ļ��������
************************************************************/
static void LCD_PIN_CONFIG(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef	  SPI_InitStructure;

	/*����ʱ��ʹ��*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);

	/*��ʼ������������*/
	GPIO_InitStructure.GPIO_Pin = LCD_RES_PIN|LCD_DC_PIN|LCD_BLK_PIN|LCD_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/*ȷ�����������ŵ��ϵ�״̬*/
	GPIO_SetBits(GPIOC,LCD_RES_PIN|LCD_DC_PIN|LCD_BLK_PIN|LCD_CS_PIN);

	/*��ʼ��ͨѶ����*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_15;

	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*SPI�ĳ�ʼ������*/
	SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_CPHA=SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low;
	SPI_InitStructure.SPI_CRCPolynomial=0;
	SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;
	SPI_InitStructure.SPI_Direction=SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_Mode=SPI_Mode_Master;
	SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;

	SPI_Init(SPI2,&SPI_InitStructure);
	SPI_Cmd(SPI2,ENABLE);
}

/************************************************************
*@brief:	[HARD]SPIЭ��㴮��д���ʽ
*@param:	[I]BYTE:	һ�ֽڴ�С������
*@retval:	NONE
*@note:		MSB
************************************************************/
static void LCD_SPI_SEND_DATA(uint8_t BYTE)
{
	LCD_CS_0();
	LCD_NOP();
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPI2,BYTE);
	LCD_NOP();
	LCD_CS_1();
}

#endif

#if LCD_USE_DMA_CTRL_MODE	== LCD_USE_DMA
/************************************************************
*@brief:	[HARD+DMA]DMA����
*@param:	[I]Buf_Addr:	�����ַ
*@param:	[I]Buf_size:	�����С
************************************************************/
static void LCD_DMA_Config(uint32_t Buf_Addr,uint16_t Buf_size)
{
	static DMA_InitTypeDef	DMA_InitTypeDefStructure;
	static bool Is_DMA_Config = false;

	if(Is_DMA_Config == false)
	{
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

		/*DMA1ͨ��5����*/
		DMA_InitTypeDefStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
		DMA_InitTypeDefStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
		DMA_InitTypeDefStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
		DMA_InitTypeDefStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
		DMA_InitTypeDefStructure.DMA_Mode=DMA_Mode_Normal;
		DMA_InitTypeDefStructure.DMA_DIR=DMA_DIR_PeripheralDST;
		DMA_InitTypeDefStructure.DMA_M2M=DMA_M2M_Disable;
		DMA_InitTypeDefStructure.DMA_Priority=DMA_Priority_High;
		DMA_InitTypeDefStructure.DMA_PeripheralBaseAddr=(uint32_t)&SPI2->DR;

		Is_DMA_Config = true;
	}
	DMA_InitTypeDefStructure.DMA_BufferSize=Buf_size;
	DMA_InitTypeDefStructure.DMA_MemoryBaseAddr=Buf_Addr;


	DMA_Init(DMA1_Channel5,&DMA_InitTypeDefStructure);
}

#endif

/************************************************************
*@brief:	��LCDд���ֽڴ�С������
*@param:	[I]BYTE:	һ�ֽڴ�С������
*@retval:	NONE
************************************************************/
static void LCD_WRITE_DATA_BYTE(uint8_t BYTE)
{
	LCD_SPI_SEND_DATA(BYTE);
}

/************************************************************
*@brief:	��LCDд����ִ�С������
*@param:	[I]HALFWORD:	���ִ�С������
*@retval:	NONE
*@note:		MSB
************************************************************/
static void LCD_WRITE_DATA_HALFWORD(uint16_t HALFWORD)
{
	LCD_SPI_SEND_DATA((uint8_t)(HALFWORD>>8 & 0x00FF));
	LCD_SPI_SEND_DATA((uint8_t)(HALFWORD & 0x00FF));
}

/************************************************************
*@brief:	��LCDд��������(�Ĵ�����ַ)
*@param:	[I]CMD:	һ�ֽ�������
*@retval:	NONE
************************************************************/
static void LCD_WRITE_CMD(uint8_t CMD)
{
	LCD_DC_0();
	LCD_SPI_SEND_DATA(CMD);
	LCD_DC_1();
}

/************************************************************
*@brief:	Һ��������ʼ��(ԭ���������������޸�)
************************************************************/
void LCD_Init(void)
{
	LCD_PIN_CONFIG();

	LCD_STATUS.BIT.SCAN_MODE = 2;

	/*��λ(��Ҫ����)*/
	LCD_RES_0();
	LCD_Delay_ms(100);
	LCD_RES_1();
	LCD_Delay_ms(100);

	/*���������*/
	LCD_BLK_1();
	LCD_Delay_ms(100);

	//************* Start Initial Sequence **********//
	LCD_WRITE_CMD(0x11); 				//Sleep out 
	LCD_Delay_ms(120);              	//Delay 120ms 
	//************* Start Initial Sequence **********// 

	LCD_WRITE_CMD(0xB1); 
	LCD_WRITE_DATA_BYTE(0x05); 
	LCD_WRITE_DATA_BYTE(0x3C); 
	LCD_WRITE_DATA_BYTE(0x3C); 
	LCD_WRITE_CMD(0xB2); 
	LCD_WRITE_DATA_BYTE(0x05);
	LCD_WRITE_DATA_BYTE(0x3C); 
	LCD_WRITE_DATA_BYTE(0x3C); 
	LCD_WRITE_CMD(0xB3); 
	LCD_WRITE_DATA_BYTE(0x05); 
	LCD_WRITE_DATA_BYTE(0x3C); 
	LCD_WRITE_DATA_BYTE(0x3C); 
	LCD_WRITE_DATA_BYTE(0x05); 
	LCD_WRITE_DATA_BYTE(0x3C); 
	LCD_WRITE_DATA_BYTE(0x3C); 
	//------------------------------------End ST7735S Frame Rate---------------------------------// 
	LCD_WRITE_CMD(0xB4); //Dot inversion 
	LCD_WRITE_DATA_BYTE(0x03); 
	//------------------------------------ST7735S Power Sequence---------------------------------// 
	LCD_WRITE_CMD(0xC0); 
	LCD_WRITE_DATA_BYTE(0x28); 
	LCD_WRITE_DATA_BYTE(0x08); 
	LCD_WRITE_DATA_BYTE(0x04); 
	LCD_WRITE_CMD(0xC1); 
	LCD_WRITE_DATA_BYTE(0XC0); 
	LCD_WRITE_CMD(0xC2); 
	LCD_WRITE_DATA_BYTE(0x0D); 
	LCD_WRITE_DATA_BYTE(0x00); 
	LCD_WRITE_CMD(0xC3); 
	LCD_WRITE_DATA_BYTE(0x8D); 
	LCD_WRITE_DATA_BYTE(0x2A); 
	LCD_WRITE_CMD(0xC4); 
	LCD_WRITE_DATA_BYTE(0x8D); 
	LCD_WRITE_DATA_BYTE(0xEE); 

	/*����ɨ��ģʽ*/
	LCD_WRITE_CMD(0xC5); //VCOM 
	LCD_WRITE_DATA_BYTE(0x1A); 
	LCD_WRITE_CMD(0x36);
	switch(LCD_STATUS.BIT.SCAN_MODE)
	{
		case 0:	LCD_WRITE_DATA_BYTE(0x08);break;
		case 1:	LCD_WRITE_DATA_BYTE(0xC8);break;
		case 2:	LCD_WRITE_DATA_BYTE(0x78);break;
		case 3:	LCD_WRITE_DATA_BYTE(0xA8);break;
		default: break;
	}

	LCD_WRITE_CMD(0xE0); 
	LCD_WRITE_DATA_BYTE(0x04); 
	LCD_WRITE_DATA_BYTE(0x22); 
	LCD_WRITE_DATA_BYTE(0x07); 
	LCD_WRITE_DATA_BYTE(0x0A); 
	LCD_WRITE_DATA_BYTE(0x2E); 
	LCD_WRITE_DATA_BYTE(0x30); 
	LCD_WRITE_DATA_BYTE(0x25); 
	LCD_WRITE_DATA_BYTE(0x2A); 
	LCD_WRITE_DATA_BYTE(0x28); 
	LCD_WRITE_DATA_BYTE(0x26); 
	LCD_WRITE_DATA_BYTE(0x2E); 
	LCD_WRITE_DATA_BYTE(0x3A); 
	LCD_WRITE_DATA_BYTE(0x00); 
	LCD_WRITE_DATA_BYTE(0x01); 
	LCD_WRITE_DATA_BYTE(0x03); 
	LCD_WRITE_DATA_BYTE(0x13); 
	LCD_WRITE_CMD(0xE1); 
	LCD_WRITE_DATA_BYTE(0x04); 
	LCD_WRITE_DATA_BYTE(0x16); 
	LCD_WRITE_DATA_BYTE(0x06); 
	LCD_WRITE_DATA_BYTE(0x0D); 
	LCD_WRITE_DATA_BYTE(0x2D); 
	LCD_WRITE_DATA_BYTE(0x26); 
	LCD_WRITE_DATA_BYTE(0x23); 
	LCD_WRITE_DATA_BYTE(0x27); 
	LCD_WRITE_DATA_BYTE(0x27); 
	LCD_WRITE_DATA_BYTE(0x25); 
	LCD_WRITE_DATA_BYTE(0x2D); 
	LCD_WRITE_DATA_BYTE(0x3B); 
	LCD_WRITE_DATA_BYTE(0x00); 
	LCD_WRITE_DATA_BYTE(0x01); 
	LCD_WRITE_DATA_BYTE(0x04); 
	LCD_WRITE_DATA_BYTE(0x13); 
	//------------------------------------End ST7735S Gamma Sequence-----------------------------// 
	LCD_WRITE_CMD(0x3A); //65k mode 
	LCD_WRITE_DATA_BYTE(0x05); 
	LCD_WRITE_CMD(0x29); //Display on 

	LCD_FillColor(0,0,240,240,LCD_COLOR_BLACK);
	LCD_Set_Printfmt(0,0,LCD_COLOR_WHITE,LCD_COLOR_BLACK,12,false);
}

/************************************************************
*@brief:	���ء�������
*@param:	[I]x1:		���x����
*@param:	[I]y1:		���y����
*@param:	[I]x2:		�յ�x����
*@param:	[I]y2:		�յ�y����
*@retval:	NONE
************************************************************/
static void LCD_OpenWindow(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
	switch(LCD_STATUS.BIT.SCAN_MODE)
	{
		case 0:
		{
			LCD_WRITE_CMD(LCD_CMD_CASET);//�е�ַ����
			LCD_WRITE_DATA_HALFWORD(x1+2);
			LCD_WRITE_DATA_HALFWORD(x2+2);
			LCD_WRITE_CMD(LCD_CMD_RASET);//�е�ַ����
			LCD_WRITE_DATA_HALFWORD(y1+1);
			LCD_WRITE_DATA_HALFWORD(y2+1);
			LCD_WRITE_CMD(LCD_CMD_RAMWR);//������д
			break;
		}
		case 1:
		{
			LCD_WRITE_CMD(LCD_CMD_CASET);//�е�ַ����
			LCD_WRITE_DATA_HALFWORD(x1+2);
			LCD_WRITE_DATA_HALFWORD(x2+2);
			LCD_WRITE_CMD(LCD_CMD_RASET);//�е�ַ����
			LCD_WRITE_DATA_HALFWORD(y1+3);
			LCD_WRITE_DATA_HALFWORD(y2+3);
			LCD_WRITE_CMD(LCD_CMD_RAMWR);//������д
			break;
		}
		case 2:
		{
			LCD_WRITE_CMD(LCD_CMD_CASET);//�е�ַ����
			LCD_WRITE_DATA_HALFWORD(x1+3);
			LCD_WRITE_DATA_HALFWORD(x2+3);
			LCD_WRITE_CMD(LCD_CMD_RASET);//�е�ַ����
			LCD_WRITE_DATA_HALFWORD(y1+2);
			LCD_WRITE_DATA_HALFWORD(y2+2);
			LCD_WRITE_CMD(LCD_CMD_RAMWR);//������д
			break;
		}
		case 3:
		{
			LCD_WRITE_CMD(LCD_CMD_CASET);//�е�ַ����
			LCD_WRITE_DATA_HALFWORD(x1+3);
			LCD_WRITE_DATA_HALFWORD(x2+3);
			LCD_WRITE_CMD(LCD_CMD_RASET);//�е�ַ����
			LCD_WRITE_DATA_HALFWORD(y1+2);
			LCD_WRITE_DATA_HALFWORD(y2+2);
			LCD_WRITE_CMD(LCD_CMD_RAMWR);//������д
			break;
		}
	}
}

/************************************************************
*@brief:	��ָ�����������ɫ
*@param:	[I]x1:		���x����
*@param:	[I]y1:		���y����
*@param:	[I]x2:		�յ�x����
*@param:	[I]y2:		�յ�y����
*@param:	[I]color:	Ҫ������ɫ
*@retval:	NONE
************************************************************/
void LCD_FillColor(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color)
{
	uint16_t tmp;

	if(x1 > x2){tmp=x1;x1=x2;x2=tmp;}
	if(y1 > y2){tmp=y1;y1=y2;y2=tmp;}

#if LCD_USE_DMA_CTRL_MODE	== LCD_UNUSE_DMA
	uint16_t Pixel_ctr,Pixel_Sum;

	Pixel_Sum=((x2-x1)+1)*((y2-y1)+1);
	LCD_OpenWindow(x1,y1,x2-1,y2-1);
	for(Pixel_ctr=0;Pixel_ctr<Pixel_Sum;Pixel_ctr++)
	{
		LCD_WRITE_DATA_HALFWORD(color);
	}

#elif LCD_USE_DMA_CTRL_MODE	== LCD_USE_DMA

	uint16_t X_ctr,Y_ctr;
	Y_ctr=(x2-x1+1)*(y2-y1+1)/LCD_W;

	LCD_OpenWindow(x1,y1,x2-1,y2-1);

	LCD_DMA2SPI_DISABLE();
	LCD_DMA_Config((uint32_t)&LCD_OSD_Buffer,LCD_DMA_BufferSize);
	LCD_DMA2SPI_ENABLE();

	LCD_CS_0();

	while(Y_ctr>0)
	{
		for(X_ctr=0;X_ctr<LCD_DMA_BufferSize;X_ctr+=2)
		{
			LCD_OSD_Buffer[X_ctr]=color>>8;
			LCD_OSD_Buffer[X_ctr+1]=color;
		}

		LCD_DMA2SPI_SEND();

		while(DMA_GetCurrDataCounter(DMA1_Channel5) > 0);

		LCD_DMA2SPI_DISABLE();
		DMA_SetCurrDataCounter(DMA1_Channel5,LCD_DMA_BufferSize);
		LCD_DMA2SPI_ENABLE();

		Y_ctr--;
	}

	while(DMA_GetCurrDataCounter(DMA1_Channel5) > 0);

	LCD_CS_1();

#endif
}

/************************************************************
*@brief:	��ָ������������ص�
*@param:	[I]x:		���ص�x����
*@param:	[I]y:		���ص�y����
*@param:	[I]color:	Ҫ������ɫ
*@retval:	NONE
************************************************************/
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
	if(x >= LCD_W)x=LCD_W-1;
	if(y >= LCD_H)y=LCD_H-1;
	LCD_OpenWindow(x,y,x,y); 
	LCD_WRITE_DATA_HALFWORD(color);
}

/************************************************************
*@brief:	����ָ���������ֱ��
*@param:	[I]x1:		���x����
*@param:	[I]y1:		���y����
*@param:	[I]x2:		�յ�x����
*@param:	[I]y2:		�յ�y����
*@param:	[I]color:	��������ɫ
*@retval:	NONE
************************************************************/
void LCD_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color)
{
	uint16_t Step; 
	uint16_t X_Current,Y_Current;
	
	int32_t lError_X=0,lError_Y=0,lDelta_X,lDelta_Y,lDistance; 
	int32_t lIncrease_X,lIncrease_Y;

	lDelta_Y=y2-y1;
	lDelta_X=x2-x1;
	
	X_Current=x1;
	Y_Current=y1;

	if(lDelta_X>0) 
		lIncrease_X=1;
	else if(lDelta_X==0) 
		lIncrease_X=0;
	else
	{
		lIncrease_X=-1;
		lDelta_X=-lDelta_X;
	}

	if (lDelta_Y>0)
		lIncrease_Y=1;
	else if(lDelta_Y==0)
		lIncrease_Y=0;
	else
	{
		lIncrease_Y=-1;
		lDelta_Y=-lDelta_Y;
	}

	if(lDelta_X>lDelta_Y)
		lDistance=lDelta_X;
	else 
		lDistance=lDelta_Y; 

	for(Step=0;Step<=lDistance+1;Step++)
	{
		LCD_DrawPoint(X_Current,Y_Current,color); 
		lError_X += lDelta_X ; 
		lError_Y += lDelta_Y ; 
		if(lError_X>lDistance) 
		{
			lError_X-=lDistance; 
			X_Current+=lIncrease_X; 
		}
	
		if(lError_Y>lDistance) 
		{
			lError_Y-=lDistance; 
			Y_Current+=lIncrease_Y; 
		}
	}
}

/************************************************************
*@brief:	����ָ��������ƾ���
*@param:	[I]x1:		���x����
*@param:	[I]y1:		���y����
*@param:	[I]x2:		�յ�x����
*@param:	[I]y2:		�յ�y����
*@param:	[I]color:	������ɫ
*@retval:	NONE
************************************************************/
void LCD_DrawRectangle(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color)
{
	LCD_DrawLine(x1,y1,x2,y1,color);
	LCD_DrawLine(x1,y1,x1,y2,color);
	LCD_DrawLine(x1,y2,x2,y2,color);
	LCD_DrawLine(x2,y1,x2,y2,color);
}

/************************************************************
*@brief:	ASCII��ʾ���ַ�
*@param:	[I]x:			���x����
*@param:	[I]y:			���y����
*@param:	[I]Char:		Ҫ��ʾ���ַ�
*@param:	[I]font_color:	������ɫ
*@param:	[I]bg_color:	������ɫ
*@param:	[I]sizey:		�ֺŴ�С(һ��ȡ������ֱ��������ص�)
*@param:	[I]mode:		����ģʽ(��ֵ������Ϊ���²���)
*			@arg:		true:���õ���ģʽ�����屳�������ڵ����б���
*			@arg:		false:�����õ���ģʽ�����屳�����ڵ����б���
*@retval:	true:		��ʾ�ɹ�
*@retval:	false:		��ʾʧ��,ԭ�����Ϊ��������Ч���ֺŴ�С
************************************************************/
bool LCD_ShowChar(uint16_t x,uint16_t y,uint8_t Char,uint16_t font_color,uint16_t bg_color,uint8_t sizey,bool mode)
{
	if(sizey != 12 && sizey != 16 && sizey != 24 && sizey != 32)return false;

	uint8_t sizex,bit_ctr,m=0;
	const uint8_t * tmp;
	uint16_t Byte_ctr,ByteNum;									//һ���ַ����õ����ֽ���(����1����2��)
	uint16_t x0=x;

	sizex=sizey/2;
	ByteNum=(sizex/8+((sizex%8)?1:0))*sizey;
	Char=Char-' ';												//�õ�ƫ�ƺ��ֵ
	LCD_OpenWindow(x,y,x+sizex-1,y+sizey-1);  					//���ù��λ��

	#if LCD_FONTLIB_USE_ASCII_12 == 1
		if(sizey == 12)tmp=&ascii_12x6[Char][0];						//����6x12����
	#endif

	#if LCD_FONTLIB_USE_ASCII_16 == 1
		if(sizey == 16)tmp=&ascii_16x8[Char][0];						//����8x16����
	#endif

	#if LCD_FONTLIB_USE_ASCII_24 == 1
		if(sizey == 24)tmp=&ascii_24x12[Char][0];						//����12x24����
	#endif

	#if LCD_FONTLIB_USE_ASCII_32 == 1
		if(sizey == 32)tmp=&ascii_32x16[Char][0];						//����16x32����
	#endif


	for(Byte_ctr=0;Byte_ctr<ByteNum;Byte_ctr++)
	{
		for(bit_ctr=0;bit_ctr<8;bit_ctr++)
		{
			if(!mode)//�ǵ���ģʽ
			{
				if(*tmp & (0x01 << bit_ctr))LCD_WRITE_DATA_HALFWORD(font_color);
				else LCD_WRITE_DATA_HALFWORD(bg_color);
				m++;
				if(m%sizex==0)
				{
					m=0;
					break;
				}
			}
			else//����ģʽ
			{
				if(*tmp & (0x01 << bit_ctr))LCD_DrawPoint(x,y,font_color);	//��һ����
				x++;
				if((x-x0)==sizex)
				{
					x=x0;
					y++;
					break;
				}
			}
		}

		tmp++;
	}

	return true;
}

#if LCD_FONTLIB_USE_CHINNESE_12 == 1
/************************************************************
*@brief:	��ʾ���������ַ�(12x12)
*@param:	[I]x:			���x����
*@param:	[I]y:			���y����
*@param:	[I]*s:			Ҫ��ʾ�ĺ���
*@param:	[I]font_color:	������ɫ
*@param:	[I]bg_color:	������ɫ
*@param:	[I]sizey:		�ֺŴ�С(һ��ȡ������ֱ��������ص�)
*@param:	[I]mode:		����ģʽ(��ֵ������Ϊ���²���)
*			@arg:		true:���õ���ģʽ�����屳�������ڵ����б���
*			@arg:		false:�����õ���ģʽ�����屳�����ڵ����б���
*@retval:	NONE
************************************************************/
static void LCD_ShowChinese12x12(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
{
	uint8_t i,j,m=0;
	uint16_t k;
	uint16_t HZnum;//������Ŀ
	uint16_t TypefaceNum;//һ���ַ���ռ�ֽڴ�С
	uint16_t x0=x;
	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;

	HZnum=sizeof(tfont12)/sizeof(typFNT_GB12);	//ͳ�ƺ�����Ŀ
	for(k=0;k<HZnum;k++) 
	{
		if((tfont12[k].Index[0]==*(s))&&(tfont12[k].Index[1]==*(s+1)))
		{
			LCD_OpenWindow(x,y,x+sizey-1,y+sizey-1);
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{	
					if(!mode)//�ǵ��ӷ�ʽ
					{
						if(tfont12[k].Msk[i]&(0x01<<j))LCD_WRITE_DATA_HALFWORD(fc);
						else LCD_WRITE_DATA_HALFWORD(bc);
						m++;
						if(m%sizey==0)
						{
							m=0;
							break;
						}
					}
					else//���ӷ�ʽ
					{
						if(tfont12[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//��һ����
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}
	}
}

#endif

#if LCD_FONTLIB_USE_CHINNESE_16 == 1
/************************************************************
*@brief:	��ʾ���������ַ�(16x16)
*@param:	[I]x:			���x����
*@param:	[I]y:			���y����
*@param:	[I]*s:			Ҫ��ʾ�ĺ���
*@param:	[I]font_color:	������ɫ
*@param:	[I]bg_color:	������ɫ
*@param:	[I]sizey:		�ֺŴ�С(һ��ȡ������ֱ��������ص�)
*@param:	[I]mode:		����ģʽ(��ֵ������Ϊ���²���)
*			@arg:		true:���õ���ģʽ�����屳�������ڵ����б���
*			@arg:		false:�����õ���ģʽ�����屳�����ڵ����б���
*@retval:	NONE
************************************************************/
static void LCD_ShowChinese16x16(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
{
	uint8_t i,j,m=0;
	uint16_t k;
	uint16_t HZnum;//������Ŀ
	uint16_t TypefaceNum;//һ���ַ���ռ�ֽڴ�С
	uint16_t x0=x;
  TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	//ͳ�ƺ�����Ŀ
	for(k=0;k<HZnum;k++) 
	{
		if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
		{ 	
			LCD_OpenWindow(x,y,x+sizey-1,y+sizey-1);
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{	
					if(!mode)//�ǵ��ӷ�ʽ
					{
						if(tfont16[k].Msk[i]&(0x01<<j))LCD_WRITE_DATA_HALFWORD(fc);
						else LCD_WRITE_DATA_HALFWORD(bc);
						m++;
						if(m%sizey==0)
						{
							m=0;
							break;
						}
					}
					else//���ӷ�ʽ
					{
						if(tfont16[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//��һ����
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}
	}
}

#endif

#if LCD_FONTLIB_USE_CHINNESE_24 == 1
/************************************************************
*@brief:	��ʾ���������ַ�(24x24)
*@param:	[I]x:			���x����
*@param:	[I]y:			���y����
*@param:	[I]*s:			Ҫ��ʾ�ĺ���
*@param:	[I]font_color:	������ɫ
*@param:	[I]bg_color:	������ɫ
*@param:	[I]sizey:		�ֺŴ�С(һ��ȡ������ֱ��������ص�)
*@param:	[I]mode:		����ģʽ(��ֵ������Ϊ���²���)
*			@arg:		true:���õ���ģʽ�����屳�������ڵ����б���
*			@arg:		false:�����õ���ģʽ�����屳�����ڵ����б���
*@retval:	NONE
************************************************************/
static void LCD_ShowChinese24x24(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
{
	uint8_t i,j,m=0;
	uint16_t k;
	uint16_t HZnum;//������Ŀ
	uint16_t TypefaceNum;//һ���ַ���ռ�ֽڴ�С
	uint16_t x0=x;
	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
	HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);	//ͳ�ƺ�����Ŀ
	for(k=0;k<HZnum;k++) 
	{
		if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1)))
		{ 	
			LCD_OpenWindow(x,y,x+sizey-1,y+sizey-1);
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{	
					if(!mode)//�ǵ��ӷ�ʽ
					{
						if(tfont24[k].Msk[i]&(0x01<<j))LCD_WRITE_DATA_HALFWORD(fc);
						else LCD_WRITE_DATA_HALFWORD(bc);
						m++;
						if(m%sizey==0)
						{
							m=0;
							break;
						}
					}
					else//���ӷ�ʽ
					{
						if(tfont24[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//��һ����
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}
	}
}

#endif

#if LCD_FONTLIB_USE_CHINNESE_32 == 1
/************************************************************
*@brief:	��ʾ���������ַ�(32x32)
*@param:	[I]x:			���x����
*@param:	[I]y:			���y����
*@param:	[I]*s:			Ҫ��ʾ�ĺ���
*@param:	[I]font_color:	������ɫ
*@param:	[I]bg_color:	������ɫ
*@param:	[I]sizey:		�ֺŴ�С(һ��ȡ������ֱ��������ص�)
*@param:	[I]mode:		����ģʽ(��ֵ������Ϊ���²���)
*			@arg:		true:���õ���ģʽ�����屳�������ڵ����б���
*			@arg:		false:�����õ���ģʽ�����屳�����ڵ����б���
*@retval:	NONE
************************************************************/
static void LCD_ShowChinese32x32(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
{
	uint8_t i,j,m=0;
	uint16_t k;
	uint16_t HZnum;//������Ŀ
	uint16_t TypefaceNum;//һ���ַ���ռ�ֽڴ�С
	uint16_t x0=x;
	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
	HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);	//ͳ�ƺ�����Ŀ
	for(k=0;k<HZnum;k++) 
	{
		if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1)))
		{ 	
			LCD_OpenWindow(x,y,x+sizey-1,y+sizey-1);
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{	
					if(!mode)//�ǵ��ӷ�ʽ
					{
						if(tfont32[k].Msk[i]&(0x01<<j))LCD_WRITE_DATA_HALFWORD(fc);
						else LCD_WRITE_DATA_HALFWORD(bc);
						m++;
						if(m%sizey==0)
						{
							m=0;
							break;
						}
					}
					else//���ӷ�ʽ
					{
						if(tfont32[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//��һ����
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}
	}
}

#endif

/************************************************************
*@brief:	��ʾ�ַ���
*@param:	[I]x:			���x����
*@param:	[I]y:			���y����
*@param:	[I]*s:			Ҫ��ʾ�ĺ����ַ���
*@param:	[I]font_color:	������ɫ
*@param:	[I]bg_color:	������ɫ
*@param:	[I]sizey:		�ֺŴ�С(һ��ȡ������ֱ��������ص�)
*@param:	[I]mode:		����ģʽ(��ֵ������Ϊ���²���)
*			@arg:		true:���õ���ģʽ�����屳�������ڵ����б���
*			@arg:		false:�����õ���ģʽ�����屳�����ڵ����б���
*@retval:	true��ʾ��ʾ�����ַ����ɹ�
*@retval:	false��ʾ��ʾ�����ַ���ʧ��
************************************************************/
bool LCD_ShowChinese(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
{
	while(*s != '\0')
	{
		switch(sizey)
		{
			#if LCD_FONTLIB_USE_CHINNESE_12 == 1
			case 12 : LCD_ShowChinese12x12(x,y,s,fc,bc,sizey,mode);break;
			#endif

			#if LCD_FONTLIB_USE_CHINNESE_16 == 1
			case 16 : LCD_ShowChinese16x16(x,y,s,fc,bc,sizey,mode);break;
			#endif

			#if LCD_FONTLIB_USE_CHINNESE_24 == 1
			case 24 : LCD_ShowChinese24x24(x,y,s,fc,bc,sizey,mode);break;
			#endif

			#if LCD_FONTLIB_USE_CHINNESE_32 == 1
			case 32 : LCD_ShowChinese32x32(x,y,s,fc,bc,sizey,mode);break;
			#endif

			default:	return false;
		}
		s+=2;
		x+=sizey;
	}

	return true;
}

/************************************************************
*@brief:	LCD����Printf��ʽ����Ϊ��ӡ��ǰ�ú���
*@param:	[I]x:				��ӡ����Һ�������x����
*@param:	[I]y:				��ӡ����Һ�������y����
*@param:	[I]font_color:		��ӡ����������ɫ
*@param:	[I]bg_color:		��ӡ�������屳����ɫ
*@param:	[I]sizey:			��ӡ���������ֺŴ�С
*@param:	[I]mode:			��ӡ�������Ƿ��ǵ���
*@retval:	None
*@note:		�ú���ӦΪLCD_Printf��ǰ�ú���
************************************************************/
__inline void LCD_Set_Printfmt(uint16_t x,uint16_t y,uint16_t font_color,uint16_t bg_color,uint8_t sizey,bool mode)
{
	LCD_STATUS.BIT.fmt_bg_color=bg_color;
	LCD_STATUS.BIT.fmt_font_color=font_color;
	LCD_STATUS.BIT.fmt_mode=mode;
	LCD_STATUS.BIT.fmt_sizey=sizey;
	LCD_STATUS.BIT.fmt_x=x;
	LCD_STATUS.BIT.fmt_y=y;
}

/************************************************************
*@brief:	������LCD��printf�������볬���ն˵�ͬ
************************************************************/
void LCD_Printf(const char *fmt,...)
{
	uint8_t chinese_buf[3]={'\0'};
	__IO char * dispbuf_ptr=LCD_Printf_Buffer;

	va_list ap;
	va_start(ap,fmt);
	vsprintf(LCD_Printf_Buffer,fmt,ap);
	va_end(ap);

	while(*dispbuf_ptr !='\0')
	{
		if(*dispbuf_ptr > 127)
		{
			chinese_buf[0]=*dispbuf_ptr++;
			chinese_buf[1]=*dispbuf_ptr++;

			/*���м��*/
			if(LCD_STATUS.BIT.fmt_x+LCD_STATUS.BIT.fmt_sizey/2 > LCD_W)
			{LCD_STATUS.BIT.fmt_x=0;LCD_STATUS.BIT.fmt_y+=LCD_STATUS.BIT.fmt_sizey;}

			/*������ʾ*/
			LCD_ShowChinese(LCD_STATUS.BIT.fmt_x,LCD_STATUS.BIT.fmt_y,chinese_buf,LCD_STATUS.BIT.fmt_font_color,
			LCD_STATUS.BIT.fmt_bg_color,LCD_STATUS.BIT.fmt_sizey,LCD_STATUS.BIT.fmt_mode);

			/*��ȷ����*/
			LCD_STATUS.BIT.fmt_x+=LCD_STATUS.BIT.fmt_sizey;
		}
		else if(*dispbuf_ptr == '\n')
		{
			LCD_STATUS.BIT.fmt_x=0;
			LCD_STATUS.BIT.fmt_y+=LCD_STATUS.BIT.fmt_sizey;
			dispbuf_ptr++;
		}
		else if(*dispbuf_ptr == '\t')
		{
			if(LCD_STATUS.BIT.fmt_x+2*LCD_STATUS.BIT.fmt_sizey > LCD_W)
			{LCD_STATUS.BIT.fmt_x=0;LCD_STATUS.BIT.fmt_y+=LCD_STATUS.BIT.fmt_sizey;}
			else
			{LCD_STATUS.BIT.fmt_x+=2*LCD_STATUS.BIT.fmt_sizey;}
			dispbuf_ptr++;
		}
		else if(*dispbuf_ptr == '\r'){LCD_STATUS.BIT.fmt_x=0;}
		else
		{
			/*���м��*/
			if(LCD_STATUS.BIT.fmt_x+LCD_STATUS.BIT.fmt_sizey/2 > LCD_W)
			{LCD_STATUS.BIT.fmt_x=0;LCD_STATUS.BIT.fmt_y+=LCD_STATUS.BIT.fmt_sizey;}

			/*ASCII��ʾ*/
			LCD_ShowChar(LCD_STATUS.BIT.fmt_x,LCD_STATUS.BIT.fmt_y,*dispbuf_ptr++,LCD_STATUS.BIT.fmt_font_color,
			LCD_STATUS.BIT.fmt_bg_color,LCD_STATUS.BIT.fmt_sizey,LCD_STATUS.BIT.fmt_mode);

			/*��ȷ����*/
			LCD_STATUS.BIT.fmt_x+=LCD_STATUS.BIT.fmt_sizey/2;
		}
	}
}

/************************************************************
*@brief:	LCD��ʾͼƬ
*@param:	[I]x:				ͼƬ��Һ�������x����
*@param:	[I]y:				ͼƬ��Һ�������y����
*@param:	[I]x_dir_width:		ͼƬ��x��������س���
*@param:	[I]y_dir_width:		ͼƬ��y��������س���
*@param:	[I]pic:				ָ��ͼƬ����ĵ�ַ
*@retval:	None
************************************************************/
void LCD_ShowPicture(uint16_t x,uint16_t y,uint16_t x_dir_width,uint16_t y_dir_width,const uint8_t* pic)
{
#if	 LCD_USE_DMA_CTRL_MODE	== LCD_UNUSE_DMA
	uint16_t i,j;
	uint32_t k=0;

	LCD_Delay_us(1);										//��Ҫ�Ĵ�����ʱ

	LCD_OpenWindow(x,y,x+x_dir_width-1,y+y_dir_width-1);
	for(i=0;i<x_dir_width;i++)
	{
		for(j=0;j<y_dir_width;j++)
		{
			LCD_WRITE_DATA_BYTE(pic[k*2]);
			LCD_WRITE_DATA_BYTE(pic[k*2+1]);
			k++;
		} 
	}
#elif LCD_USE_DMA_CTRL_MODE	== LCD_USE_DMA
	const uint8_t*	ptr;
	uint32_t Pxiel_Sum;

	Pxiel_Sum=x_dir_width*y_dir_width*2;
	LCD_OpenWindow(x,y,x+x_dir_width-1,y+y_dir_width-1);

	/*����ָ��ָ��ͼƬ�������ڵ�ַ*/
	ptr=&pic[0];

	if(Pxiel_Sum>LCD_DMA_BUF_MAX)
	{
		/*ת�˵�һ����*/
		DMA_Cmd(DMA1_Channel5,DISABLE);
		LCD_DMA_Config((uint32_t)ptr,LCD_DMA_BUF_MAX);
		DMA_Cmd(DMA1_Channel5,ENABLE);

		LCD_CS_0();
		LCD_NOP();
		while(DMA_GetCurrDataCounter(DMA1_Channel5) > 0);
		LCD_DMA2SPI_SEND();
		LCD_NOP();
		LCD_CS_1();

		/*ת�˵ڶ�����*/
		ptr+=LCD_DMA_BUF_MAX;

		LCD_DMA2SPI_DISABLE();
		LCD_DMA_Config((uint32_t)ptr,Pxiel_Sum-LCD_DMA_BUF_MAX);
		LCD_DMA2SPI_ENABLE();

		LCD_CS_0();
		LCD_NOP();
		while(DMA_GetCurrDataCounter(DMA1_Channel5) > 0);
		LCD_DMA2SPI_SEND();
		LCD_NOP();
		LCD_CS_1();
	}
	else
	{
		LCD_DMA2SPI_DISABLE();
		LCD_DMA_Config((uint32_t)ptr,Pxiel_Sum);
		LCD_DMA2SPI_ENABLE();

		LCD_CS_0();
		LCD_NOP();
		while(DMA_GetCurrDataCounter(DMA1_Channel5) > 0);
		LCD_DMA2SPI_SEND();
		LCD_NOP();
		LCD_CS_1();
	}

#endif

}

/************************************************************
*						End of File
************************************************************/
