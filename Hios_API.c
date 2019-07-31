#include <Hios_core.h>
#include <Hios_API.h>
#include <Hios.h>
#include <stdio.h>
void HIOS_LOCK(void)//��������sys�ж�
{
	//CTRL��λ���ƿ���  1 ��  0��
//	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
	if(0==LockCount)
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
	else
	{
		LockCount++;
	}
}
void HIOS_UNLOCK(void)//��������sys�ж�
{
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
	if(1==LockCount)
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk; 
	else
	{
		LockCount--;
	}
}

//��ʼ���Ĵ�����
void HIOS_StackInit(FUNC FuncAdr,void* arg,H_PCB* pcb)//������ַ�ͼĴ�����ַ
{
	STACKREG *reg;
	u32* AutoStack;
	reg=&pcb->StackReg;
	
	//��ʼ���Ĵ�����
	reg->hiR0=(u32)arg;//�˴���ŵ���һ����ַ,Ϊʲô��һ����ַ������
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
	reg->hiR13=(u32)pcb-32;//ջַ,��ԭ�������������������˸�Ԥ���ռ�
	reg->hiR14=0;
	reg->hiR15=(u32)FuncAdr;
	reg->hiXpsr=THUMB2;
	reg->hiExc_Return=EXC_RETURN_MODE;
	
	/*��ʼ������ʱջ,�����λ��Ӳ���Զ�����*/
	//�е�ַԽ��Σ��
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
	H_LIST* TaskNode;//�ֿ�д,��Ȼ����һ����ַ����
	u8* tableflag;
	if((NULL==FuncAdr)||(NULL==TaskStack)||(0==StackSize)||TaskPri>7)
	{
		return (H_PCB*)NULL;
	}
	StackByte4=(u8*)(((u32)TaskStack+StackSize)&ALIGN4);//
	
	pcb=(H_PCB*)(((u32)StackByte4-sizeof(H_PCB))&ALIGN8);//���ֽڶ���
	
	HIOS_StackInit(FuncAdr,arg,pcb);//��ʼ���Ĵ�����
	
	if(NULL==taskStat)//Ĭ��ready̬
	{
		pcb->taskarg.TaskSta=TASKREADY;
	}else
	{
		pcb->taskarg.TaskSta=taskStat->TaskSta;
		pcb->taskarg.time=taskStat->time;
	}

	pcb->Point.PstrPcb=pcb;//ָ���Լ��Ļ�ַ
	pcb->TaskPrio=TaskPri;//�������ȼ�
	
	
	TaskNode=&pcb->Point.PcbNode;
	
	/*����ready��ԭ�Ӳ���*/
	HIOS_LOCK();
	
	if(TASKREADY==(pcb->taskarg.TaskSta))//����ready��
	{
		ElemReady=&GlobalTable.table[TaskPri];//��ȡready��ĳ��Ԫ��
		tableflag=&GlobalTable.PriFlag;
		
		
		TaskInsertReadyTable(ElemReady,TaskNode,tableflag,TaskPri);
	}
	if(TASKDELAY==(pcb->taskarg.TaskSta))//����delay��
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
	
	GlobalPcb=pcb;//�����������е�������Ϣ
	
	//�л�����
	//ContextSwitch(CurStackRegAdr,NextStackRegAdr);
}
H_LIST* GetReadyTabHighTask(H_LIST* Rootlink)
{
	//������ڵ�
	//������һ�����أ�Ӧ�ý��˽ڵ�ɾ��//////////////////////////////////////
	return Rootlink->Tail;
}


