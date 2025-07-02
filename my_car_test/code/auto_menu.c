#include "zf_common_headfile.h"
#include "auto_menu.h"
#include "key.h"

//按键信号量及按键反馈信号量
#ifdef  MENU_USE_RTT
extern rt_sem_t key1_sem;
extern rt_sem_t key2_sem;
extern rt_sem_t key3_sem;
extern rt_sem_t key4_sem;
extern rt_sem_t button_feedback_sem;
#endif
/*-------------------按键--------------------
                    button1返回
                    button2确定
                    button3下翻
                    button4上翻
---------------------------------------------*/
uint8 button1=0,button2=0,button3=0,button4=0;
uint8 first_in_page_flag = 0;
uint8 is_clear_flag=0;

uint8* p_index_xy_dad,*p_index_xy_son;

static menu_unit *p_unit	 	=NULL;//单元指针
static menu_unit *p_unit_last 	=NULL;//上一次的单元指针

static menu_unit *P_dad_head 	= NULL;

uint8 DAD_NUM=1;

#ifdef USE_STATIC_MENU
menu_unit   my_menu_unit[MEM_SIZE];
param_set   my_param_set[MEM_SIZE];
uint8       my_index[MEM_SIZE*2];
static int  static_cnt=0;
#endif

//函数数组指针
void (*current_operation_menu)(void);

void dad_name_init(){
	char* p = NULL;
	p = (char*)malloc(STR_LEN_MAX);
	memset(p,0,STR_LEN_MAX);
	strcpy(p,"Page ");
	DAD_NUM = IND1+1;
	for(uint8 i=0;i<DAD_NUM;i++){
		if(i<10){
			p[5] = '0'+ i;
			strcpy(P_dad_head->name,p);
		}else{
			p[5] = '0'+ i/10;
			p[6] = '0'+ i%10;
			strcpy(P_dad_head->name,p);
		}
		P_dad_head = P_dad_head->up;
	}
	free(p);
}

//子菜单单元连接
void son_link(menu_unit *p_1,menu_unit *p_2,menu_unit *dad)
{
		p_1->up 	=   p_2;
		p_2->down  	=   p_1;
		p_1->enter	=	p_1;
		p_2->enter	=	p_2;
		p_1->back	=	dad;
		p_2->back	=	dad;
}

