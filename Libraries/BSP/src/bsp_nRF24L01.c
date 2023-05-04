/*********************************************************************
*@file:		nRf24L01.c
*@brief:	nRF24L01射频通讯模块
*@author:	Pan Zhikang
*@date:		2022-3-26
*********************************************************************/
#include "bsp_nRf24L01.h"

uint8_t Tx_Data[PAYLOAD_WIDTH];
uint8_t Rx_Data[PAYLOAD_WIDTH];

/*********************************************************************
*@brief:	nRF24L01内部延时函数
*@param:	需要延时的时间xus(单位为us)
*@retval:	None
*********************************************************************/
static void RF_DelayXus(uint16_t xus)
{
	uint16_t i=0;
	for(i=0;i<xus;i++)
	{
		__asm("NOP");
		__asm("NOP");
	}
}

/*********************************************************************
*@brief:	nRF24L01的SPI引脚配置
*@param:	None
*@retval:	None
*********************************************************************/
static void nRF24L01_Pin_Config(void)
{
	GPIO_InitTypeDef		GPIO_InitTypeStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);

	GPIO_InitTypeStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitTypeStructure.GPIO_Pin=RF_Pin_CS;						//CS
	GPIO_InitTypeStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(RF_Port_CS,&GPIO_InitTypeStructure);

	GPIO_InitTypeStructure.GPIO_Pin=RF_Pin_CE;						//CE
	GPIO_Init(RF_Port_CE,&GPIO_InitTypeStructure);

	GPIO_InitTypeStructure.GPIO_Pin=RF_Pin_SCL;						//SCL
	GPIO_Init(RF_Port_SCL,&GPIO_InitTypeStructure);

	GPIO_InitTypeStructure.GPIO_Pin=RF_Pin_MOSI;					//MOSI
	GPIO_Init(RF_Port_MOSI,&GPIO_InitTypeStructure);

	GPIO_InitTypeStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitTypeStructure.GPIO_Pin=RF_Pin_MISO;					//MISO
	GPIO_Init(RF_Port_MISO,&GPIO_InitTypeStructure);

	RF_CS_High();													//片选拉高，防上电使能
	RF_CE_Low();													//使能芯片
}

/*********************************************************************
*@brief:	SPI读/写一个字节的地址/数据
*@param:	欲写入的数据Data
*@retval:	所读出的8位数据
*********************************************************************/
static uint8_t SPI_WriteReadByte(uint8_t Data)
{
	uint8_t bit_ctr;

	for(bit_ctr=0;bit_ctr<8;bit_ctr++)
	{
		if(Data&0x80)
		{
			RF_MOSI_High();
		}
		else
		{
			RF_MOSI_Low();
		}
		Data<<=1;
		RF_SCL_High();							//主机在上升沿自高位发送数据
		RF_DelayXus(10);						//等待从机接收时钟信号
		if(RF_MISO_Read())
		{
			Data|=0x01;
		}
		RF_SCL_Low();
		RF_DelayXus(10);						//等待从机接收时钟信号
	}
	return Data;
}

/*********************************************************************
*@brief:	nRF24L01写寄存器
*@param:	(读写命令字+地址)=Address
*@param:	欲写入的数据Data
*@retval:	None
*********************************************************************/
void nRF24L01_WriteREG(uint8_t Address,uint8_t Data)
{
	RF_CS_Low();
	SPI_WriteReadByte(Address);
	SPI_WriteReadByte(Data);
	RF_CS_High();
}

/*********************************************************************
*@brief:	nRF24L01读取寄存器数据
*@param:	(读写命令字+地址)=Address
*@retval:	从指定寄存器读取到的数据Data
*********************************************************************/
uint8_t	nRf24L01_ReadREG(uint8_t Address)
{
	uint8_t Data;
	RF_CS_Low();
	SPI_WriteReadByte(Address);
	Data = SPI_WriteReadByte(0);
	RF_CS_High();
	return Data;
}

