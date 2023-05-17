#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "Camera.h"
#include "bsp_serial.h"

USART_DataFrameType cameraSerialDataFrame;      //摄像头串口数据帧格式
Camera_dataType cameraData;                     //摄像头接收数据句柄
bool cameraValidData = false;                   //摄像头有效数据接收标志位

/**
 * @brief:  摄像头串口初始化
*/
void Camera_Init(void)
{
    USART_InitTypeDef   USART_InitStruct;
    GPIO_InitTypeDef    GPIO_InitStruct;
    NVIC_InitTypeDef    NVIC_InitStruct;

    Display_Logged(LOG_RANK_INFO,"Init cam...\n");
    cameraData.isOK = false;

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
    Display_Logged(LOG_RANK_WARNNING,"Cam:need ack...\n");
}

/**
 * @brief:  查看摄像头状态
 * @retval: true:   摄像头初始化完成
 *          false:  尚未初始化完成
*/
bool Camera_getStatus(void)
{
    if( cameraData.isOK)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief:  摄像头停止图像采样
*/
void Camera_stopSample(void)
{
    serial_Printf(TTYS_CAMERA,"X");
}

/**
 * @brief:  摄像头开启图像采样
*/
void Camera_startSample(void)
{
    serial_Printf(TTYS_CAMERA,"O");
    Display_Logged(LOG_RANK_INFO,"Robot:O\n");
}

/**
 * @brief:  摄像头接收数据处理
 * @param:  None
 * @retval: -1:收到错误的数据格式
 *          -2:收到错误的数据内容
*/
int Camera_dataHandle(void)
{
    /*裁剪用临时字符串*/
    char tmp[CAMERA_TMPDATA_BUFSIZE];

    /*拷贝原有字符串*/
    strcpy(tmp,cameraSerialDataFrame.RX_BUF);


    /*1.裁剪形状相关字符串*/
	char * str = strtok(tmp,",");
    /*判断是否为错误的数据接收格式*/
    if( str == NULL )
    {
        goto ERR_FORMAT_HANDLE;
    }

    /*判断形状*/
    if( strcmp(str,"R") == 0 )              //鉴定为正方型
    {
        cameraData.shape = shapeSquare;
        cameraData.cmd = justCarry;
        cameraValidData = true;
        Display_Logged(LOG_RANK_OK,"Cam:Is square\n");
        return 0;
    }
    else if( strcmp(str,"T") == 0 )         //鉴定为正三角形
    {
        cameraData.shape = shapeTriangle;
        cameraData.cmd = justCarry;
        cameraValidData = true;
        Display_Logged(LOG_RANK_OK,"Cam:Is triangle\n");
        return 0;
    }
    else if( strcmp(str,"C") == 0 )         //鉴定为圆形
    {
        cameraData.shape = shapeCircle;
        cameraData.cmd = justCarry;
        cameraValidData = true;
        Display_Logged(LOG_RANK_OK,"Cam:Is circle\n");
        return 0;
    }
    else if( strcmp(str,"RC") == 0 )         //鉴定为偏差方型
    {
        cameraData.shape = shapeSquare;
        cameraData.cmd = needAdjust;
        Display_Logged(LOG_RANK_OK,"Cam:Near square\n");
    }
    else if( strcmp(str,"TC") == 0 )         //鉴定为偏差三角型
    {
        cameraData.shape = shapeTriangle;
        cameraData.cmd = needAdjust;
        Display_Logged(LOG_RANK_OK,"Cam:Near triangle\n");
    }
    else
    {
        goto ERR_DATATYPE_HANDLE;
    }

    // /*1.裁剪形状相关字符串*/
	// char * str = strtok(tmp,",");
    // /*判断是否为错误的数据接收格式*/
    // if( str == NULL )
    // {
    //     goto ERR_FORMAT_HANDLE;
    // }

    // /*存储并打印*/
    // strcpy(&cameraData.shape,str);
	// Display_Logged(LOG_RANK_OK,"shape:%s\n", str);

    // /*2.裁剪x坐标偏差相关字符串*/
    // str = strtok(NULL, ",");
    // /*判断是否为错误的数据接收格式*/
    // if( str == NULL )
    // {
    //     goto ERR_FORMAT_HANDLE;
    // }

    // /*存储并打印*/
    // cameraData.x_err = atoi(str);
    // Display_Logged(LOG_RANK_OK,"x_err:%d\n", cameraData.x_err);

	// /*3.裁剪y坐标偏差相关字符串*/
    // str = strtok(NULL, ",");
    // /*判断是否为错误的数据接收格式*/
    // if( str == NULL )
    // {
    //     goto ERR_FORMAT_HANDLE;
    // }

    /*存储并打印*/
    //cameraData.y_err = atoi(str);
    //Display_Logged(LOG_RANK_OK,"y_err:%d\n", cameraData.y_err);

    /*通知task进程对数据进行处理*/
    cameraValidData = true;
    return 0;

ERR_FORMAT_HANDLE:
        Display_Logged(LOG_RANK_ERROR,"Cam:Error data format!\n");      //错误的数据格式
        return -1;

ERR_DATATYPE_HANDLE:
        Display_Logged(LOG_RANK_ERROR,"Cam:Error data content!\n");     //错误的数据内容
        return -2;

}

/**
 * @brief:  检查摄像头串口线上是否已有数据接收到且总线空闲
*/
void Camera_ttyS_CheckBusIDLE(void)
{
    /*检查数据是否接收完成*/
    if(cameraSerialDataFrame.FrameBit.FrameFinishFlag)
    {
        /*打印日志信息*/
        if(strlen(cameraSerialDataFrame.RX_BUF) > 25)
        {
            Display_Logged(LOG_RANK_ERROR,"Cam:%s\n","too long string!");
        }
        else
        {
            Display_Logged(LOG_RANK_INFO,"Cam:%s\n",cameraSerialDataFrame.RX_BUF);
        }

        /*若摄像头未初始化完成*/
        if( cameraData.isOK == false )
        {
            /*如果为摄像头上电初始化信息*/
            if( strcmp(cameraSerialDataFrame.RX_BUF,"OK") == 0 )
            {
                cameraData.isOK = true;
            }
            else
            {
                Display_Logged(LOG_RANK_ERROR,"Cam:error ack!\n");
                Display_Logged(LOG_RANK_WARNNING,"[-]Cam:ack retry\n");
            }
        }
        else
        {
            /*告知摄像头停止图像识别*/
            //Camera_stopSample();
            /*处理数据*/
            Camera_dataHandle();
        }

        /*清空数据接收区*/
        cameraSerialDataFrame.FrameBit.FrameFinishFlag = 0;
        cameraSerialDataFrame.FrameBit.FrameCNT = 0;
    }
}

/**
 * @brief:  摄像头串口接收事件回调函数
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

        SYS_DEBUG("%s\n",cameraSerialDataFrame.RX_BUF);
    }
}
