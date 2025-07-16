//-------------------------------------------------------------------------------------------------------------------
//  简介:八邻域图像处理
//400
//------------------------------------------------------------------------------------------------------------------
//3.30斜率记录左圆环右边界
//k1       k2       k4       k6
//2.3-3.5  2.2-4.0  2.3-4.3  2.1-4.5
//3.30斜率记录
//k1       k2       k4       k6
//2.4-4.0  2.2-4.5  2.3-5    1.9-3.9
#include "image.h"
#include "bsptimer.h"
#include "menu.h"

__attribute__((aligned(4))) uint8 bin_image[MT9V03X_H][MT9V03X_W];
/*变量声明*/
uint8 original_image[image_h][image_w];
uint8 image_thereshold;         //图像分割阈值
uint8 map_x,map_y;
int16 max_encoder = 1700;

int16 right_bor[image_h];
int16 left_bor[image_h];
int16 center_line[image_h];//中线数组
uint8 flag_r_cir=0;//全局标志位
uint8 flag_l_cir=0;//全局标志位
uint8 flag_cross=0;//全局标志位
uint8 flag_others=0;//全局标志位
uint8 left_right_flag=0;//判断是不是弯道，0直线，1左弯，2右弯
uint8 start_point_r[2] = {0}; //右边起点的x，y值
uint8 start_point_l[2] = {0}; //左边起点的x，y值

uint16 num = 500;
 //存放点的x，y坐标
uint16 points_l[800][2] = { { 0 } };//左线
uint16 points_r[800][2] = { { 0 } };//右线
uint16 dir_r[800] = { 0 };//用来存储右边生长方向
uint16 dir_l[800] = { 0 };//用来存储左边生长方向
uint16 data_stastics_l = 0;//统计左边找到点的个数
uint16 data_stastics_r = 0;//统计右边找到点的个数
uint8 hightest = 0;//最高点
uint8_t width_begin[MT9V03X_H - 1]=
{
        106, 105, 105, 104, 103, 103, 102, 101, 101, 100,
               99,  98,  98,  97,  96,  95,  95,  94,  93,  92,
               92,  91,  90,  89,  89,  88,  87,  86,  86,  85,
               84,  83,  83,  82,  81,  80,  80,  79,  78,  77,
               77,  76,  75,  74,  74,  73,  72,  71,  71,  70,
               69,  68,  68,  67,  66,  65,  65,  64,  63,  62,
               62,  61,  60,  59,  59,  58,  57,  56,  56,  55,
               54,  53,  53,  52,  51,   50,  50,  49,  48,  47,
               47,  46,  45,  44,  44,   43,  42,  41,  41,  40 ,
               39,  38 , 38 , 37 , 36,   35,  35,  34,  33 , 32,
               32,  31, 30,   29 , 29,   28,  27,  26 , 26, 25,
               24,23,23,22,21,20,19,19,18,17,16,16,15,14,13,13,12
};
uint8 width_zzh[MT9V03X_H-1] = {0      }; //前90行的赛道宽度

int16 Black_jiexian=0;//检测该图像上处是否存在全黑情况，并且得到全黑界限的纵坐标


/*
函数名称：void image_process(void)
功能说明：最终处理函数
参数说明：无
函数返回：无
修改时间：2024年2月27日
备    注：
example： image_process();
 */
void image_process(void)
{
    uint8 hightest = 0;//定义一个最高行，tip：这里的最高指的是y值的最小
    //大津化二值化处理
        turn_to_bin();
        /*提取赛道边界*/
        image_filter(bin_image);//滤波
        image_draw_rectan(bin_image);//-预处理
        width_change();
        //stop_line();
        //清零
        data_stastics_l = 0;
        data_stastics_r = 0;
        if (get_start_point(image_h - 2))//找到起点了，再执行八领域，没找到就一直找
        {
            //printf("正在开始八领域\n");
            search_l_r((uint16)USE_num, bin_image, &data_stastics_l, &data_stastics_r, start_point_l[0], start_point_l[1], start_point_r[0], start_point_r[1], &hightest);
            //printf("八邻域已结束\n");
        //  // 从爬取的边界线内提取边线 ， 这个才是最终有用的边线
            R_border(data_stastics_r);
            L_border(data_stastics_l);
        //  //处理函数放这里，不要放到if外面去了，不要放到if外面去了，不要放到if外面去了，重要的事说三遍
            stop_line();
            cross_delay_judge();
            Element_test();
            straight_judge();
            straight_judge_2();
            judge_bar();
            Element_handle();
            tft180_displayimage03x((const uint8 *)bin_image, dis_image_w, dis_image_h);   //show bin_image
            for(int m = 0;m<image_h;m++)
            {
                if(left_bor[m]<=image_w&&left_bor[m]>=0)
                {tft180_draw_point(Ctrl_x(left_bor[m])*dis_image_w/image_w ,Ctrl_y(m)*dis_image_h/image_h, RGB565_GREEN);}
                if(right_bor[m]<=image_w&&right_bor[m]>=0)
                {tft180_draw_point(Ctrl_x(right_bor[m])*dis_image_w/image_w,Ctrl_y(m)*dis_image_h/image_h, RGB565_RED);}
                if(center_line[m]<=image_w&&center_line[m]>=0)
                {tft180_draw_point(Ctrl_x(center_line[m])*dis_image_w/image_w,Ctrl_y(m)*dis_image_h/image_h, RGB565_BLUE);}//显示起点 显示中线
            }
           // tft180_displayimage03x((const uint8 *)bin_image, dis_image_w, dis_image_h);   //show bin_image
        }

             center_valid();
             int i =0;
             for(i = 0; i<9;i++)
             {
                 r_dir_sum[i] = 0;       //生长方向数目数组归零
                 l_dir_sum[i] = 0;
             }
}
//int count_line = 0;
//void stop_line(void)
//{
//    int i = 0;
//    for(i = 0; i < 126; i++)
//    {
//        if(bin_image[110][i] != 0 && bin_image[110][i + 1] == 0)
//        {
//            count_line ++;
//        }
//    }
//    if(count_line >=6)
//    {
//
//        bsppwm_control(0,0,0,0);
//        bsppwm_wushua_control(-1,-1);  //x从0~100   70  70
//        draw_cir1(bin_image);
//        count_line = 0;
//    }
//}
uint8_t cir_x = 0,cir_y = 0;
float cir_k1 = 0.0,cir_k2 = 0.0;
void cir_point(int flag)
{
    int i =0;

    //flag 1 left else 2 right
    if(flag == 1)
    {
        for(i = data_stastics_l - 1;i > 1;i--)
        {
            if(dir_l[i-1] == 6 && points_l[i][1] > 10)
            {
                cir_x = points_l[i-1][0];
                cir_y = points_l[i-1][1];
                break;
            }
        }
    }
    else
    {
        for(i = data_stastics_r - 1;i > 1;i--)
        {
//            cir_k1 = cir_k(points_r[i+4][0], points_r[i + 4][1], points_r[i][0], points_r[i][1]);
//            cir_k2 = cir_k(points_r[i-4][0], points_r[i - 4][1], points_r[i][0], points_r[i][1]);
//            if(my_abs2(cir_k1- cir_k2) > 0.1)
//            {
//                cir_x = points_r[i][0];
//                cir_y = points_r[i][1];
//            }
            if(dir_r[i-1] == 6 && points_r[i][1] > 10)
            {
                cir_x = points_r[i-1][0];
                cir_y = points_r[i-1][1];
                break;
            }
        }
    }
}
float cir_k(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2)
{
   // float k = my_abs2(1.0 *(x1 - x2)/(1.0*(y1-y2)));
    float k1 = (y1 - y2);
    return k1;

}
void width_change(void)
{
    for(uint8_t i = 0;i < 128; i++)
    {
        width_zzh[i] = width_begin[i] - 10;
    }
}
bool man_what_can_i_say(uint8_t i)
{

    return 1;
}
/*
函数名称：int my_abs(int value)
功能说明：求绝对值
参数说明：
函数返回：绝对值
修改时间：2022年9月8日
备    注：
example：  my_abs( x)；
 */
int my_abs(int value)
{
if(value>=0) return value;
else return -value;
}
float my_abs2(float value)
{
    if(value>=0) return value;
    else return -value;
}
int16_t my_abs3(int16_t value)
{
    if(value>=0) return value;
    else return -value;
}
uint8 limit_a_b(uint8 point,uint8 min,uint8 max)
{
    if(point<=min)
        point = min;
    else if(point>max)
        point = max-1;
    return point;
}
//-----------------------------------------------------------------------------------------------------------------
//@brief    坐标显示位置限制
//@since    v1.0
//-----------------------------------------------------------------------------------------------------------------
uint8 Ctrl_x(uint8 data)
{
    if(data>159)
        return 158;
    else if(data<2)
    return 2;
    else return data;
}

uint8 Ctrl_y(uint8 data)
{
    if(data>119)
        return 119;
    else if(data<2)
    return 2;
    else return data;
}

