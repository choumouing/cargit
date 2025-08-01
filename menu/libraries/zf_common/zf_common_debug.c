/*********************************************************************************************************************
* MM32F327X-G9P Opensourec Library 即（MM32F327X-G9P 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
* 
* 本文件是 MM32F327X-G9P 开源库的一部分
* 
* MM32F327X-G9P 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
* 
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
* 
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
* 
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
* 
* 文件名称          zf_common_debug
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          IAR 8.32.4 or MDK 5.37
* 适用平台          MM32F327X_G9P
* 店铺链接          https://seekfree.taobao.com/
* 
* 修改记录
* 日期              作者                备注
* 2022-08-10        Teternal            first version
********************************************************************************************************************/

#include "zf_common_fifo.h"
#include "zf_common_interrupt.h"

#include "zf_driver_gpio.h"
#include "zf_driver_pwm.h"
#include "zf_driver_uart.h"

#include "zf_common_debug.h"

#if DEBUG_UART_USE_INTERRUPT                                                    // 如果启用 debug uart 接收中断
uint8                       debug_uart_buffer[DEBUG_RING_BUFFER_LEN];           // 数据存放数组
uint8                       debug_uart_data = 0;
fifo_struct                 debug_uart_fifo;
#endif

static debug_output_struct  debug_output_info;
static volatile uint8       zf_debug_init_flag = 0;
static volatile uint8       zf_debug_assert_enable = 1;

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     debug 软延时函数 在 120MHz 下是一秒多的时间 各单片机需要根据各自时钟试验
// 参数说明     void
// 返回参数     void
// 使用示例     debug_delay();
// 备注信息     本函数在文件内部调用 用户不用关注 也不可修改
//-------------------------------------------------------------------------------------------------------------------
static void debug_delay (void)
{
    vuint32 loop_1 = 0, loop_2 = 0;
    for(loop_1 = 0; 0xFF >= loop_1; loop_1 ++)
    {
        for(loop_2 = 0; 0xFFFF >= loop_2; loop_2 ++)
        {
            __NOP();
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     debug 保护处理 主要是防止断言后出现信号维持而导致硬件失控
// 参数说明     void
// 返回参数     void
// 使用示例     debug_protective_handler();
// 备注信息     本函数在文件内部调用 用户不用关注 也不可修改
//-------------------------------------------------------------------------------------------------------------------
static void debug_protective_handler (void)
{
    const pwm_channel_enum pin_list[43] = 
    {
    TIM1_PWM_CH1_A8 , TIM1_PWM_CH1_E9 , TIM1_PWM_CH2_A9 , TIM1_PWM_CH2_E11,
    TIM1_PWM_CH3_A10, TIM1_PWM_CH3_E13, TIM1_PWM_CH4_A11, TIM1_PWM_CH4_E14,
    TIM2_PWM_CH1_A0 , TIM2_PWM_CH1_A5 , TIM2_PWM_CH1_A15, TIM2_PWM_CH2_A1 ,
    TIM2_PWM_CH2_B3 , TIM2_PWM_CH3_A2 , TIM2_PWM_CH3_B10, TIM2_PWM_CH4_A3 ,
    TIM2_PWM_CH4_B11, TIM3_PWM_CH1_A6 , TIM3_PWM_CH1_B4 , TIM3_PWM_CH1_C6 ,
    TIM3_PWM_CH2_A7 , TIM3_PWM_CH2_B5 , TIM3_PWM_CH2_C7 , TIM3_PWM_CH3_B0 ,
    TIM3_PWM_CH3_C8 , TIM3_PWM_CH4_B1 , TIM3_PWM_CH4_C9 , TIM4_PWM_CH1_B6 ,
    TIM4_PWM_CH1_D12, TIM4_PWM_CH2_B7 , TIM4_PWM_CH2_D13, TIM4_PWM_CH3_B8 ,
    TIM4_PWM_CH3_D14, TIM4_PWM_CH4_B9 , TIM4_PWM_CH4_D15, TIM5_PWM_CH1_A0 ,
    TIM5_PWM_CH2_A1 , TIM5_PWM_CH3_A2 , TIM5_PWM_CH4_A3 , TIM8_PWM_CH1_C6 ,
    TIM8_PWM_CH2_C7 , TIM8_PWM_CH3_C8 , TIM8_PWM_CH4_C9 ,
    };

    uint32 loop_temp = 0;

    for(loop_temp = 0; 43 > loop_temp; loop_temp ++)
    {
        uint8 gpio_group_index = (((pin_list[loop_temp] & 0xE0) >> 5));
        uint8 gpio_bit_index = ((pin_list[loop_temp] & 0x1F));
        uint32 gpio_af_state = 0;
        if(7 >= gpio_bit_index)
        {
            gpio_af_state = (gpio_group[gpio_group_index]->AFRL);
            gpio_af_state = (gpio_af_state & (GPIO_AFRL_AFRY(0xF) << (gpio_bit_index * 4)));
            gpio_af_state = (gpio_af_state >> (gpio_bit_index * 4));
        }
        else
        {
            gpio_af_state = (gpio_group[gpio_group_index]->AFRH);
            gpio_af_state = (gpio_af_state & (GPIO_AFRH_AFRY(0xF) << ((gpio_bit_index - 8) * 4)));
            gpio_af_state = (gpio_af_state >> ((gpio_bit_index - 8) * 4));
        }
        if(gpio_af_state == ((pin_list[loop_temp] & 0xF00) >> 8))
        {
            gpio_init((gpio_pin_enum)((pin_list[loop_temp] & 0xFF)), GPO, 0, GPO_PUSH_PULL);
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     debug 串口输出接口
// 参数说明     *str        需要输出的字符串
// 返回参数     void
// 使用示例     debug_uart_str_output("Log message");
// 备注信息     本函数在文件内部调用 用户不用关注 也不可修改
//-------------------------------------------------------------------------------------------------------------------
static void debug_uart_str_output (const char *str)
{
    uart_write_string(DEBUG_UART_INDEX, str);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     debug 输出接口
// 参数说明     *type       log 类型
// 参数说明     *file       文件名
// 参数说明     line        目标行数
// 参数说明     *str        信息
// 返回参数     void
// 使用示例     debug_output("Log message", file, line, str);
// 备注信息     本函数在文件内部调用 用户不用关注 也不可修改
//-------------------------------------------------------------------------------------------------------------------
static void debug_output (char *type, char *file, int line, char *str)
{
    char *file_str;
    vuint16 i = 0, j = 0;
    vint16 len_origin = 0;
    vint16 show_len = 0;
    vint16 show_line_index = 0;
    len_origin = strlen(file);

    char output_buffer[256];
    char file_path_buffer[64];

    if(debug_output_info.type_index)
    {
        debug_output_info.output_screen_clear();
    }

    if(zf_debug_init_flag)
    {
        if(debug_output_info.type_index)
        {
            // 需要分行将文件的路径和行数输出
            // <不输出完整路径 只输出一级目录 例如 src/main.c>
            // 输出 line : xxxx
            debug_output_info.output_screen(0, show_line_index ++, type);

            file_str = file;
            len_origin = strlen(file);
            show_len = (debug_output_info.display_x_max / debug_output_info.font_x_size);

            while('\0' != *file_str ++);

            // 只取一级目录 如果文件放在盘符根目录 或者 MDK 的工程根目录 就会直接输出当前目录
            for(j = 0; (2 > j) && (0 <= len_origin); len_origin --)             // 查找两个 '/'
            {
                file_str --;
                if(('/' == *file_str) || (0x5C == *file_str))
                {
                    j ++;
                }
            }

            // 文件路径保存到数组中
            if(0 <= len_origin)
            {
                file_str ++;
                sprintf(output_buffer, "file: %s", file_str);
            }
            else
            {
                if(0 == j)
                {
                    sprintf(output_buffer, "file: mdk/%s", file_str);
                }
                else
                {
                    sprintf(output_buffer, "file: %s", file_str);
                }
            }

            // 屏幕显示路径
            for(i = 0; i < ((strlen(output_buffer) / show_len) + 1); i ++)
            {
                for(j = 0; j < show_len; j ++)
                {
                    if(strlen(output_buffer) < (j + i * show_len))
                    {
                        break;
                    }
                    file_path_buffer[j] = output_buffer[j + i * show_len];
                }
                
                file_path_buffer[j] = '\0';                                     // 末尾添加\0
                
                debug_output_info.output_screen(0, debug_output_info.font_y_size * show_line_index ++, file_path_buffer);
            }

            // 屏幕显示行号
            sprintf(output_buffer, "line: %d", line);
            debug_output_info.output_screen(0, debug_output_info.font_y_size * show_line_index ++, output_buffer);

            // 屏幕显示 Log 如果有的话
            if(NULL != str)
            {
                for(i = 0; i < ((strlen(str) / show_len) + 1); i ++)
                {
                    for(j = 0; j < show_len; j ++)
                    {
                        if(strlen(str) < (j + i * show_len))
                        {
                            break;
                        }
                        file_path_buffer[j] = str[j + i * show_len];
                    }
                    
                    file_path_buffer[j] = '\0';                                 // 末尾添加\0
                    
                    debug_output_info.output_screen(0, debug_output_info.font_y_size * show_line_index ++, file_path_buffer);
                }
            }
        }
        else
        {
            char output_buffer[256];
            memset(output_buffer, 0, 256);
            debug_output_info.output_uart(type);
            if(NULL != str)
            {
                sprintf(output_buffer, "\r\nfile %s line %d: %s.\r\n", file, line, str);
            }
            else
            {
                sprintf(output_buffer, "\r\nfile %s line %d.\r\n", file, line);
            }
            debug_output_info.output_uart(output_buffer);
        }
    }
}

#if DEBUG_UART_USE_INTERRUPT                                                    // 条件编译 只有在启用串口中断才编译

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     读取 debug 环形缓冲区数据
// 参数说明     *data       读出数据存放的数组指针
// 返回参数     uint32      读出数据的实际长度
// 使用示例     uint8 data[64]; uint32 len = debug_read_ring_buffer(data);
// 备注信息     本函数需要开启 DEBUG_UART_USE_INTERRUPT 宏定义才可使用
//-------------------------------------------------------------------------------------------------------------------
uint32 debug_read_ring_buffer (uint8 *data)
{
    uint32 data_len = sizeof(data);
    fifo_read_buffer(&debug_uart_fifo, data, &data_len, FIFO_READ_AND_CLEAN);
    return data_len;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     debug 串口中断处理函数 isr.c 中对应串口中断服务函数调用
// 参数说明     void
// 返回参数     void
// 使用示例     debug_interrupr_handler();
// 备注信息     本函数需要开启 DEBUG_UART_USE_INTERRUPT 宏定义才可使用
//              并且本函数默认放置在 UART1 的串口接收中断处理处
//-------------------------------------------------------------------------------------------------------------------
void debug_interrupr_handler (void)
{
    if(zf_debug_init_flag)
    {
        uart_query_byte(DEBUG_UART_INDEX, &debug_uart_data);                    // 读取串口数据
        fifo_write_buffer(&debug_uart_fifo, &debug_uart_data, 1);               // 存入 FIFO
    }
}

#endif

//-------------------------------------------------------------------------     // printf 重定向 此部分不允许用户更改
#if defined(__ICCARM__)
#define PUTCHAR_PROTOTYPE int32_t fputc (int32_t ch, FILE *f)
#define GETCHAR_PROTOTYPE int32_t fgetc (FILE *f)
#elif defined(__GNUC__)
#define PUTCHAR_PROTOTYPE int32_t __io_putchar (int32_t ch)
#define GETCHAR_PROTOTYPE int32_t __io_getchar ()
#endif

#if defined(__ICCARM__)
PUTCHAR_PROTOTYPE
{
    if(zf_debug_init_flag)
    {
        uart_write_byte(DEBUG_UART_INDEX, (ch & 0xFF));
    }
    return ch;
}

GETCHAR_PROTOTYPE
{
    int data = 0;
    if(zf_debug_init_flag)
    {
        data = uart_read_byte(DEBUG_UART_INDEX);
    }
    return data;
}
#else
int32_t fputc (int32_t ch, FILE* f)
{
    if(zf_debug_init_flag)
    {
        uart_write_byte(DEBUG_UART_INDEX, (ch & 0xFF));
    }
    return ch;
}

int fgetc(FILE *f)
{
    int data = 0;
    if(zf_debug_init_flag)
    {
        data = uart_read_byte(DEBUG_UART_INDEX);
    }
    return data;
}
#endif
//-------------------------------------------------------------------------     // printf 重定向 此部分不允许用户更改

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     启用断言
// 参数说明     void
// 返回参数     void
// 使用示例     debug_assert_enable();
// 备注信息     断言默认开启 建议开启断言
//-------------------------------------------------------------------------------------------------------------------
void debug_assert_enable (void)
{
    zf_debug_assert_enable = 1;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     禁用断言
// 参数说明     void
// 返回参数     void
// 使用示例     debug_assert_disable();
// 备注信息     断言默认开启 不建议禁用断言
//-------------------------------------------------------------------------------------------------------------------
void debug_assert_disable (void)
{
    zf_debug_assert_enable = 0;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     debug 断言处理函数
// 参数说明     pass        判断是否触发断言
// 参数说明     *file       文件名
// 参数说明     line        目标行数
// 返回参数     void
// 使用示例     zf_assert(0);
// 备注信息     这个函数不是直接调用的 此部分不允许用户更改
//              使用 zf_commmon_debug.h 中的 zf_assert(x) 接口
//-------------------------------------------------------------------------------------------------------------------
void debug_assert_handler (uint8 pass, char *file, int line)
{
    do
    {
        if(pass || !zf_debug_assert_enable)
        {
            break;
        }

        static uint8 assert_nest_index = 0;

        if(0 != assert_nest_index)
        {
            while(1);
        }
        assert_nest_index ++;

        interrupt_global_disable();
        debug_protective_handler();

        while(1)
        {
            // 如果代码跳转到这里停住了
            // 一般你的函数参数传递出错了
            // 或者你自己调用的 zf_assert(x) 接口处报错了

            // 如果调用了 debug_init 初始化了 log 输出
            // 就在对应串口输出去查看是哪个文件的哪一行报错

            // 如果没有初始化 debug
            // 那就看看这个 file 的字符串值和 line 的行数
            // 那代表报错的文件路径名称和对应报错行数

            // 再去调试看看是为什么参数出错

            debug_output("Assert error", file, line, NULL);
            debug_delay();
        }
    }while(0);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     debug 调试信息处理函数
// 参数说明     pass        判断是否触发断言
// 参数说明     *str        输出的信息
// 参数说明     *file       文件名
// 参数说明     line        目标行数
// 返回参数     void
// 使用示例     zf_log(0, "Log Message");
// 备注信息     这个函数不是直接调用的 此部分不允许用户更改
//              使用 zf_commmon_debug.h 中的 zf_log(x, str) 接口
//-------------------------------------------------------------------------------------------------------------------
void debug_log_handler (uint8 pass, char *str, char *file, int line)
{
    do
    {
        if(pass)
        {
            break;
        }
        if(zf_debug_init_flag)
        {
            debug_output("Log message", file, line, str);
//            printf("Log message from %s line %d :\"%s\".\r\n", file, line, str);
        }
    }while(0);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     debug 输出绑定信息初始化
// 参数说明     *info       debug 输出的信息结构体
// 返回参数     void
// 使用示例     debug_output_struct_init(info);
// 备注信息     这个函数一般不由用户调用
//-------------------------------------------------------------------------------------------------------------------
void debug_output_struct_init (debug_output_struct *info)
{
    info->type_index            = 0;

    info->display_x_max         = 0xFFFF;
    info->display_y_max         = 0xFFFF;

    info->font_x_size           = 0xFF;
    info->font_y_size           = 0xFF;

    info->output_uart           = NULL;
    info->output_screen         = NULL;
    info->output_screen_clear   = NULL;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     debug 输出绑定初始化
// 参数说明     *info       debug 输出的信息结构体
// 返回参数     void
// 使用示例     debug_output_init(info);
// 备注信息     这个函数一般不由用户调用
//-------------------------------------------------------------------------------------------------------------------
void debug_output_init (debug_output_struct *info)
{
    debug_output_info.type_index            = info->type_index;

    debug_output_info.display_x_max         = info->display_x_max;
    debug_output_info.display_y_max         = info->display_y_max;

    debug_output_info.font_x_size           = info->font_x_size;
    debug_output_info.font_y_size           = info->font_y_size;
    
    debug_output_info.output_uart           = info->output_uart;
    debug_output_info.output_screen         = info->output_screen;
    debug_output_info.output_screen_clear   = info->output_screen_clear;

    zf_debug_init_flag = 1;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     debug 串口初始化
// 参数说明     void
// 返回参数     void
// 使用示例     debug_init();
// 备注信息     开源库示例默认调用 但默认禁用中断接收
//-------------------------------------------------------------------------------------------------------------------
void debug_init (void)
{
    debug_output_struct info;
    debug_output_struct_init(&info);
    info.output_uart = debug_uart_str_output;
    debug_output_init(&info);

    uart_init(
        DEBUG_UART_INDEX,                                                       // 在 zf_common_debug.h 中查看对应值
        DEBUG_UART_BAUDRATE,                                                    // 在 zf_common_debug.h 中查看对应值
        DEBUG_UART_TX_PIN,                                                      // 在 zf_common_debug.h 中查看对应值
        DEBUG_UART_RX_PIN);                                                     // 在 zf_common_debug.h 中查看对应值

#if DEBUG_UART_USE_INTERRUPT                                                    // 条件编译 只有在启用串口中断才编译
    fifo_init(&debug_uart_fifo, FIFO_DATA_8BIT, debug_uart_buffer, DEBUG_RING_BUFFER_LEN);
    uart_rx_interrupt(DEBUG_UART_INDEX, 1);                                     // 使能对应串口接收中断
#endif
}

