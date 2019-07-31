 AREA  fun,CODE,READONLY
 EXPORT ContextSwitch
 EXPORT PendSvSetPri
 EXPORT TrigPendSv
 EXPORT PendSV_Handler
 EXPORT Start_HIOS
 IMPORT HIOS_TaskSched		;C���Ա�д,��PendSV_Handler����
 IMPORT CurStackRegAdr		;��ǰ����ջ
 IMPORT NextStackRegAdr		;��һ������ջ
PendSvSetPri
NVIC_SYSPRI14 EQU 0xE000ED22;ϵͳ���ȼ��Ĵ���
NVIC_PENDSV_PRI EQU 0xFF	;PendSV���ȼ�
	LDR R0,=NVIC_SYSPRI14
	LDR R1,=NVIC_PENDSV_PRI		;��Ĵ���R1��ֵ
	STRB R1,[R0]				;��R1���ֽ�����д��R0Ϊ��ַ�ļĴ���
	BX LR;���ص��ô�


TrigPendSv
NVIC_INT_CTRL EQU 0xE000ED04;�жϿ��ƼĴ���
NVIC_PENDSVSET EQU 0x10000000;PendSV����ֵ
	LDR R0,=NVIC_INT_CTRL
	LDR R1,=NVIC_PENDSVSET
	STR R1,[R0]
	BX LR

PendSV_Handler		;�޲���,�����жϣ��˸��Ĵ����Զ���ջ
	PUSH {R14}		;�����жϱ���R14
	
	LDR	  R0,=HIOS_TaskSched
	ADR.W R14,{PC}+0x7  ;���溯�����÷��ص�ַ
	BX    R0			;ִ��HIOS_TaskSched
	
	
	;���浱ǰ������Ϣ
	MOV   R14,R13		;Spջָ��
	LDR   R0,=CurStackRegAdr
	LDR   R12,[R0]		;
	ADD   R14,#0x4		;SP�����ƶ�һ��,ָ��࿪�س�����R0
	LDMIA R14!,{R0-R3}	;Դ�Ĵ���->Ŀ�ļĴ���  IAģʽ�Ȳ���ջָ��������
	STMIA R12!,{R0-R11} ;��һ�����Ѿ��ָ��˼Ĵ����е�R0-R3,�Ĵ������ݱ��浽ջ����
	LDMIA R14,{R0-R3}	;ȡ����λ�Ĵ�����ʣ���ĸ�����R0-R3
	SUB   R14,#0x10		;�����ƶ��Ĵ�ָ��R0
	STMIA R12!,{R0}		;��R0�д�ŵ�R12�͵�ջ����
	STMIA R12!,{R14}	;ջ��R13//R12����ָ��R13//�д�żĴ���R14������
	STMIA R12!,{R1-R3}
	POP   {R0}			;SP-R13ָ�����ж�ʱ�Զ�ѹ��EXC_RETURN��ַ���ڰ˸��Ĵ���ջ��R0����
	STMIA R12,{R0}
	
	;�ָ������ֳ�
	LDR	  R0,=NextStackRegAdr
	LDR   R1,[R0]
	ADD   R1,#0x10
	LDMIA R1!,{R4-R11}	;�ָ��Ĵ���R4-R11
	ADD   R1,#0x4		;ָ��ջ���е�R13
	LDMIA R1,{R13}
	ADD   R1,#0x10		;ָ��ջ����hiExc_Return
	LDMIA R1,{R0}
	BX    R0			;ʣ�°˸��Ĵ�����Ӳ���Զ��ָ�
	
	
	
Start_HIOS
	LDR R0,=NextStackRegAdr
	LDR R13,[R0]
	ADD R13,#0x40		;SPָ��XPSR
	POP {R0}
	MSR XPSR,R0
	SUB R13,#0x8		;ָ��PC
	LDMIA R13,{R0}		;ȡ��PCֵ
	SUB R13,#0x3C       ;ָ��R0
	STMDB R13,{R0}		;�ȼ��ٲ���,R0����ջ����   Ŀ�ļĴ�����Դ�Ĵ���
	POP {R0-R12}
	ADD R13,#0x4
	POP {LR}
	SUB R13,#0x40
	POP {PC}

ContextSwitch
	;���浱ǰ����ջ��Ϣ
	MOV	  R3, R0;��CurStackRegAdr�ݸ�R3
	
	STMIA R3!,{R0,R4-R12}		;ֱ�ӵ�14�ᱨ��
	STMIA R3!,{R13}
	STMIA R3!,{R14}
	MRS   R2,XPSR				;��ȡXPSR�Ĵ���ֵ
	STMIA R3,{R2}				;����ṹ������
	
	;�ָ�ջ��Ϣ������������
	LDMIA R1!,{R0,R4-R12}
	LDMIA R1,{R13}				;Spָ��hiR13
	ADD   R1,#8					;ָ��hiXpsr
	LDMIA R1,{R2}				;��ȡXPSRֵ������R2��
	MSR   XPSR,R2				;��R2�з���XPSR�Ĵ���
	SUB   R1,#4					;SPָ��hiR14
	
	LDMIA R1,{PC}				;�����hiR14��ʼ����
	