//------------------------------------------------------------------------------------------------------------------
//  @brief     动态阈值
//  @since      v1.0
//------------------------------------------------------------------------------------------------------------------
uint8 otsuThreshold(uint8 *image, uint16 col, uint16 row)
{
#define GrayScale 256
    uint16 Image_Width  = col;
    uint16 Image_Height = row;
    int X; uint16 Y;
    uint8* data = image;
    int HistGram[GrayScale] = {0};

    uint32 Amount = 0;
    uint32 PixelBack = 0;
    uint32 PixelIntegralBack = 0;
    uint32 PixelIntegral = 0;
    int32 PixelIntegralFore = 0;
    int32 PixelFore = 0;
    double OmegaBack=0, OmegaFore=0, MicroBack=0, MicroFore=0, SigmaB=0, Sigma=0; // 类间方差;
    uint8 MinValue=0, MaxValue=0;
    uint8 Threshold = 0;


    for (Y = 0; Y <Image_Height; Y++) //Y<Image_Height改为Y =Image_Height；以便进行 行二值化
    {
        //Y=Image_Height;
        for (X = 0; X < Image_Width; X++)
        {
        HistGram[(int)data[Y*Image_Width + X]]++; //统计每个灰度值的个数信息
        }
    }

    for (MinValue = 0; MinValue < 256 && HistGram[MinValue] == 0; MinValue++) ;        //获取最小灰度的值
    for (MaxValue = 255; MaxValue > MinValue && HistGram[MinValue] == 0; MaxValue--) ; //获取最大灰度的值

    if (MaxValue == MinValue)
    {
        return MaxValue;          // 图像中只有一个颜色
    }
    if (MinValue + 1 == MaxValue)
    {
        return MinValue;      // 图像中只有二个颜色
    }

    for (Y = MinValue; Y <= MaxValue; Y++)
    {
        Amount += HistGram[Y];        //  像素总数
    }

    PixelIntegral = 0;
    for (Y = MinValue; Y <= MaxValue; Y++)
    {
        PixelIntegral += HistGram[Y] * Y;//灰度值总数
    }
    SigmaB = -1;
    for (Y = MinValue; Y < MaxValue; Y++)
    {
          PixelBack = PixelBack + HistGram[Y];    //前景像素点数
          PixelFore = Amount - PixelBack;         //背景像素点数
          OmegaBack = (double)PixelBack / Amount;//前景像素百分比
          OmegaFore = (double)PixelFore / Amount;//背景像素百分比
          PixelIntegralBack += HistGram[Y] * Y;  //前景灰度值
          PixelIntegralFore = PixelIntegral - PixelIntegralBack;//背景灰度值
          MicroBack = (double)PixelIntegralBack / PixelBack;//前景灰度百分比
          MicroFore = (double)PixelIntegralFore / PixelFore;//背景灰度百分比
          Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);//g
          if (Sigma > SigmaB)//遍历最大的类间方差g
          {
              SigmaB = Sigma;
              Threshold = (uint8)Y;
          }
    }
   return Threshold;
}

//------------------------------------------------------------------------------------
//@brief    大津法二值化
//@since    v1.0
//------------------------------------------------------------------------------------
void turn_to_bin(void)
{
    uint16 i, j;
    image_thereshold = otsuThreshold(mt9v03x_image[0],image_w,image_h);
    for(i = 0; i <image_h; i++)
    {
        for(j  = 0; j < image_w; j++)
        {
            if(mt9v03x_image[i][j]>image_thereshold)
                bin_image[i][j] = 255;
            else bin_image[i][j] = 0;
        }
    }
}
/*
函数名称：void draw_cross(uint8 (*screen)[SCREEN_WIDTH])
功能说明：给图像中心画一个绿蓝
参数说明：uint8(*image)[image_w] 图像首地址
函数返回：无
修改时间：2024年2月25日
备    注：
example： draw_cross(bin_image);
 */
void draw_cir1(uint8 (*screen)[SCREEN_WIDTH])
{
    uint8 i = image_w/2;
    uint8 j = image_h/2;
    int8 count = 15;
    while(count >-16)
    {
        tft180_draw_point((i)*dis_image_w/image_w-15, (j+count)*dis_image_h/image_h, RGB565_GREEN);
        tft180_draw_point((i+count)*dis_image_w/image_w, (j)*dis_image_h/image_h-15, RGB565_GREEN);
        tft180_draw_point((i+1)*dis_image_w/image_w+15, (j+count)*dis_image_h/image_h, RGB565_BLUE);
        tft180_draw_point((i+count)*dis_image_w/image_w, (j+1)*dis_image_h/image_h+15, RGB565_BLUE);
        count--;
    }
}
void draw_error(uint8 (*screen)[SCREEN_WIDTH])
{
    uint8 i = image_w/2;
    uint8 j = image_h/2;
    int8 count = 7;
    while(count >-7)
    {
        tft180_draw_point((i+count)*dis_image_w/image_w+7, (j-count)*dis_image_h/image_h, RGB565_RED);
        tft180_draw_point((i+count)*dis_image_w/image_w, (j)*dis_image_h/image_h-7-count, RGB565_RED);
        tft180_draw_point((i+1+count)*dis_image_w/image_w+7, (j+count)*dis_image_h/image_h, RGB565_RED);
        tft180_draw_point((i+count)*dis_image_w/image_w, (j+1+count)*dis_image_h/image_h+7, RGB565_RED);
        count--;
    }
}
void draw_cir2(uint8 (*screen)[SCREEN_WIDTH])
{
    uint8 i = image_w/2;
    uint8 j = image_h/2;
    int8 count = 7;
    while(count >-7)
    {
        tft180_draw_point((i+count)*dis_image_w/image_w+7, (j-count)*dis_image_h/image_h, RGB565_GREEN);
        tft180_draw_point((i+count)*dis_image_w/image_w, (j)*dis_image_h/image_h-7-count, RGB565_RED);
        tft180_draw_point((i+1+count)*dis_image_w/image_w+7, (j+count)*dis_image_h/image_h, RGB565_RED);
        tft180_draw_point((i+count)*dis_image_w/image_w, (j+1+count)*dis_image_h/image_h+7, RGB565_BLUE);
        count--;
    }
}
float k1=0.0,k2=0.0,k3=0.0,k4= 0.0,k5 = 0.0, k6 = 0.0;
uint8_t sum_dir_r(void)
{
    k1=0.0,k2=0.0,k3=0.0,k4= 0.0,k5 = 0.0, k6 = 0.0;
    k1 = (120.0 - 60.0)/(float)(right_bor[120] - right_bor[60]);
    k2 = (120.0 - 10.0)/(float)(right_bor[120] - right_bor[10]);
    k3 = (60.0 - 5.0)/(float)(right_bor[5] - right_bor[60]);
    k4 = (120.0 - 90.0)/(float)(right_bor[120] - right_bor[90]);
    k5 = (40 - 5.0)/(float)(right_bor[5] - right_bor[40]);
    k6 = (80.0 - 25.0)/(float)(right_bor[80] - right_bor[25]);
    k1 = control_kk(k1);
    k4 = control_kk(k4);
    k6 = control_kk(k6);
    k2 = control_kk(k2);
    //k1       k2       k4       k6
    //2.3-3.5  2.2-4.0  2.3-4.3  2.1-4.5

    //float edge = 2.2;
    if(k1>=2.3  && k1 <= 5.0 &&k4 >= 2.3 && k6 > 1.8 && k6 <=5.0&& k2>=2.2 && k2<= 5.0){
        draw_error(bin_image);
        return 1;
    }
    else {
        //draw_cross(bin_image);
        return 0;
    }
    //k2>edge && k3>edge&&&& k5 > edge

}
float control_kk(float k)
{
    if(k >5.0 )
        k = 5.0;
    else if(k < 0.1)
        k = 0.0;
    return k;
}
uint8_t sum_dir_l(void)  // 计算左边线斜率 模拟未丢线
{
        k1=0.0,k2=0.0,k3=0.0,k4= 0.0,k5 = 0.0, k6 = 0.0;
        k1 = (120.0 - 60.0)/(float)(left_bor[60] - left_bor[120]);
        k2 = (120.0 - 10.0)/(float)(left_bor[10] - left_bor[120]);
        k3 = (60.0 - 5.0)/(float)(left_bor[5] - left_bor[60]);
        k4 = (120.0 - 90.0)/(float)(left_bor[90] - left_bor[120]);
        k5 = (40 - 5.0)/(float)(left_bor[5] - left_bor[40]);
        k6 = (80.0 - 25.0)/(float)(left_bor[25] - left_bor[80]);
        k1 = control_kk(k1);
        k4 = control_kk(k4);
        k6 = control_kk(k6);
        k2 = control_kk(k2);
        //3.30斜率记录
        //k1       k2       k4       k6
        //2.4-4.0  2.2-4.5  2.3-5    1.9-3.9
       // float edge = 2.2;
        if(k1>=2.2 &&k2>=2.2 && k2 <=5.0 &&  k4 > 2.2&&k4 <= 5.0 && k6 > 1.9 && k6 <= 5.0){
            //draw_cross(bin_image);
            return 1;
        }
        else {
            //draw_cross(bin_image);
            return 0;
        }
        //k2>edge && k3>edge&&&& k5 > edge
}

/*
函数名称：void draw_cross(uint8 (*screen)[SCREEN_WIDTH])
功能说明：给图像中心画一个十字+
参数说明：uint8(*image)[image_w] 图像首地址
函数返回：无
修改时间：2024年2月25日
备    注：
example： draw_cross(bin_image);
 */
void draw_cross(uint8 (*screen)[SCREEN_WIDTH])
{
    uint8 i = image_w/2;
    uint8 j = image_h/2;
    int8 count = 15;
    while(count >-16)
    {
        tft180_draw_point((i)*dis_image_w/image_w, (j+count)*dis_image_h/image_h, RGB565_RED);
        tft180_draw_point((i+count)*dis_image_w/image_w, (j)*dis_image_h/image_h, RGB565_RED);
        tft180_draw_point((i+1)*dis_image_w/image_w, (j+count)*dis_image_h/image_h, RGB565_RED);
        tft180_draw_point((i+count)*dis_image_w/image_w, (j+1)*dis_image_h/image_h, RGB565_RED);
        count--;
    }


}
uint8_t l_l_judge(void)        //十字丢线判断
 {
        uint8_t sum1 =0;
        uint8_t i = 0;
        for(i = 0; i < image_h ; i++)
        {
            if(left_bor[i] <= 4&&right_bor[i] >= 156)
                sum1 ++;
        }
        if(sum1 > 30)
        return 1;
        if(sum1 > 10)
            return 2;
        return 0;
 }
/*
函数名称：void get_start_point(uint8 start_row)
功能说明：寻找两个边界的边界点作为八邻域循环的起始点
参数说明：输入任意行数
函数返回：无
修改时间：2022年9月8日
备    注：
example：  get_start_point(image_h-2)
 */

uint8 get_start_point(uint8 star_row)
{
    uint8 i = 0,r_found = 0,l_found = 0;

    start_point_l[0] = 0;
    start_point_l[1] = 0;

    start_point_r[0] = 0;
    start_point_r[1] = 0;

    for(i = image_w / 2; i > border_min;i--)
    {
        start_point_l[0] = i;
        start_point_l[1] = star_row;
        if(bin_image[star_row][i] == 255&&bin_image[star_row][i-1] == 0)
        {
            l_found = 1;
            break;
        }
    }

    for(i = image_w/2; i < border_max; i++)
    {
        start_point_r[0] = i;
        start_point_r[1] = star_row;
        if(bin_image[star_row][i] == 255&&bin_image[star_row][i+1] == 0)
        {
            r_found = 1;
            break;
        }
    }
    if(r_found&&l_found)
        return 1;
    else return 0;
}