/*********************************************************************
*@brief:	nRF24L01写数据缓冲器
*@param:	(读写命令字+地址)=Address
*@param:	数据数组指针*Data
*@param:	数组宽度Width(字节)
*@retval:	无
*********************************************************************/
void nRf24L01_WriteBuffer(uint8_t Address,uint8_t* Data,uint8_t Width)
{
    uint8_t byte_ctr=0;
	RF_CS_Low();
    SPI_WriteReadByte(Address);
    for(byte_ctr=0;byte_ctr<Width;byte_ctr++)
    {
        SPI_WriteReadByte(Data[byte_ctr]);
    }
	RF_CS_High();
}

/*********************************************************************
*@brief:	nRF24L01读数据缓冲器
*@param:	(读写命令字+地址)=Address
*@param:	接收数据的数组指针*Data
*@param:	数组宽度Width(字节)
*@retval:	None
*********************************************************************/
void nRf24L01_ReadBuffer(uint8_t Address,uint8_t* Data,uint8_t Width)
{
    uint8_t byte_ctr = 0;
	RF_CS_Low();
    SPI_WriteReadByte(Address);
    for(byte_ctr = 0;byte_ctr < Width;byte_ctr ++ )
    {
        Data[byte_ctr]=SPI_WriteReadByte(0);
    }
	RF_CS_High();
}

/*********************************************************************
*@brief:	nRF24L01进入发射模式
*@param:	None
*@retval:	None
*********************************************************************/
void nRf24L01_IntoTxMode(void)
{
	RF_CE_Low();																//拉低CE引脚
	nRF24L01_WriteREG(Write_REG+CONFIG,0x0A);									//PWR_UP,取消RX，进入待机模式I
	RF_DelayXus(15);															//延时15us，确保CE有效时间
}

/*********************************************************************
*@brief:	nRF24L01进入接收模式
*@param:	None
*@retval:	None
*********************************************************************/
void nRf24L01_IntoRxMode(void)
{
	RF_CE_Low();																//拉低CE引脚
	nRF24L01_WriteREG(Write_REG+CONFIG,0x0B);									//PRIM_RX,进入接收模式
	RF_CE_High();																//拉高CE引脚，进入待机模式II
	RF_DelayXus(2000);															//延时15us，确保CE有效时间
}

/*********************************************************************
*@brief:	nRF24L01发送数据
*@param:	欲发送的数据Data
*@retval:	None
*@note:		若写入的数据宽度未满足PAYLOAD_WIDTH，则nRF24L01会将数据
*			以PAYLOAD_WIDTH的宽度进行发送
*********************************************************************/
void nRF24L01_TransmitData(uint8_t* Tx_Data)
{
	nRf24L01_WriteBuffer(W_TX_PAYLOAD,Tx_Data,PAYLOAD_WIDTH);
	RF_CE_High();																//拉高CE引脚，发送数据
	RF_DelayXus(15);															//延时15us，确保CE有效时间、
	RF_CE_Low();																//拉低CE引脚
}

/*********************************************************************
*@brief:	nRF24L01接收数据
*@param:	None
*@retval:	None
*********************************************************************/
void nRF24L01_ReceiveData(uint8_t* Rx_Data)
{
	RF_CE_Low();
	nRf24L01_ReadBuffer(R_RX_PAYLOAD,Rx_Data,PAYLOAD_WIDTH);
	RF_CE_High();
}

/*********************************************************************
*@brief:	重设当前频道
*@param:	欲重新设置的频道号Channel
*@retval:	无
*@note:		重设频道应在空闲状态下进行
*********************************************************************/
void nRF24L01_ChannelReset(unsigned char Channel)
{
	RF_CE_Low();																//拉低CE引脚，进入待机模式I
	nRF24L01_WriteREG(FLUSH_TX,0);												//清空TX FIFO寄存器
	nRF24L01_WriteREG(FLUSH_RX,0);												//清空RX FIFO寄存器
    nRF24L01_WriteREG(Write_REG+RF_CH,Channel);
	RF_DelayXus(10);															//延时15us，确保CE有效时间
}

