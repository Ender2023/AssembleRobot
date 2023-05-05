#include <stdbool.h>
#include "Camera.h"
#include "bsp_serial.h"

USART_DataFrameType cameraSerialDataFrame;      //摄像头串口数据帧格式

/**
 * @brief:  摄像头串口初始化
*/
void Camera_Init(void)
{
    USART_InitTypeDef   USART_InitStruct;
    GPIO_InitTypeDef    GPIO_InitStruct;
    NVIC_InitTypeDef    NVIC_InitStruct;

    Display_Logged("Init camera...\n");

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | TTYS_CAMERA_RCC,ENABLE);

    /*IO配置*/
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOA,&GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA,&GPIO_InitStruct);

    /*串口配置*/
    USART_InitStruct.USART_BaudRate = TTYS_CAMERA_BUADRATE;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;

    USART_Init(TTYS_CAMERA,&USART_InitStruct);

    /*串口中断配置*/
    USART_ITConfig(TTYS_CAMERA,USART_IT_RXNE,ENABLE);
    USART_ITConfig(TTYS_CAMERA,USART_IT_IDLE,ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStruct);

    USART_Cmd(TTYS_CAMERA,ENABLE);
    Display_Logged("Camera init done!\n");
}

/**
 * @brief:  摄像头停止图像采样
*/
void Camera_stopSample(void)
{
    serial_Printf(TTYS_CAMERA,"stop\n");
}

/**
 * @brief:  摄像头开启图像采样
*/
void Camera_startSample(void)
{
    serial_Printf(TTYS_CAMERA,"start\n");
}

/**
 * @brief:  检查摄像头串口线上是否已有数据接收到且总线空闲
 * @param:  None
 * @retval: true:有数据接受完成
 *          false:未有数据接收完成
*/
bool Camera_ttyS_BusIDLE(void)
{
    /*检查数据是否接收完成*/
    if(cameraSerialDataFrame.FrameBit.FrameFinishFlag)
    {
        /*打印日志信息*/
        Display_Logged("Camera:%s\n",cameraSerialDataFrame.RX_BUF);

        /*清空数据接收区*/
        cameraSerialDataFrame.FrameBit.FrameFinishFlag = 0;
        cameraSerialDataFrame.FrameBit.FrameCNT = 0;
        
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief:  摄像头串口接收事件回调函数
 * 
*/
void CAMERA_TTYSX_BUS_IRQHANDLER(void)
{
    if(USART_GetITStatus(TTYS_CAMERA,USART_IT_RXNE))
    {
        if(cameraSerialDataFrame.FrameBit.FrameCNT < SERIAL_MAXDATA_SIZE)
        cameraSerialDataFrame.RX_BUF[cameraSerialDataFrame.FrameBit.FrameCNT ++] = USART_ReceiveData(TTYS_CAMERA);
        /*通过对USART_DR的读操作可以将USART_IT_RXNE清0*/
    }
    if(USART_GetITStatus(TTYS_CAMERA,USART_IT_IDLE) == SET )
    {
        cameraSerialDataFrame.FrameBit.FrameFinishFlag = 1;		                        //代表总线进入空闲状态，数据接收完毕
        cameraSerialDataFrame.RX_BUF[cameraSerialDataFrame.FrameBit.FrameCNT] = '\0';   //清空结尾，避免多读
        USART_ReceiveData(TTYS_CAMERA);				                                    //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)
        Camera_stopSample();                                                            //告知摄像头停止图像识别
        SYS_DEBUG("%s\n",cameraSerialDataFrame.RX_BUF);
    }
}
