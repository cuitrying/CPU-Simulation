#ifndef __OPERATION_H__
#define __OPERATION_H__

//基本操作 
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

//多线程加入的操作 
void lockOP(int n,int id);//请求互斥对象，加锁 
void unlockOP(int n,int id);//释放互斥对象，解锁
void sleepOP(int n,int id);//休眠 

#endif
