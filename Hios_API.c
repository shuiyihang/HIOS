#include <Hios_core.h>
#include <Hios_API.h>
#include <Hios.h>
#include <stdio.h>
void HIOS_LOCK(void)//上锁，关sys中断
{
	//CTRL零位控制开关  1 开  0关
//	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
	if(0==LockCount)
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
	else
	{
		LockCount++;
	}
}
void HIOS_UNLOCK(void)//解锁，开sys中断
{
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
	if(1==LockCount)
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk; 
	else
	{
		LockCount--;
	}
}

//初始化寄存器组
void HIOS_StackInit(FUNC FuncAdr,void* arg,H_PCB* pcb)//函数地址和寄存器基址
{
	STACKREG *reg;
	u32* AutoStack;
	reg=&pcb->StackReg;
	
	//初始化寄存器组
	reg->hiR0=(u32)arg;//此处存放的是一个地址,为什么给一个地址？？？
	reg->hiR1=0;
	reg->hiR2=0;
	reg->hiR3=0;
	reg->hiR4=0;
	reg->hiR5=0;
	reg->hiR6=0;
	reg->hiR7=0;
	reg->hiR8=0;
	reg->hiR9=0;
	reg->hiR10=0;
	reg->hiR11=0;
	reg->hiR12=0;
	reg->hiR13=(u32)pcb-32;//栈址,在原来基础上再向下生长八个预留空间
	reg->hiR14=0;
	reg->hiR15=(u32)FuncAdr;
	reg->hiXpsr=THUMB2;
	reg->hiExc_Return=EXC_RETURN_MODE;
	
	/*初始化运行时栈,下面八位由硬件自动操作*/
	//有地址越界危险
	AutoStack=(u32*)pcb;
	*(--AutoStack)=reg->hiXpsr;
	*(--AutoStack)=reg->hiR15;
	*(--AutoStack)=reg->hiR14;
	*(--AutoStack)=reg->hiR12;
	*(--AutoStack)=reg->hiR3;
	*(--AutoStack)=reg->hiR2;
	*(--AutoStack)=reg->hiR1;
	*(--AutoStack)=reg->hiR0;
	
}

H_PCB* HIOS_TaskCreat(FUNC FuncAdr,void* arg,u8* TaskStack,u32 StackSize,u8 TaskPri,TaskArg* taskStat)
{
	u8 *StackByte4;
	H_PCB* pcb;
	H_LIST*  ElemReady;
	H_LIST* TaskNode;//分开写,不然不是一个地址变量
	u8* tableflag;
	if((NULL==FuncAdr)||(NULL==TaskStack)||(0==StackSize)||TaskPri>7)
	{
		return (H_PCB*)NULL;
	}
	StackByte4=(u8*)(((u32)TaskStack+StackSize)&ALIGN4);//
	
	pcb=(H_PCB*)(((u32)StackByte4-sizeof(H_PCB))&ALIGN8);//八字节对齐
	
	HIOS_StackInit(FuncAdr,arg,pcb);//初始化寄存器组
	
	if(NULL==taskStat)//默认ready态
	{
		pcb->taskarg.TaskSta=TASKREADY;
	}else
	{
		pcb->taskarg.TaskSta=taskStat->TaskSta;
		pcb->taskarg.time=taskStat->time;
	}

	pcb->Point.PstrPcb=pcb;//指向自己的基址
	pcb->TaskPrio=TaskPri;//任务优先级
	
	
	TaskNode=&pcb->Point.PcbNode;
	
	/*插入ready表，原子操作*/
	HIOS_LOCK();
	
	if(TASKREADY==(pcb->taskarg.TaskSta))//插入ready表
	{
		ElemReady=&GlobalTable.table[TaskPri];//获取ready表某个元素
		tableflag=&GlobalTable.PriFlag;
		
		
		TaskInsertReadyTable(ElemReady,TaskNode,tableflag,TaskPri);
	}
	if(TASKDELAY==(pcb->taskarg.TaskSta))//插入delay表
	{
		pcb->StillTime=GlobalTicks+taskStat->time;
		DelayTabInsert(TaskNode);
	}
	HIOS_UNLOCK();
	return pcb;
}

void HIOS_TaskSwitch(H_PCB* pcb)
{
	CurStackRegAdr=&GlobalPcb->StackReg;
	NextStackRegAdr=&pcb->StackReg;
	
	GlobalPcb=pcb;//更新现在运行的任务信息
	
	//切换任务
	//ContextSwitch(CurStackRegAdr,NextStackRegAdr);
}
H_LIST* GetReadyTabHighTask(H_LIST* Rootlink)
{
	//传入根节点
	//查找下一个返回，应该将此节点删除//////////////////////////////////////
	return Rootlink->Tail;
}


