#include <stdio.h>
#include <windows.h>
#include <process.h>
#include "parameter.h" 

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