/*
函数名称：void search_l_r(uint16 break_flag, uint8(*image)[image_w],uint16 *l_stastic, uint16 *r_stastic,
                            uint8 l_start_x, uint8 l_start_y, uint8 r_start_x, uint8 r_start_y,uint8*hightest)

功能说明：八邻域正式开始找右边点的函数，输入参数有点多，调用的时候不要漏了，这个是左右线一次性找完。
参数说明：
break_flag_r            ：最多需要循环的次数
(*image)[image_w]       ：需要进行找点的图像数组，必须是二值图,填入数组名称即可
                       特别注意，不要拿宏定义名字作为输入参数，否则数据可能无法传递过来
*l_stastic              ：统计左边数据，用来输入初始数组成员的序号和取出循环次数
*r_stastic              ：统计右边数据，用来输入初始数组成员的序号和取出循环次数
l_start_x               ：左边起点横坐标
l_start_y               ：左边起点纵坐标
r_start_x               ：右边起点横坐标
r_start_y               ：右边起点纵坐标
hightest                ：循环结束所得到的最高高度
函数返回：无
修改时间：2022年9月25日
备    注：
example：
    search_l_r((uint16)USE_num,image,&data_stastics_l, &data_stastics_r,start_point_l[0],
                start_point_l[1], start_point_r[0], start_point_r[1],&hightest);
 */
uint8_t l_dir_sum[9]={0};
uint8_t r_dir_sum[9]={0};
void search_l_r(uint16 break_flag, uint8(*image)[image_w], uint16 *l_stastic, uint16 *r_stastic, uint8 l_start_x, uint8 l_start_y, uint8 r_start_x, uint8 r_start_y, uint8*hightest)
{

    uint8 i = 0, j = 0;


    //左边变量
    uint8 search_filds_l[8][2] = { {  0 } };
    uint8 index_l = 0;
    uint8 temp_l[8][2] = { {  0 } };
    uint8 center_point_l[2] = {  0 };
    uint16 l_data_statics;//统计左边
    //定义八个邻域
    static int8 seeds_l[8][2] = { {0,  1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1}};
    //{-1,-1},{0,-1},{+1,-1},
    //{-1, 0},       {+1, 0},
    //{-1,+1},{0,+1},{+1,+1},
    //这个是顺时针

    //右边变量
    uint8 search_filds_r[8][2] = { {  0 } };
    uint8 center_point_r[2] = { 0 };//中心坐标点
    uint8 index_r = 0;//索引下标
    uint8 temp_r[8][2] = { {  0 } };
    uint16 r_data_statics;//统计右边
    //定义八个邻域
    static int8 seeds_r[8][2] = { {0,  1},{1,1},{1,0}, {1,-1},{0,-1},{-1,-1}, {-1,  0},{-1, 1}};
    //{-1,-1},{0,-1},{+1,-1},
    //{-1, 0},       {+1, 0},
    //{-1,+1},{0,+1},{+1,+1},
    //这个是逆时针

    l_data_statics = *l_stastic;//统计找到了多少个点，方便后续把点全部画出来
    r_data_statics = *r_stastic;//统计找到了多少个点，方便后续把点全部画出来

    //第一次更新坐标点  将找到的起点值传进来
    center_point_l[0] = l_start_x;//x
    center_point_l[1] = l_start_y;//y

    center_point_r[0] = r_start_x;//x
    center_point_r[1] = r_start_y;//y

        //开启邻域循环

    while(break_flag--)
    {
        //left
        for(i = 0;i<8;i++)
        {
            search_filds_l[i][0] = center_point_l[0] + seeds_l[i][0];
            search_filds_l[i][1] = center_point_l[1] + seeds_l[i][1];
        }
        points_l[l_data_statics][0] = center_point_l[0];
        points_l[l_data_statics][1] = center_point_l[1];
        bin_image[points_l[l_data_statics][1]][points_l[l_data_statics][0]] = 31;
        l_data_statics++;

        //右边
        for (i = 0; i < 8; i++)//传递8F坐标
        {
            search_filds_r[i][0] = center_point_r[0] + seeds_r[i][0];//x
            search_filds_r[i][1] = center_point_r[1] + seeds_r[i][1];//y
        }
        //中心坐标点填充到已经找到的点内
        points_r[r_data_statics][0] = center_point_r[0];//x
        points_r[r_data_statics][1] = center_point_r[1];//y
        bin_image[points_r[r_data_statics][1]][points_r[r_data_statics][0]]=50;


        index_l = 0;
        for(i = 0;i<8; i ++)
        {
            temp_l[i][0] = 0;
            temp_l[i][1] = 0;
        }

        //left check!!
        for(i = 0; i < 8; i ++)
        {
            if(bin_image[search_filds_l[i][1]][search_filds_l[i][0]] == 0&&
                    bin_image[search_filds_l[(i+1)&7][1]][search_filds_l[(i+1)&7][0]] == 255 //search_filds_l[i][1] > 1
                   )
                {
                    temp_l[index_l][0] = search_filds_l[i][0];
                    temp_l[index_l][1] = search_filds_l[i][1];
                    index_l ++;
                    if(center_point_l[1] < 5)
                        dir_l[l_data_statics-1] = -1;
                    else {
                        dir_l[l_data_statics - 1]= (i);/////////////////////
                        l_dir_sum[i]++;
                    }

                }
            if(index_l)
            {
                //更新坐标点；
                center_point_l[0] = temp_l[0][0];//x
                center_point_l[1] = temp_l[0][1];//y
                for(j = 0;j<index_l;j++)
                {
                    if(center_point_l[0] > temp_l[j][0])
                    {
                        center_point_l[0] = temp_l[j][0];
                        center_point_l[1] = temp_l[j][1];

                    }
                }
            }
        }

        if ((points_r[r_data_statics][0]== points_r[r_data_statics-1][0]&& points_r[r_data_statics][0] == points_r[r_data_statics - 2][0]
                    && points_r[r_data_statics][1] == points_r[r_data_statics - 1][1] && points_r[r_data_statics][1] == points_r[r_data_statics - 2][1])
                    ||(points_l[l_data_statics-1][0] == points_l[l_data_statics - 2][0] && points_l[l_data_statics-1][0] == points_l[l_data_statics - 3][0]
                        && points_l[l_data_statics-1][1] == points_l[l_data_statics - 2][1] && points_l[l_data_statics-1][1] == points_l[l_data_statics - 3][1]))
                {
                    //printf("三次进入同一个点，退出\n");
                    break;
                }
                if (my_abs(points_r[r_data_statics][0] - points_l[l_data_statics - 1][0]) < 2
                    && my_abs(points_r[r_data_statics][1] - points_l[l_data_statics - 1][1]) < 2
                    )
                {
                    //printf("\n左右相遇退出\n");
                    *hightest = (points_r[r_data_statics][1] + points_l[l_data_statics - 1][1]) >> 1;//取出最高点
                    //printf("\n在y=%d处退出\n",*hightest);
                    break;
                }
                if ((points_r[r_data_statics][1] < points_l[l_data_statics - 1][1]))///
                {
                    //printf("\n如果左边比右边高了，左边等待右边\n");
                    continue;//如果左边比右边高了，左边等待右边
                }
                if (dir_l[l_data_statics - 1] == 7
                    && (points_r[r_data_statics][1] > points_l[l_data_statics - 1][1]))//左边比右边高且已经向下生长了
                {
                    //printf("\n左边开始向下了，等待右边，等待中... \n");
                    center_point_l[0] = points_l[l_data_statics - 1][0];//x
                    center_point_l[1] = points_l[l_data_statics - 1][1];//y
                    l_data_statics--;
                }


                r_data_statics++;

                index_r = 0;
                for(i = 0;i < 8; i++)
                {
                    temp_r[i][0] = 0;
                    temp_r[i][1] = 0;
                }
                //右边判断
                for (i = 0; i < 8; i++)
                {
                    if (bin_image[search_filds_r[i][1]][search_filds_r[i][0]] == 0
                        && bin_image[search_filds_r[(i + 1) & 7][1]][search_filds_r[(i + 1) & 7][0]] == 255//&&search_filds_r[i][1] > 1
                        )
                    {
                        temp_r[index_r][0] = search_filds_r[(i)][0];
                        temp_r[index_r][1] = search_filds_r[(i)][1];
                        index_r++;//索引加一
                        if(center_point_r[1] < 5)//////////////////////////
                            dir_r[r_data_statics - 1] = -1;//记录生长方向
                        else {
                            dir_r[r_data_statics - 1] = (i);//记录生长方向
                        r_dir_sum[i]++;
                        }
                        //printf("dir[%d]:%d\n", r_data_statics - 1, dir_r[r_data_statics - 1]);
                    }
                    if (index_r)
                    {

                        //更新坐标点
                        center_point_r[0] = temp_r[0][0];//x
                        center_point_r[1] = temp_r[0][1];//y
                        for (j = 0; j < index_r; j++)
                        {
                            if (center_point_r[1] > temp_r[j][1])
                            {
                                center_point_r[0] = temp_r[j][0];//x
                                center_point_r[1] = temp_r[j][1];//y
                            }
                        }

                    }
                }
     }

    //取出循环次数
    *l_stastic = l_data_statics;
    *r_stastic = r_data_statics;

}



/*
函数名称：void R_border(uint16 total_R)
功能说明：从八邻域边界里提取需要的边线(zzh
参数说明：
total_R  ：找到的点的总数
函数返回：无
修改时间：2024年2月24日
备    注：
example: R_border(data_stastics_r);
 */

void R_border(uint16 total_R)
{
    uint16 i = 0;
      uint8 h = 0;
      for(h = 0; h < image_h;h++)
      {
          right_bor[h] = 200;
      }
      while(i<total_R)
      {
          h = points_r[i][1];
          if (points_r[i][0] < right_bor[h])
          {
              right_bor[h] = points_r[i][0];
          }
              i++;
      }
}

/*
函数名称：void L_border(uint16 total_L)
功能说明：从八邻域边界里提取需要的边线(zzh
参数说明：
total_L  ：找到的点的总数
函数返回：无
修改时间：2024年2月24日
备    注：
example: L_border(data_stastics_L);
 */
