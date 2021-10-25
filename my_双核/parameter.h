#ifndef __PARAMETER_H__
#define __PARAMETER_H__

/*
在参数文件中保存全局变量（容易出现重复定义的问题 ） 
*/ 

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
extern short ip[3];//程序计数器
extern short flag[3];//标志寄存器
extern short ir[3];//指令寄存器
extern short tempData[3][4], tempPtr[3][4];//数据寄存器4个，地址寄存器4个，用数组模拟
extern codeNode codeSeg[512];//模拟内存段
extern short dataSeg[8192];//模拟数据段
extern HANDLE lock1,lock2;//互斥对象 

#endif
