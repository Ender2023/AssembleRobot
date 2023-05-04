/*********************************************************************
*@file:		nRf24L01.c
*@brief:	nRF24L01��ƵͨѶģ��
*@author:	Pan Zhikang
*@date:		2022-3-26
*********************************************************************/
#include "bsp_nRf24L01.h"

uint8_t Tx_Data[PAYLOAD_WIDTH];
uint8_t Rx_Data[PAYLOAD_WIDTH];

/*********************************************************************
*@brief:	nRF24L01�ڲ���ʱ����
*@param:	��Ҫ��ʱ��ʱ��xus(��λΪus)
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
*@brief:	nRF24L01��SPI��������
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

	RF_CS_High();													//Ƭѡ���ߣ����ϵ�ʹ��
	RF_CE_Low();													//ʹ��оƬ
}

/*********************************************************************
*@brief:	SPI��/дһ���ֽڵĵ�ַ/����
*@param:	��д�������Data
*@retval:	��������8λ����
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
		RF_SCL_High();							//�������������Ը�λ��������
		RF_DelayXus(10);						//�ȴ��ӻ�����ʱ���ź�
		if(RF_MISO_Read())
		{
			Data|=0x01;
		}
		RF_SCL_Low();
		RF_DelayXus(10);						//�ȴ��ӻ�����ʱ���ź�
	}
	return Data;
}

/*********************************************************************
*@brief:	nRF24L01д�Ĵ���
*@param:	(��д������+��ַ)=Address
*@param:	��д�������Data
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
*@brief:	nRF24L01��ȡ�Ĵ�������
*@param:	(��д������+��ַ)=Address
*@retval:	��ָ���Ĵ�����ȡ��������Data
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
*@brief:	nRF24L01д���ݻ�����
*@param:	(��д������+��ַ)=Address
*@param:	��������ָ��*Data
*@param:	������Width(�ֽ�)
*@retval:	��
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
*@brief:	nRF24L01�����ݻ�����
*@param:	(��д������+��ַ)=Address
*@param:	�������ݵ�����ָ��*Data
*@param:	������Width(�ֽ�)
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
*@brief:	nRF24L01���뷢��ģʽ
*@param:	None
*@retval:	None
*********************************************************************/
void nRf24L01_IntoTxMode(void)
{
	RF_CE_Low();																//����CE����
	nRF24L01_WriteREG(Write_REG+CONFIG,0x0A);									//PWR_UP,ȡ��RX���������ģʽI
	RF_DelayXus(15);															//��ʱ15us��ȷ��CE��Чʱ��
}

/*********************************************************************
*@brief:	nRF24L01�������ģʽ
*@param:	None
*@retval:	None
*********************************************************************/
void nRf24L01_IntoRxMode(void)
{
	RF_CE_Low();																//����CE����
	nRF24L01_WriteREG(Write_REG+CONFIG,0x0B);									//PRIM_RX,�������ģʽ
	RF_CE_High();																//����CE���ţ��������ģʽII
	RF_DelayXus(2000);															//��ʱ15us��ȷ��CE��Чʱ��
}

/*********************************************************************
*@brief:	nRF24L01��������
*@param:	�����͵�����Data
*@retval:	None
*@note:		��д������ݿ��δ����PAYLOAD_WIDTH����nRF24L01�Ὣ����
*			��PAYLOAD_WIDTH�Ŀ�Ƚ��з���
*********************************************************************/
void nRF24L01_TransmitData(uint8_t* Tx_Data)
{
	nRf24L01_WriteBuffer(W_TX_PAYLOAD,Tx_Data,PAYLOAD_WIDTH);
	RF_CE_High();																//����CE���ţ���������
	RF_DelayXus(15);															//��ʱ15us��ȷ��CE��Чʱ�䡢
	RF_CE_Low();																//����CE����
}

/*********************************************************************
*@brief:	nRF24L01��������
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
*@brief:	���赱ǰƵ��
*@param:	���������õ�Ƶ����Channel
*@retval:	��
*@note:		����Ƶ��Ӧ�ڿ���״̬�½���
*********************************************************************/
void nRF24L01_ChannelReset(unsigned char Channel)
{
	RF_CE_Low();																//����CE���ţ��������ģʽI
	nRF24L01_WriteREG(FLUSH_TX,0);												//���TX FIFO�Ĵ���
	nRF24L01_WriteREG(FLUSH_RX,0);												//���RX FIFO�Ĵ���
    nRF24L01_WriteREG(Write_REG+RF_CH,Channel);
	RF_DelayXus(10);															//��ʱ15us��ȷ��CE��Чʱ��
}

