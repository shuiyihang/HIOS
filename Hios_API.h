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
H_PCB* HIOS_TaskCreat(FUNC FuncAdr,void* arg,u8* TaskStack,u32 StackSize,u8 TaskPri,TaskArg* taskStat);//���񴴽�
void HIOS_TaskSwitch(H_PCB* pcb);
void HIOS_TaskStart(H_PCB* pcb);
void Start_HIOS(void);//��ʼ����ϵͳ
void ListInsert(H_LIST* tableNode,H_LIST* taskNode);//�������
void SetReadyPriFlag(u8* tableflag,u8 TaskPri);//����ready���־λ
void TaskInsertReadyTable(H_LIST* tableNode,H_LIST* taskNode,u8* tableflag,u8 TaskPri);//����ready��
u8 GetTaskReadyHighPri(u8 PriFlag);//�õ�������ȼ���������
H_PCB* HIOS_StartInit(void);

H_PCB* GetPcbAdr(H_LIST* Node);//�õ�PCBָ��
H_LIST* GetReadyTabHighTask(H_LIST* link);//�õ�ready��������ȼ�����
H_PCB* HIOS_ReadyTabSched(void);//ready�����,��������PCB��ַ


void ReadyToDelay(u32 ticks);//״̬�л�,����Ҫ�ӳٵ�ʱ��
void DelayTabSche(void);//delay��ĵ���,��delay��ready
H_LIST* DelayTabDel(H_LIST* RootDelayTab);//ɾ��delay�ڵ�
void DelayTabInsert(H_LIST* Node);//����delay�ڵ�,����ʱ�䰴��С�����˳������
void  DelayTabInit(void);//delay���ʼ��

void HIOS_TaskSched(void);
	
void HIOS_LOCK(void);
void HIOS_UNLOCK(void);

/*��ͨ���ú���*/
void  SystickInit(u16 Ms);
void  Task1(void* arg);
void  Task2(void* arg);
void  Task3(void* arg);
void HIOS_RootTask(void);//������
void HardWareInit(void);


/*
����Ϊ��ຯ��
*/
extern void PendSV_Handler(void);
extern void TrigPendSv(void);//����PendSv�ж�
extern void PendSvSetPri(void);//����PendSv���ȼ����

extern void ContextSwitch(STACKREG* CurStackRegAdr,STACKREG* NextStackRegAdr);//�����л�



#endif