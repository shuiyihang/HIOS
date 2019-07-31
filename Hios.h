#ifndef  __HIOS_H_
#define  __HIOS_H_
#include "sys.h"
#include <Hios_core.h>
#define  THUMB2    0x01000000
#define  ALIGN4    0xFFFFFFFC//四字节对齐
#define  ALIGN8    0xFFFFFFF8//八字节对齐
#define  STACKSIZE    400
#define  EXC_RETURN_MODE		0xFFFFFFF9//thread模式,使用MSP栈


#define  TASKREADY       10
#define  TASKDELAY       20

#define  TICKSCHEDSET    30
#define  TICKSCHEDCLR    40

//主函数使用，与系统无关可以不管
#define  KEY_LEFT        1
#define  KEY_RIGHT       2
#define  KEY_Up					 3
#define  KEY_DOWN				 4
#define  KEY_NOVALUE     5
extern u8 Dir;
extern u8 GolbalX,GolbalY;
//
extern H_LIST*   watch;
extern H_PCB*    watchPcb;

extern u8 AuxSeekPri[8];//辅助查找优先级节点
typedef void (*FUNC)(void*);//带形参void*不确定类型参数




#endif