void L_border(uint16 total_L)
{
    uint16 i = 0;
    uint8 h = 0;
    for(h =0; h < image_h;h++)
        {
            left_bor[h] = -1;
        }
    while(i<total_L)
    {
        h = points_l[i][1];
        if (points_l[i][0] > left_bor[h])
        {
            left_bor[h] = points_l[i][0];
        }
            i++;
    }

}
float k;
/*
函数名称：void L_border(uint16 total_L)
功能说明：cross边线(zzh
参数说明：
total_L  ：找到的点的总数
函数返回：无
修改时间：2024年2月24日
备    注：
example: L_border_cross(data_stastics_L);
 */
void L_border_cross(uint16 total_L)
{
    uint16 i = total_L - 1;
    uint8 h = 0;
    uint8 count = 0;
    uint8 x1=0,y1=0,x2=0,y2=0;
    float k_l = 0;
    uint8 h2 = 0;
    for(h =0; h < image_h;h++)
    {
        left_bor[h] = -1;
    }
    if(flag_cross == 1)
    {
    while(i>0)
    {
        h = points_l[i][1];
        if(h2 > h)
        {
            i --;
            continue;
        }
        h2 = 0;
        if(dir_l[i] == 6 && dir_l[i+1] != 6 && i - 2 > 1 && h > 3)
        {
            x1 = points_l[i+5][0];
            y1 = points_l[i+5][1];
            x2 = points_l[i+9][0];
            y2 = points_l[i+9][1];
            k_l = abs((y1 - y2)/(x1-x2));
            k = k_l<(1/k_l)?(1/k_l):k_l;
            while(count<100 && h+count <127)
            {
                if((points_l[i][0] - ((float)count/4) )< 2)
                {
                    left_bor[h+count] = 2;
                }
                else left_bor[h+count] = points_l[i][0] - ((float)count/4);
                //tft180_draw_point(Ctrl_x((left_bor[h+count])*dis_image_w/image_w), Ctrl_y((h+count)*dis_image_h/image_h), RGB565_RED);
                count ++;
            }
            h2 = h + count-1;
            count = 0;
        }
        else if (points_l[i][0] > left_bor[h])
        {
            left_bor[h] = points_l[i][0];
        }
            i--;
        }
    }
   else if(flag_cross == 2)
    {
        for( i =0; i < data_stastics_l;i++)
        {
            if(dir_l[i] == 2 && dir_l[i-1] == 4 && dir_l[i+1] == 2 )
            {
                x1 = points_l[i][0];
                y1 = points_l[i][1];
                x2 = i;

                break;

            }
        }
        for( i = 24; i < 127;i++)
        {
            left_bor[i] = x1 - (i- x2)/4 - 10;
        }
        x1= 0;
        y1 = 0;
        x2 = 0;

    }
}
void R_border_cross(uint16 total_R)
{
    uint16 i = total_R - 1;
    uint8 h = 0;
    uint8 count = 0;
    uint8 x1=0,y1=0,x2=0,y2=0;
    float k_r = 0;
    uint8 h2 = 0;
    for(h =0; h < image_h;h++)
        {
            right_bor[h] = 200;
        }
    if(flag_cross == 1)
    {
        while(i>0)
        {
            h = points_r[i][1];
            if(h2 > h)
            {
                i --;
                continue;
            }
            h2 = 0;
            if(dir_r[i-2] == 6&&dir_r[i-1] == 6&&dir_r[i] == 6 && dir_r[i+1] != 6 && i - 2 > 1 && h > 3)
            {
                x1 = points_r[i+7][0];
                y1 = points_r[i+7][1];

                x2 = points_r[i+11][0];
                y2 = points_r[i+11][1];
                k_r = abs((y1 - y2)/(x1-x2));\
                k = k_r<(1/k_r)?(1/k_r):k_r;
                while(count<100 && h+count <127)
                {
                    if(points_r[i][0] + ((float)count/4)> 158)
                    {
                        right_bor[h+count] = 158;
                    }
                    else right_bor[h+count] = points_r[i][0] + ((float)count/4);
                    //tft180_draw_point(Ctrl_x((right_bor[h+count])*dis_image_w/image_w), Ctrl_y((h+count)*dis_image_h/image_h), RGB565_BLUE);
                    count ++;
                }
                h2 = h + count-1;
                count = 0;
            }
            else if (points_r[i][0] < right_bor[h])
            {
               right_bor[h] = points_r[i][0];
            }
                i--;
        }
    }
    else if(flag_cross == 2)
    {
        for( i =0; i < data_stastics_r;i++)
        {
            if(dir_r[i] == 2 && dir_r[i+1] == 2 && (dir_r[i-1] == 4 || dir_r[i-1] == 5)&&points_r[i][0] > 70)
            {
                x1 = points_r[i][0];
                y1 = points_r[i][1];
                x2 = i;

                break;
            }
        }
        cir_x = x1;
        cir_y = y1;
        if(x1 < 70)
        {
            for( i = 24; i < 127;i++)
            {
                right_bor[i] = left_bor[i] + width_zzh[127 - i];
            }
        }
        else{
            for( i = 24; i < 127;i++)
            {
                right_bor[i] = x1 + (i- x2)/4;
            }

        }
        x1= 0;
        y1 = 0;
        x2 = 0;

    }
}

int16_t length = 0;
int r_lose_judge[image_h] = {0};
int l_lose_judge[image_h] = {0};
int16_t r_lose_num = 0, l_lose_num = 0;
void lose_line() //计算丢线情况，最长直道长度
{
    r_lose_num = 0;
    l_lose_num = 0;
    length = 0;
    uint16_t i = 0;

        for(i= image_h-1;i> 1;i--)
        {
            if(left_bor[i] <= 3 && left_bor[i] > 0)
            {
                l_lose_judge[i] = 1;
                l_lose_num ++;
            }
                else l_lose_judge[i] = 0;  // 丢线记为为1，
        }


        for(i= image_h-1;i> 1;i--)
        {
            if(right_bor[i] >= 156 &&right_bor[i] < 160)
            {
                r_lose_judge[i] = 1;
//                if(i < image_h -30)
                r_lose_num++;
            }
                else r_lose_judge[i] = 0;  // 丢线记为为1，
        }

    for(i = image_h-1;i >0;i --)
    {
        if(bin_image[i][80]!=0&&bin_image[i-1][80]!=0) //从底部开始找线
            length ++;
    }


}

//判断丢线行位置和数目
uint8 lose_num=0, nolose_num=0;
void lose_location_test(uint8_t type, uint8_t startline, uint8_t endline)
{//1左2右
    lose_num=0, nolose_num=0;
    uint8_t y = startline;
    if(type ==1)
    {
        for(y = startline; y>endline; y--)
        {
            lose_num += l_lose_judge[y];
            nolose_num = (startline-endline+1) -lose_num;
        }
    }
    if(type ==2)
    {
        for(y=startline; y>endline; y--)
        {
            lose_num += r_lose_judge[y];
            nolose_num = (startline-endline+1) -lose_num;
        }
    }
}
/*
函数名称：Cir1_judge(void)
说明：判断圆环  右侧圆环判断
参数说明：输入任意行数
函数返回：无
修改时间：2024年3月16日
备    注：
example：  Cir1_judge(image_h-2)
 */
uint8_t edge_x = 0,edge_y = 0,edge_i = 0;      //补线的起点
uint8_t edge_x2 = 0,edge_y2 = 0,edge_i2 = 0;
uint8_t edge_x3 = 0,edge_y3 = 0,edge_i3 = 0;
int16 encoder_sum_l4=800,encoder_sum_l7=600,encoder_sum_r4=800,encoder_sum_r7=500,encoder_sum=0;
uint8_t mmm = 0;
bool Cir1_judge(void)
{

    lose_location_test(2, 20, 0);
    if(cir_stage == 0 &&nolose_num > 10&&sum_dir_l()&&l_lose_num < 40&& r_lose_num >50 && length > 124&&r_dir_sum[4] > 50&&l_dir_sum[4] > 65)// && && r_lose_num >30 && length > 80 && my_abs(l_line_qulv)<4 && r_dir_sum[4] > 10
    {
        lose_location_test(1, 127, 110);
        if(nolose_num > 13)
        {
//            lose_location_test(2, 125, 115);
//            if(nolose_num > 5)
//            {
                mmm = nolose_num;
                cir_stage = 1;
                draw_cir1(bin_image);
                return 1;
//            }
        }
    }
    return 0;

}//stop
bool Cir2_judge(void)
{

   lose_location_test(1, 20, 0);
    if(cir_stage == 0 && nolose_num > 10 && sum_dir_r()&&r_lose_num < 50&& l_lose_num >40 && length > 124&&l_dir_sum[4] > 50&&r_dir_sum[4] > 65)// && && r_lose_num >30 && length > 80 && my_abs(l_line_qulv)<4 && r_dir_sum[4] > 10
    {
        lose_location_test(1, 127, 110);
        if(nolose_num > 13)
        {
            mmm = nolose_num;
            cir_stage = 1;
            draw_cir1(bin_image);
            return 1;
        }
    }
    return 0;
}
uint8_t cir_stage = 0;
uint8_t flagg = 0;
uint8_t Cir_hengyi_Flag=0;//0正常，1左圆环，2右圆环

