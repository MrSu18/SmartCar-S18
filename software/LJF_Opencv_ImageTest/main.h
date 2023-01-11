//配置摄像头参数
#define MT9V03X_W               188             	//图像宽度 	范围1-188 //对封装库的修改：适配LCD屏幕的大小
#define MT9V03X_H               90             	//图像高度	范围1-120

typedef unsigned char       uint8;
typedef unsigned short      uint16;

extern uint8 mt9v03x_image[MT9V03X_H][MT9V03X_W];

void LCDDrawPoint(uint8 row, uint8 column,uint8 R,uint8 G,uint8 B);//画点RGB调色
