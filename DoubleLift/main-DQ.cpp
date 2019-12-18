#include<stdio.h>
#include<stdlib.h>
#define MAX_FLOOR_NUM 5
#define InitFloor1 1
#define InitFloor2 5
//����״̬�궨��
#define GoingUp 1
#define GoingDown 2
#define Idle 0
#define Stop 3
#define Others 5
//���ֶ�������ʱ��궨��
#define CloseTestTime 40  //���Ų���ʱ�䣬ÿ��40��t�������Σ������˽��������
#define CloseOpenTime 20   //���ݿ�����ʱ��
#define InOutTime 25    //�˽�������ʱ��
#define UpTime 55
#define DownTime 60
#define ToIdleTime 30  //����ͣ��ʱ��
#define IdleTimeLimit 300  //����ĳһ�㾲ֹ�������ʱ����ص�һ�����
#define TimeLimit  30000  //�ó�����ģ��ĵ�������ʱ��β�����10000��
//���õ����ݽṹ�Ķ���
/*
��������
1 3 1 500  50
4 1  2  500 50
5 2 2 500  100
3 1 1 100  70
5 2 1 500  30000

�����ȴ��ĵ�������
1 2
5 1
*/
typedef struct People{
    int ID;
    int InFloor;
    int OutFloor;
    int GiveupTime;
    int choice;  // �������
    struct People *next;  //���ڲ�������ÿ���People�������ڲ����ɾ��
}People;
typedef struct Activity{
    int time;
    int num;  //���ݵ�ִ�����
    void (*action)(int);   //ͨ����ʱ����������ͨ��Time����ʱ��˳��ִ��
    struct Activity *next;  //�������������¼��ĵ�����
}Activity;
typedef struct Elevator{
    int ID;
    struct Elevator *next;
}Elevator;
//һЩȫ�ֱ����ĵĶ���͸���ֵ����Щȫ�ֱ���Ϊ��������������
int Floor1=InitFloor1;  //���ݵĵ�ǰ¥��ţ���ʼ��һ¥�Ⱥ�
int Floor2=InitFloor2;
int Time=0;   //��ʱ������ʱ�䣬ȫ�ֵ�ʱ�̱�ʾ����ʼΪ0
int State1=0;  //���ݴ�ʱ��״̬����ʼΪ����Idle
int State2=0;
int Count=0;  //������������
int LastDirection1=1;  //������һ�ε���������1Ϊ������2Ϊ�½�����ʼ��Ϊ����
int LastDirection2=0;
int CallUp[MAX_FLOOR_NUM]={0};
int CallDown[MAX_FLOOR_NUM]={0};
int CallCar1[MAX_FLOOR_NUM]={0};
int CallCar2[MAX_FLOOR_NUM]={0};
People WaitQueue1[MAX_FLOOR_NUM];   //��ͬ¥�㽨����һ���ȴ��������飬ÿ��¥�㽨���ȴ�����Ⱥ������
People WaitQueue2[MAX_FLOOR_NUM];
Elevator elevator1[MAX_FLOOR_NUM];   //�����ڵĲ�ͬ���Ҫ�������˵Ĵ洢ջ
Elevator elevator2[MAX_FLOOR_NUM];
Activity activity={0,0,NULL,NULL};
void timer(void); //��ʱ����������
void controller(int num);  //��������������
void manager(void); //��Ա����������
void acter(int time,int num,void(*action)(int));   //��ͬʱ�̵��¼������νӺ�������
void ComePeople(int num);  //��ȡ�������ݵ��˵���Ϣ
int OtherFloor(int num);   //���ǵ�ǰ������������ʱ��ͨ���ж��ϴ�����������ȷ��֮��ȥ���Ĳ�
//����ִ�ж�����������
void ToFirstIdle(int num);
void OpenDoor(int num);
void CloseDoor(int num);
void Up(int num);
void Down(int num);
void In(int num);
void Out(int num);
void IfInOut(int num);
void ToIdle(int num);  //����ͣ��
int main(void)
{
    int i;  //�Ե���ջ�͵ȴ����г�ʼ��
	for(i=0;i<MAX_FLOOR_NUM;i++)
	{
		elevator1[i].next=NULL;
		WaitQueue1[i].next=NULL;
		elevator1[i].ID=0;
		WaitQueue1[i].ID=WaitQueue1[i].OutFloor=WaitQueue1[i].GiveupTime=WaitQueue1[i].InFloor=WaitQueue1[i].choice=0;
		elevator2[i].next=NULL;
		WaitQueue2[i].next=NULL;
		elevator2[i].ID=0;
		WaitQueue2[i].ID=WaitQueue2[i].OutFloor=WaitQueue2[i].GiveupTime=WaitQueue2[i].InFloor=WaitQueue2[i].choice=0;
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
        if(Time>=TimeLimit)  //��������㣬�������н���
            break;
        manager(); //��Ա����
        if(order%2!=0)
            controller(1);
        else if(order%2==0)
            controller(2);
        temp=activity.next;  //����1�Ļִ��
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
void manager(void)  //����������ʱ����ʵ�������м����Ҫ�����ȴ�
{
    int i;
    People *temp,*pre;
    for(i=0;i<MAX_FLOOR_NUM;i++)
    {
        pre=&WaitQueue1[i];
        temp=WaitQueue1[i].next;
        while(temp!=NULL)
        {
            if(temp->GiveupTime<=Time)
            {
                if(Floor1==i+1&&(State1==Idle||State1==Stop))  //��ʱ���Գ��������
                    break;
                pre->next=temp->next;
                printf("%d����û�%d�����˵���%d�ȴ�����\n",i+1,temp->ID,temp->choice);
                free((void*)temp);
                temp=pre->next;
            }
            else
            {
                pre=temp;
                temp=temp->next;
            }
        }
        if(WaitQueue2[i].next==NULL)
            continue;
        pre=&WaitQueue2[i];
        temp=WaitQueue2[i].next;
        while(temp!=NULL)
        {
            if(temp->GiveupTime<=Time)
            {
                if(Floor2==i+1&&(State2==Idle||State2==Stop))  //��ʱ���Գ��������
                    break;
                pre->next=temp->next;
                printf("%d����û�%d�����˵���%d�ȴ�����\n",i+1,temp->ID,temp->choice);
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
    if(num==1)  //����1�ķ������
    {
        if(LastDirection1)  //�������ϴ�������ʱ
        {
            for(i=Floor1+1;i<=MAX_FLOOR_NUM;i++)
            {
                if(CallUp[i-1]||CallDown[i-1]||CallCar1[i-1])  //���߲�������ʱ
                {
                    LastDirection1=1;
                    return  i;
                }
            }
            for(i=Floor1-1;i>=1;i--)
            {
                if(CallUp[i-1]||CallDown[i-1]||CallCar1[i-1])  //���ײ�������ʱ
                {
                    LastDirection1=0;
                    return -i;
                }
            }
        }
        else  //�����ݴ�ʱ������ʱ
        {
            for(i=Floor1-1;i>=1;i--)
            {
                if(CallUp[i-1]||CallDown[i-1]||CallCar1[i-1])  //���ײ�������ʱ
                {
                    LastDirection1=0;
                    return -i;
                }
            }
            for(i=Floor1+1;i<=MAX_FLOOR_NUM;i++)  //���߲�������ʱ
            {
                if(CallUp[i-1]||CallDown[i-1]||CallCar1[i-1])
                {
                    LastDirection1=1;
                    return i;
                }
            }
        }
        return 0;  //��������Ҳ������ʱ
    }
    else   //����2�ķ������
    {
        if(LastDirection2)  //�������ϴ�������ʱ
        {
            for(i=Floor2+1;i<=MAX_FLOOR_NUM;i++)
            {
                if(CallUp[i-1]||CallDown[i-1]||CallCar2[i-1])  //���߲�������ʱ
                {
                    LastDirection2=1;
                    return  i;
                }
            }
            for(i=Floor2-1;i>=1;i--)
            {
                if(CallUp[i-1]||CallDown[i-1]||CallCar2[i-1])  //���ײ�������ʱ
                {
                    LastDirection2=0;
                    return -i;
                }
            }
        }
        else  //�����ݴ�ʱ������ʱ
        {
            for(i=Floor2-1;i>=1;i--)
            {
                if(CallUp[i-1]||CallDown[i-1]||CallCar2[i-1])  //���ײ�������ʱ
                {
                    LastDirection2=0;
                    return -i;
                }
            }
            for(i=Floor2+1;i<=MAX_FLOOR_NUM;i++)  //���߲�������ʱ
            {
                if(CallUp[i-1]||CallDown[i-1]||CallCar2[i-1])
                {
                    LastDirection2=1;
                    return i;
                }
            }
        }
        return 0;  //��������Ҳ������ʱ
    }
}
void controller(int num)
{
    int flag;  //�жϵ��ݵ��������ж�֮��ᵽ�Ĳ�
    if(num==1&&(State1==Idle||State1==Stop))
    {
        flag=OtherFloor(1);   //����1������
        if(CallUp[Floor1-1]||CallDown[Floor1-1]||CallCar1[Floor1-1])  //��ǰ�������н���
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
        flag=OtherFloor(2);   //����2������
        if(CallUp[Floor2-1]||CallDown[Floor2-1]||CallCar2[Floor2-1])  //��ǰ�������н���
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
    printf("---------------------------------һλ�û�����-------------------------------------\n");
    printf("�����뵽��¥�㣺");
    scanf("%d",&InFloor);
    printf("�������뿪¥�㣺");
    scanf("%d",&newPeople->OutFloor);
    printf("������ѡ��ĵ�����ţ�");
    scanf("%d",&newPeople->choice);
    printf("�������������ʱ�䣺");
    scanf("%d",&newPeople->GiveupTime);
    newPeople->GiveupTime+=Time;
    newPeople->ID=Count;
    newPeople->InFloor=InFloor;
    //���¥��ĵȴ�����
    if(newPeople->choice==1) temp=&WaitQueue1[InFloor-1];
    else   temp=&WaitQueue2[InFloor-1];
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
    printf("��������һλ�û�������ʱ�䣺");
    scanf("%d",&InterTime);
    printf("---------------------------------------------------\n");
    acter(Time+InterTime,0,ComePeople);  //0��������ʹ�ã�ֻ��Ϊ�˷��ϴ󲿷ֺ����ĵ��ù淶
}
void acter(int time,int num,void(*action)(int))
{
    Activity *newAct=(Activity*)malloc(sizeof(Activity)),*temp=&activity;
    newAct->time=time;  //�ö���Ҫ������ʱ��
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
    printf("��ʱ�����δ�յ����󣬵���%d����%d¥��������\n",num,num==1?InitFloor1:InitFloor2);
    if(num==1)  CallCar1[InitFloor1-1]=1;
    else CallCar2[InitFloor2-1]=1;
}
void OpenDoor(int num)
{
    printf("����%d�������š���\n",num);
    acter(Time+CloseTestTime,num,IfInOut);
    if(num==1)
    {
        State1=Others;
        if(elevator1[Floor1-1].next)
            acter(Time+InOutTime,num,Out);
        else if(WaitQueue1[Floor1-1].next)
            acter(Time+InOutTime,num,In);
    }
    else
    {
        State2=Others;
        if(elevator2[Floor2-1].next)
            acter(Time+InOutTime,num,Out);
        else if(WaitQueue2[Floor2-1].next)
            acter(Time+InOutTime,num,In);
    }
}
void CloseDoor(int num)
{
    printf("����%d�������š���\n",num);
    if(num==1)  State1=Stop;
    else if(num==2)  State2=Stop;
}
void IfInOut(int num)
{
    if(num==1)   //���Ե���1�Ƿ��г���
    {
        if(WaitQueue1[Floor1-1].next||elevator1[Floor1-1].next)
        acter(Time+CloseTestTime,1,IfInOut);
        else
        {
            State1=Others;
            CallUp[Floor1-1]=CallDown[Floor1-1]=CallCar1[Floor1-1]=0;
            acter(Time+CloseOpenTime,1,CloseDoor);
        }
    }
    else   //���Ե���2�Ƿ��г���
    {
        if(WaitQueue2[Floor2-1].next||elevator2[Floor2-1].next)
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
    printf("����%d��ͣ����¥��%d����\n",num,num==1?Floor1:Floor2);
    if(num==1) State1=Stop;
    else State2=Stop;
}
void Up(int num)
{
    printf("����%d��������%d¥�㡭��\n",num,num==1?++Floor1:++Floor2);
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
    printf("����%d���½���%d¥�㡭��\n",num,num==1?--Floor1:--Floor2);
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
        People *temp=WaitQueue1[Floor1-1].next;
        Elevator *q;
        if(temp!=NULL&&temp->choice==1)
        {
            q=(Elevator*)malloc(sizeof(Elevator));
            q->ID=temp->ID;
            printf("�û�%d��%d¥������˵���%d����\n",temp->ID,Floor1,num);
            CallCar1[temp->OutFloor-1]=1;
            q->next=elevator1[temp->OutFloor-1].next;
            elevator1[temp->OutFloor-1].next=q;
            WaitQueue1[Floor1-1].next=temp->next;
            free((void*)temp);
        }
        if(WaitQueue1[Floor1-1].next)
            acter(Time+InOutTime,1,In);
    }
    else
    {
        People *temp=WaitQueue2[Floor2-1].next;
        Elevator *q;
        if(temp!=NULL&&temp->choice==2)
        {
            q=(Elevator*)malloc(sizeof(Elevator));
            q->ID=temp->ID;
            printf("�û�%d��%d¥������˵���%d����\n",temp->ID,Floor2,num);
            CallCar2[temp->OutFloor-1]=1;
            q->next=elevator2[temp->OutFloor-1].next;
            elevator2[temp->OutFloor-1].next=q;
            WaitQueue2[Floor2-1].next=temp->next;
            free((void*)temp);
        }
        if(WaitQueue2[Floor2-1].next)
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
            printf("�û�%d�Ѵ�%d¥���߳�����%d����\n",temp->ID,Floor1,num);
            elevator1[Floor1-1].next=temp->next;
            free((void*)temp);
        }
        if(elevator1[Floor1-1].next)
            acter(Time+InOutTime,1,Out);
        else if(WaitQueue1[Floor1-1].next)
            acter(Time+InOutTime,1,In);
    }
    else
    {
        Elevator *temp=elevator2[Floor2-1].next;
        if(temp!=NULL)
        {
            printf("�û�%d�Ѵ�%d¥���߳�����%d����\n",temp->ID,Floor2,num);
            elevator2[Floor2-1].next=temp->next;
            free((void*)temp);
        }
        if(elevator2[Floor2-1].next)
            acter(Time+InOutTime,2,Out);
        else if(WaitQueue2[Floor2-1].next)
            acter(Time+InOutTime,2,In);
    }
}