void Cir_r_handle(void)
{
    switch(cir_stage)
    {
        case 1:
            encoder_sum += encoder_data_1;
            lose_location_test(2, 100,20);
            if(lose_num > 10)
            {
                lose_location_test(2, 40, 0);
                if(nolose_num >= 10)
                {
                    cir_stage = 2;                //上方为黑中为白 补线直行
                    encoder_sum = 0;

                }
                lose_location_test(2, 30,0);
                if(lose_num >= 10)
                {
                    encoder_sum = 0;
                    cir_stage = 2;
                }
            }
            if(encoder_sum > max_encoder)
                {
                    flag_r_cir = 0;
                    cir_stage =0;
                    encoder_sum =0;
                }
            break;
        case 2:
            encoder_sum += encoder_data_1;
            lose_location_test(2, 60,30);
            if(nolose_num > 15&&cir_stage == 2)
            {
                lose_location_test(2, 30,0);
                if(lose_num >15)
                {
                    cir_stage = 3;                 // 上方为白 中部为黑  直行
                    encoder_sum = 0;
                }
            }
            if(encoder_sum > max_encoder)
                {
                    flag_r_cir = 0;
                    cir_stage =0;
                    encoder_sum =0;

                }
            break;

        case 3:
            encoder_sum += encoder_data_1;
            lose_location_test(2, 60,30);
            if(lose_num > 15 && cir_stage == 3)
            {
                lose_location_test(2, 30,0);
                if(nolose_num > 25)
                {
                    encoder_sum= 0;
                           //Cir_hengyi_Flag = 2;
                    cir_stage = 4;                      //若上方为黑，下方为白 准备补线入环
                }
            }
            if(encoder_sum > max_encoder)
                {
                    flag_r_cir = 0;
                    cir_stage =0;
                    encoder_sum =0;
                }
            break;
        case 4 :
            encoder_sum += encoder_data_1;
//            if(encoder_sum > 300)
//                Cir_hengyi_Flag = 0;
                if(encoder_sum >encoder_sum_r4 )////75  500
                {
                    cir_stage = 5;
                    encoder_sum = 0;                    // 入环  补线走700
                }
            break;
        case 5:
            encoder_sum += encoder_data_1;

            if(encoder_sum > 1300)
            {
                encoder_sum = 0;
                cir_stage = 6;              //圆环内走完1500进入下一状态
            }
            break;
        case 6:
                lose_location_test(2, 75,25);
                if (lose_num > 20)
                {
                    lose_location_test(1, 75, 25);
                    if(lose_num > 20)
                    {
                        encoder_sum = 0;
                        cir_stage = 7;              //若检测到右侧大部分丢线，左侧顶部丢线，准备拉线出环
                    }
                 }
            break;
        case 7:

            encoder_sum += encoder_data_1;
            if(encoder_sum> encoder_sum_r7)
            {
                cir_stage = 8;                  // 出线过程 拉线
                encoder_sum =0;
            }
           break;
        case 8:
            lose_location_test(2,20,0);
            if(nolose_num >15)
            {
                lose_location_test(1,60,0);
                if(nolose_num > 50)
                {
                    cir_stage = 9;
                }
            }
            break;
        case 9:
            encoder_sum += encoder_data_1;
            if(encoder_sum >1000)  //积分一段距离并补线环岛处理结束(所有标志位清零)
            {
                flag_r_cir = 0;
                cir_stage =0;
                encoder_sum =0;
            }
            break;


    }
    int i =0;
    //处理
    if(cir_stage ==1)    //半宽补线补左线
    {
        for(i = 0;i<image_h-1 ;i++)
        {
            left_bor[i] = left_bor[i] + 25;
            right_bor[i] = left_bor[i] + width_zzh[128-i -1] + 10+5 ;

        }
    }
    else if((cir_stage ==2) || (cir_stage ==3))
    {
        cir_point(2);
        for(i = 0;i<image_h-1 ;i++)
        {
            left_bor[i] = left_bor[i] +4;
            right_bor[i] = left_bor[i] + width_zzh[128- i -1]+10+5+5+5;
        }
    }
    else if(cir_stage ==4)   //赛道宽度补线
    {
        cir_point(2);
//        for(i = 0;i<image_h-11;i++)
//        {
//
//            right_bor[i] = 155;
//            left_bor[i] = right_bor[i] - width_zzh[118 - i - 1]-20-10 -10;
//
//        }
        float count = 0;
        for(i = cir_y;i<image_h;i++)
        {
            right_bor[i] = 155;
            left_bor[i] = cir_x - count ;
            count ++;
        }
        count = 0;
        for(i = cir_y;i > 10; i--)
        {
            right_bor[i] = 155;
            left_bor[i] = cir_x + count;
            count ++;
        }
        count = 0;
    }
    else if(cir_stage ==5)   //在环岛
    {
        //无需操作

    }
    else if(cir_stage ==6)   //出环岛
    {
//        for(i = 0;i<image_h-31;i++)
//        {
//
//
//            left_bor[i] = right_bor[i] - width_zzh[90 - i - 1];
//        }
    }
    else if(cir_stage==7)
    {
        for(i = 0;i<image_h-11;i++)
        {
            right_bor[i] = 155;
            left_bor[i] = right_bor[i] - width_zzh[118 - i - 1] - 30;
        }
    }
    else if(cir_stage == 8)
    {
//
    }
    else if (cir_stage == 9)
    {
        for(i = 0;i<image_h-11;i++)
        {
            left_bor[i] = left_bor[i] + 5;
            right_bor[i] = left_bor[i] + width_zzh[118- i -1];
        }
    }
}
void Cir_l_handle(void)
    {
        switch(cir_stage)
        {
            case 1:
                encoder_sum += encoder_data_1;
                lose_location_test(1, 50,20);
                if(lose_num > 15)
                {
                    lose_location_test(1, 20, 0);
                    if(nolose_num >= 10)
                    {
                        cir_stage = 2;                //上方为黑中为白 补线直行
                        encoder_sum = 0;
                    }
                }
                if(encoder_sum > max_encoder)
                    {
                        flag_r_cir = 0;
                        cir_stage =0;
                        encoder_sum =0;
                    }
                break;
            case 2:
               // tft180_show_int(    1,  110, 2, 1);// 显示 int16 数据
                encoder_sum += encoder_data_1;

                lose_location_test(1, 40,20);
                if(nolose_num > 15&&cir_stage == 2)
                {
                    lose_location_test(1, 20,0);
                    if(lose_num >15)
                    {
                        cir_stage = 3;                 // 上方为白 中部为黑  直行
                        encoder_sum=0;
                    }
                }
                if(encoder_sum > max_encoder)
                    {
                        flag_r_cir = 0;
                        cir_stage =0;
                        encoder_sum =0;
                    }
                break;
            case 3:
               // tft180_show_int(    1,  110, 3, 1);// 显示 int16 数据
                encoder_sum += encoder_data_1;

                lose_location_test(1, 70,30);
                if(lose_num > 15 && cir_stage == 3)
                {
                    lose_location_test(1, 40,0);
                    if(nolose_num > 15)
                    {
                        cir_stage = 4;                      //若上方为黑，下方为白 准备补线入环
                       // Cir_hengyi_Flag = 1;
                        encoder_sum = 0;
                    }
                }
                if(encoder_sum > max_encoder)
                    {
                        flag_r_cir = 0;
                        cir_stage =0;
                        encoder_sum =0;
                    }
                break;
            case 4 :
               // tft180_show_int(    1,  110, 4,1);// 显示 int16 数据

                encoder_sum += encoder_data_1;
                    if(encoder_sum > encoder_sum_l4)////75  500
                    {
                        cir_stage = 5;
                        encoder_sum = 0;
                    }

                break;
            case 5:
               // tft180_show_int(    1,  110, 5, 1);// 显示 int16 数据
                encoder_sum += encoder_data_1;
                if(encoder_sum > 500)
                {
                    encoder_sum = 0;
                    cir_stage = 6;              //圆环内走完1500进入下一状态
                }
                break;
            case 6:
                    lose_location_test(1, 89,5);
                    if (lose_num > 10)
                    {
                        lose_location_test(2, 89, 25);
                        if(lose_num > 10)
                        {
                            encoder_sum = 0 ;
                            cir_stage = 7;              //若检测到右侧大部分丢线，左侧顶部丢线，准备拉线出环
                        }

                     }
                break;
            case 7:
                //tft180_show_int(    1,  110, 7, 1);// 显示 int16 数据

                encoder_sum += encoder_data_1;
                if(encoder_sum > encoder_sum_l7)
                {
                    cir_stage = 8;                  // 出线过程 拉线
                    encoder_sum =0;
                }
               break;
            case 8:
                lose_location_test(1,20,0);
                if(nolose_num >10)
                {
                    lose_location_test(2, 60, 10);
                    if(nolose_num > 45)
                    {
                        cir_stage = 9;
                    }
                }

                break;
            case 9:
                encoder_sum += encoder_data_1;
                if(encoder_sum >1000)  //积分一段距离并补线环岛处理结束(所有标志位清零)
                {
                    flag_l_cir = 0;
                    cir_stage =0;
                    encoder_sum =0;
                }
                break;
        }
        int i =0;
        //处理
        if(cir_stage ==1)    //半宽补线补左线
        {
            for(i = 0;i<image_h-11 ;i++)
            {

                right_bor[i] = right_bor[i] -1;
                left_bor[i] = right_bor[i] - width_zzh[117-i-1] - 10 -15-5;

            }
        }
        else if((cir_stage ==2) || (cir_stage ==3))
        {
            cir_point(1);

            for(i = 0;i<image_h-1 ;i++)
            {
                right_bor[i] = right_bor[i] -1;
                left_bor[i] = right_bor[i] - width_zzh[128-i-1]-10 -15-5;

            }
        }
        else if(cir_stage ==4)   //赛道宽度补线
        {
            for(i = 5;i<image_h-11;i++)
            {
                left_bor[i] = 5;
                right_bor[i] = left_bor[i] + width_zzh[117 - i - 1]+10+ 5;
            }
//            cir_point(1);
//            float count = 0;
//            for(i = cir_y;i<image_h;i++)
//            {
//                left_bor[i] = 5;
//                right_bor[i] = cir_x + count-10;
//                count ++;
//            }
//            count = 0;
//            for(i = cir_y;i > 10; i--)
//            {
//                left_bor[i] = 5;
//                right_bor[i] = cir_x - count-10 ;//左圆环处理
//                count ++;
//            }
//            count = 0;
        }
        else if(cir_stage ==5)   //在环岛
        {
            ;//无需操作
        }
        else if(cir_stage ==6)   //出环岛
        {
    //        for(i = 0;i<image_h-31;i++)
    //        {
    //
    //
    //            left_bor[i] = right_bor[i] - width_zzh[90 - i - 1];
    //        }
        }
        else if(cir_stage==7)
        {
            for(i = 0;i<image_h-11;i++)
            {
               // left_bor[i] = right_bor[i] - width_zzh[90 - i - 1]+10;
                right_bor[i] = left_bor[i] + width_zzh[117 - i - 1]  +5;
            }
        }
        else if(cir_stage == 8)
        {

        }
        else if (cir_stage == 9)
        {
            for(i = 0;i<image_h-11;i++)
            {
                right_bor[i] = right_bor[i] -1;
                left_bor[i] = right_bor[i] - width_zzh[117-i-1]-5;
            }
        }
    }
