#include<iostream>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#define NAME "data"
#define size (7*sizeof(int))
using namespace std;
int compute(int );		//考拉茲猜想算式的函式
int main()
{
	int a;
	cin>>a;
	int fd = shm_open(NAME,O_CREAT| O_RDWR ,0600);			//建立一個共享記憶體區
	ftruncate(fd,size);						//共享記憶體區大小為7個(int)大小
	int *data = (int *)mmap(0,size , PROT_READ | PROT_WRITE, MAP_SHARED,fd,0);	//共享記憶體區對應到data地址空間
		
	int pid = fork();						//建立父子行程

	if(pid > 0)				//父行程
	{
		data[2]=a;			//把輸入的值(1~100)正整數，存入data[2]
		data[3]=1;			//透過data[3]來決定現在誰來計算，data[3]=-1，父行程；data[3]=1，子行程
		data[5]=0;			//計算總共經過多少次計算
		data[4]=0;			//比較現在計算結果的大小，如果目前的計算結果比較大，把它存入data[4]
		data[6]=0;			//把現在比較大的計算結果的計算次數，存入data[6]
		pid_t x= getpid();		//取得父行程的pid
		data[0]=x;			//父行程的pid存入data[0]
		
		while(1)
		{
			if(data[3]==-1)			//data[3]=-1的話，由父行程計算
			{
				if(data[2]==1)			//如果計算結果為1的話，印出結果。
				{
					cout<<"["<<data[0]<<" Parent]: "<<data[2]<<endl;
					cout<<"Max: "<<data[4]<<endl;
					cout<<"Order: "<<data[6]<<endl;
					break;
				}
				data[2]=compute(data[2]);	//把計算結果丟回data[2];
				data[5]++;			//計算次數+1
				if(data[2]>=data[4])		//比較data[2](目前的計算結果)和最大的計算結果誰大。
				{
					data[6]=data[5];	//如果data[2](目前的計算結果)大，把data[5](目前的計算次數)存入data[6]
					data[4]=data[2];	//data[2](目前的計算結果)，則存入data[4]
				}
				cout<<"["<<data[0]<<" Parent]: "<<data[2]<<endl;	//印出結果
				data[3]=1;						//data[3]=1，讓子行程計算
				if(data[2]==1)					//印出結果
				{
					cout<<"Max: "<<data[4]<<endl;
					cout<<"Order: "<<data[6]<<endl;
				break;
				}
			}
		}
	}
	else if (pid == 0)			//子行程
	{
		
		pid_t x= getpid();		//取得子行程的pid
		data[1]=x;			//子行程的pid存入data[0]
		while(1)
		{
			if(data[3]==1)		//data[3]=1的話，由子行程計算
			{
				if(data[2]==1)		//如果計算結果為1的話，回到父行程。
					data[3]=-1;
				else			
				{
					data[2]=compute(data[2]);	//把計算結果丟回data[2];
					data[5]++;			//計算次數+1

				if(data[2]>=data[4])		//比較data[2](目前的計算結果)和最大的計算結果誰大
				{
					data[6]=data[5];	//如果data[2](目前的計算結果)大，把data[5](目前的計算次數)存入data[6]
					data[4]=data[2];	//data[2](目前的計算結果)，則存入data[4]
				}
				
				cout<<"["<<data[1]<<" Child]: "<<data[2]<<endl;		//印出結果
				data[3]=-1;					//換父行程
				}
			}
		}
	}	
	else
	cout<<"Error"<<endl;
	
	munmap(data,size);	//釋放data
	close(fd);		//關閉共享記憶體
	shm_unlink(NAME);	//刪除共享記憶體
	
	return 0;
}
int compute(int a)		//計算考拉茲猜想
{
	if(a%2==0)
	{
		a=a/2;
		return a;
	}
	
	else
	{
		a=a*3+1;
		return a;
	}
}
