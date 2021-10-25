#include <stdio.h>
#include <windows.h>
#include <process.h>
#include "parameter.h"
#include "operation.h"


//���ݴ��� 
void transmitOP(int n,int id)
{
	short fromNum, toNum;
	short from = codeSeg[n].objects[1], to = codeSeg[n].objects[0];
	if (from == 0)//�����������������ּĴ��� 
	{
		fromNum = codeSeg[n].immediateNum;
		if (to < 5)//���ݼĴ���
		{
			tempData[id][to - 1] = fromNum;
		}
		else//��ַ�Ĵ��� 
		{
			tempPtr[id][to - 5] = fromNum;
		}
	}
	else if (from >= 1 && from <= 4)//�����ݼĴ��������ݶ� 
	{
		toNum = (tempPtr[id][to - 5] - 16384) / 2;
		dataSeg[toNum] = tempData[id][from - 1];
	}
	else if (from >= 5 && from <= 8)//�����ݶε����ݼĴ��� 
	{
		fromNum = (tempPtr[id][from - 5] - 16384) / 2;
		tempData[id][to - 1] = dataSeg[fromNum];
	}
	return;
}

//��������
void arithmeticOP(int n,int id)
{
	int opr = codeSeg[n].operate;
	if (opr == 2)//�ӷ�
	{
		addOP(n,id);
	}
	else if (opr == 3)//����
	{
		subtractOP(n,id);
	}
	else if (opr == 4)//�˷�
	{
		multiplyOP(n,id);
	}
	else if (opr == 5)//����
	{
		divideOP(n,id);
	}
	return;
}

//�߼�����
void logicalOP(int n,int id)
{
	int opr = codeSeg[n].operate;
	if (opr == 6)//�߼��� 
	{
		andOP(n,id);
	}
	else if (opr == 7)//�߼��� 
	{
		orOP(n,id);
	}
	else if (opr == 8)//�߼��� 
	{
		notOP(n,id);
	}
	return;
}

//�ӷ�
void addOP(int n,int id)//�ӷ�
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

//����
void subtractOP(int n,int id)//����
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

//�˷�
void multiplyOP(int n,int id)//�˷�
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

//����
void divideOP(int n,int id)//����
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

//�Ƚ�����
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

//��ת����
void skipOP(int n,int id)//��ת����
{
	//����������ip���ټ�4��������Ӧ�ټ�ȥ4 
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

//������
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

//������
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
//������
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
//�������
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

//�������
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

//�������߳�ָ�� ������ֱ�ӵ��ÿ⺯�� 
void lockOP(int n,int id)//���� 
{
	WaitForSingleObject(lock1,INFINITE);
	return; 
}

void unlockOP(int n,int id)//���� 
{
	ReleaseMutex(lock1);
	return ; 
}

void sleepOP(int n,int id)//���� 
{
	short time=codeSeg[n].immediateNum;
	Sleep(time);
	return ;
}

