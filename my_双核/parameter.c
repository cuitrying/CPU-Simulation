#include <stdio.h>
#include <windows.h>
#include <process.h>

//�����ڴ�����
typedef struct NODE1
{   //ֱ�ӳ�ʼ�������� 
	char binaryIns[34];//32λ������ָ��
	int decimalIns;//ʮ����ָ��
	short operate;//������
	short objects[2];//������������
	short immediateNum;//������
}codeNode;

short ip[3] = {0};//���������
short flag[3] = {0};//��־�Ĵ���
short ir[3] = {0};//ָ��Ĵ���
short tempData[3][4] = { 0 }, tempPtr[3][4] = { 0 };//���ݼĴ���4������ַ�Ĵ���4����������ģ��
codeNode codeSeg[512];//ģ���ڴ��
short dataSeg[8192];//ģ�����ݶ�
HANDLE lock1,lock2;//������� 
