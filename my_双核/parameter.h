#ifndef __PARAMETER_H__
#define __PARAMETER_H__

/*
�ڲ����ļ��б���ȫ�ֱ��������׳����ظ���������� �� 
*/ 

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
extern short ip[3];//���������
extern short flag[3];//��־�Ĵ���
extern short ir[3];//ָ��Ĵ���
extern short tempData[3][4], tempPtr[3][4];//���ݼĴ���4������ַ�Ĵ���4����������ģ��
extern codeNode codeSeg[512];//ģ���ڴ��
extern short dataSeg[8192];//ģ�����ݶ�
extern HANDLE lock1,lock2;//������� 

#endif
