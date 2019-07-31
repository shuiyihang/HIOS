#ifndef  __HIOS_CORE_H_
#define  __HIOS_CORE_H_
#include "sys.h"
#define  PRIORITYNUM    8//优先级个数
/*
AAPCS规定
父函数调用子函数
由父函数备份R0-R3寄存器中有用值，没有则不予处理
子函数返回时也不用恢复其中的值
*/
typedef  struct
{
	u32 hiR0;//函数参数使用
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
	u32 hiR13;//SP栈指针
	u32 hiR14;//LR链接寄存器
	u32 hiR15;//PC
	u32 hiXpsr;
	u32 hiExc_Return;//内核模式和SP类型决定,三个取值
	
	
}STACKREG;


typedef struct list//delay，ready表使用
{
	struct list* Head;
	struct list* Tail;
}H_LIST;

typedef struct  link
{
	H_LIST PcbNode;//一个PCB节点,挂在ready表上
	struct pcb*  PstrPcb; //指向自身的PCB基址
}PointLink;

typedef struct 
{
	u8 TaskSta;//任务运行状态
	u32 time;//延迟时间
}TaskArg;

typedef  struct pcb
{
	STACKREG StackReg;
	PointLink  Point;//链结点
	u8 TaskPrio;//任务优先级			0-7
	
	TaskArg taskarg;
	u32 StillTime;//delay时间
	
}H_PCB;

/*
需要ready表，双向循环链表
只需要8个优先级
*/
typedef  struct
{
	H_LIST table[PRIORITYNUM];
	u8  PriFlag;//八位,每位表示头节点下是否有任务
}ReadyTable;






#endif