H_PCB* HIOS_ReadyTabSched()//��������PCB��ַ
{
	H_PCB *pcb;
	H_LIST* Node;
//	PointLink* ReadyHighNode;//�Ѿ�������߸��ڵ�
	u8 HighPriIndex;//������ȼ��±�
	HighPriIndex=GetTaskReadyHighPri(GlobalTable.PriFlag);
//	ReadyHighNode=(PointLink*)GetReadyTabHighTask();
//	
//	=ReadyHighNode->PstrPcb;
	Node=GlobalTable.table[HighPriIndex].Tail;
	pcb=GetPcbAdr(Node);
	return pcb;
	
}
void HIOS_TaskSched()//��PendSv�ж������
{
	
	H_PCB *pcb;
	//��һ��delay�����
	//����һ��ȫ�ֱ�־
	if(TICKSCHEDSET==HardTrigPendSv)//������Ӳ������
	{
		HardTrigPendSv=TICKSCHEDCLR;
		DelayTabSche();//delay�����
	}
	
	//readyTable����
	pcb=HIOS_ReadyTabSched();
	
	//�����л�
	HIOS_TaskSwitch(pcb);
}
void ListInsert(H_LIST* tableNode,H_LIST* taskNode)//ͷ�巨
{

	if((NULL==tableNode->Tail)&&(NULL==tableNode->Head))
	{
		//�״β���
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
void SetReadyPriFlag(u8* tableflag,u8 TaskPri)//����ready�����ȼ��ı�־//�����ȼ���������
{
	u8 operate;
	operate=AuxSeekPri[TaskPri];
	*tableflag=(*tableflag)|operate;//���������־��־λ
}

void ReadyTableInit(ReadyTable* table)//ready���ʼ��
{
	int i;
	table->PriFlag=0;//��λ��־λΪ��
	for(i=0;i<7;i++)
	{
		table->table[i].Head=table->table[i].Tail=NULL;
	}
}
/*
tableNode  ready����ĳ���ڵ�
taskNode   ����ڵ�
tableflag  ready���λ��־
TaskPri  	 �������ȼ�
*/
void TaskInsertReadyTable(H_LIST* tableNode,H_LIST* taskNode,u8* tableflag,u8 TaskPri)//�������ready��
{
	H_PCB* mark;
	ListInsert(tableNode,taskNode);
	mark=GetPcbAdr(taskNode);
	SetReadyPriFlag(tableflag,TaskPri);
}

u8 GetTaskReadyHighPri(u8 PriFlag)//��ready���в���������ȼ�
{
	u8 index=0;//����readytable�±�
	//ready��û�о�������
	if(0==(PriFlag&0xFF))return 8;//�����ٸģ�Ӧ�ò���ʹ��
	while(index<8&&(!(AuxSeekPri[index]&PriFlag)))//�Ӹ����ȼ�����Ѱ��
	{
		index++;
	}
	return index;
}
/*
tableRoot  ���ڵ�
taskpri    �������ȼ�
readypri   ready�İ˸���־λ
���ؽڵ�
*/
H_LIST* ReadyTabDelEle(H_LIST* tableRoot,u8 taskpri,u8* readypri)//��readyɾ��,ֻ��ͷ�ڵ㴦ɾ��
{
	//
	H_LIST* curtask;
	curtask=tableRoot->Tail;
	if(tableRoot==curtask->Tail)//ֻ����һ������
	{
		tableRoot->Head=tableRoot->Tail=NULL;
		*readypri=(*readypri)&(~AuxSeekPri[taskpri]);//�����־λ
	}else
	{
		//ֻɾ��Ԫ��
		tableRoot->Tail=curtask->Tail;
		curtask->Tail->Head=tableRoot;
	}
	return curtask;
	
}


void HIOS_TaskStart(H_PCB* pcb)//main��������
{
	NextStackRegAdr=&pcb->StackReg;
	GlobalPcb=pcb;
	Start_HIOS();
}
H_PCB* HIOS_StartInit()//main��������
{
	H_PCB* RootPcb;
	GlobalPcb=(H_PCB*)NULL;
	ReadyTableInit(&GlobalTable);
	DelayTabInit();
	RootPcb=HIOS_TaskCreat((FUNC)HIOS_RootTask,NULL,RootStack,STACKSIZE,5,NULL);//�û�������ȼ�
	return RootPcb;
}


/*--------------------------����delay״̬----------------------------------*/
H_PCB* GetPcbAdr(H_LIST* Node)//�õ�PCBָ��
{
	H_PCB* pcb;
	PointLink* link;
	link=(PointLink*)Node;
	pcb=link->PstrPcb;//�õ�PCBָ��
	return pcb;
	
}
void  DelayTabInit()
{
	GlobalDealyTable.Head=GlobalDealyTable.Tail=NULL;
}

/////////////////����������
///////////////////////
void DelayTabInsert(H_LIST* Node)//����delay�ڵ�,����ʱ�䰴��С�����˳������
{
	//��ѭ����˫�����ʾ
	H_LIST* tempCompare;
	H_PCB*  task;
	H_PCB*  already;//�Ѿ��ź����
	u8 Linkflag=1;
	if(NULL==GlobalDealyTable.Tail)//��δ����
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
				Linkflag=0;//����һ����־λ
				break;
			}else
			{
				tempCompare=tempCompare->Tail;
				already=GetPcbAdr(tempCompare);
			}
		}
		if(Linkflag)//Ҫ����Ľڵ�ʱ��С�ڻ��ߵ���
		{
			Node->Tail=tempCompare;
			Node->Head=tempCompare->Head;
			tempCompare->Head->Tail=Node;
			tempCompare->Head=Node;
		}
		
	}
}