//父菜单单元连接
void dad_link(menu_unit *p_1,menu_unit *p_2,menu_unit *son)
{	
	p_1-> up	= p_2;
	p_2-> down  = p_1;
	p_1-> back	= p_1;
	p_2-> back	= p_2;
	if(son!=NULL)
		p_1->enter	=	son;
}
void unit_default(menu_unit *p,uint8 ind)
{
	p->type_t = USE_FUN;
	p->current_operation = NULL_FUN;
	p->m_index[0]=ind;
	p->m_index[1]=255;
}	
void unit_index_init(menu_unit *_p1,uint8 ind_0,uint8 ind_1){
	_p1->m_index[0]=ind_0;
	_p1->m_index[1]=ind_1;	
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      待修改参数配置
//  @param      p_param     	   		待修改参数指针
//  @param      t     	        		待修改参数数据类型
//  @param      delta       	    	按键每触发一次，参数所修改的大小
//  @param      num     	  		 		参数的整数显示位数
//  @param      point_num     	 		参数的小数显示位数   
//  @param      t1     	 						单元种类   
//  @param      _name[STR_LEN_MAX]  参数名字
//  @return     void
//  Sample usage:               			unit_param_set(&param_test,TYPE_FLOAT,0.001,1,4,"par_test");
//-------------------------------------------------------------------------------------------------------------------
void unit_param_set(void* p_param,type_value t,float delta,uint8 num,uint8 point_num,unit_type t1,const char _name[STR_LEN_MAX]){
	static menu_unit *p_middle = NULL;
	menu_unit *p1 = NULL,*p2 = NULL;
	param_set *p1_par = NULL,*p2_par = NULL;
	static menu_unit *dad;
	
#ifdef USE_STATIC_MENU
	p1 = my_menu_unit+static_cnt;
	p1_par = my_param_set+static_cnt;
    static_cnt++;
	p2 = my_menu_unit+static_cnt;
	p2_par = my_param_set+static_cnt;
    static_cnt++;
#else
	p1 = malloc(sizeof(menu_unit));
	p1_par = malloc(sizeof(param_set));

	p2 = malloc(sizeof(menu_unit));
	p2_par = malloc(sizeof(param_set));
#endif

	if(P_dad_head==NULL){
#ifdef USE_STATIC_MENU
	    dad = my_menu_unit+static_cnt;
	    static_cnt++;
#else
		dad = malloc(sizeof(menu_unit));
#endif
		P_dad_head = dad;
		unit_default(dad,0);
		p_unit = dad;
		p_unit_last = NULL;
		unit_index_init(p1,0,0);
		unit_index_init(p2,0,1);
		dad_link(dad,dad,p1);		
		son_link(p1,p2,dad);
		son_link(p2,p1,dad);
	}else{
		if(IND2>=SON_NUM-2){
#ifdef USE_STATIC_MENU
            dad = my_menu_unit+static_cnt;
            static_cnt++;
#else
			dad = malloc(sizeof(menu_unit));
#endif
			unit_default(dad,IND1+1);
			unit_index_init(p1,IND1+1,0);
			unit_index_init(p2,IND1+1,1);
			dad_link(P_dad_head->down,dad,NULL);		
			dad_link(dad,P_dad_head,p1);		
			son_link(p1,p2,dad);
			son_link(p2,p1,dad);
		}else{
			unit_index_init(p1,IND1,IND2+1);
			unit_index_init(p2,IND1,IND2+2);		
			son_link(p_middle,p1,dad);
			son_link(p1,p2,dad);
			son_link(p2,SON_BEGIN_UNIT,dad);
		}
	}
	p_middle = p2;
	p1->par_set = p1_par;
	p1->par_set->p_par=p_param;
	p1->par_set->par_type=t;
	p1->par_set->delta=delta;
	p1->par_set->num=num;
	p1->par_set->point_num=point_num;
	p1->type_t=t1;
	memset(p1->name,0,STR_LEN_MAX);
	strcpy(p1->name, _name);
	p1->name[strlen(_name)]='+';

	p2->par_set = p2_par;
	p2->par_set->p_par=p_param;
	p2->par_set->par_type=t;
	p2->par_set->delta=-delta;
	p2->par_set->num=num;
	p2->par_set->point_num=point_num;
	p2->type_t=t1;
	memset(p2->name,0,STR_LEN_MAX);
	strcpy(p2->name, _name);
	p2->name[strlen(_name)]='-';

}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      初始化函数菜单单元
//  @param      fun     	   				需触发的函数指针
//  @param      _name       	  		显示的字符串  
//  @return     void
//  Sample usage:               		fun_init(Flash_Read,"flash_read");
//-------------------------------------------------------------------------------------------------------------------
void fun_init(void* fun,const char* _name)
{
	menu_unit *p2 = NULL;
	menu_unit *p1 = NULL;
	
#ifdef USE_STATIC_MENU
    p1 = my_menu_unit+static_cnt;
	static_cnt++;
#else
	p1 = malloc(sizeof(menu_unit));
#endif
	p1->par_set = NULL;
	
	if(IND2>=SON_NUM-1){
#ifdef USE_STATIC_MENU
	    p2 = my_menu_unit+static_cnt;
	    static_cnt++;
#else
		p2 = malloc(sizeof(menu_unit));
#endif

		unit_default(p2,IND1+1);
		unit_index_init(p1,IND1+1,0);
		dad_link(P_dad_head->down,p2,NULL);	
		dad_link(p2,P_dad_head,p1);
		son_link(p1,p1,p2);
	}
	else{
		unit_index_init(p1,IND1,IND2+1);
		son_link(SON_END_UNIT,p1,P_dad_head->down);
		son_link(p1,SON_BEGIN_UNIT,P_dad_head->down);
	}
	
	p1->current_operation = fun;

	for(uint8 i=0;i<STR_LEN_MAX;i++)p1->name[i]=0;
	strcpy(p1->name, _name);
	p1->type_t = USE_FUN;
}

//字符串索引初始化
void index_xy_init()
{
#ifdef USE_STATIC_MENU
    p_index_xy_dad = my_index;
    p_index_xy_son = my_index+DAD_NUM*2;
#else
	p_index_xy_dad =(uint8*)malloc(sizeof(uint8)*DAD_NUM*2);
	p_index_xy_son =(uint8*)malloc(sizeof(uint8)*SON_NUM*2);
#endif
	uint8 half;
	half = (DAD_NUM+1)/2;
	for(uint8 i=0;i<half;i++){
		DAD_INDEX(i,0)=DIS_X*0;
		DAD_INDEX(i,1)=DIS_Y*i;
	}
	for(uint8 i=half;i<DAD_NUM;i++){
		DAD_INDEX(i,0)=DIS_X*1;
		DAD_INDEX(i,1)=DIS_Y*(i-half);
	}
	for(uint8 i=0;i<SON_NUM;i++){
		SON_INDEX(i,0)=DIS_X*0;
		SON_INDEX(i,1)=DIS_Y*i;
	}
}


void flash_init_wz()
{
	//初始化需要保存的参数  按索引顺序初始化下去（注意：在data_flash.h中需声明参数）
	#if AUTO_READ
	menu_unit* p;
	p = P_dad_head->enter;
	while(1){
		if(p->m_index[1]<p->up->m_index[1]){
			flash_unit_init(p->par_set.p_par,p->par_set.par_type);
			p = p->up;
		}else{
			flash_unit_init(p->par_set.p_par,p->par_set.par_type);
			p = p->back->up->enter;
		}
		if(p->par_set.type_t==USE_FUN)
			break;
	}
	#endif
	//index初始化，不用管
	//上电读取参数
	#if AUTO_READ
	flash_index_init();
	flash_read();
	#endif
}

//是否清空屏幕
uint8 is_menu_clear()
{
	return (((p_unit->back)!=(p_unit))&&button1)||(((p_unit->enter)!=(p_unit))&&button2);
}
uint8 begin_menu_flag=1;
//父级菜单显示
void center_menu()
{
	uint8 index = p_unit->m_index[0];

	if(first_in_page_flag)
		showstr(DAD_INDEX(index,0),DAD_INDEX(index,1),MOUSE_LOOK);
	
	if(button3||button4){
		if(index==0){
			showstr(DAD_INDEX(DAD_NUM-1,0),DAD_INDEX(DAD_NUM-1,1)," ");
			showstr(DAD_INDEX(0,0) ,DAD_INDEX(0,1),MOUSE_LOOK);
			showstr(DAD_INDEX(1,0) ,DAD_INDEX(1,1)," ");
		}else if(index==DAD_NUM-1){
			showstr(DAD_INDEX(DAD_NUM-2,0),DAD_INDEX(DAD_NUM-2,1)," ");
			showstr(DAD_INDEX(DAD_NUM-1,0),DAD_INDEX(DAD_NUM-1,1),MOUSE_LOOK);
			showstr(DAD_INDEX(0,0) ,DAD_INDEX(0,1) ," ");			
		}else{
			showstr(DAD_INDEX(index-1,0),DAD_INDEX(index-1,1)," ");
			showstr(DAD_INDEX(index  ,0),DAD_INDEX(index  ,1),MOUSE_LOOK);
			showstr(DAD_INDEX(index+1,0),DAD_INDEX(index+1,1)," ");					
		}
	}else if((is_clear_flag==1&&(button1))||(begin_menu_flag)){
		menu_unit* p = NULL;
		p = p_unit;
		for(uint8 i=0;i<DAD_NUM;i++){
			showstr(DAD_INDEX(p->m_index[0],0)+MOUSE_DIS,DAD_INDEX(p->m_index[0],1),p->name);
			p = p->up;
		}
		begin_menu_flag = 0;
	}
}

//子级菜单显示
void assist_menu()
{
	uint8 index = p_unit->m_index[1];
	if(first_in_page_flag)
		showstr(SON_INDEX(index,0),SON_INDEX(index,1),MOUSE_LOOK);
	
	if(button3||button4){
		if(index==0){
			showstr(SON_INDEX(p_unit->down->m_index[1],0),SON_INDEX(p_unit->down->m_index[1],1)," ");
			showstr(SON_INDEX(0,0) ,SON_INDEX(0,1),MOUSE_LOOK);
			showstr(SON_INDEX(1,0) ,SON_INDEX(1,1)," ");
		}else if(index==p_unit->back->enter->down->m_index[1]){
			showstr(SON_INDEX(index-1,0),SON_INDEX(index-1,1)," ");
			showstr(SON_INDEX(index,0),SON_INDEX(index,1),MOUSE_LOOK);
			showstr(SON_INDEX(0,0) ,SON_INDEX(0,1) ," ");			
		}else{
			showstr(SON_INDEX(index-1,0),SON_INDEX(index-1,1)," ");
			showstr(SON_INDEX(index  ,0),SON_INDEX(index  ,1),MOUSE_LOOK);
			showstr(SON_INDEX(index+1,0),SON_INDEX(index+1,1)," ");					
		}
	}else if(is_clear_flag==1&&(button2)){
		menu_unit* p = NULL;
		p = p_unit;
		for(uint8 i=0;i<SON_NUM;i++){
			showstr(SON_INDEX(p->m_index[1],0)+MOUSE_DIS,SON_INDEX(p->m_index[1],1),p->name);
			p = p->up;
		}
	}				 
}
	
//-------------------------------------------------------------------------------------------------------------------
// @brief		修改参数大小
// @param		param  	 	参数
// @return	void
// Sample usage:				change_value(TYPE_FLOAT,0.01,&P_speed);button2按下时修改P_speed的值为P_speed+0.01
// attention	 :				注意待修改参数的类型（TYPE_DOUBLE,TYPE_FLOAT,TYPE_INT...其他类型可自行添加）
//-------------------------------------------------------------------------------------------------------------------
void change_value(param_set* param)
{
    uint8 type=param->par_type;float delta_x=param->delta;void* value=param->p_par;
    uint8 num=param->num;uint8 point_num=param->point_num;
	static uint8 last_index = 0;
	uint8 is_show_num = (p_unit_last->par_set==NULL?1:p_unit_last->par_set->p_par!=p_unit->par_set->p_par);
	if(p_unit->par_set->p_par!=NULL){
		if(type==TYPE_FLOAT){
			float *p_value;
			p_value = (float*)(value);
			if(IS_OK)
			{
				*p_value +=	delta_x;
				showfloat(0,(SON_NUM+1)*DIS_Y,*p_value,num,point_num);
			}
			if(is_show_num){
				showstr(0,(SON_NUM+1)*DIS_Y,"            ");
				showfloat(0,(SON_NUM+1)*DIS_Y,*p_value,num,point_num);
			}
		}else if(type==TYPE_DOUBLE){
			double *p_value;
			p_value = (double*)(value);
			if(IS_OK)
			{
				*p_value +=	(double)delta_x;
				showfloat(0,(SON_NUM+1)*DIS_Y,*p_value,num,point_num);
			}
			if(is_show_num){
				showstr(0,(SON_NUM+1)*DIS_Y,"            ");
				showfloat(0,(SON_NUM+1)*DIS_Y,*p_value,num,point_num);
			}
		}else if(type==TYPE_INT){
			int *p_value;
			p_value = (int*)(value);
			if(IS_OK)
			{
				*p_value +=	(int)delta_x;
				showint32(0,(SON_NUM+1)*DIS_Y,*p_value,num);
			}
			if(is_show_num){
				showstr(0,(SON_NUM+1)*DIS_Y,"        ");
				showint32(0,(SON_NUM+1)*DIS_Y,*p_value,num);
			}			
		}else if(type==TYPE_UINT16){
			uint16 *p_value;
			p_value = (uint16*)(value);
			if(IS_OK)
			{
				*p_value +=	(int)delta_x;
				showuint16(0,(SON_NUM+1)*DIS_Y,*p_value,num);
			}
			if(is_show_num){
				showstr(0,(SON_NUM+1)*DIS_Y,"         ");
				showuint16(0,(SON_NUM+1)*DIS_Y,*p_value,num);
			}			
		}else if(type==TYPE_UINT32){
			uint32 *p_value;
			p_value = (uint32*)(value);
			if(IS_OK)
			{
				*p_value +=	(int)delta_x;
				showuint32(0,(SON_NUM+1)*DIS_Y,*p_value,num);
			}
			if(is_show_num){
				showstr(0,(SON_NUM+1)*DIS_Y,"         ");
				showuint32(0,(SON_NUM+1)*DIS_Y,*p_value,num);
			}			
		}
	}
	last_index = p_unit->m_index[1];	
}

//是否为第一次进入新页面
void is_first_in_page()
{
	first_in_page_flag = (p_unit_last!=p_unit)&&(button1||button2);
}

void show_menu()
{
	if(p_unit->m_index[1]==255)
		center_menu();//显示父菜单
	else
		assist_menu();//显示子菜单
}

//效果函数
void fun_menu()
{
	if(p_unit->type_t==NORMAL_PAR||p_unit->type_t==PID_PAR){
		change_value(p_unit->par_set);
	}else{
		current_operation_menu = p_unit->current_operation;		//函数指针	
		(*current_operation_menu)();
	}
}
//显示进程钩子函数
void show_process(void *parameter)
{
    #ifdef  MENU_USE_RTT
    while(1)
	{	
		//测试		
		button1=(RT_EOK==rt_sem_take(key1_sem,RT_WAITING_NO));
		button2=(RT_EOK==rt_sem_take(key2_sem,RT_WAITING_NO));
		button3=(RT_EOK==rt_sem_take(key3_sem,RT_WAITING_NO));
		button4=(RT_EOK==rt_sem_take(key4_sem,RT_WAITING_NO));

		is_clear_flag = is_menu_clear();
		
		if(button1||button2||button3||button4){
			rt_sem_release(button_feedback_sem);
		}
			
		if(is_clear_flag)
			clear_hhh(0,0,SCREEN_W,SON_NUM*16,IPS200_BGCOLOR);

		if			(button1==1)
			p_unit=p_unit->back;
		else	if(button2==1)
			p_unit=p_unit->enter;
		else	if(button3==1)
			p_unit=p_unit->up;
		else	if(button4==1)
			p_unit=p_unit->down;
	
		is_first_in_page();
		
		//显示函数
		show_menu();
		
		//效果函数
		fun_menu();
		
	    p_unit_last=p_unit;
		//线程挂起	
		rt_thread_mdelay(10);
	}
    #else
    button_entry(NULL);

    is_clear_flag = is_menu_clear();

    if(is_clear_flag)
        clear();

    if        (button1==1)
        p_unit=p_unit->back;
    else    if(button2==1)
        p_unit=p_unit->enter;
    else    if(button3==1)
        p_unit=p_unit->up;
    else    if(button4==1)
        p_unit=p_unit->down;

    is_first_in_page();

    //显示函数
    show_menu();

    //效果函数
    fun_menu();

    p_unit_last=p_unit;
    #endif
}

//菜单系统初始化(勿动顺序！)
void menu_init()
{
    /*---------------屏幕初始化----------------*/
    screen_init(IPS200_TYPE_SPI);

    /*---------------按键初始化----------------*/
    key_into();

    /*---------------待修改参数----------------*/
    UNIT_SET();

    /*---------------导入的回调函数----------------*/
    FUN_INIT();

    /*---------------菜单名字初始化----------------*/
    dad_name_init();

    /*---------------字符串索引初始化----------------*/
    index_xy_init();

    /*-----------------配置flash---------------*/
    #ifdef USE_FLASH
    flash_init_wz();
    #endif

    /*----------------菜单线程初始化----------------*/
    #ifdef  MENU_USE_RTT
    rt_thread_t tid;
    //创建显示线程
    tid = rt_thread_create("display", show_process, RT_NULL, 1024*2, 11, 5);
    //启动显示线程
    if(RT_NULL != tid)
    {
        rt_thread_startup(tid);
    }
    #endif
}

//更改夜间或白天模式
static uint16 IPS200_BGCOLOR = RGB565_WHITE;
void day_night(){
	if(IS_OK){
		if(IPS200_BGCOLOR==RGB565_WHITE){
		    IPS200_BGCOLOR = RGB565_BLACK;
		    ips200_set_color(RGB565_WHITE,RGB565_BLACK);
		    showstr(0,(SON_NUM+1)*16,"BLACK");
		}
		else if(IPS200_BGCOLOR==RGB565_BLACK){
		    IPS200_BGCOLOR = RGB565_WHITE;
            ips200_set_color(RGB565_BLACK,RGB565_WHITE);
			showstr(0,(SON_NUM+1)*16,"WHITE");
		}
	}
}

void rand_color(){
    if(IS_OK){
        uint16 color;
        color = rand()%(32768*2);
        ips200_set_color(color,~color);
        showstr(0,(SON_NUM+1)*16,"rand");
    }
}
//菜单空闲函数
void NULL_FUN(){

}

float test_a=1.1f;
int test_b=100;
double test_c=100;
uint16 test_d=20;
uint32 test_e=32;

void UNIT_SET(){
	//菜单单元调参参数初始化
    unit_param_set(&test_a,TYPE_FLOAT ,0.5  ,3  ,3,NORMAL_PAR,"test_a");
    unit_param_set(&test_b,TYPE_INT   ,2    ,6  ,0,NORMAL_PAR,"test_b");
    unit_param_set(&test_c,TYPE_DOUBLE,11.11,4  ,4,NORMAL_PAR,"test_c");
    unit_param_set(&test_d,TYPE_UINT16,1    ,6  ,0,NORMAL_PAR,"test_d");
    unit_param_set(&test_e,TYPE_UINT32,1    ,6  ,0,NORMAL_PAR,"test_e");
}

void FUN_INIT(){
	//菜单单元函数指针初始化
	fun_init(NULL_FUN	,"NULL_FUN1");
	fun_init(day_night	,"day_night");
	fun_init(rand_color	,"rand_color");
	fun_init(NULL_FUN	,"NULL_FUN2");
}