/*
函数名称：void cross_road(void)
功能说明：十字路口判断
参数说明：无
函数返回：无
修改时间：2024年2月27日
备    注：无
*/
u8 cross_judge(void)
{
    uint16 i = 0;
    uint8 num1 = 0;
    uint8 num2 = 0,num3 = 0,num4 = 0;
    if(l_l_judge() && length > 110)
    {
        //draw_cir1(bin_image);
        for(i = 0;i<data_stastics_l;i++)
        {
            if(dir_r[i] == 6)
            {
                num1 ++;
            }
            if(dir_l[i] == 6)
            {
                num2 ++;
            }

        }
        if(num2 >5 && num1 >5&&r_dir_sum[4] > 20&&l_dir_sum[4] > 20)
        {
            //draw_cross(bin_image);

            num1 = 0;
            num2 = 0;
            return 1;
        }
    }
    if(l_l_judge() == 2 && length >110)
    {
        for(i = 0;i<data_stastics_l;i++)
        {

            if(dir_r[i] == 2)
            {
                num4 ++;
            }
            if(dir_l[i] == 2)
            {
                num3 ++;
            }
        }
        if(num3 >5 && num4 >4&&r_dir_sum[4] > 10&&l_dir_sum[4] > 10)
        {
           // draw_cross(bin_image);

            num3 = 0;
            num4 = 0;
            return 2;
        }
    }
    return 0;
}
void Element_handle(void)
{
    uint8 m = 0;
    Black_jiexian=0;//检测该图像上处是否存在全黑情况，并且得到全黑界限的纵坐标

    int i =0;
    if(flag_r_cir == 1)
    {
       // cir_point(2);
        Cir_r_handle();
        for(i = 0;i < image_h; i ++)
        {
//            if(i<70&&i>40&&cir_stage == 4)
//                center_line[i] = 110;
//            else
            center_line[i] = (left_bor[i]+right_bor[i])/2;
        }
        if(cir_stage == 7 || cir_stage == 7)
            center_line[125] = 80;
    }
    else if(flag_l_cir == 1)
    {
        //cir_point(1);
        Cir_l_handle();
        for(i = 0;i < image_h; i ++)
        {

            center_line[i] = (left_bor[i]+right_bor[i])/2;
        }
        if(cir_stage == 7 || cir_stage == 7)
            center_line[125] = 80;
    }
    else if(flag_cross ==1)
    {
        L_border_cross(data_stastics_l);
        R_border_cross(data_stastics_r);
        for( m = 0; m < image_h;m ++)
        {
            center_line[m] = (left_bor[m]+right_bor[m])/2;
        }
        m = 0;
       // flag_cross = 0;
    }

    else
        {
//            for(m=0;m<image_h-3;m++)
//            {
//                if((abs(right_bor[m]-right_bor[m+1])>2)&&((abs(right_bor[m]-right_bor[m-1])>2)))
//                {
//                    right_bor[m]=(right_bor[m+1]+right_bor[m-1])>>1;
//                }
//                if((abs(left_bor[m]-left_bor[m+1])>2)&&((abs(left_bor[m]-left_bor[m-1])>2)))
//                {
//                    left_bor[m]=(left_bor[m+1]+left_bor[m-1])>>1;
//                }
//            }
//
//
//                center_line[126]=(left_bor[126]+right_bor[126])>>1;
//                for(m=125;m>0;m--)
//                {
//                    if((left_bor[m]==200)||(right_bor[m]==-1))//全黑异常情况
//                    {
//                        center_line[m]=-1;
//                        Black_jiexian=(Black_jiexian==0)?m:Black_jiexian;//只变化一次，所以找到全黑最下处
//                        continue;//跳过接下来的判断
//                    }
//                    if(((left_bor[m]==2)&&(right_bor[m]==157)))//左右都靠边，可能是十字
//                    {
//                        center_line[m]=center_line[m+1];//直接先等于下面一个中线点
//                    }
//                    else if((left_bor[m]!=2)&&(right_bor[m]!=157))//正常情况，没有左右哪一边靠边
//                    {
//                        center_line[m] = (right_bor[m]+left_bor[m])>>1;
//                        if(abs(center_line[m]-center_line[m+1])>=2)
//                        {
//                            if((left_bor[m]-2)>(157-right_bor[m]))
//                            {
//                                center_line[m]=(center_line[m+1]+left_bor[m]-left_bor[m+1]);
//                            }
//                            else
//                            {
//                                center_line[m]=(center_line[m+1]+right_bor[m]-right_bor[m+1]);//据右线点和上一个中线点和上一个右线点求得
//                            }
//                        }
//                    }
//                    else if((left_bor[m]==2)&&(right_bor[m]!=157))//左边靠边
//                    {
//                        center_line[m]=(right_bor[m]-(right_bor[m+1]-center_line[m+1]));//据右线点和下一个中线点和下一个右线点求得
//                    }
//                    else if((left_bor[m]!=2)&&(right_bor[m]==157))//右边靠边
//                    {
//                        center_line[m]=(left_bor[m]+(center_line[m+1]-left_bor[m+1]));//据左线点和下一个中线点和下一个左线点求得
//                    }
//                }
//               }
//                int16 temp_shang=5;
//                if(Black_jiexian!=0)temp_shang=Black_jiexian;
//                {
//                    for(m=40;m>temp_shang;m--)
//                    {
//                        if(center_line[m]-center_line[m+1]>=4)
//                        {
//                            center_line[m]=center_line[m+1];
//                        }
//                        else if(center_line[m]-center_line[m+1]<=-4)
//                        {
//                            center_line[m]=center_line[m+1];
//                        }
//                    }
//                }

            for(i = 126;i >=1 ;i--)
            {
                center_line[i] = (right_bor[i] + left_bor[i] )>>1;
                if(Black_jiexian==0)
                {
                    if(left_bor[i]==-1||right_bor[i]==200)
                    {
                        Black_jiexian=i;
                    }
                }
            }
        }


}
uint16_t encoder_sum_cross = 0;
int16 l_line_qulv=0, r_line_qulv=0;
uint8_t flag_others2=1;//用来区别直线加速,1是直线，0是元素
void Element_test(void)
{
    l_line_qulv = 1000*Process_Curvity(left_bor[MT9V03X_H-20], MT9V03X_H-20, left_bor[MT9V03X_H-35], MT9V03X_H-35, left_bor[MT9V03X_H-50], MT9V03X_H-50);
    r_line_qulv = 1000*Process_Curvity(right_bor[MT9V03X_H-20], MT9V03X_H-20, right_bor[MT9V03X_H-35], MT9V03X_H-35, right_bor[MT9V03X_H-50], MT9V03X_H-50);
    lose_line();

    if(Cir1_judge() && flag_cross != 1 )
    {
        flag_r_cir = 1;
        flag_cross = 0;
        flag_others2=0;
    }
    else if(Cir2_judge() && flag_cross != 1  && flag_r_cir != 1)
    {
        flag_l_cir = 1;
        flag_cross = 0;
        flag_others2=0;
    }
    else if (cross_judge() == 1)
    {
            flag_cross = 1;
            flag_others2=0;
    }
    else if(cross_judge() == 2)
    {
            flag_cross = 0;
            flag_others2=0;
    }
    else
    {
        flag_others = 1;
        flag_cross = 0;
        flag_others2=1;
    }

}
bool flag_str =0;
bool flag_str_str=0;
u8 straight_judge(void)
{
    u8 i = 0, distance = 0;
    if(length > 115 && flag_l_cir != 1 && flag_r_cir !=1)
    {
        for(i = 13; i < 20; i ++)
        {
            distance = my_abs(center_line[i]- center_line[i + 50]);
            if(distance > 5)
            {
                flag_str = 0;
                return 0;
            }

        }
        flag_str = 1;
        return 1;
    }
    else {
        flag_str = 0;

         }
    return 0;
}
u8 straight_judge_2(void)
{
    u8 i = 0, distance = 0;
    if(length > 115 && flag_l_cir != 1 && flag_r_cir !=1)
    {
        for(i = 13; i < 20; i ++)
        {
            distance = my_abs(center_line[i]- center_line[i + 50]);
            if(distance > 2)
            {
                flag_str_str = 0;
                return 0;
            }

        }
        flag_str_str = 1;
        return 1;
    }
    else {
        flag_str_str = 0;

         }
    return 0;
}
/*
函数名称：void center_valid(void)
功能说明：八邻域后求出有效中线，并在屏幕绘出边界线＋中线
参数说明：无
函数返回：无
修改时间：2024年2月27日
备    注：
example： center_valid();
 */

