#include <stdio.h>
#include <windows.h>
#include <process.h>

//�������ĵĵ�ַ 
#define core1 0
#define core2 256

//�����ڴ�����
typedef struct NODE1
{   //ֱ�ӳ�ʼ�������� 
	char binaryIns[34];//32λ������ָ��
	int decimalIns;//ʮ����ָ��
	short operate;//������
	short objects[2];//������������
	short immediateNum;//������
}codeNode;

//ȫ����ʼ��Ϊ��
short ip[3] = {0};//���������
short flag[3] = {0};//��־�Ĵ���
short ir[3] = {0};//ָ��Ĵ���
short tempData[3][4] = { 0 }, tempPtr[3][4] = { 0 };//���ݼĴ���4������ַ�Ĵ���4����������ģ��
codeNode codeSeg[512];//ģ���ڴ��
short dataSeg[8192];//ģ�����ݶ�
HANDLE lock1,lock2;//������� 

void readCode(const char *fileName,int id);//��ȡָ�� 
void processCode(int n);//����ָ�� 
void addOP(int n,int id);//�ӷ� 
void subtractOP(int n,int id);//���� 
void multiplyOP(int n,int id);//�˷� 
void divideOP(int n,int id);//���� 
void arithmeticOP(int n,int id);//�������� 
void andOP(int n,int id);//������ 
void orOP(int n,int id);//������ 
void notOP(int n,int id);//������ 
void logicalOP(int n,int id);//�߼����� 
void transmitOP(int n,int id);//���ݴ��� 
void compareOP(int n,int id);//�Ƚ����� 
void skipOP(int n,int id);//��ת 
void inOP(int n,int id);//���� 
void outOP(int n,int id);//��� 
void outstate(int id);//����Ĵ��� 
int operation(void);//ִ��ָ�� 
void outStorage(void);//����洢�� 
void lockOP(int n,int id);//���󻥳���󣬼��� 
void unlockOP(int n,int id);//�ͷŻ�����󣬽���
void sleepOP(int n,int id);//���� 
unsigned __stdcall run(void* pArguments);//���г���

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

//����ָ��
void readCode(const char * fileName,int id)
{
	int num,i;
	FILE* fPtr;
	fPtr = fopen(fileName, "r");
	
	if(id == 1) 
	{
		num=core1;
		i=num;
	}
	if(id == 2)
	{
		num=core2/4;
		i=num;
	}

	//�˴���ȡ��eofӦע��ѭ������
	while (!feof(fPtr))
	{
		fgets(codeSeg[num].binaryIns, 34, fPtr);//34= sizeof(codeSeg[num].binaryIns)
		codeSeg[num].binaryIns[32] = '\0';//д���ַ�����β 
		num++;
	}
	for(;i<num-1;i++)//�ڶ�ȡָ��֮������ָ��ķ�������ֹѭ������ʱ�޸� 
	{
		processCode(i);
	}
	fclose(fPtr);
	return;
}

//����ָ��
void processCode(int n)
{
	int i, sum;
	//��ȡ������
	for (i = 0, sum = 0; i < 8; i++)
	{
		sum *= 2;
		sum += (codeSeg[n].binaryIns[i] - '0');
	}
	codeSeg[n].operate = sum;
	//��ȡ��������
	for (sum = 0; i < 12; i++)
	{
		sum *= 2;
		sum += (codeSeg[n].binaryIns[i] - '0');
	}
	codeSeg[n].objects[0] = sum;
	for (sum = 0; i < 16; i++)
	{
		sum *= 2;
		sum += (codeSeg[n].binaryIns[i] - '0');
	}
	codeSeg[n].objects[1] = sum;
	sum = 0;
	//��ȡ������:ת������
	if (codeSeg[n].binaryIns[16] == '0')
	{
		for (; i < 32; i++)
		{
			sum *= 2;
			sum += (codeSeg[n].binaryIns[i] - '0');
		}
	}
	else
	{
		for (i = 17; i < 32; i++)
		{
			sum *= 2;
			if (codeSeg[n].binaryIns[i] == '0')
			{
				sum++;
			}
		}
		sum++;
		sum *= -1;
	}

	codeSeg[n].immediateNum = sum;
	codeSeg[n].decimalIns = codeSeg[n].operate * 256 + codeSeg[n].objects[0] * 16 + codeSeg[n].objects[1];
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

//����Ĵ���״̬
void outstate(int id)
{
	int i,core;
	
	if(id == 1) core=core1;
	if(id == 2) core=core2;
	
	WaitForSingleObject(lock2,INFINITE);
	
	printf("\n");
	printf("id = %d\n",id);
	printf("ip = %hd\n", ip[id]+core);
	printf("flag = %hd\n", flag[id]);
	printf("ir = %hd\n", ir[id]);
	for (i = 0; i < 4; i++)//�Ĵ���1-4
	{
		printf("ax%d = %hd", i + 1, tempData[id][i]);
		if (i != 3)
		{
			printf(" ");
		}
		else
		{
			printf("\n");
		}
	}
	for (i = 0; i < 4; i++)//�Ĵ���5-8
	{
		printf("ax%d = %hd", i + 5, tempPtr[id][i]);
		if (i != 3)
		{
			printf(" ");
		}
		else
		{
			printf("\n");
		}
	}
	ReleaseMutex(lock2); 
	return;
}

//����ڴ�
void outStorage(void)
{
	int i, j;
	unsigned long decimal = 0;
	printf("\ncodeSegment :\n");//����ڴ�� 
	for (i = 0; i < 128; i++)
	{
		decimal = 0;
		if (codeSeg[i].immediateNum >= 0)//ʮ����ֵ����ǰ16λ��ֵ����������Ӧ������ֵ����� 
		{
			decimal = codeSeg[i].decimalIns * 65536 + codeSeg[i].immediateNum;
		}
		else
		{
			for (j = 16; j < 32; j++)
			{
				decimal *= 2;
				decimal += codeSeg[i].binaryIns[j] - '0';
			}
			decimal += codeSeg[i].decimalIns * 65536;
		}
		printf("%lu", decimal);
		if ((i + 1) % 8 == 0)
		{
			printf("\n");
		}
		else
		{
			printf(" ");
		}
	}
	printf("\ndataSegment :\n");//������ݶ� 
	for (i = 0; i < 256; i++)
	{
		printf("%hd", dataSeg[i]);
		if ((i + 1) % 16 == 0)
		{
			printf("\n");
		}
		else
		{
			printf(" ");
		}
	}
	return;
}

void lockOP(int n,int id)
{
	WaitForSingleObject(lock1,INFINITE);
	return; 
}

void unlockOP(int n,int id)
{
	ReleaseMutex(lock1);	
	return ; 
}

void sleepOP(int n,int id)
{
	short time=codeSeg[n].immediateNum;
	Sleep(time);
	return ;
}
