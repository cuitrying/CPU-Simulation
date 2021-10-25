#include <stdio.h>
#include <windows.h>
#include <process.h>

//定义内存类型
typedef struct NODE1
{   //直接初始化有限制 
	char binaryIns[34];//32位二进制指令
	int decimalIns;//十进制指令
	short operate;//操作码
	short objects[2];//两个操作对象
	short immediateNum;//立即数
}codeNode;

short ip[3] = {0};//程序计数器
short flag[3] = {0};//标志寄存器
short ir[3] = {0};//指令寄存器
short tempData[3][4] = { 0 }, tempPtr[3][4] = { 0 };//数据寄存器4个，地址寄存器4个，用数组模拟
codeNode codeSeg[512];//模拟内存段
short dataSeg[8192];//模拟数据段
HANDLE lock1,lock2;//互斥对象 