void center_valid(void)
{
    // if(sum_dir_r()&&r_lose_num < 50&& l_lose_num >60 && length > 120&&l_dir_sum[4] > 40&&r_dir_sum[4] > 55)// && && r_lose_num >30 && length > 80 && my_abs(l_line_qulv)<4 && r_dir_sum[4] > 10

    tft180_draw_point(Ctrl_x(cir_x)  *dis_image_w/image_w,   Ctrl_y(cir_y)  *dis_image_h/image_h, RGB565_BLUE);
    tft180_draw_point(Ctrl_x(cir_x+1)*dis_image_w/image_w,   Ctrl_y(cir_y+1)*dis_image_h/image_h, RGB565_BLUE);
    tft180_draw_point(Ctrl_x(cir_x+1)*dis_image_w/image_w,   Ctrl_y(cir_y-1)*dis_image_h/image_h, RGB565_BLUE);
    tft180_draw_point(Ctrl_x(cir_x+1)*dis_image_w/image_w,   Ctrl_y(cir_y)  *dis_image_h/image_h, RGB565_BLUE);
    tft180_draw_point(Ctrl_x(cir_x-1)*dis_image_w/image_w,   Ctrl_y(cir_y+1)*dis_image_h/image_h, RGB565_BLUE);
    tft180_draw_point(Ctrl_x(cir_x-1)*dis_image_w/image_w,   Ctrl_y(cir_y-1)*dis_image_h/image_h, RGB565_BLUE);
    tft180_draw_point(Ctrl_x(cir_x-1)*dis_image_w/image_w,   Ctrl_y(cir_y)  *dis_image_h/image_h, RGB565_BLUE);
    tft180_draw_point(Ctrl_x(cir_x)  *dis_image_w/image_w,   Ctrl_y(cir_y+1)*dis_image_h/image_h, RGB565_BLUE);
    tft180_draw_point(Ctrl_x(cir_x)  *dis_image_w/image_w,   Ctrl_y(cir_y-1)*dis_image_h/image_h, RGB565_BLUE);

    tft180_set_color(RGB565_BLACK, RGB565_WHITE);//设置字体颜色和背景颜色
    tft180_show_char(     1,  8*10, 'a');
    tft180_set_color(RGB565_BLACK, RGB565_WHITE);//设置字体颜色和背景颜色
    tft180_show_int(    10,  8*10, error_speed, 3);// 显示 int16 数据encoder_data_1
    tft180_set_color(RGB565_RED, RGB565_WHITE);//设置字体颜色和背景颜色
    tft180_show_int(    40,  8*10, error_sum_speed, 3);// 显示 int16 数据encoder_data_1
    tft180_set_color(RGB565_BLUE, RGB565_WHITE);//设置字体颜色和背景颜色
    tft180_show_int(    75,  8*10, error_base_pwm, 3);// 显示 int16 数据encoder_data_1
    tft180_set_color(RGB565_RED, RGB565_WHITE);//设置字体颜色和背景颜色
    tft180_show_int(    110,  8*10, encoder_data_1,3);// 显示 int16 数据encoder_data_1

    tft180_set_color(RGB565_BLACK, RGB565_WHITE);//设置字体颜色和背景颜色
    tft180_show_char(     1,  8*11, 'b');
    tft180_set_color(RGB565_RED, RGB565_WHITE);//设置字体颜色和背景颜色encoder_sum
    tft180_show_int(    10,  8*11,error, 3);// 显示 int16 数据encoder_data_1
    tft180_set_color(RGB565_BLUE, RGB565_WHITE);//设置字体颜色和背景颜色
    tft180_show_int(  40,  8*11, center_line[40], 3); // 显示 float 数据  4bit整数  1bit小数 总共会有 7个字符的显示占位
    tft180_set_color(RGB565_GREEN, RGB565_WHITE);//设置字体颜色和背景颜色error
    tft180_show_int(  75,  8*11, center_line[60], 3); // 显示 float 数据  5bit整数  4bit小数 总共会有 7个字符的显示占位
    tft180_set_color(RGB565_GREEN, RGB565_WHITE);//设置字体颜色和背景颜色error
    tft180_show_int(  110,  8*11, center_line[80], 3); // 显示 float 数据  5bit整数  4bit小数 总共会有 7个字符的显示占位

    tft180_set_color(RGB565_BLACK, RGB565_WHITE);//设置字体颜色和背景颜色
    tft180_show_char(     1,  8*12, 'c');
    tft180_set_color(RGB565_BLACK, RGB565_WHITE);//设置字体颜色和背景颜色
    tft180_show_int(    10,  8*12, error_hengyi, 3);// 显示 int16 数据
    tft180_set_color(RGB565_RED, RGB565_WHITE);//设置字体颜色和背景颜色
    tft180_show_int(    40,  8*12, Find_hengyi_error(), 3);// 显示 int16 数据
    tft180_set_color(RGB565_BLACK, RGB565_WHITE);//设置字体颜色和背景颜色
    tft180_show_int(    75,  8*12, flag_1, 3);// 显示 int16 数据
    tft180_set_color(RGB565_BLUE, RGB565_WHITE);//设置字体颜色和背景颜色
    tft180_show_int(    110,  8*12, Base_Encoder, 3);// 显示 int16 数据

    tft180_set_color(RGB565_BLACK, RGB565_WHITE);//设置字体颜色和背景颜色
    tft180_show_char(     1,  8*13, 'd');
    tft180_set_color(RGB565_PURPLE, RGB565_WHITE);//设置字体颜色和背景颜色
    tft180_show_int(  10,  8*13, imu_cha*1, 3); // 显示 float 数据  4bit整数  1bit小数 总共会有 7个字符的显示占位
    tft180_set_color(RGB565_BLACK, RGB565_WHITE);//设置字体颜色和背景颜色
    tft180_show_int(  40,  8*13, imu_last, 3); // 显示 float 数据  4bit整数  1bit小数 总共会有 7个字符的显示占位
    tft180_set_color(RGB565_PURPLE, RGB565_WHITE);//设置字体颜色和背景颜色error_pwm
    tft180_show_int(  75,  8*13, eulerAngle.yaw, 3); // 显示 float 数据  4bit整数  1bit小数 总共会有 7个字符的显示占位
    tft180_set_color(RGB565_PURPLE, RGB565_WHITE);//设置字体颜色和背景颜色error_pwm
    tft180_show_int(  110,  8*13, imu660ra_gyro_z, 3); // 显示 float 数据  4bit整数  1bit小数 总共会有 7个字符的显示占位

    tft180_set_color(RGB565_BLACK, RGB565_WHITE);//设置字体颜色和背景颜色
    tft180_show_char(     1,  8*14, 'G');
    tft180_set_color(RGB565_PURPLE, RGB565_WHITE);//设置字体颜色和背景颜色
    tft180_show_int(  10,  8*14, temp_1, 3); // 显示 float 数据  4bit整数  1bit小数 总共会有 7个字符的显示占位
    tft180_set_color(RGB565_BLACK, RGB565_WHITE);//设置字体颜色和背景颜色
    tft180_show_int(  40,  8*14, temp_2, 3); // 显示 float 数据  4bit整数  1bit小数 总共会有 7个字符的显示占位
    tft180_set_color(RGB565_PURPLE, RGB565_WHITE);//设置字体颜色和背景颜色error_pwm
    tft180_show_int(  75,  8*14, temp_3, 3); // 显示 float 数据  4bit整数  1bit小数 总共会有 7个字
    tft180_set_color(RGB565_PURPLE, RGB565_WHITE);//设置字体颜色和背景颜色error_pwm
    tft180_show_int(  110,  8*14, temp_4, 3); // 显示 float 数据  4bit整数  1bit小数 总共会有 7个字
    tft180_set_color(RGB565_PURPLE, RGB565_WHITE);//设置字体颜色和背景颜色error_pwm
    tft180_show_int(  126,  8*14, temp_2-temp_4, 3); // 显示 float 数据  4bit整数  1bit小数 总共会有 7个字

    tft180_set_color(RGB565_BLACK, RGB565_WHITE);//设置字体颜色和背景颜色error_pwm
    tft180_show_int(  101,  8*0, flag_stop, 3); // 显示 float 数据  4bit整数  1bit小数 总共会有 7个字
    tft180_set_color(RGB565_RED, RGB565_WHITE);//设置字体颜色和背景颜色error_pwm
    tft180_show_int(  130,  8*0, encoder_sum, 3); // 显示 float 数据  4bit整数  1bit小数 总共会有 7个字
    tft180_set_color(RGB565_RED, RGB565_WHITE);//设置字体颜色和背景颜色error_pwm
    tft180_show_int(  101,  8*1, flag_others2, 3); // 显示 float 数据  4bit整数  1bit小数 总共会有 7个字
    tft180_set_color(RGB565_BLACK, RGB565_WHITE);//设置字体颜色和背景颜色error_pwm
    tft180_show_int(  110,  8*1, flag_str, 3); // 显示 float 数据  4bit整数  1bit小数 总共会有 7个字
    tft180_set_color(RGB565_BLACK, RGB565_WHITE);//设置字体颜色和背景颜色error_pwm
    tft180_show_int(  120,  8*1, flag_str_str, 3); // 显示 float 数据  4bit整数  1bit小数 总共会有 7个字
    tft180_set_color(RGB565_BLACK, RGB565_WHITE);//设置字体颜色和背景颜色error_pwm
    tft180_show_int(  101,  8*2, flag_cross, 3); // 显示 float 数据  4bit整数  1bit小数 总共会有 7个字
    tft180_set_color(RGB565_BLACK, RGB565_WHITE);//设置字体颜色和背景颜色error_pwm
    tft180_show_int(  120,  8*2, cross_jud, 3); // 显示 float 数据  4bit整数  1bit小数 总共会有 7个字
    tft180_set_color(RGB565_BLACK, RGB565_WHITE);//设置字体颜色和背景颜色error_pwm
    tft180_show_int(  101,  8*3, encoder_data_1_sum, 8); // 显示 float 数据  4bit整数  1bit小数 总共会有 7个字
    tft180_set_color(RGB565_BLACK, RGB565_WHITE);//设置字体颜色和背景颜色error_pwm
    tft180_show_int(  101,  8*4, image_process_flag, 8); // 显示 float 数据  4bit整数  1bit小数 总共会有 7个字
    tft180_set_color(RGB565_BLACK, RGB565_WHITE);//设置字体颜色和背景颜色error_pwm
    tft180_show_int(  101,  8*5,cir_stage, 4); // 显示 float 数据  4bit整数  1bit小数 总共会有 7个字
    tft180_set_color(RGB565_BLACK, RGB565_WHITE);//设置字体颜色和背景颜色error_pwm
    tft180_show_int(  101,  8*6,flag_bar, 4); // 显示 float 数据  4bit整数  1bit小数 总共会有 7个字
}
void image_filter(uint8(*bin_image)[image_w])//形态学滤波，简单来说就是膨胀和腐蚀的思想
{
    uint16 i, j;
    uint32 num = 0;


    for (i = 1; i < image_h - 1; i++)
    {
        for (j = 1; j < (image_w - 1); j++)
        {
            //统计八个方向的像素值
            num =
                bin_image[i - 1][j - 1] + bin_image[i - 1][j] + bin_image[i - 1][j + 1]
                + bin_image[i][j - 1] + bin_image[i][j + 1]
                + bin_image[i + 1][j - 1] + bin_image[i + 1][j] + bin_image[i + 1][j + 1];


            if (num >= threshold_max && bin_image[i][j] == 0)
            {

                bin_image[i][j] = 255;//白  可以搞成宏定义，方便更改

            }
            if (num <= threshold_min && bin_image[i][j] == 255)
            {

                bin_image[i][j] = 0;//黑

            }

        }
    }

}

