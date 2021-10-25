#include <stdio.h>
#include <windows.h>
#include <process.h>
#include "parameter.h"
#include "operation.h"


//数据传输 
void transmitOP(int n,int id)
{
	short fromNum, toNum;
	short from = codeSeg[n].objects[1], to = codeSeg[n].objects[0];
	if (from == 0)//来自立即数：到两种寄存器 
	{
		fromNum = codeSeg[n].immediateNum;
		if (to < 5)//数据寄存器
		{
			tempData[id][to - 1] = fromNum;
		}
		else//地址寄存器 
		{
			tempPtr[id][to - 5] = fromNum;
		}
	}
	else if (from >= 1 && from <= 4)//从数据寄存器到数据段 
	{
		toNum = (tempPtr[id][to - 5] - 16384) / 2;
		dataSeg[toNum] = tempData[id][from - 1];
	}
	else if (from >= 5 && from <= 8)//从数据段到数据寄存器 
	{
		fromNum = (tempPtr[id][from - 5] - 16384) / 2;
		tempData[id][to - 1] = dataSeg[fromNum];
	}
	return;
}

//算数运算
void arithmeticOP(int n,int id)
{
	int opr = codeSeg[n].operate;
	if (opr == 2)//加法
	{
		addOP(n,id);
	}
	else if (opr == 3)//减法
	{
		subtractOP(n,id);
	}
	else if (opr == 4)//乘法
	{
		multiplyOP(n,id);
	}
	else if (opr == 5)//除法
	{
		divideOP(n,id);
	}
	return;
}

//逻辑运算
void logicalOP(int n,int id)
{
	int opr = codeSeg[n].operate;
	if (opr == 6)//逻辑且 
	{
		andOP(n,id);
	}
	else if (opr == 7)//逻辑或 
	{
		orOP(n,id);
	}
	else if (opr == 8)//逻辑非 
	{
		notOP(n,id);
	}
	return;
}

//加法
void addOP(int n,int id)//加法
{
	int from = codeSeg[n].objects[1], to = codeSeg[n].objects[0];
	if (from == 0)
	{
		tempData[id][to - 1] += codeSeg[n].immediateNum;
	}
	else if (from >= 5 && from <= 8)
	{
		tempData[id][to - 1] += dataSeg[(tempPtr[id][from - 5] - 16384) / 2];
	}
	return;
}

//减法
void subtractOP(int n,int id)//减法
{
	int from = codeSeg[n].objects[1], to = codeSeg[n].objects[0];
	if (from == 0)
	{
		tempData[id][to - 1] -= codeSeg[n].immediateNum;
	}
	else if (from >= 5 && from <= 8)
	{
		tempData[id][to - 1] -= dataSeg[(tempPtr[id][from - 5] - 16384) / 2];
	}
	return;
}

//乘法
void multiplyOP(int n,int id)//乘法
{
	int from = codeSeg[n].objects[1], to = codeSeg[n].objects[0];
	if (from == 0)
	{
		tempData[id][to - 1] *= codeSeg[n].immediateNum;
	}
	else if (from >= 5 && from <= 8)
	{
		tempData[id][to - 1] *= dataSeg[(tempPtr[id][from - 5] - 16384) / 2];
	}
	return;
}

//除法
void divideOP(int n,int id)//除法
{
	int from = codeSeg[n].objects[1], to = codeSeg[n].objects[0];
	if (from == 0)
	{
		tempData[id][to - 1] /= codeSeg[n].immediateNum;
	}
	else if (from >= 5 && from <= 8)
	{
		tempData[id][to - 1] /= dataSeg[(tempPtr[id][from - 5] - 16384) / 2];
	}
	return;
}

//比较运算
void compareOP(int n,int id)
{
	int fromNum, toNum, from = codeSeg[n].objects[1], to = codeSeg[n].objects[0];
	if (from == 0)
	{
		fromNum = codeSeg[n].immediateNum;
	}
	else if (from >= 5 && from <= 8)
	{
		fromNum = dataSeg[(tempPtr[id][from - 5] - 16384) / 2];
	}
	toNum = tempData[id][to - 1];

	if (toNum == fromNum)
	{
		flag[id] = 0;
	}
	else if (toNum > fromNum)
	{
		flag[id] = 1;
	}
	else
	{
		flag[id] = -1;
	}
	return;
}

