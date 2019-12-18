#include<stdio.h>
#include<stdlib.h>
#define MAX_FLOOR_NUM 5
#define InitFloor 1
//电梯状态宏定义
#define GoingUp 1
#define GoingDown 2
#define Idle 0
#define Stop 3
#define Others 5
//各种动作所用时间宏定义
#define CloseTestTime 40  //关门测试时间，每个40个t测试依次，若无人进出则关门
#define CloseOpenTime 20   //电梯开关门时间
#define InOutTime 25    //人进出电梯时间
#define UpTime 55
#define DownTime 60
#define ToIdleTime 30  //电梯停靠时间
#define IdleTimeLimit 300  //若在某一层静止超过这个时间则回到一层候命
#define TimeLimit  10000  //该程序所模拟的电梯运行时间段不超过10000秒
//需用的数据结构的定义
/*
样例输入
第1位：1 3 500 200
第2位：4 1 500 100
第3位：5 2 1000 200
第4位：3 1 100 100
第5位：5 2 1000 10000

放弃等待的调试数据
1 2
5 1
*/
typedef struct People{
    int ID;
    int InFloor;
    int OutFloor;
    int GiveupTime;
    int CallDirection;
    struct People *next;  //用于不断生成每层的People链表，便于插入和删除
}People;
typedef struct Activity{
    int time;
    void (*action)(void);   //通过计时器建单链表，通过Time来按时间顺序执行
    struct Activity *next;  //用来不断生成事件的单链表
}Activity;
typedef struct Elevator{
    int ID;
    struct Elevator *next;
}Elevator;
//一些全局变量的的定义和赋初值，这些全局变量为各个函数所调用
int Floor=InitFloor;  //电梯的当前楼层号，初始在一楼等候
int Time=0;   //计时器所用时间，全局的时刻表示，初始为0
int State=0;  //电梯此时的状态，初始为静候Idle
int Count=0;  //到来的总人数
int LastDirection=1;  //电梯上一次的升降方向，1为上升，2为下降，初始必为上升
int CallUp[MAX_FLOOR_NUM]={0};
int CallDown[MAX_FLOOR_NUM]={0};
int CallCar[MAX_FLOOR_NUM]={0};
People WaitQueue[MAX_FLOOR_NUM];   //不同楼层建立起一个等待队列数组，每个楼层建立等待的人群单链表
Elevator elevator[MAX_FLOOR_NUM];   //电梯内的不同层的要出来的人的存储栈
Activity activity={0,NULL,NULL};
void timer(void); //计时器函数声明
void controller(void);  //控制器函数声明
void manager(void); //人员管理函数声明
void acter(int time,void(*action)(void));   //不同时刻的事件函数衔接函数声明
void ComePeople(void);  //读取来坐电梯的人的信息
int OtherFloor(void);   //当非当前层有升降请求时，通过判断上次升降方向来确定之后去往哪层
//电梯执行动作函数声明
void ToFirstIdle(void);
void OpenDoor(void);
void CloseDoor(void);
void Up(void);
void Down(void);
void In(void);
void Out(void);
void IfInOut(void);
void ToIdle(void);  //电梯停靠
int main(void)
{
    int i;  //对电梯栈和等待队列初始化
	for(i=0;i<MAX_FLOOR_NUM;i++)
	{
		elevator[i].next=NULL;
		WaitQueue[i].next=NULL;
		elevator[i].ID=0;
		WaitQueue[i].ID=WaitQueue[i].OutFloor=WaitQueue[i].GiveupTime=0;
	}
    ComePeople();
	timer();
}
void timer(void)
{
    Activity *temp=activity.next;
    while(1)
    {

        if(Time>=TimeLimit)  //程序结束点，电梯运行结束
            break;
        manager(); //人员流动
        controller();   //电梯动作控制
        temp=activity.next;
        if(temp==NULL)
            Time=TimeLimit;
        if(temp&&temp->time<=Time)
        {
            temp->action();
            activity.next=temp->next;
            free((void*)temp);
        }
        Time++;
    }
}
void manager(void)  //出整个队列时，其实还可能中间的人要放弃等待
{
    int i;
    People *temp,*pre;
    for(i=0;i<MAX_FLOOR_NUM;i++)
    {
        if(WaitQueue[i].next==NULL)
            continue;
        pre=&WaitQueue[i];
        temp=WaitQueue[i].next;
        while(temp!=NULL)
        {
            if(temp->GiveupTime<=Time)
            {
                if(Floor-1==i&&(State==Idle||State==Stop))  //此时可以出入电梯了
                    break;
                pre->next=temp->next;
                printf("%d层的用户%d放弃了电梯等待……\n",i+1,temp->ID);
                if(temp->CallDirection==1)
                    CallUp[temp->InFloor-1]--;
                else if(temp->CallDirection==2)
                    CallDown[temp->InFloor-1]--;
                free((void*)temp);
                temp=pre->next;
            }
            else
            {
                pre=temp;
                temp=temp->next;
            }
        }
    }
}
int OtherFloor(void)
{
    int i;
    if(LastDirection)  //当电梯上次在上升时
    {
        for(i=Floor+1;i<=MAX_FLOOR_NUM;i++)
        {
            if(CallUp[i-1]||CallDown[i-1]||CallCar[i-1])  //当高层有请求时
            {
                LastDirection=1;
                return  1;
            }
        }
        for(i=Floor-1;i>=1;i--)
        {
            if(CallUp[i-1]||CallDown[i-1]||CallCar[i-1])  //当底层有请求时
            {
                LastDirection=2;
                return 2;
            }
        }
    }
    else  //当电梯此时正向下时
    {
        for(i=Floor-1;i>=1;i--)
        {
            if(CallUp[i-1]||CallDown[i-1]||CallCar[i-1])  //当底层有请求时
            {
                LastDirection=2;
                return 2;
            }
        }
        for(i=Floor+1;i<=MAX_FLOOR_NUM;i++)  //当高层有请求时
        {
            if(CallUp[i-1]||CallDown[i-1]||CallCar[i-1])
            {
                LastDirection=1;
                return 1;
            }
        }
    }
    return 0;  //当其他层也无请求时
}
void controller(void)
{
    int flag;  //判断电梯的请求以判断之后会到哪层
    if(State==Idle||State==Stop)
    {
        flag=OtherFloor();
        if(CallUp[Floor-1]||CallDown[Floor-1]||CallCar[Floor-1])  //当前层有人有进出
        {
            State=Others;
            acter(Time+CloseOpenTime,OpenDoor);
        }
        else
        {
            if(flag==1)
            {
                State=Others;
                acter(Time+UpTime,Up);
            }
            else if(flag==2)
            {
                State=Others;
                acter(Time+DownTime,Down);
            }
            else
            {
                State=Idle;
                if(Floor!=InitFloor)
                    acter(Time+IdleTimeLimit,ToFirstIdle);
            }
        }
    }
}
void ComePeople(void)
{
    int InFloor,InterTime;
    People *temp=NULL;
    Count++;
    People *newPeople=(People *)malloc(sizeof(People));
    printf("---------------------------------一位用户到来-------------------------------------\n");
    printf("请输入到来楼层：");
    scanf("%d",&InFloor);
    printf("请输入离开楼层：");
    scanf("%d",&newPeople->OutFloor);
    printf("请输入最大容忍时间：");
    scanf("%d",&newPeople->GiveupTime);
    newPeople->GiveupTime+=Time;
    newPeople->ID=Count;
    newPeople->InFloor=InFloor;
    //入该楼层的等待队列
    temp=&WaitQueue[InFloor-1];
    while(temp->next!=NULL)
        temp=temp->next;
    temp->next=newPeople;
    newPeople->next=NULL;
    if(newPeople->OutFloor>InFloor)
    {
        CallUp[InFloor-1]++;
        newPeople->CallDirection=1;
    }
    else
    {
        CallDown[InFloor-1]++;
        newPeople->CallDirection=2;
    }
    printf("请输入下一位用户到来的时间：");
    scanf("%d",&InterTime);
    printf("---------------------------------------------------\n");
    acter(Time+InterTime,ComePeople);
}
void acter(int time,void(*action)(void))
{
    Activity *newAct=(Activity*)malloc(sizeof(Activity)),*temp=&activity;
    newAct->time=time;  //该动作要发生的时刻
    newAct->action=action;
    while(temp->next!=NULL){
		if(temp->next->time>time)
			break;
		temp=temp->next;
	}
	newAct->next=temp->next;
	temp->next=newAct;
}
void ToFirstIdle(void)
{
    if(State!=Idle||Floor==InitFloor)
        return;
    printf("长时间电梯未收到请求，电梯将于1楼候命……\n");
    CallCar[InitFloor-1]=1;
}
void OpenDoor(void)
{
    printf("电梯即将开门……\n");
    acter(Time+CloseTestTime,IfInOut);
    State=Others;
    if(elevator[Floor-1].next)
        acter(Time+InOutTime,Out);
    else if(WaitQueue[Floor-1].next)
        acter(Time+InOutTime,In);

}
void CloseDoor(void)
{
    printf("电梯即将关门……\n");
    State=Stop;
}
void IfInOut(void)
{
    if(WaitQueue[Floor-1].next||elevator[Floor-1].next)
        acter(Time+CloseTestTime,IfInOut);
    else
    {
        State=Others;
        CallUp[Floor-1]=CallDown[Floor-1]=CallCar[Floor-1]=0;
        acter(Time+CloseOpenTime,CloseDoor);
    }
}
void ToIdle(void)
{
    printf("电梯已停靠在楼层%d……\n",Floor);
    State=Stop;
}
void Up(void)
{
    int flag=0,i;
    printf("电梯已上升到%d楼层……\n",++Floor);
    State=GoingUp;
    /*
    for(i=Floor+1;i<=MAX_FLOOR_NUM;i++)
    {
        if(CallUp[i-1]||CallDown[i-1]||CallUp[i-1])
        {
            flag=1;
            break;
        }
    }
    if(!flag) return;

    if(OtherFloor()==2)
    {
        if(Floor!=InitFloor)
            acter(Time+IdleTimeLimit,ToFirstIdle);
        return;
    }*/
    if((CallCar[Floor-1]||CallDown[Floor-1]||CallUp[Floor-1])||Floor==MAX_FLOOR_NUM)
        acter(Time+ToIdleTime,ToIdle);
    else
        acter(Time+UpTime,Up);
}
void Down(void)
{
    int i,flag=0;
    printf("电梯已下降到%d楼层……\n",--Floor);
    State=GoingDown;
    /*
    for(i=Floor-1;i>=1;i--)
    {
        if(CallUp[i-1]||CallDown[i-1]||CallUp[i-1])
        {
            flag=1;
            break;
        }
    }
    if(!flag) return;

    if(OtherFloor()==1)
    {
        if(Floor!=InitFloor)
            acter(Time+IdleTimeLimit,ToFirstIdle);
        return;
    }*/
    if((CallCar[Floor-1]||CallDown[Floor-1]||CallUp[Floor-1])||Floor==InitFloor)
        acter(Time+ToIdleTime,ToIdle);
    else
        acter(Time+DownTime,Down);
}
void In(void)
{
    People *temp=WaitQueue[Floor-1].next;
    Elevator *q;
    if(temp!=NULL)
    {
        q=(Elevator*)malloc(sizeof(Elevator));
        q->ID=temp->ID;
        printf("用户%d从%d楼层进入了电梯……\n",temp->ID,Floor);
        CallCar[temp->OutFloor-1]=1;
        q->next=elevator[temp->OutFloor-1].next;
        elevator[temp->OutFloor-1].next=q;
        WaitQueue[Floor-1].next=temp->next;
        free((void*)temp);
    }
    if(WaitQueue[Floor-1].next)
        acter(Time+InOutTime,In);
}
void Out(void)
{
    Elevator *temp=elevator[Floor-1].next;
    if(temp!=NULL)
    {
        printf("用户%d已从%d楼层走出电梯……\n",temp->ID,Floor);
        elevator[Floor-1].next=temp->next;
        free((void*)temp);
    }
    if(elevator[Floor-1].next)
        acter(Time+InOutTime,Out);
    else if(WaitQueue[Floor-1].next)
        acter(Time+InOutTime,In);
}
