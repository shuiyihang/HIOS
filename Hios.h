#ifndef  __HIOS_H_
#define  __HIOS_H_
#include "sys.h"
#include <Hios_core.h>
#define  THUMB2    0x01000000
#define  ALIGN4    0xFFFFFFFC//���ֽڶ���
#define  ALIGN8    0xFFFFFFF8//���ֽڶ���
#define  STACKSIZE    400
#define  EXC_RETURN_MODE		0xFFFFFFF9//threadģʽ,ʹ��MSPջ


#define  TASKREADY       10
#define  TASKDELAY       20

#define  TICKSCHEDSET    30
#define  TICKSCHEDCLR    40

//������ʹ�ã���ϵͳ�޹ؿ��Բ���
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

extern u8 AuxSeekPri[8];//�����������ȼ��ڵ�
typedef void (*FUNC)(void*);//���β�void*��ȷ�����Ͳ���




#endif