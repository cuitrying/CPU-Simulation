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
	HANDLE hThread1,hThread2;//定义句柄 
	unsigned ThreadID1,ThreadID2;//线程的标识号 
	int id1=1,id2=2;//用id代表核心 
	
	dataSeg[0]=100;//按要求初始化内存 
	readCode("dict1.dic",id1);
	readCode("dict2.dic",id2); 
	
	//分别为内存和输出的互斥对象 
	lock1=CreateMutex(NULL,FALSE,NULL);
	lock2=CreateMutex(NULL,FALSE,NULL);
	
	//运行线程 
	hThread1=(HANDLE)_beginthreadex(NULL,0,run,(void*)&id1,0,&ThreadID1);
	hThread2=(HANDLE)_beginthreadex(NULL,0,run,(void*)&id2,0,&ThreadID2);
	
	WaitForSingleObject(hThread1, INFINITE);//等待线程1 
	CloseHandle (hThread1);//线程结束 
 	WaitForSingleObject(hThread2, INFINITE);//等待线程2 
	CloseHandle (hThread2);
	
	outStorage();//输出内存
	return 0;
 }
 //进行操作(main中用循环加停机指令判断
unsigned __stdcall run(void* pArguements)
{
	int *ptr=(int *)pArguements;
	int n=0,tempOpr,id;
	id=*ptr;

	do
	{
		n = ip[id] / 4;//保存相应内存数组的序号
		tempOpr = codeSeg[n].operate;//保存操作码

		//把dowhile放在函数内便于修改代码 
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

		ir[id] = codeSeg[n].decimalIns;//修改指令寄存器
		ip[id] += 4;//修改程序计数器

		outstate(id);//输出状态
	
	}while(tempOpr!=0);

	_endthreadex(0);//结束线程
	return 0;
}