/*********************************************************************
*@brief:	nRF24L01初始化
*@param:	None
*@retval:	None
*********************************************************************/
void nRf24L01_Init(void)
{
	uint8_t TRX_ADDR_P0_CFG[ADDRESS_WIDTH]={0xAA,0x55,0x12,0x34};
	nRF24L01_Pin_Config();
	nRF24L01_WriteREG(Write_REG+CONFIG,0x08);									//(0x00)开CRC校验，掉电模式，发射模式
	nRF24L01_WriteREG(Write_REG+EN_AA_Enhanced,0x00);							//(0x01)不允许自动应答
	nRF24L01_WriteREG(Write_REG+EN_RXADDR,0x01);								//(0x02)允许通道1接受数据
	nRF24L01_WriteREG(Write_REG+SETUP_AW,ADDRESS_WIDTH-2);						//(0x03)发射/接收地址长度为AddressWidth,根据有效性-2
	nRF24L01_WriteREG(Write_REG+RF_CH,32);										//(0x05)通信频道为32(0010 0000),有效范围为(0~127)
	nRF24L01_WriteREG(Write_REG+RF_SETUP,0x03);									//(0x06)传输速率1Mbps，发射功率最大12dBm
//	nRF24L01_WriteREG(Write_REG+STATUS,0x70);									//(0x07)默认参数
	nRf24L01_WriteBuffer(Write_REG+RX_ADDR_P0,TRX_ADDR_P0_CFG,ADDRESS_WIDTH);	//(0x0A)通道0的接收地址
//	nRf24L01_WriteBuffer(Write_REG+RX_ADDR_P1,TRX_ADDR_P0_CFG,ADDRESS_WIDTH);	//(0x0B)通道1的接收地址
//	nRf24L01_WriteBuffer(Write_REG+RX_ADDR_P2,TRX_ADDR_P0_CFG,ADDRESS_WIDTH);	//(0x0C)通道2的接收地址
//	nRf24L01_WriteBuffer(Write_REG+RX_ADDR_P3,TRX_ADDR_P0_CFG,ADDRESS_WIDTH);	//(0x0D)通道3的接收地址
//	nRf24L01_WriteBuffer(Write_REG+RX_ADDR_P4,TRX_ADDR_P0_CFG,ADDRESS_WIDTH);	//(0x0E)通道4的接收地址
//	nRf24L01_WriteBuffer(Write_REG+RX_ADDR_P5,TRX_ADDR_P0_CFG,ADDRESS_WIDTH);	//(0x0F)通道5的接收地址
	nRf24L01_WriteBuffer(Write_REG+TX_ADDR,TRX_ADDR_P0_CFG,ADDRESS_WIDTH);		//(0x10)发射地址宽度
	nRF24L01_WriteREG(Write_REG+RX_PW_P0,4);									//(0x11)通道0有效数据宽度为3
//	nRF24L01_WriteREG(Write_REG+RX_PW_P1,0);									//(0x12)通道1有效数据宽度
//	nRF24L01_WriteREG(Write_REG+RX_PW_P2,0);									//(0x13)通道2有效数据宽度
//	nRF24L01_WriteREG(Write_REG+RX_PW_P3,0);									//(0x14)通道3有效数据宽度
//	nRF24L01_WriteREG(Write_REG+RX_PW_P4,0);									//(0x15)通道4有效数据宽度
//	nRF24L01_WriteREG(Write_REG+RX_PW_P5,0);									//(0x16)通道5有效数据宽度
	nRF24L01_WriteREG(Write_REG+FIFO_STATUS,0x11);								//(0x17)不重复发送上一数据						
}

/************************************************************************
*							End Of File
************************************************************************/
