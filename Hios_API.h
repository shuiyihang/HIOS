#ifndef  __HIOS_API_H_
#define  __HIOS_API_H_
#include <Hios_core.h>
#include <Hios.h>
extern u8 LockCount;
extern u32 GlobalTicks;
extern u8  HardTrigPendSv;




extern H_PCB* GlobalPcb;
extern H_PCB* NoIdlePCB;
extern H_PCB* Task1PCB;
extern H_PCB* Task2PCB;
extern H_PCB* Task3PCB;
extern H_PCB* RootPCB;
extern ReadyTable 		GlobalTable;
extern H_LIST     GlobalDealyTable;

extern u8  RootStack[STACKSIZE];
extern u8  NoIdleStack[STACKSIZE];
extern u8  Task1Stack[STACKSIZE];
extern u8  Task2Stack[STACKSIZE];
extern u8  Task3Stack[STACKSIZE];

extern STACKREG* CurStackRegAdr;
extern STACKREG* NextStackRegAdr;

void HIOS_StackInit(FUNC FuncAdr,void* arg,H_PCB* pcb);
H_PCB* HIOS_TaskCreat(FUNC FuncAdr,void* arg,u8* TaskStack,u32 StackSize,u8 TaskPri,TaskArg* taskStat);//任务创建
void HIOS_TaskSwitch(H_PCB* pcb);
void HIOS_TaskStart(H_PCB* pcb);
void Start_HIOS(void);//开始操作系统
void ListInsert(H_LIST* tableNode,H_LIST* taskNode);//链表插入
void SetReadyPriFlag(u8* tableflag,u8 TaskPri);//设置ready表标志位
void TaskInsertReadyTable(H_LIST* tableNode,H_LIST* taskNode,u8* tableflag,u8 TaskPri);//插入ready表
u8 GetTaskReadyHighPri(u8 PriFlag);//得到最高优先级就绪任务
H_PCB* HIOS_StartInit(void);

H_PCB* GetPcbAdr(H_LIST* Node);//得到PCB指针
H_LIST* GetReadyTabHighTask(H_LIST* link);//得到ready表最高优先级任务
H_PCB* HIOS_ReadyTabSched(void);//ready表调度,返回任务PCB地址


void ReadyToDelay(u32 ticks);//状态切换,参数要延迟的时间
void DelayTabSche(void);//delay表的调度,从delay到ready
H_LIST* DelayTabDel(H_LIST* RootDelayTab);//删除delay节点
void DelayTabInsert(H_LIST* Node);//插入delay节点,结束时间按从小到大的顺序排列
void  DelayTabInit(void);//delay表初始化

void HIOS_TaskSched(void);
	
void HIOS_LOCK(void);
void HIOS_UNLOCK(void);

/*普通调用函数*/
void  SystickInit(u16 Ms);
void  Task1(void* arg);
void  Task2(void* arg);
void  Task3(void* arg);
void HIOS_RootTask(void);//根任务
void HardWareInit(void);


/*
以下为汇编函数
*/
extern void PendSV_Handler(void);
extern void TrigPendSv(void);//触发PendSv中断
extern void PendSvSetPri(void);//设置PendSv优先级最低

extern void ContextSwitch(STACKREG* CurStackRegAdr,STACKREG* NextStackRegAdr);//任务切换



#endif