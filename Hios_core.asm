 AREA  fun,CODE,READONLY
 EXPORT ContextSwitch
 EXPORT PendSvSetPri
 EXPORT TrigPendSv
 EXPORT PendSV_Handler
 EXPORT Start_HIOS
 IMPORT HIOS_TaskSched		;C语言编写,被PendSV_Handler调用
 IMPORT CurStackRegAdr		;当前任务栈
 IMPORT NextStackRegAdr		;下一个任务栈
PendSvSetPri
NVIC_SYSPRI14 EQU 0xE000ED22;系统优先级寄存器
NVIC_PENDSV_PRI EQU 0xFF	;PendSV优先级
	LDR R0,=NVIC_SYSPRI14
	LDR R1,=NVIC_PENDSV_PRI		;向寄存器R1赋值
	STRB R1,[R0]				;将R1中字节数据写入R0为地址的寄存器
	BX LR;返回调用处


TrigPendSv
NVIC_INT_CTRL EQU 0xE000ED04;中断控制寄存器
NVIC_PENDSVSET EQU 0x10000000;PendSV触发值
	LDR R0,=NVIC_INT_CTRL
	LDR R1,=NVIC_PENDSVSET
	STR R1,[R0]
	BX LR

PendSV_Handler		;无参数,进入中断，八个寄存器自动入栈
	PUSH {R14}		;进入中断保存R14
	
	LDR	  R0,=HIOS_TaskSched
	ADR.W R14,{PC}+0x7  ;保存函数调用返回地址
	BX    R0			;执行HIOS_TaskSched
	
	
	;保存当前任务信息
	MOV   R14,R13		;Sp栈指针
	LDR   R0,=CurStackRegAdr
	LDR   R12,[R0]		;
	ADD   R14,#0x4		;SP向上移动一次,指向多开拓出来的R0
	LDMIA R14!,{R0-R3}	;源寄存器->目的寄存器  IA模式先操作栈指针再增加
	STMIA R12!,{R0-R11} ;上一命令已经恢复了寄存器中的R0-R3,寄存器数据保存到栈组中
	LDMIA R14,{R0-R3}	;取出八位寄存器中剩下四个存入R0-R3
	SUB   R14,#0x10		;向下移动四次指向R0
	STMIA R12!,{R0}		;将R0中存放的R12送到栈组中
	STMIA R12!,{R14}	;栈组R13//R12现在指向R13//中存放寄存器R14中内容
	STMIA R12!,{R1-R3}
	POP   {R0}			;SP-R13指向入中断时自动压入EXC_RETURN地址，在八个寄存器栈组R0下面
	STMIA R12,{R0}
	
	;恢复任务现场
	LDR	  R0,=NextStackRegAdr
	LDR   R1,[R0]
	ADD   R1,#0x10
	LDMIA R1!,{R4-R11}	;恢复寄存器R4-R11
	ADD   R1,#0x4		;指向栈组中的R13
	LDMIA R1,{R13}
	ADD   R1,#0x10		;指向栈组中hiExc_Return
	LDMIA R1,{R0}
	BX    R0			;剩下八个寄存器由硬件自动恢复
	
	
	
Start_HIOS
	LDR R0,=NextStackRegAdr
	LDR R13,[R0]
	ADD R13,#0x40		;SP指向XPSR
	POP {R0}
	MSR XPSR,R0
	SUB R13,#0x8		;指向PC
	LDMIA R13,{R0}		;取出PC值
	SUB R13,#0x3C       ;指向R0
	STMDB R13,{R0}		;先减再操作,R0放入栈组中   目的寄存器，源寄存器
	POP {R0-R12}
	ADD R13,#0x4
	POP {LR}
	SUB R13,#0x40
	POP {PC}

ContextSwitch
	;保存当前任务栈信息
	MOV	  R3, R0;将CurStackRegAdr递给R3
	
	STMIA R3!,{R0,R4-R12}		;直接到14会报错
	STMIA R3!,{R13}
	STMIA R3!,{R14}
	MRS   R2,XPSR				;获取XPSR寄存器值
	STMIA R3,{R2}				;存入结构体组中
	
	;恢复栈信息，运行新任务
	LDMIA R1!,{R0,R4-R12}
	LDMIA R1,{R13}				;Sp指向hiR13
	ADD   R1,#8					;指向hiXpsr
	LDMIA R1,{R2}				;获取XPSR值，存入R2中
	MSR   XPSR,R2				;从R2中放入XPSR寄存器
	SUB   R1,#4					;SP指向hiR14
	
	LDMIA R1,{PC}				;程序从hiR14开始运行
	
