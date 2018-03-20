#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

struct temporary
	{
	float e_time;
	float deadline;
	float period;
	};
typedef struct tset
{	int num;
	struct task
	{
	float e_time;
	float deadline;
	float period;
	}*t[100];
}taskset;
/*min function*/
int min(int a, int b)
{
	if (a<b)
		return a;
	else
		return b;
}
/*Main function*/
int main(int argc,char* argv[])
{
int total_taskset,i,j,k,t,n,flag;
float sum=0,temp,e,p,a=0,w=0,w_previous=0,f=0,count=0,u;
float time[100],l_factor[100];
double value;
FILE *ptr_file;	
ptr_file =fopen("data.txt", "r");	 
if (ptr_file != NULL)
	{
	printf("file opened\n");
	fscanf(ptr_file,"%d",&total_taskset);
	printf("The total number of tasksets are %d\n",total_taskset);
	taskset task_set[total_taskset];
	for(i=0;i<total_taskset;i++)
		{
		printf("------------------------------------------------------------------\n");
		fseek(ptr_file, +1L, SEEK_CUR);	
		fscanf(ptr_file,"%d",&task_set[i].num);
		printf("The number of tasks in Taskset %d is %d\n",i+1,task_set[i].num);
		for(j=0;j<task_set[i].num;j++)
			{
			fseek(ptr_file, +1L, SEEK_CUR);	
			task_set[i].t[j]=malloc(sizeof(task_set[i].t[j]));
			fscanf(ptr_file,"%f %f %f",&task_set[i].t[j]->e_time,&task_set[i].t[j]->deadline,&task_set[i].t[j]->period);
			printf("the execution time is %.1f\n",task_set[i].t[j]->e_time);
			printf("the deadline is %.1f\n",task_set[i].t[j]->deadline);
			printf("the period is %.1f\n",task_set[i].t[j]->period);
			printf("------------------------------------------------------------------\n");
			}
		
		}
	/*ordering based on period*/
	for(i=0;i<total_taskset;i++)
		{
		n=task_set[i].num;
		for(j=0;j<task_set[i].num;j++)
			for(k=j+1;k<task_set[i].num;k++)
				{
				if(task_set[i].t[j]->period>task_set[i].t[k]->period)
					{ 
					memcpy(&task_set[i].t[n],&task_set[i].t[j],sizeof(task_set[i].t[j]));
					memcpy(&task_set[i].t[j],&task_set[i].t[k],sizeof(task_set[i].t[j]));
					memcpy(&task_set[i].t[k],&task_set[i].t[n],sizeof(task_set[i].t[j]));
					}
				}
			
		}
	/*ordering based on deadline*/
	for(i=0;i<total_taskset;i++)
		{
		n=task_set[i].num;
		for(j=0;j<task_set[i].num;j++)
			memcpy(&task_set[i].t[j+n],&task_set[i].t[j],sizeof(task_set[i].t[j]));
		for(j=0+n;j<task_set[i].num+n;j++)
			for(k=j+1;k<task_set[i].num+n;k++)
				{
				if(task_set[i].t[j]->deadline>task_set[i].t[k]->deadline)
					{ 
					memcpy(&task_set[i].t[2*n],&task_set[i].t[j],sizeof(task_set[i].t[j]));
					memcpy(&task_set[i].t[j],&task_set[i].t[k],sizeof(task_set[i].t[j]));
					memcpy(&task_set[i].t[k],&task_set[i].t[2*n],sizeof(task_set[i].t[j]));
					}
				}
			
		}
	/*edf analysis*/
	printf("\nEDF ANALYSIS\n");
	printf("------------------------------------------------------------------\n");
	for(i=0;i<total_taskset;i++)
		{
		printf("------------------------------------------------------------------\n");
		n=task_set[i].num;
		if(task_set[i].t[n-1]->deadline>=task_set[i].t[n-1]->period)
			{
			printf("FOR TASKSET%d\n",i+1);
			sum=0;
			printf("PERFORMING UTILIZATION ANALYSIS\n");
			n=task_set[i].num;
			for(j=0;j<n;j++)
				{
				e=task_set[i].t[j]->e_time;
				p=task_set[i].t[j]->period;
				temp=e/p;
				sum=sum+(temp);
				}
			if(sum < 1)
				{
				printf("utilization is less than 1\n");
				printf("TASKSET%d SCHEDULABLE\n",i+1);
				}
			else
				{
				printf("utilization is greater than 1\n");
				printf("TASKSET%d NOT SCHEDULABLE\n",i+1);
				}			
			}
		else
			{
			printf("FOR TASKSET%d\n",i+1);
			printf("PERFORMING DENSITY CHECK\n");
			sum=0;
			n=task_set[i].num;
			for(j=0;j<n;j++)
				{
				e=task_set[i].t[j]->e_time;
				/*min of density and period*/
				if(task_set[i].t[j]->deadline < task_set[i].t[j]->period)
					{
					p=task_set[i].t[j]->deadline;
					}
				else
					p=task_set[i].t[j]->period;
				temp=e/p;
				sum=sum+(temp);
				}
			if(sum < 1)
				printf("TASKSET%d SCHEDULABLE\n",i+1);
			else
				{
				printf("density is not less than 1\n");
				printf("FINDING BUSY PERIOD\n");
				w=0;
				w_previous=0;
				a=0;
				for(j=0;j<n;j++)
					a=a+task_set[i].t[j]->e_time;
				do
					{
					w_previous=w;
					w=0;
					for(j=0;j<n;j++)
						{
						value=(double)(a/task_set[i].t[j]->period);
						w=w+(ceil(value)*task_set[i].t[j]->e_time);
						}
					a=w;
					}while(w!=w_previous);
				printf("the busy period for taskset%d is %f\n",i+1,w);
				/*finding deadline instances*/
				k=0;
				for(j=0;j<n;j++)
					{
					sum=0;
					time[k]=task_set[i].t[j]->deadline;
					sum=task_set[i].t[j]->deadline;
					k++;
					if(sum+task_set[i].t[j]->period > w)
						continue;
					while(sum<=w)
						{
						sum=sum+task_set[i].t[j]->period;
						if(sum > w)	
							continue;
						time[k]=sum;
						k++;
						}
					}
				/*ordering deadline instances*/
				for(j=0;j<k;j++)
					for(t=j+1;t<k;t++)
						if(time[j]>time[t])
							{ 
							time[k]=time[j];
							time[j]=time[t];
							time[t]=time[k];
							}
				n=task_set[i].num;
				sum=0;
				/*finding h*/
				for(t=0;t<k;t=t+n)
					{
					for(j=0;j<n;j++)
						{
						sum=sum+task_set[i].t[j]->e_time;
						l_factor[t+j]=sum;
						}					
					}
				/*calculating loading factor*/
				time[k]=0;
				for(t=0;t<k;t++)
					{
					if(time[t]==time[t+1])
						continue;
					u=l_factor[t]/time[t];
					if(u>1)
						{
						printf("loading factor of task %d is greater than 1\n",j);
						printf("TASKSET%d NOT SCHEDULABLE\n",i+1);
						break;
						}
						
					}
				if(u<=1)
					{
					printf("loading factors are less than or equal to 1\n");
					printf("TASKSET%d IS SCHEDULABLE\n",i+1);
					}
				
				}
			}

		}
	/*rm analysis*/
	printf("\nRM ANALYSIS\n");
	printf("------------------------------------------------------------------\n");
	for(i=0;i<total_taskset;i++)
		{
		printf("------------------------------------------------------------------\n");
		n=task_set[i].num;
		if(task_set[i].t[n-1]->deadline>=task_set[i].t[n-1]->period)
			{
			printf("FOR TASKSET%d\n",i+1);
			branch1:
			sum=0;
			printf("PERFORMING UTILIZATION ANALYSIS\n");
			n=task_set[i].num;
			for(j=0;j<n;j++)
				{
				e=task_set[i].t[j]->e_time;
				if(task_set[i].t[j]->deadline < task_set[i].t[j]->period)
					{
					p=task_set[i].t[j]->deadline;
					}
				else
					p=task_set[i].t[j]->period;
				temp=e/p;
				sum=sum+(temp);
				}
			value=(double)1/n;
			value = pow(2.0, value);
			value= n*( value-1);
			printf("the utilization is %f \n",sum);
			printf("the bound is %f \n",value);
			if(sum<value)
				{
				printf("utilization is less than optimal bound\n");
				printf("TASKSET%d SCHEDULABLE\n",i+1);
				}
			else
				{
				if(sum<1)
					{
					n=task_set[i].num;
					printf("utilization is greater than bound and less than 1\n");
					branch2:
					printf("PERFORMING RT ANALYSIS\n");
					w=0;
					w_previous=0;
					a=0;
					for(j=0;j<n;j++)
						a=a+task_set[i].t[j]->e_time;
					do
						{w_previous=w;
						w=0;
						for(j=n-1;j>0;j--)
							{
							value=(double)(a/task_set[i].t[j]->period);
							w=w+(ceil(value)*task_set[i].t[j]->e_time);
							}
						w=w+task_set[i].t[n-1]->e_time;
						a=w;
						}while(w!=w_previous);
					printf("the worst case execution time for taskset%d is %f\n",i+1,w);
					printf("the deadline for taskset%d is %f\n",i+1,task_set[i].t[n-1]->deadline);
					if(w<task_set[i].t[n-1]->deadline)
						printf("TASKSET%d SCHEDULABLE\n",i+1);
					else	
						printf("TASKSET%d NOT SCHEDULABLE\n",i+1);
					
					}
				else
					{
					printf("utilization is greater than 1\n");
					printf("TASKSET%d NOT SCHEDULABLE\n",i+1);
					}
				}
			}
		else
			{
			printf("FOR TASKSET%d\n",i+1);
			printf("ANALYSING ORDER BASED ON PERIOD AND DEADLINE\n");
			n=task_set[i].num;
			flag=0;
			for(j=0;j<task_set[i].num;j++)
				{
				if(task_set[i].t[j]->e_time==task_set[i].t[j+n]->e_time)
					flag++;
				if(task_set[i].t[j]->deadline==task_set[i].t[j+n]->deadline)
					flag++;
				if(task_set[i].t[j]->period==task_set[i].t[j+n]->period)
					flag++;
				}
			if(flag==(n*3))
				{
				printf("ORDER IS SAME\n");
				goto branch1;
				}
			else
				{
				printf("ORDER NOT SAME\n");
				printf("CALCULATING EFFECTIVE UTILIZATION\n");
				for(j=0;j<n;j++)
					{
					f=0;
					count=0;
					e=task_set[i].t[j]->e_time;
					p=task_set[i].t[j]->period;
					f=e/p;
					for(k=0;k<n;k++)
						{	if (k==j)
								continue;
							if(task_set[i].t[k]->period < task_set[i].t[j]->deadline)
								{
								f=f+((task_set[i].t[k]->e_time)/(task_set[i].t[k]->period));
								count++;
								}
							if(task_set[i].t[k]->period > task_set[i].t[j]->deadline)
								f=f+((e=task_set[i].t[k]->e_time)/(p=task_set[i].t[j]->period));
						}
					value=(double)(1/(count+1));
					value = pow(2.0, value);
					value= (count+1)*( value-1);
					printf("the effective utilization for task%d is%f \n",j+1,f);
					printf("the bound is %f \n",value);
					if(f<value)
						{
						printf("the effective utilization is less than bound\n");
						continue;
						}
					else
						{
						printf("the effective utilization is greater than bound\n");
						goto branch2;
						}
					}
				printf("the effective utilizations are all calulated and determined to be less than bound\n");
				printf("TASKSET%d SCHEDULABLE\n",i+1);
				}
			}		
		}
	printf("\nDM ANALYSIS\n");
	printf("------------------------------------------------------------------\n");
	for(i=0;i<total_taskset;i++)
		{
		printf("------------------------------------------------------------------\n");
		n=task_set[i].num;
		if(task_set[i].t[n-1]->deadline==task_set[i].t[n-1]->period)
			{
			printf("FOR TASKSET%d\n",i+1);
			branch3:
			sum=0;
			printf("PERFORMING UTILIZATION ANALYSIS\n");
			n=task_set[i].num;
			for(j=n;j<(2*n);j++)
				{
				e=task_set[i].t[j]->e_time;
				if(task_set[i].t[j]->deadline < task_set[i].t[j]->period)
					{
					p=task_set[i].t[j]->deadline;
					}
				else
					p=task_set[i].t[j]->period;
				temp=e/p;
				sum=sum+(temp);
				}
			value=(double)1/n;
			value = pow(2.0, value);
			value= n*( value-1);
			printf("the utilization is %f \n",sum);
			printf("the bound is %f \n",value);
			if(sum<value)
				{
				printf("utilization is less than optimal bound\n");
				printf("TASKSET%d SCHEDULABLE\n",i+1);
				}
			else
				{
				if(sum<1)
					{
					n=task_set[i].num;
					printf("utilization is greater than bound and less than 1\n");
					branch4:
					printf("PERFORMING RT ANALYSIS\n");
					w=0;
					w_previous=0;
					a=0;
					for(j=n;j<2*n;j++)
						a=a+task_set[i].t[j]->e_time;
					do
						{w_previous=w;
						w=0;
						
						for(j=(2*n)-1;j>0;j--)
							{
							value=(double)(a/task_set[i].t[j]->period);
							w=w+(ceil(value)*task_set[i].t[j]->e_time);
							}
						w=w+task_set[i].t[(2*n)-1]->e_time;
						a=w;
						if(w>task_set[i].t[(2*n)-1]->deadline)
							{
							printf("worst case execution time is greater than deadline\n");
							printf("TASKSET%d NOT SCHEDULABLE\n",i+1);
							goto b1;
							}
						}while(w!=w_previous);
					printf("the worst case execution time for taskset%d is %f\n",i+1,w);
					printf("the deadline for taskset%d is %f\n",i+1,task_set[i].t[(2*n)-1]->deadline);
					if(w<task_set[i].t[(2*n)-1]->deadline)
						printf("TASKSET%d SCHEDULABLE\n",i+1);
					else	
						printf("TASKSET%d NOT SCHEDULABLE\n",i+1);
					
					}
				else
					{
					printf("utilization is greater than 1\n");
					printf("TASKSET%d NOT SCHEDULABLE\n",i+1);
					}
				b1: printf("\n");
				}
			}
		else
			{
			printf("FOR TASKSET%d\n",i+1);
			printf("ANALYSING ORDER BASED ON PERIOD AND DEADLINE\n");
			n=task_set[i].num;
			flag=0;
			for(j=0;j<task_set[i].num;j++)
				{
				if(task_set[i].t[j]->e_time==task_set[i].t[j+n]->e_time)
					flag++;
				if(task_set[i].t[j]->deadline==task_set[i].t[j+n]->deadline)
					flag++;
				if(task_set[i].t[j]->period==task_set[i].t[j+n]->period)
					flag++;
				}
			if(flag==(n*3))
				{
				printf("ORDER IS SAME\n");
				goto branch3;
				}
			else
				{
				printf("ORDER NOT SAME\n");
				goto branch4;
					
				
				}
			}		

		}
	}
	
else
	printf("File open unsuccessful");
	
return 0;
}

