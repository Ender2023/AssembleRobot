/*********************************************************************
*@file:		nRf24L01.h
*@brief:	nRF24L01射频通讯模块
*@author:	Pan Zhikang
*@date:		2022-3-26
*********************************************************************/
#ifndef	__BSP_NRF24L01__H
#define	__BSP_NRF24L01__H
#include "stm32f10x.h"

/************************************************************************
*						 nRF24L01移植相关定义
************************************************************************/
#define		RF_Port_CS		GPIOD
#define		RF_Pin_CS		GPIO_Pin_11

#define		RF_Port_MISO	GPIOD
#define		RF_Pin_MISO		GPIO_Pin_12

#define		RF_Port_MOSI	GPIOD
#define		RF_Pin_MOSI		GPIO_Pin_10

#define		RF_Port_SCL		GPIOD
#define		RF_Pin_SCL		GPIO_Pin_13

#define		RF_Port_CE		GPIOD
#define		RF_Pin_CE		GPIO_Pin_14

#define		RF_CS_Low()		GPIO_ResetBits(RF_Port_CS,RF_Pin_CS)
#define		RF_CS_High()	GPIO_SetBits(RF_Port_CS,RF_Pin_CS)

#define		RF_CE_Low()		GPIO_ResetBits(RF_Port_CE,RF_Pin_CE)
#define		RF_CE_High()	GPIO_SetBits(RF_Port_CE,RF_Pin_CE)

#define		RF_SCL_Low()	GPIO_ResetBits(RF_Port_SCL,RF_Pin_SCL)
#define		RF_SCL_High()	GPIO_SetBits(RF_Port_SCL,RF_Pin_SCL)

#define 	RF_MOSI_Low()	GPIO_ResetBits(RF_Port_MOSI,RF_Pin_MOSI)
#define		RF_MOSI_High()	GPIO_SetBits(RF_Port_MOSI,RF_Pin_MOSI)

#define		RF_MISO_Read()	GPIO_ReadInputDataBit(RF_Port_MISO,RF_Pin_MISO)

/************************************************************************
*						nRF24L01常用参数配置
************************************************************************/
#define		ADDRESS_WIDTH	4
#define		PAYLOAD_WIDTH	4

/************************************************************************
*						nRF24L01特殊命令字
************************************************************************/
#define		Read_REG		0x00
#define		Write_REG		0x20
#define		R_RX_PAYLOAD	0x61
#define		W_TX_PAYLOAD	0xA0
#define		FLUSH_TX		0xE1
#define		FLUSH_RX		0xE2
#define		REUSE_TX_PL		0xE3
#define		NOP				0xFF

/************************************************************************
*						nRF24L01寄存器地址定义
************************************************************************/
#define		CONFIG			0x00
#define		EN_AA_Enhanced	0x01
#define		EN_RXADDR		0x02
#define		SETUP_AW		0x03
#define		SETUP_RETR		0x04
#define		RF_CH			0x05
#define		RF_SETUP		0x06
#define		STATUS			0x07
#define		OBSERVE_TX		0x08
#define		RPD				0x09
#define		RX_ADDR_P0		0x0A
#define		RX_ADDR_P1		0x0B
#define		RX_ADDR_P2		0x0C
#define		RX_ADDR_P3		0x0D
#define		RX_ADDR_P4		0x0E
#define		RX_ADDR_P5		0x0F
#define 	TX_ADDR			0x10
#define		RX_PW_P0		0x11
#define		RX_PW_P1		0x12
#define		RX_PW_P2		0x13
#define		RX_PW_P3		0x14
#define		RX_PW_P4		0x15
#define		RX_PW_P5		0x16
#define		FIFO_STATUS		0x17
#define		DYNPD			0x1C
#define		FEATURE			0x1D

/************************************************************************
*					nRF24L01函数与变量对外调用声明
************************************************************************/
extern 		uint8_t 		Tx_Data[PAYLOAD_WIDTH];
extern 		uint8_t 		Rx_Data[PAYLOAD_WIDTH];

void 		nRF24L01_WriteREG(uint8_t Address,uint8_t Data);
uint8_t		nRf24L01_ReadREG(uint8_t Address);
void 		nRf24L01_WriteBuffer(uint8_t Address,uint8_t* Data,uint8_t Width);
void 		nRf24L01_ReadBuffer(uint8_t Address,uint8_t* Data,uint8_t Width);
void 		nRf24L01_Init(void);
void 		nRf24L01_IntoTxMode(void);
void 		nRf24L01_IntoRxMode(void);
void 		nRF24L01_TransmitData(uint8_t* Tx_Data);
void 		nRF24L01_ReceiveData(uint8_t* Rx_Data);
void 		nRF24L01_ChannelReset(unsigned char Channel);

#endif

/************************************************************************
*							End Of File
************************************************************************/