/*********************************************************************
*@brief:	nRF24L01��ʼ��
*@param:	None
*@retval:	None
*********************************************************************/
void nRf24L01_Init(void)
{
	uint8_t TRX_ADDR_P0_CFG[ADDRESS_WIDTH]={0xAA,0x55,0x12,0x34};
	nRF24L01_Pin_Config();
	nRF24L01_WriteREG(Write_REG+CONFIG,0x08);									//(0x00)��CRCУ�飬����ģʽ������ģʽ
	nRF24L01_WriteREG(Write_REG+EN_AA_Enhanced,0x00);							//(0x01)�������Զ�Ӧ��
	nRF24L01_WriteREG(Write_REG+EN_RXADDR,0x01);								//(0x02)����ͨ��1��������
	nRF24L01_WriteREG(Write_REG+SETUP_AW,ADDRESS_WIDTH-2);						//(0x03)����/���յ�ַ����ΪAddressWidth,������Ч��-2
	nRF24L01_WriteREG(Write_REG+RF_CH,32);										//(0x05)ͨ��Ƶ��Ϊ32(0010 0000),��Ч��ΧΪ(0~127)
	nRF24L01_WriteREG(Write_REG+RF_SETUP,0x03);									//(0x06)��������1Mbps�����书�����12dBm
//	nRF24L01_WriteREG(Write_REG+STATUS,0x70);									//(0x07)Ĭ�ϲ���
	nRf24L01_WriteBuffer(Write_REG+RX_ADDR_P0,TRX_ADDR_P0_CFG,ADDRESS_WIDTH);	//(0x0A)ͨ��0�Ľ��յ�ַ
//	nRf24L01_WriteBuffer(Write_REG+RX_ADDR_P1,TRX_ADDR_P0_CFG,ADDRESS_WIDTH);	//(0x0B)ͨ��1�Ľ��յ�ַ
//	nRf24L01_WriteBuffer(Write_REG+RX_ADDR_P2,TRX_ADDR_P0_CFG,ADDRESS_WIDTH);	//(0x0C)ͨ��2�Ľ��յ�ַ
//	nRf24L01_WriteBuffer(Write_REG+RX_ADDR_P3,TRX_ADDR_P0_CFG,ADDRESS_WIDTH);	//(0x0D)ͨ��3�Ľ��յ�ַ
//	nRf24L01_WriteBuffer(Write_REG+RX_ADDR_P4,TRX_ADDR_P0_CFG,ADDRESS_WIDTH);	//(0x0E)ͨ��4�Ľ��յ�ַ
//	nRf24L01_WriteBuffer(Write_REG+RX_ADDR_P5,TRX_ADDR_P0_CFG,ADDRESS_WIDTH);	//(0x0F)ͨ��5�Ľ��յ�ַ
	nRf24L01_WriteBuffer(Write_REG+TX_ADDR,TRX_ADDR_P0_CFG,ADDRESS_WIDTH);		//(0x10)�����ַ���
	nRF24L01_WriteREG(Write_REG+RX_PW_P0,4);									//(0x11)ͨ��0��Ч���ݿ��Ϊ3
//	nRF24L01_WriteREG(Write_REG+RX_PW_P1,0);									//(0x12)ͨ��1��Ч���ݿ��
//	nRF24L01_WriteREG(Write_REG+RX_PW_P2,0);									//(0x13)ͨ��2��Ч���ݿ��
//	nRF24L01_WriteREG(Write_REG+RX_PW_P3,0);									//(0x14)ͨ��3��Ч���ݿ��
//	nRF24L01_WriteREG(Write_REG+RX_PW_P4,0);									//(0x15)ͨ��4��Ч���ݿ��
//	nRF24L01_WriteREG(Write_REG+RX_PW_P5,0);									//(0x16)ͨ��5��Ч���ݿ��
	nRF24L01_WriteREG(Write_REG+FIFO_STATUS,0x11);								//(0x17)���ظ�������һ����						
}

/************************************************************************
*							End Of File
************************************************************************/
