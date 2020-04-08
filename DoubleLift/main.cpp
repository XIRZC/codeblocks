#include<stdio.h>
#include<stdlib.h>
#define MAX_FLOOR_NUM 5
#define InitFloor1 1
#define InitFloor2 5
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
#define TimeLimit  30000  //该程序所模拟的电梯运行时间段不超过10000秒
//需用的数据结构的定义
/*
样例输入
第1位：1 3 1 500  50
第2位：4 1  2  500 50
第3位：5 2 2 500  100
第4位：3 1 1 100  70
第5位：5 2 1 500  30000
*/
typedef struct People{
    int ID;
    int InFloor;
    int OutFloor;
    int GiveupTime;
    int choice;  // 电梯序号
    struct People *next;  //用于不断生成每层的People链表，便于插入和删除
}People;
typedef struct Activity{
    int time;
    int num;  //电梯的执行序号
    void (*action)(int);   //通过计时器建单链表，通过Time来按时间顺序执行
    struct Activity *next;  //用来不断生成事件的单链表
}Activity;
typedef struct Elevator{
    int ID;
    struct Elevator *next;
}Elevator;
//一些全局变量的的定义和赋初值，这些全局变量为各个函数所调用
int Floor1=InitFloor1;  //电梯的当前楼层号，初始在一楼等候
int Floor2=InitFloor2;
int Time=0;   //计时器所用时间，全局的时刻表示，初始为0
int State1=0;  //电梯此时的状态，初始为静候Idle
int State2=0;
int Count=0;  //到来的总人数
int LastDirection1=1;  //电梯上一次的升降方向，1为上升，2为下降，初始必为上升
int LastDirection2=0;
int CallUp[MAX_FLOOR_NUM]={0};
int CallDown[MAX_FLOOR_NUM]={0};
int CallCar1[MAX_FLOOR_NUM]={0};
int CallCar2[MAX_FLOOR_NUM]={0};
People WaitQueue[MAX_FLOOR_NUM];   //不同楼层建立起一个等待队列数组，每个楼层建立等待的人群单链表
Elevator elevator1[MAX_FLOOR_NUM];   //电梯内的不同层的要出来的人的存储栈
Elevator elevator2[MAX_FLOOR_NUM];
Activity activity={0,0,NULL,NULL};
void timer(void); //计时器函数声明
void controller(int num);  //控制器函数声明
void manager(void); //人员管理函数声明
void acter(int time,int num,void(*action)(int));   //不同时刻的事件函数衔接函数声明
void ComePeople(int num);  //读取来坐电梯的人的信息
int OtherFloor(int num);   //当非当前层有升降请求时，通过判断上次升降方向来确定之后去往哪层
//电梯执行动作函数声明
void ToFirstIdle(int num);
void OpenDoor(int num);
void CloseDoor(int num);
void Up(int num);
void Down(int num);
void In(int num);
void Out(int num);
void IfInOut(int num);
void ToIdle(int num);  //电梯停靠
int main(void)
{
    int i;  //对电梯栈和等待队列初始化
	for(i=0;i<MAX_FLOOR_NUM;i++)
	{
		elevator1[i].next=NULL;
		WaitQueue[i].next=NULL;
		elevator1[i].ID=0;
		WaitQueue[i].ID=WaitQueue[i].OutFloor=WaitQueue[i].GiveupTime=WaitQueue[i].InFloor=WaitQueue[i].choice=0;
		elevator2[i].next=NULL;
		elevator2[i].ID=0;
	}
    ComePeople(0);
	timer();
}
void timer(void)
{
    int order=0;
    Activity *temp=activity.next;
    while(1)
    {
        order++;
        if(Time>=TimeLimit)  //程序结束点，电梯运行结束
            break;
        manager(); //人员流动
        if(order%2!=0)
            controller(1);
        else if(order%2==0)
            controller(2);
        temp=activity.next;  //电梯1的活动执行
        if(temp==NULL)
            Time=TimeLimit;
        if(temp&&temp->time<=Time)
        {
            temp->action(temp->num);
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
                if(Floor1==i+1&&(State1==Idle||State1==Stop))  //此时可以出入电梯了
                        break;
                pre->next=temp->next;
                printf("%d层的用户%d放弃了电梯%d等待……\n",i+1,temp->ID,temp->choice);
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
int OtherFloor(int num)
{
    int i;
    if(num==1)  //电梯1的方向抉择
    {
        if(LastDirection1)  //当电梯上次在上升时
        {
            for(i=Floor1+1;i<=MAX_FLOOR_NUM;i++)
            {
                if(CallUp[i-1]||CallDown[i-1]||CallCar1[i-1])  //当高层有请求时
                {
                    LastDirection1=1;
                    return  i;
                }
            }
            for(i=Floor1-1;i>=1;i--)
            {
                if(CallUp[i-1]||CallDown[i-1]||CallCar1[i-1])  //当底层有请求时
                {
                    LastDirection1=0;
                    return -i;
                }
            }
        }
        else  //当电梯此时正向下时
        {
            for(i=Floor1-1;i>=1;i--)
            {
                if(CallUp[i-1]||CallDown[i-1]||CallCar1[i-1])  //当底层有请求时
                {
                    LastDirection1=0;
                    return -i;
                }
            }
            for(i=Floor1+1;i<=MAX_FLOOR_NUM;i++)  //当高层有请求时
            {
                if(CallUp[i-1]||CallDown[i-1]||CallCar1[i-1])
                {
                    LastDirection1=1;
                    return i;
                }
            }
        }
        return 0;  //当其他层也无请求时
    }
    else   //电梯2的方向抉择
    {
        if(LastDirection2)  //当电梯上次在上升时
        {
            for(i=Floor2+1;i<=MAX_FLOOR_NUM;i++)
            {
                if(CallUp[i-1]||CallDown[i-1]||CallCar2[i-1])  //当高层有请求时
                {
                    LastDirection2=1;
                    return  i;
                }
            }
            for(i=Floor2-1;i>=1;i--)
            {
                if(CallUp[i-1]||CallDown[i-1]||CallCar2[i-1])  //当底层有请求时
                {
                    LastDirection2=0;
                    return -i;
                }
            }
        }
        else  //当电梯此时正向下时
        {
            for(i=Floor2-1;i>=1;i--)
            {
                if(CallUp[i-1]||CallDown[i-1]||CallCar2[i-1])  //当底层有请求时
                {
                    LastDirection2=0;
                    return -i;
                }
            }
            for(i=Floor2+1;i<=MAX_FLOOR_NUM;i++)  //当高层有请求时
            {
                if(CallUp[i-1]||CallDown[i-1]||CallCar2[i-1])
                {
                    LastDirection2=1;
                    return i;
                }
            }
        }
        return 0;  //当其他层也无请求时
    }
}
void controller(int num)
{
    int flag;  //判断电梯的请求以判断之后会到哪层
    if(num==1&&(State1==Idle||State1==Stop))
    {
        flag=OtherFloor(1);   //电梯1控制器
        if(CallUp[Floor1-1]||CallDown[Floor1-1]||CallCar1[Floor1-1])  //当前层有人有进出
        {
            State1=Others;
            acter(Time+CloseOpenTime,1,OpenDoor);
        }
        else
        {
            if(flag>0)
            {
                State1=Others;
                acter(Time+UpTime,1,Up);
            }
            else if(flag<0)
            {
                State1=Others;
                acter(Time+DownTime,1,Down);
            }
            else
            {
                State1=Idle;
                if(Floor1!=InitFloor1)
                    acter(Time+IdleTimeLimit,1,ToFirstIdle);
            }
        }
    }
    if(num==2&&(State2==Idle||State2==Stop))
    {
        flag=OtherFloor(2);   //电梯2控制器
        if(CallUp[Floor2-1]||CallDown[Floor2-1]||CallCar2[Floor2-1])  //当前层有人有进出
        {
            State2=Others;
            acter(Time+CloseOpenTime,2,OpenDoor);
        }
        else
        {
            if(flag>0)
            {
                State2=Others;
                acter(Time+UpTime,2,Up);
            }
            else if(flag<0)
            {
                State2=Others;
                acter(Time+DownTime,2,Down);
            }
            else
            {
                State2=Idle;
                if(Floor2!=InitFloor2)
                    acter(Time+IdleTimeLimit,2,ToFirstIdle);
            }
        }
    }
}
void ComePeople(int num)
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
    printf("请输入选择的电梯序号：");
    scanf("%d",&newPeople->choice);
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
    }
    else
    {
        CallDown[InFloor-1]++;
    }
    printf("请输入下一位用户到来的时间：");
    scanf("%d",&InterTime);
    printf("---------------------------------------------------\n");
    acter(Time+InterTime,0,ComePeople);  //0不作参数使用，只是为了符合大部分函数的调用规范
}
void acter(int time,int num,void(*action)(int))
{
    Activity *newAct=(Activity*)malloc(sizeof(Activity)),*temp=&activity;
    newAct->time=time;  //该动作要发生的时刻
    newAct->action=action;
    newAct->num=num;
    while(temp->next!=NULL){
		if(temp->next->time>time)
			break;
		temp=temp->next;
	}
	newAct->next=temp->next;
	temp->next=newAct;
}
void ToFirstIdle(int num)
{
    if(num==1)
    {
        if(State1!=Idle||Floor1==InitFloor1)
        return;
    }
    else
    {
        if(State2!=Idle||Floor2==InitFloor2)
        return;
    }
    printf("长时间电梯未收到请求，电梯%d将于1楼候命……\n",num);
    if(num==1)  CallCar1[InitFloor1-1]=1;
    else CallCar2[InitFloor2-1]=1;
}
void OpenDoor(int num)
{
    if(num==1&&!(CallUp[Floor1-1]||CallDown[Floor1-1]||CallCar1[Floor1-1]))  return;
    if(num==2&&!(CallUp[Floor2-1]||CallDown[Floor2-1]||CallCar2[Floor2-1]))  return;
    printf("电梯%d即将开门……\n",num);
    acter(Time+CloseTestTime,num,IfInOut);
    if(num==1)
    {
        State1=Others;
        if(elevator1[Floor1-1].next)
            acter(Time+InOutTime,num,Out);
        else if(WaitQueue[Floor1-1].next)
            acter(Time+InOutTime,num,In);
    }
    else
    {
        State2=Others;
        if(elevator2[Floor2-1].next)
            acter(Time+InOutTime,num,Out);
        else if(WaitQueue[Floor2-1].next)
            acter(Time+InOutTime,num,In);
    }
}
void CloseDoor(int num)
{
    printf("电梯%d即将关门……\n",num);
    if(num==1)  State1=Stop;
    else if(num==2)  State2=Stop;
}
void IfInOut(int num)
{
    if(num==1)   //测试电梯1是否有出入
    {
        if(WaitQueue[Floor1-1].next||elevator1[Floor1-1].next)
        acter(Time+CloseTestTime,1,IfInOut);
        else
        {
            State1=Others;
            CallUp[Floor1-1]=CallDown[Floor1-1]=CallCar1[Floor1-1]=0;
            acter(Time+CloseOpenTime,1,CloseDoor);
        }
    }
    else   //测试电梯2是否有出入
    {
        if(WaitQueue[Floor2-1].next||elevator2[Floor2-1].next)
        acter(Time+CloseTestTime,2,IfInOut);
        else
        {
            State1=Others;
            CallUp[Floor2-1]=CallDown[Floor2-1]=CallCar2[Floor2-1]=0;
            acter(Time+CloseOpenTime,2,CloseDoor);
        }
    }
}
void ToIdle(int num)
{
    if(num==1&&!((CallCar1[Floor1-1]||CallDown[Floor1-1]||CallUp[Floor1-1])||Floor1==MAX_FLOOR_NUM||Floor1==InitFloor1)) return;
    if(num==2&&!((CallCar2[Floor2-1]||CallDown[Floor2-1]||CallUp[Floor2-1])||Floor2==MAX_FLOOR_NUM||Floor2==InitFloor2)) return;
    printf("电梯%d已停靠在楼层%d……\n",num,num==1?Floor1:Floor2);
    if(num==1) State1=Stop;
    else State2=Stop;
}
void Up(int num)
{
    printf("电梯%d已上升到%d楼层……\n",num,num==1?++Floor1:++Floor2);
    if(num==1)
    {
        State1=GoingUp;
        if((CallCar1[Floor1-1]||CallDown[Floor1-1]||CallUp[Floor1-1])||Floor1==MAX_FLOOR_NUM)
            acter(Time+ToIdleTime,1,ToIdle);
        else
            acter(Time+UpTime,1,Up);
    }
    else
    {
        State2=GoingUp;
        if((CallCar2[Floor2-1]||CallDown[Floor2-1]||CallUp[Floor2-1])||Floor2==MAX_FLOOR_NUM)
            acter(Time+ToIdleTime,2,ToIdle);
        else
            acter(Time+UpTime,2,Up);
    }
}
void Down(int num)
{
    printf("电梯%d已下降到%d楼层……\n",num,num==1?--Floor1:--Floor2);
    if(num==1)
    {
        State1=GoingDown;
        if((CallCar1[Floor1-1]||CallDown[Floor1-1]||CallUp[Floor1-1])||Floor1==InitFloor1)
            acter(Time+ToIdleTime,1,ToIdle);
        else
            acter(Time+DownTime,1,Down);
    }
    else
    {
        State2=GoingDown;
        if((CallCar2[Floor2-1]||CallDown[Floor2-1]||CallUp[Floor2-1])||Floor2==InitFloor2)
            acter(Time+ToIdleTime,2,ToIdle);
        else
            acter(Time+DownTime,2,Down);
    }
}
void In(int num)
{
    if(num==1)
    {
        People *temp=WaitQueue[Floor1-1].next;
        Elevator *q;
        if(temp!=NULL&&temp->choice==1)
        {
            q=(Elevator*)malloc(sizeof(Elevator));
            q->ID=temp->ID;
            printf("用户%d从%d楼层进入了电梯%d……\n",temp->ID,Floor1,num);
            CallCar1[temp->OutFloor-1]=1;
            q->next=elevator1[temp->OutFloor-1].next;
            elevator1[temp->OutFloor-1].next=q;
            WaitQueue[Floor1-1].next=temp->next;
            free((void*)temp);
        }
        if(WaitQueue[Floor1-1].next)
            acter(Time+InOutTime,1,In);
    }
    else
    {
        People *temp=WaitQueue[Floor2-1].next;
        Elevator *q;
        if(temp!=NULL&&temp->choice==2)
        {
            q=(Elevator*)malloc(sizeof(Elevator));
            q->ID=temp->ID;
            printf("用户%d从%d楼层进入了电梯%d……\n",temp->ID,Floor2,num);
            CallCar2[temp->OutFloor-1]=1;
            q->next=elevator2[temp->OutFloor-1].next;
            elevator2[temp->OutFloor-1].next=q;
            WaitQueue[Floor2-1].next=temp->next;
            free((void*)temp);
        }
        if(WaitQueue[Floor2-1].next)
            acter(Time+InOutTime,2,In);
    }
}
void Out(int num)
{
    if(num==1)
    {
        Elevator *temp=elevator1[Floor1-1].next;
        if(temp!=NULL)
        {
            printf("用户%d已从%d楼层走出电梯%d……\n",temp->ID,Floor1,num);
            elevator1[Floor1-1].next=temp->next;
            free((void*)temp);
        }
        if(elevator1[Floor1-1].next)
            acter(Time+InOutTime,1,Out);
        else if(WaitQueue[Floor1-1].next)
            acter(Time+InOutTime,1,In);
    }
    else
    {
        Elevator *temp=elevator2[Floor2-1].next;
        if(temp!=NULL)
        {
            printf("用户%d已从%d楼层走出电梯%d……\n",temp->ID,Floor2,num);
            elevator2[Floor2-1].next=temp->next;
            free((void*)temp);
        }
        if(elevator2[Floor2-1].next)
            acter(Time+InOutTime,2,Out);
        else if(WaitQueue[Floor2-1].next)
            acter(Time+InOutTime,2,In);
    }
}
