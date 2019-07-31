#ifndef  __HIOS_CORE_H_
#define  __HIOS_CORE_H_
#include "sys.h"
#define  PRIORITYNUM    8//���ȼ�����
/*
AAPCS�涨
�����������Ӻ���
�ɸ���������R0-R3�Ĵ���������ֵ��û�����账��
�Ӻ�������ʱҲ���ûָ����е�ֵ
*/
typedef  struct
{
	u32 hiR0;//��������ʹ��
	u32 hiR1;
	u32 hiR2;
	u32 hiR3;
	u32 hiR4;
	u32 hiR5;
	u32 hiR6;
	u32 hiR7;
	u32 hiR8;
	u32 hiR9;
	u32 hiR10;
	u32 hiR11;
	u32 hiR12;//
	u32 hiR13;//SPջָ��
	u32 hiR14;//LR���ӼĴ���
	u32 hiR15;//PC
	u32 hiXpsr;
	u32 hiExc_Return;//�ں�ģʽ��SP���;���,����ȡֵ
	
	
}STACKREG;


typedef struct list//delay��ready��ʹ��
{
	struct list* Head;
	struct list* Tail;
}H_LIST;

typedef struct  link
{
	H_LIST PcbNode;//һ��PCB�ڵ�,����ready����
	struct pcb*  PstrPcb; //ָ�������PCB��ַ
}PointLink;

typedef struct 
{
	u8 TaskSta;//��������״̬
	u32 time;//�ӳ�ʱ��
}TaskArg;

typedef  struct pcb
{
	STACKREG StackReg;
	PointLink  Point;//�����
	u8 TaskPrio;//�������ȼ�			0-7
	
	TaskArg taskarg;
	u32 StillTime;//delayʱ��
	
}H_PCB;

/*
��Ҫready��˫��ѭ������
ֻ��Ҫ8�����ȼ�
*/
typedef  struct
{
	H_LIST table[PRIORITYNUM];
	u8  PriFlag;//��λ,ÿλ��ʾͷ�ڵ����Ƿ�������
}ReadyTable;






#endif