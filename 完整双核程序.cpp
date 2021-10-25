#include <stdio.h>
#include <windows.h>
#include <process.h>

//两个核心的地址 
#define core1 0
#define core2 256

//定义内存类型
typedef struct NODE1
{   //直接初始化有限制 
	char binaryIns[34];//32位二进制指令
	int decimalIns;//十进制指令
	short operate;//操作码
	short objects[2];//两个操作对象
	short immediateNum;//立即数
}codeNode;

//全部初始化为零
short ip[3] = {0};//程序计数器
short flag[3] = {0};//标志寄存器
short ir[3] = {0};//指令寄存器
short tempData[3][4] = { 0 }, tempPtr[3][4] = { 0 };//数据寄存器4个，地址寄存器4个，用数组模拟
codeNode codeSeg[512];//模拟内存段
short dataSeg[8192];//模拟数据段
HANDLE lock1,lock2;//互斥对象 

void readCode(const char *fileName,int id);//读取指令 
void processCode(int n);//处理指令 
void addOP(int n,int id);//加法 
void subtractOP(int n,int id);//减法 
void multiplyOP(int n,int id);//乘法 
void divideOP(int n,int id);//除法 
void arithmeticOP(int n,int id);//算数运算 
void andOP(int n,int id);//且运算 
void orOP(int n,int id);//或运算 
void notOP(int n,int id);//非运算 
void logicalOP(int n,int id);//逻辑运算 
void transmitOP(int n,int id);//数据传输 
void compareOP(int n,int id);//比较运算 
void skipOP(int n,int id);//跳转 
void inOP(int n,int id);//输入 
void outOP(int n,int id);//输出 
void outstate(int id);//输出寄存器 
int operation(void);//执行指令 
void outStorage(void);//输出存储区 
void lockOP(int n,int id);//请求互斥对象，加锁 
void unlockOP(int n,int id);//释放互斥对象，解锁
void sleepOP(int n,int id);//休眠 
unsigned __stdcall run(void* pArguments);//运行程序

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

//输出寄存器状态
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
	for (i = 0; i < 4; i++)//寄存器1-4
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
	for (i = 0; i < 4; i++)//寄存器5-8
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

//输出内存
void outStorage(void)
{
	int i, j;
	unsigned long decimal = 0;
	printf("\ncodeSegment :\n");//输出内存段 
	for (i = 0; i < 128; i++)
	{
		decimal = 0;
		if (codeSeg[i].immediateNum >= 0)//十进制值等于前16位的值和立即数对应二进制值的组合 
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
	printf("\ndataSegment :\n");//输出数据段 
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