H_PCB* HIOS_ReadyTabSched()//返回任务PCB地址
{
	H_PCB *pcb;
	H_LIST* Node;
//	PointLink* ReadyHighNode;//已就绪的最高根节点
	u8 HighPriIndex;//最高优先级下标
	HighPriIndex=GetTaskReadyHighPri(GlobalTable.PriFlag);
//	ReadyHighNode=(PointLink*)GetReadyTabHighTask();
//	
//	=ReadyHighNode->PstrPcb;
	Node=GlobalTable.table[HighPriIndex].Tail;
	pcb=GetPcbAdr(Node);
	return pcb;
	
}
void HIOS_TaskSched()//在PendSv中断里调用
{
	
	H_PCB *pcb;
	//加一个delay表调度
	//定义一个全局标志
	if(TICKSCHEDSET==HardTrigPendSv)//调度由硬件触发
	{
		HardTrigPendSv=TICKSCHEDCLR;
		DelayTabSche();//delay表调度
	}
	
	//readyTable调度
	pcb=HIOS_ReadyTabSched();
	
	//任务切换
	HIOS_TaskSwitch(pcb);
}
void ListInsert(H_LIST* tableNode,H_LIST* taskNode)//头插法
{

	if((NULL==tableNode->Tail)&&(NULL==tableNode->Head))
	{
		//首次插入
		tableNode->Tail=tableNode->Head=taskNode;
		taskNode->Head=taskNode->Tail=tableNode;
	}else
	{
		taskNode->Tail=tableNode->Tail;
		taskNode->Head=tableNode;
		tableNode->Tail->Head=taskNode;
		tableNode->Tail=taskNode;
	}
	
}
void SetReadyPriFlag(u8* tableflag,u8 TaskPri)//设置ready表优先级的标志//该优先级上有任务
{
	u8 operate;
	operate=AuxSeekPri[TaskPri];
	*tableflag=(*tableflag)|operate;//或操作，标志标志位
}

void ReadyTableInit(ReadyTable* table)//ready表初始化
{
	int i;
	table->PriFlag=0;//八位标志位为零
	for(i=0;i<7;i++)
	{
		table->table[i].Head=table->table[i].Tail=NULL;
	}
}
/*
tableNode  ready表中某个节点
taskNode   任务节点
tableflag  ready表八位标志
TaskPri  	 任务优先级
*/
void TaskInsertReadyTable(H_LIST* tableNode,H_LIST* taskNode,u8* tableflag,u8 TaskPri)//任务插入ready表
{
	H_PCB* mark;
	ListInsert(tableNode,taskNode);
	mark=GetPcbAdr(taskNode);
	SetReadyPriFlag(tableflag,TaskPri);
}

u8 GetTaskReadyHighPri(u8 PriFlag)//从ready表中查找最高优先级
{
	u8 index=0;//返回readytable下标
	//ready表没有就绪任务
	if(0==(PriFlag&0xFF))return 8;//后续再改，应该不会使用
	while(index<8&&(!(AuxSeekPri[index]&PriFlag)))//从高优先级向下寻找
	{
		index++;
	}
	return index;
}
/*
tableRoot  根节点
taskpri    任务优先级
readypri   ready的八个标志位
返回节点
*/
H_LIST* ReadyTabDelEle(H_LIST* tableRoot,u8 taskpri,u8* readypri)//从ready删除,只在头节点处删除
{
	//
	H_LIST* curtask;
	curtask=tableRoot->Tail;
	if(tableRoot==curtask->Tail)//只有这一个任务
	{
		tableRoot->Head=tableRoot->Tail=NULL;
		*readypri=(*readypri)&(~AuxSeekPri[taskpri]);//清除标志位
	}else
	{
		//只删除元素
		tableRoot->Tail=curtask->Tail;
		curtask->Tail->Head=tableRoot;
	}
	return curtask;
	
}


void HIOS_TaskStart(H_PCB* pcb)//main函数调用
{
	NextStackRegAdr=&pcb->StackReg;
	GlobalPcb=pcb;
	Start_HIOS();
}
H_PCB* HIOS_StartInit()//main函数调用
{
	H_PCB* RootPcb;
	GlobalPcb=(H_PCB*)NULL;
	ReadyTableInit(&GlobalTable);
	DelayTabInit();
	RootPcb=HIOS_TaskCreat((FUNC)HIOS_RootTask,NULL,RootStack,STACKSIZE,5,NULL);//用户最高优先级
	return RootPcb;
}