///////////////////////////////////////////



H_LIST* DelayTabDel(H_LIST* RootDelayTab)//ɾ��delay�ڵ�
{
	//ɾ���Ŀ϶��ǵ�һ���ڵ���
	H_LIST* DelNode;
	DelNode=RootDelayTab->Tail;
	if(NULL!=DelNode->Tail)//���нڵ�
	{
		DelNode->Tail->Head=RootDelayTab;
	}
	RootDelayTab->Tail=DelNode->Tail;

	return RootDelayTab->Tail;
}

void DelayTabSche()//delay��ĵ���,��delay��ready
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
				//��delay��ɾ��
				NextNode=DelayTabDel(&GlobalDealyTable);//�����һ���ڵ�
				
				//���������ready��
				TaskPri=pcb->TaskPrio;
				tableNode=&GlobalTable.table[TaskPri];
				taskNode=&pcb->Point.PcbNode;
				tableflag=&GlobalTable.PriFlag;
	
				TaskInsertReadyTable(tableNode,taskNode,tableflag,TaskPri);
				
				if(NULL==NextNode)break;
				else
				{
					delayNode=NextNode;//��������
				}	
			}else
			{
				break;
			}
		}
	}
}
void ReadyToDelay(u32 ticks)//״̬�л�,����Ҫ�ӳٵ�ʱ��
{
	u8 taskpri;
	H_LIST* taskNode;
	u8* readypri;//ready��İ�λ���ȼ���־
	H_LIST*  tableRoot;//��Ӧready��ĳ�����ȼ��ĸ��ڵ�
	if(0!=ticks)//���������Ҫ��0
	{
		//��ǰ�������ز���
		taskpri=GlobalPcb->TaskPrio;//���������������ȼ�
		tableRoot=&GlobalTable.table[taskpri];
		readypri=&GlobalTable.PriFlag;
		
		HIOS_LOCK();
		//��readyɾ������
		taskNode=ReadyTabDelEle(tableRoot,taskpri,readypri);
		
		
		GlobalPcb->taskarg.time=ticks;
		
		GlobalPcb->StillTime=GlobalTicks+ticks;//����ʱ��
		
		//��ӵ�delay��
		DelayTabInsert(taskNode);
		
		HIOS_UNLOCK();
	}
	TrigPendSv();//ֻ����ready����
	//tick�жϴ���ready��delay����
}


