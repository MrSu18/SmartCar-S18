//��������ͷ����
#define MT9V03X_W               188             	//ͼ���� 	��Χ1-188 //�Է�װ����޸ģ�����LCD��Ļ�Ĵ�С
#define MT9V03X_H               90             	//ͼ��߶�	��Χ1-120

typedef unsigned char       uint8;
typedef unsigned short      uint16;

extern uint8 mt9v03x_image[MT9V03X_H][MT9V03X_W];

void LCDDrawPoint(uint8 row, uint8 column,uint8 R,uint8 G,uint8 B);//����RGB��ɫ
