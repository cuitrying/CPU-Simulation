#include <stdio.h>
#include <windows.h>
#include <process.h>
#include "parameter.h"
#include "readin.h"
#include "operation.h"
#include "outresult.h"

unsigned __stdcall run(void* pArguements);

int main()
{
	HANDLE hThread1,hThread2;//������ 
	unsigned ThreadID1,ThreadID2;//�̵߳ı�ʶ�� 
	int id1=1,id2=2;//��id������� 
	
	dataSeg[0]=100;//��Ҫ���ʼ���ڴ� 
	readCode("dict1.dic",id1);
	readCode("dict2.dic",id2); 
	
	//�ֱ�Ϊ�ڴ������Ļ������ 
	lock1=CreateMutex(NULL,FALSE,NULL);
	lock2=CreateMutex(NULL,FALSE,NULL);
	
	//�����߳� 
	hThread1=(HANDLE)_beginthreadex(NULL,0,run,(void*)&id1,0,&ThreadID1);
	hThread2=(HANDLE)_beginthreadex(NULL,0,run,(void*)&id2,0,&ThreadID2);
	
	WaitForSingleObject(hThread1, INFINITE);//�ȴ��߳�1 
	CloseHandle (hThread1);//�߳̽��� 
 	WaitForSingleObject(hThread2, INFINITE);//�ȴ��߳�2 
	CloseHandle (hThread2);
	
	outStorage();//����ڴ�
	return 0;
 }
 //���в���(main����ѭ����ͣ��ָ���ж�
unsigned __stdcall run(void* pArguements)
{
	int *ptr=(int *)pArguements;
	int n=0,tempOpr,id;
	id=*ptr;

	do
	{
		n = ip[id] / 4;//������Ӧ�ڴ���������
		tempOpr = codeSeg[n].operate;//���������

		//��dowhile���ں����ڱ����޸Ĵ��� 
		switch(tempOpr)
		{
			case 0: break;
			
			case 1: transmitOP(n,id); break;
			case 2: case 3: case 4: case 5: arithmeticOP(n,id);break;
			case 6: case 7: case 8: logicalOP(n,id);break;
			case 9: compareOP(n,id); break;
			case 10: skipOP(n,id); break;
			case 11: inOP(n,id); break;
			case 12: outOP(n,id);break;
			case 13: lockOP(n,id);break;
			case 14: unlockOP(n,id);break;
			case 15: sleepOP(n,id);break;
		
		}

		ir[id] = codeSeg[n].decimalIns;//�޸�ָ��Ĵ���
		ip[id] += 4;//�޸ĳ��������

		outstate(id);//���״̬
	
	}while(tempOpr!=0);

	_endthreadex(0);//�����߳�
	return 0;
}
