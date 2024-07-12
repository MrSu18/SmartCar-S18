# SmartCar-S18
>   -   第十八届智能车-摄像头三轮组，15cm摄像头限高
>
>   -   图像：灰度搜线+边线逆透视+单边巡线
>   -   控制：转向换->左右轮目标速度
>   -   [【广东工业大学-急先锋】征战华南赛！！！_哔哩哔哩_bilibili](https://www.bilibili.com/video/BV1Vh4y17778/?spm_id_from=333.788)

**PS: 如果对你有用请点个star支持下**

# 1. 逆透视说明

1.   使用OPENCV中的’getPerspectiveTransform’函数来得到透视矩阵，可以使用萝狮虎开源的去得到矩阵，此时得到了代码中debug.c中用于观看图像的**double change_un_Mat**透视矩阵。

     [智能车图像处理逆透视教程_智能车摄像头逆透视识别赛道元素-CSDN博客](https://blog.csdn.net/wu58430/article/details/126317900)

2.   由于边线透视（透视后的坐标=原图坐标×change_inverse_Mat）和萝狮虎博客中介绍的全图透视（原图坐标=透视后的坐标×change_un_Mat），是向前映射和向后映射的区别，所以，在边线透视中进行坐标变换的矩阵需要对步骤1中得到的矩阵进行求逆，即**代码中的double change_inverse_Mat=inv(change_un_Mat)**，由此得到了代码中的change_inverse_Mat。[图像变换——向前映射和向后映射_前向映射-CSDN博客](https://blog.csdn.net/glorydream2015/article/details/44873703)

     PS：在我的工程中因为TC264的算力强大，所以我每一次边线透视都是进行了一次坐标变换的计算，但是此处由于每个原图坐标变换之后的坐标都是固定的，所以为了节省算力此处可以使用**打表**，可以参考我[这个源文件中的](https://github.com/MrSu18/SmartCar-S18/blob/master/software/Seekfree_TC264_Opensource_Library/code/ImageTrack.c)void perspective_init(void)函数得到

     ```c
     uint8 perspectiv_x[120][188];
     uint8 perspectiv_y[120][188];
     ```
     
     这两个表，从而进行透视的时候只需要查表即可，在使用的时候可以先用电脑编写程序让这两个表格的结果存在TXT中再复制进去你工程里面，否则你直接定义有可能会造成MCU的ram不够，定义固定的值为const或者code比较好。
     
     ```c
      out_line[count].X = perspectiv_x[in_line[count].Y][in_line[count].X];
      out_line[count].Y = perspectiv_y[in_line[count].Y][in_line[count].X];
     ```

# 2. 元素识别处理的总框架

-   延续17届元素处理的框架，发车之后先什么元素后什么元素由按键或者代码来进行元素顺序的确定。

-  由于17届的时候元素较多，十字回环和环岛的图像较为相似，且是第一年做智能车，赛道特征提取做的不是很好，以至于图像写起来较为艰难，在进行环岛元素学习的过程中了解到了状态机这种方法，所以想到了元素的顺序也可以在发车前定好，**因此仅需要专注于区分元素与无元素之间的区别而不需要在意元素之间的误判，大大减少了元素识别处理的工作量**，并且对于一些没有特征点的角度使用强行补线也不会对车的轨迹造成太大影响。