/*
函数名称：void image_draw_rectan(uint8(*image)[image_w])
功能说明：给图像画一个黑框
参数说明：uint8(*image)[image_w] 图像首地址
函数返回：无
修改时间：2024年2月24日
备    注：
example： image_draw_rectan(bin_image);
 */
void image_draw_rectan(uint8(*image)[image_w])
{

    uint8 i = 0;
    for (i = 0; i < image_h; i++)
    {
        image[i][0] = 0;
        image[i][image_w - 1] = 0;
        bin_image[i][1] = 0;
        bin_image[i][image_w - 2] = 0;
        bin_image[i][2] = 0;
        bin_image[i][image_w - 3] = 0;

    }
    for (i = 0; i < image_w; i++)
    {
        image[0][i] = 0;
        image[image_h-1][i] = 0;

    }
}
//找误差

float Find_Error(void)//计算图像测得的中线位置和图像中心直线的误差
{
    uint8 temp1=0;
    uint8 temp2=0;
    if((flag_others==1)&&(cir_stage!=2)&&(cir_stage!=3)&&(cir_stage!=4)&&(cir_stage!=5)&&(cir_stage!=6)&&(cir_stage!=7)&&(cir_stage!=8)&&(cir_stage!=9))//普通pid
    {//非元素、不是圆环，普通情况

//        if(Black_jiexian!=0)
//        {
//            temp1=Black_jiexian+1;
//            temp2=Black_jiexian+9;
//        }
             temp1=100;
             temp2=110;




    }
    if(cir_stage==2||cir_stage==3||cir_stage==4||cir_stage==5||cir_stage==6||cir_stage==7||cir_stage==8||cir_stage==9)
    {//是圆环，再往上看一点

        temp1=80;//115
        temp2=90;//120
        if(Black_jiexian>=80)
        {
            temp1=Black_jiexian+1;
            temp2=Black_jiexian+3;
        }
    }
    if(flag_cross == 1 || flag_cross ==2) //十字pid
    {//是十字
        temp1=30;//30
        temp2=60;//60 取最下面
//          temp1=115;
//          temp2=122;
    }
//    if(((flag_cross==1||flag_cross==2)&&flag_str==1 )||(flag_others2==1&&flag_str==1))
//     {
//         temp1=120;
//         temp2=125;
//     }
    if(cross_jud==2)
    {
        temp1=70;
        temp2=80;
    }

    //取平均误差
    float error=0;
    float x=0;
        for(int i=temp1;i<=temp2;i++)
        {
            if(center_line[i]!=-1&&center_line[i]>=0)
            {
                error+=center_line[i];
                x++;
            }
        }
        error=(float)(error/x);
        return (float)(error-(MT9V03X_W/2));

}
bool Find_stop(void)//该停车时返回1
{

    bool renum;
    for(int i=55;i<=100;i++)
    {
        if(bin_image[125][i]==0&&bin_image[124][i]==0)
        {
            renum=1;
        }
        else
        {
            renum=0;
            break;
        }
    }
    return renum;
}
float error_hengyi_temp_last=99.123;
float Find_hengyi_error(void)//计算图像下方10个点与中点的误差
{
        float error=0;
        float x=0;
        int8 temp1=60;//30
        int8 temp2=70;//60 取最下面
        if(cross_jud==2)
        {
            temp1=70;
            temp2=80;
        }
        if(Black_jiexian>60)
        {
            temp1=Black_jiexian+1;
            temp2=Black_jiexian+5;
        }
        for(int i=temp1;i<=temp2;i++)
        {
            if(center_line[i]!=-1&&center_line[i]>=0)
            {
                error+=center_line[i];
                x++;
            }
        }
        error=(float)(error/x);
        error=(float)(-error+(MT9V03X_W/2));

        if(error_hengyi_temp_last==99.123)error_hengyi_temp_last=error;
        else if(error_hengyi_temp_last!=99.123)
        {
            if(error-error_hengyi_temp_last>3||error-error_hengyi_temp_last<-3)
            {
                error=(error+error_hengyi_temp_last)/2;
            }
            error_hengyi_temp_last=error;
        }





//                    if(((flag_cross==1||flag_cross==2)&&flag_str==1 )||(flag_others2==1&&flag_str==1))
//                    {
//                        error=(MT9V03X_W/2)-center_line[90];
//                    }




        return (error);

}





//三点法计算赛道曲率
float Process_Curvity(uint8 x1, uint8 y1, uint8 x2, uint8 y2, uint8 x3, uint8 y3)
{
    float K;
    int S_of_ABC = ((x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1)) / 2;
    //面积的符号表示方向
    int16 q1 = (int16)((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    uint8 AB = My_Sqrt(q1);
    q1 = (int16)((x3 - x2) * (x3 - x2) + (y3 - y2) * (y3 - y2));
    uint8 BC = My_Sqrt(q1);
    q1 = (int16)((x3 - x1) * (x3 - x1) + (y3 - y1) * (y3 - y1));
    uint8 AC = My_Sqrt(q1);
    if (AB * BC * AC == 0)
    {
        K = 0;
    }
    else
        K = (float)4 * S_of_ABC / (AB * BC * AC);
    return K;
}
u8 flag = 0;
int count_line = 0;
bool flag_stop = 0;
int stop_line_time=0;
int16 encoder_sum123 = 0;
bool flag_time_stop=0;
bool stop_line(void)
{
    int i = 0;
    count_line = 0;
    int count_line1 = 0;
     int count_line2 = 0;
    for(i = 5; i < 115; i++)
    {
        if(bin_image[110][i] != 0 && bin_image[110][i + 1] == 0&& bin_image[110][i + 2] == 0&& bin_image[110][i + 3] == 0)
        {
            count_line ++;
        }
        if(bin_image[110][i] != 0 && bin_image[110][i + 1] == 0&& bin_image[110][i + 2] == 0&& bin_image[110][i + 3] == 0)
        {
            count_line1 ++;
        }
        if(bin_image[110][i] != 0 && bin_image[110][i + 1] == 0&& bin_image[110][i + 2] == 0&& bin_image[110][i + 3] == 0)
        {
            count_line2 ++;
        }
    }

        if(encoder_sum123 < 3000 && flag == 0)
            encoder_sum123 += encoder_data_1;
        if(encoder_sum123 > 3000)
            {
                flag = 1;
                encoder_sum123=0;
            }
        if((count_line >=6 || count_line1 >= 6 || count_line2 >=6) && flag == 1)
        {

            flag_stop = 1;
        }
        else {
            flag_stop = 0;
        }
    if(flag_stop==1&&stop_line_time==0)//检测到斑马线
    {
        stop_line_time++;
        flag_time_stop=0;//归零
    }
    if(stop_line_time!=0&&stop_line_time<=18)//延时10个
    {
        stop_line_time++;
        if(stop_line_time==18)
        {
            flag_time_stop=1;//停车
        }
    }
    if(stop_line_time>18)//超过10个
    {
        stop_line_time=0;
        flag_time_stop=0;//归零
    }
    return flag_stop;
}
int cross_jud_time=0;
int cross_jud=0;

int cross_delay_judge(void)
{
    if(cross_jud==0&&(flag_cross==1||flag_cross==2))cross_jud=1;//进入十字
    if(cross_jud==1&&flag_cross==0&&flag_others2==1)cross_jud=2;//退出十字并且没有进入其他元素
    if(cross_jud==2&&flag_others2==1)
    {
        cross_jud_time++;
        if(cross_jud_time>=10)
        {
            cross_jud=0;
            cross_jud_time=0;
        }
    }
    if(cross_jud==2&&flag_others2==0)
    {
        cross_jud=0;
        cross_jud_time=0;
    }
    return cross_jud;
}
//障碍物判断
u8 flag_bar = 0;  // 1 为左侧障碍，2为右侧障碍
u8 count_l = 0,count_r = 0;
void judge_bar(void)
{
    uint8_t i = 0;
    flag_bar = 0;
    count_l = 0,count_r = 0;
    if(length >= 125 && flag_cross != 1 && flag_cross != 2&& flag_l_cir != 1&& flag_r_cir != 1 && flag_stop !=1) //停车优先级大于 障碍物
    {
         for(i = 10; i < 110; i ++ )
         {
               if(abs(right_bor[i] - right_bor[i + 3]) > 5)
               {
                  count_r ++;

               }
               if(abs(left_bor[i] - left_bor[i + 3] )> 5)
               {
                  count_l ++;

               }

         }
         if(count_l >= 6)
         {
             flag_bar = 1;

         }
         if(count_r >= 7)
         {
             flag_bar = 2;
         }

    }

}

/****************************END*****************************************/


//自己写的开方函数
uint8 My_Sqrt(int16 x)
{
    uint8 ans=0, p=0x80;
    while(p!=0)
    {
        ans += p;
        if(ans*ans>x)
        {
            ans -= p;
        }
        p = (uint8)(p/2);
    }
    return (ans);
}
/****************************END*****************************************/



int8 flag_r = 0;
int8 flag_l = 0;
/*
函数名称：void curve_judge(void)
功能说明：左右转道路判断
参数说明：无
函数返回：无
修改时间：2024年2月27日
备    注：无
*/
void curve_judge(void)
{
    uint8 i = 0;
    flag_l = 0;
    flag_r = 0;

    if(length < 80 && flag_l_cir != 1 && flag_r_cir != 1 && flag_cross != 1 && flag_cross != 2)
        {
        for(i = 0; i < image_h-1-50; i ++)
        {
            if(center_line[i] - center_line[i+50]>10)
            {
                flag_r ++;
            }
            else if(center_line[i] - center_line[i+50]<-10)
            {
                flag_l ++;
            }
        }
       if(flag_l > 20)//left弯道
       {
           lose_location_test(1, 125, 60);
           if(lose_num > 40)
               left_right_flag=1;

       }
       if(flag_r>20)//right弯道
           {
               lose_location_test(2, 125, 60);
               if(lose_num > 40)
                   left_right_flag=2;
           }
        }
    else//直行
    {
        left_right_flag=0;
    }
}
/*

这里是起点（0.0）***************——>*************x值最大
************************************************************
************************************************************
************************************************************
************************************************************
******************假如这是一副图像*************************
***********************************************************
***********************************************************
***********************************************************
***********************************************************
***********************************************************
***********************************************************
y值最大*******************************************(188.120)

*/