/*--------------------------增加delay状态----------------------------------*/
H_PCB* GetPcbAdr(H_LIST* Node)//得到PCB指针
{
	H_PCB* pcb;
	PointLink* link;
	link=(PointLink*)Node;
	pcb=link->PstrPcb;//得到PCB指针
	return pcb;
	
}
void  DelayTabInit()
{
	GlobalDealyTable.Head=GlobalDealyTable.Tail=NULL;
}

/////////////////可能有问题
///////////////////////
void DelayTabInsert(H_LIST* Node)//插入delay节点,结束时间按从小到大的顺序排列
{
	//不循环的双链表表示
	H_LIST* tempCompare;
	H_PCB*  task;
	H_PCB*  already;//已经排好序的
	u8 Linkflag=1;
	if(NULL==GlobalDealyTable.Tail)//还未插入
	{
		GlobalDealyTable.Tail=Node;
		Node->Head=&GlobalDealyTable;
		Node->Tail=NULL;
	}else
	{
		task=GetPcbAdr(Node);
		tempCompare=GlobalDealyTable.Tail;
		//already=GetPcbAdr(tempCompare);
		already=((PointLink*)tempCompare)->PstrPcb;
		while((task->StillTime)>(already->StillTime))
		{
			if(NULL==tempCompare->Tail)
			{
				tempCompare->Tail=Node;
				Node->Tail=NULL;
				Node->Head=tempCompare;
				Linkflag=0;//设置一个标志位
				break;
			}else
			{
				tempCompare=tempCompare->Tail;
				already=GetPcbAdr(tempCompare);
			}
		}
		if(Linkflag)//要插入的节点时间小于或者等于
		{
			Node->Tail=tempCompare;
			Node->Head=tempCompare->Head;
			tempCompare->Head->Tail=Node;
			tempCompare->Head=Node;
		}
		
	}
}

///////////////////////////////////////////



H_LIST* DelayTabDel(H_LIST* RootDelayTab)//删除delay节点
{
	//删除的肯定是第一个节点了
	H_LIST* DelNode;
	DelNode=RootDelayTab->Tail;
	if(NULL!=DelNode->Tail)//还有节点
	{
		DelNode->Tail->Head=RootDelayTab;
	}
	RootDelayTab->Tail=DelNode->Tail;

	return RootDelayTab->Tail;
}

void DelayTabSche()//delay表的调度,从delay到ready
{
	H_LIST* delayNode;
	H_LIST* NextNode;
	H_PCB*  pcb;
	u32 goalTicks;
	
	H_LIST* tableNode;
	H_LIST* taskNode;
	u8* tableflag;
	u8 TaskPri;
	delayNode=GlobalDealyTable.Tail;
	if(NULL!=delayNode)
	{
		while(1)
		{
			pcb=GetPcbAdr(delayNode);
			goalTicks=pcb->StillTime;
			if(goalTicks==GlobalTicks)
			{
				//从delay表删除
				NextNode=DelayTabDel(&GlobalDealyTable);//获得下一个节点
				
				//将任务插入ready表
				TaskPri=pcb->TaskPrio;
				tableNode=&GlobalTable.table[TaskPri];
				taskNode=&pcb->Point.PcbNode;
				tableflag=&GlobalTable.PriFlag;
	
				TaskInsertReadyTable(tableNode,taskNode,tableflag,TaskPri);
				
				if(NULL==NextNode)break;
				else
				{
					delayNode=NextNode;//继续插入
				}	
			}else
			{
				break;
			}
		}
	}
}
void ReadyToDelay(u32 ticks)//状态切换,参数要延迟的时间
{
	u8 taskpri;
	H_LIST* taskNode;
	u8* readypri;//ready表的八位优先级标志
	H_LIST*  tableRoot;//对应ready表某个优先级的根节点
	if(0!=ticks)//输入参数不要是0
	{
		//当前任务的相关参数
		taskpri=GlobalPcb->TaskPrio;//获得现在任务的优先级
		tableRoot=&GlobalTable.table[taskpri];
		readypri=&GlobalTable.PriFlag;
		
		HIOS_LOCK();
		//从ready删除任务
		taskNode=ReadyTabDelEle(tableRoot,taskpri,readypri);
		
		
		GlobalPcb->taskarg.time=ticks;
		
		GlobalPcb->StillTime=GlobalTicks+ticks;//结束时间
		
		//添加到delay表
		DelayTabInsert(taskNode);
		
		HIOS_UNLOCK();
	}
	TrigPendSv();//只触发ready调度
	//tick中断触发ready和delay调度
}


