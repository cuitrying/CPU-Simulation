#include <stdio.h>
#include <windows.h>
#include <process.h>
#include "parameter.h"

void processCode(int);

//读入指令
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

	//此处读取到eof应注意循环类型
	while (!feof(fPtr))
	{
		fgets(codeSeg[num].binaryIns, 34, fPtr);//34= sizeof(codeSeg[num].binaryIns)
		codeSeg[num].binaryIns[32] = '\0';//写入字符串结尾 
		num++;
	}
	for(;i<num-1;i++)//在读取指令之后就完成指令的分析，防止循环处理时修改 
	{
		processCode(i);
	}
	fclose(fPtr);
	return;
}

//处理指令
void processCode(int n)
{
	int i, sum;
	//获取操作码
	for (i = 0, sum = 0; i < 8; i++)
	{
		sum *= 2;
		sum += (codeSeg[n].binaryIns[i] - '0');
	}
	codeSeg[n].operate = sum;
	//获取操作对象
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
	//获取立即数:转换补码
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
	
	return;
}