//跳转操作
void skipOP(int n,int id)//跳转操作
{
	//条件成立则ip不再加4，操作后应再减去4 
	int condition = codeSeg[n].objects[0] * 16 + codeSeg[n].objects[1];
	if (condition == 0)
	{
		ip[id] += codeSeg[n].immediateNum - 4;
	}
	else if (condition == 1 && flag[id] == 0)
	{
		ip[id] += codeSeg[n].immediateNum - 4;
	}
	else if (condition == 2 && flag[id] == 1)
	{
		ip[id] += codeSeg[n].immediateNum - 4;
	}
	else if (condition == 3 && flag[id] == -1)
	{
		ip[id] += codeSeg[n].immediateNum - 4;
	}
	return;
}

//且运算
void andOP(int n,int id)
{
	int from = codeSeg[n].objects[1], to = codeSeg[n].objects[0];
	if (from == 0)
	{
		tempData[id][to - 1] = codeSeg[n].immediateNum && tempData[id][to - 1] ? 1 : 0;
	}
	else if (from >= 5 && from <= 8)
	{
		tempData[id][to - 1] = tempData[id][to - 1] && dataSeg[(tempPtr[id][from - 5] - 16384) / 2] ? 1 : 0;
	}

	return;
}

//或运算
void orOP(int n,int id)
{
	int from = codeSeg[n].objects[1], to = codeSeg[n].objects[0];
	if (from == 0)
	{
		tempData[id][to - 1] = codeSeg[n].immediateNum || tempData[id][to - 1] ? 1 : 0;
	}
	else if (from >= 5 && from <= 8)
	{
		tempData[id][to - 1] = tempData[id][to - 1] || dataSeg[(tempPtr[id][from - 5] - 16384) / 2] ? 1 : 0;
	}
	return;
}
//非运算
void notOP(int n,int id)
{
	int from = codeSeg[n].objects[1], to = codeSeg[n].objects[0];
	if (from != 0)
	{
		if (from >= 1 && from <= 4)
		{
			tempData[id][from - 1] = !tempData[id][from - 1] ? 1 : 0;
		}
		else
		{
			dataSeg[(tempPtr[id][from - 5] - 16384) / 2] = !dataSeg[(tempPtr[id][from - 5] - 16384) / 2] ? 1 : 0;
		}
	}
	else if (to != 0)
	{
		if (to >= 1 && to <= 4)
		{
			tempData[id][to - 1] = !tempData[id][to - 1] ? 1 : 0;
		}
		else
		{
			dataSeg[(tempPtr[id][to - 5] - 16384) / 2] = !dataSeg[(tempPtr[id][to - 5] - 16384) / 2] ? 1 : 0;
		}
	}
	return;
}
//输入操作
void inOP(int n,int id)
{
	short num;
	int from = codeSeg[n].objects[0];
	printf("in:\n");
	scanf("%hd", &num);
	if (from >= 1 && from <= 4)
	{
		tempData[id][from - 1] = num;
	}
	else
	{
		dataSeg[(tempPtr[id][from - 5] - 16384) / 2] = num;
	}
	return;
}

//输出操作
void outOP(int n,int id)
{
	int from = codeSeg[n].objects[0];
	//printf("from:%d\n",from);
	printf("id = %d    ",id);
	printf("out: ");
	if (from >= 1 && from <= 4)
	{
		printf("%hd\n", tempData[id][from - 1]);
	}
	else
	{
		printf("%hd\n", dataSeg[(tempPtr[id][from - 5] - 16384) / 2]);
	}
	return;
}

//新增多线程指令 ，等于直接调用库函数 
void lockOP(int n,int id)//加锁 
{
	WaitForSingleObject(lock1,INFINITE);
	return; 
}

void unlockOP(int n,int id)//解锁 
{
	ReleaseMutex(lock1);
	return ; 
}

void sleepOP(int n,int id)//休眠 
{
	short time=codeSeg[n].immediateNum;
	Sleep(time);
	return ;
}

