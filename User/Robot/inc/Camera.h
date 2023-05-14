#ifndef __CAMERA_H
#define __CAMERA_H

#include "system.h"
#include "turnplate.h"

#define TTYS_CAMERA                 USART1
#define TTYS_CAMERA_RCC             RCC_APB2Periph_USART1
#define TTYS_CAMERA_BUADRATE        9600

#define CAMERA_TTYSX_BUS_IRQHANDLER USART1_IRQHandler

#define CAMERA_TMPDATA_BUFSIZE      32

typedef enum{justCarry = 0,needAdjust}Camera_CMD;               //摄像头命令

/*摄像头数据类型*/
typedef struct 
{
    bool            isOK;                           			//摄像头初始化完成标志位
    Contentshape    shape;                          			//识别到的物体形状
    Camera_CMD      cmd;                                        //摄像头命令字
    uint32_t        x_err;                          			//x方向误差
    uint32_t        y_err;                          			//y方向误差
    uint32_t        angle_err;                      			//角度误差
}Camera_dataType;

extern Camera_dataType cameraData;                 		        //摄像头接收数据句柄
extern bool cameraValidData;                                    //摄像头有效数据接收标志位

void Camera_Init(void);
bool Camera_getStatus(void);
void Camera_stopSample(void);
void Camera_startSample(void);
void Camera_ttyS_CheckBusIDLE(void);

#endif
