#include<stdio.h>
#include<stdlib.h>
#define MAX_FLOOR_NUM 5
#define InitFloor 1
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
#define TimeLimit  10000  //�ó�����ģ��ĵ�������ʱ��β�����10000��
//���õ����ݽṹ�Ķ���
/*
��������
��1λ��1 3 500 200
��2λ��4 1 500 100
��3λ��5 2 1000 200
��4λ��3 1 100 100
��5λ��5 2 1000 10000

�����ȴ��ĵ�������
1 2
5 1
*/
typedef struct People{
    int ID;
    int InFloor;
    int OutFloor;
    int GiveupTime;
    int CallDirection;
    struct People *next;  //���ڲ�������ÿ���People�������ڲ����ɾ��
}People;
typedef struct Activity{
    int time;
    void (*action)(void);   //ͨ����ʱ����������ͨ��Time����ʱ��˳��ִ��
    struct Activity *next;  //�������������¼��ĵ�����
}Activity;
typedef struct Elevator{
    int ID;
    struct Elevator *next;
}Elevator;
//һЩȫ�ֱ����ĵĶ���͸���ֵ����Щȫ�ֱ���Ϊ��������������
int Floor=InitFloor;  //���ݵĵ�ǰ¥��ţ���ʼ��һ¥�Ⱥ�
int Time=0;   //��ʱ������ʱ�䣬ȫ�ֵ�ʱ�̱�ʾ����ʼΪ0
int State=0;  //���ݴ�ʱ��״̬����ʼΪ����Idle
int Count=0;  //������������
int LastDirection=1;  //������һ�ε���������1Ϊ������2Ϊ�½�����ʼ��Ϊ����
int CallUp[MAX_FLOOR_NUM]={0};
int CallDown[MAX_FLOOR_NUM]={0};
int CallCar[MAX_FLOOR_NUM]={0};
People WaitQueue[MAX_FLOOR_NUM];   //��ͬ¥�㽨����һ���ȴ��������飬ÿ��¥�㽨���ȴ�����Ⱥ������
Elevator elevator[MAX_FLOOR_NUM];   //�����ڵĲ�ͬ���Ҫ�������˵Ĵ洢ջ
Activity activity={0,NULL,NULL};
void timer(void); //��ʱ����������
void controller(void);  //��������������
void manager(void); //��Ա����������
void acter(int time,void(*action)(void));   //��ͬʱ�̵��¼������νӺ�������
void ComePeople(void);  //��ȡ�������ݵ��˵���Ϣ
int OtherFloor(void);   //���ǵ�ǰ������������ʱ��ͨ���ж��ϴ�����������ȷ��֮��ȥ���Ĳ�
//����ִ�ж�����������
void ToFirstIdle(void);
void OpenDoor(void);
void CloseDoor(void);
void Up(void);
void Down(void);
void In(void);
void Out(void);
void IfInOut(void);
void ToIdle(void);  //����ͣ��
int main(void)
{
    int i;  //�Ե���ջ�͵ȴ����г�ʼ��
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

        if(Time>=TimeLimit)  //��������㣬�������н���
            break;
        manager(); //��Ա����
        controller();   //���ݶ�������
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
void manager(void)  //����������ʱ����ʵ�������м����Ҫ�����ȴ�
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
                if(Floor-1==i&&(State==Idle||State==Stop))  //��ʱ���Գ��������
                    break;
                pre->next=temp->next;
                printf("%d����û�%d�����˵��ݵȴ�����\n",i+1,temp->ID);
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
    if(LastDirection)  //�������ϴ�������ʱ
    {
        for(i=Floor+1;i<=MAX_FLOOR_NUM;i++)
        {
            if(CallUp[i-1]||CallDown[i-1]||CallCar[i-1])  //���߲�������ʱ
            {
                LastDirection=1;
                return  1;
            }
        }
        for(i=Floor-1;i>=1;i--)
        {
            if(CallUp[i-1]||CallDown[i-1]||CallCar[i-1])  //���ײ�������ʱ
            {
                LastDirection=2;
                return 2;
            }
        }
    }
    else  //�����ݴ�ʱ������ʱ
    {
        for(i=Floor-1;i>=1;i--)
        {
            if(CallUp[i-1]||CallDown[i-1]||CallCar[i-1])  //���ײ�������ʱ
            {
                LastDirection=2;
                return 2;
            }
        }
        for(i=Floor+1;i<=MAX_FLOOR_NUM;i++)  //���߲�������ʱ
        {
            if(CallUp[i-1]||CallDown[i-1]||CallCar[i-1])
            {
                LastDirection=1;
                return 1;
            }
        }
    }
    return 0;  //��������Ҳ������ʱ
}
void controller(void)
{
    int flag;  //�жϵ��ݵ��������ж�֮��ᵽ�Ĳ�
    if(State==Idle||State==Stop)
    {
        flag=OtherFloor();
        if(CallUp[Floor-1]||CallDown[Floor-1]||CallCar[Floor-1])  //��ǰ�������н���
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
    printf("---------------------------------һλ�û�����-------------------------------------\n");
    printf("�����뵽��¥�㣺");
    scanf("%d",&InFloor);
    printf("�������뿪¥�㣺");
    scanf("%d",&newPeople->OutFloor);
    printf("�������������ʱ�䣺");
    scanf("%d",&newPeople->GiveupTime);
    newPeople->GiveupTime+=Time;
    newPeople->ID=Count;
    newPeople->InFloor=InFloor;
    //���¥��ĵȴ�����
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
    printf("��������һλ�û�������ʱ�䣺");
    scanf("%d",&InterTime);
    printf("---------------------------------------------------\n");
    acter(Time+InterTime,ComePeople);
}
void acter(int time,void(*action)(void))
{
    Activity *newAct=(Activity*)malloc(sizeof(Activity)),*temp=&activity;
    newAct->time=time;  //�ö���Ҫ������ʱ��
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
    printf("��ʱ�����δ�յ����󣬵��ݽ���1¥��������\n");
    CallCar[InitFloor-1]=1;
}
void OpenDoor(void)
{
    printf("���ݼ������š���\n");
    acter(Time+CloseTestTime,IfInOut);
    State=Others;
    if(elevator[Floor-1].next)
        acter(Time+InOutTime,Out);
    else if(WaitQueue[Floor-1].next)
        acter(Time+InOutTime,In);

}
void CloseDoor(void)
{
    printf("���ݼ������š���\n");
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
    printf("������ͣ����¥��%d����\n",Floor);
    State=Stop;
}
void Up(void)
{
    int flag=0,i;
    printf("������������%d¥�㡭��\n",++Floor);
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
    printf("�������½���%d¥�㡭��\n",--Floor);
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
        printf("�û�%d��%d¥������˵��ݡ���\n",temp->ID,Floor);
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
        printf("�û�%d�Ѵ�%d¥���߳����ݡ���\n",temp->ID,Floor);
        elevator[Floor-1].next=temp->next;
        free((void*)temp);
    }
    if(elevator[Floor-1].next)
        acter(Time+InOutTime,Out);
    else if(WaitQueue[Floor-1].next)
        acter(Time+InOutTime,In);
}
