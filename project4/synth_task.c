#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

typedef struct tset
{	int num;
	struct task
	{
	float e_time;
	float deadline;
	float period;
	float load;
	}t[51];
}taskset;
taskset tss[200000];
float calc_util[25];

void unifast(int n, float u_given)
{
	float sum = u_given,next_sum,random;
	int i; 	/*task set carrdinality is n*/
	for(i=0;i<n-1;i++)
	{
		random = rand() % 50 + 100;
		random /= 1000;
		next_sum=sum*pow(random,(1.00/(float)(n-i)));
	    calc_util[i]=sum-next_sum;
		sum=next_sum;
	}
	calc_util[n-1]=sum;
}

int main(int argc,char* argv[])
{
	int split,i,j,k,flag=0,flag2=0,sched_count=0;
	FILE * fp = fopen("Final_Stats.csv", "w");
	float tot_u=0,u_needed=0.05;
	int t,n;
	float sum=0,temp,e,p,a=0,w=0,w_previous=0,f=0,count=0,u;
	float tim[10000],l_factor[10000];
	double value;
	srand(time(NULL));
	k=0;
	do
	{
		u_needed = 0.05;
		do
		{
			for(flag2=0;flag2<5000;k++,flag2++)
			{
				if(flag == 0)
					tss[k].num=10;
				else if(flag == 1)
					tss[k].num=25;
				else
					printf("Error\n");
				split = tss[k].num/3;
				tot_u=0;
				unifast(tss[k].num,u_needed);
				for(i=0,j=3;i<tss[k].num;i++)
				{
					if(i<split)
					{
						tss[k].t[i].period = ((rand() %  (int)pow(10,j+1)) + pow(10,j)); 
						tss[k].t[i].e_time = tss[k].t[i].period * calc_util [i];
						tss[k].t[i].deadline = (rand() % (int)tss[k].t[i].period + tss[k].t[i].e_time);
						tot_u += calc_util[i];
					}
					else 
					{
						split += split;
						j++;
						i--;
					}
				}
			}
			printf("%d tasksets for utilisation %f created with %d tasks per taskset\n\n", flag2, u_needed, tss[k-1].num);
			u_needed += 0.1;
		}while(u_needed <= 0.96);
		flag++;
	}while(flag<2);
	do
	{
		u_needed = 0.05;
		do
		{
			for(flag2=0;flag2<5000;k++,flag2++)
			{
				if(flag == 2)
					tss[k].num=10;
				else if(flag == 3)
					tss[k].num=25;
				split = tss[k].num/3;
				tot_u=0;
				unifast(tss[k].num,u_needed);
				for(i=0,j=3;i<tss[k].num;i++)
				{
					if(i<split)
					{
						tss[k].t[i].period = ((rand() %  (int)pow(10,j+1)) + pow(10,j)); 
						tss[k].t[i].e_time = tss[k].t[i].period * calc_util [i];
						tss[k].t[i].deadline = (rand() % (int)tss[k].t[i].period + (tss[k].t[i].e_time+((tss[k].t[i].period - tss[k].t[i].e_time)/2)));
						tot_u += calc_util[i];
					}
					else 
					{
						split += split;
						j++;
						i--;
					}
				}
			}

			printf("%d tasksets for utilisation %f created with %d tasks per taskset\n\n", flag2, u_needed, tss[k-1].num);
			u_needed += 0.1;
		}while(u_needed <= 0.96);
		flag++;
	}while(flag<4);

	for(i=0;i<200000;i++)
		{
		n=tss[i].num;
		for(j=0;j<n;j++)
			for(k=j+1;k<n;k++)
				{
				if(tss[i].t[j].period>tss[i].t[k].period)
					{ 
					tss[i].t[n]=tss[i].t[j];
					tss[i].t[j]=tss[i].t[k];
					tss[i].t[k]=tss[i].t[n];
					}
				}
			
		}
	for(i=0;i<200000;i++)
		{
		n=tss[i].num;
		for(j=0;j<n;j++)
			tss[i].t[j+n]=tss[i].t[j];
		for(j=0+n;j<2*n;j++)
			for(k=j+1;k<2*n;k++)
				{
				if(tss[i].t[j].deadline > tss[i].t[k].deadline)
					{ 
					tss[i].t[2*n]=tss[i].t[j];
					tss[i].t[j]=tss[i].t[k];
					tss[i].t[k]=tss[i].t[2*n];
					}
				}
			
		}
	printf("\nEDF ANALYSIS\n");

	fprintf(fp, "EDF\nRange\tSched_Count\n");

	printf("------------------------------------------------------------------\n");
	for(i=0;i<200000;i++)
		{
		if(i%5000 == 0 && i!=0)
		{
			fprintf(fp, "%d\t%d\n",i,sched_count );
			sched_count =0;
		}
		n=tss[i].num;
		if(tss[i].t[n-1].deadline>=tss[i].t[n-1].period)
			{
			sum=0;
			n=tss[i].num;
			for(j=0;j<n;j++)
				{
				e=tss[i].t[j].e_time;
				p=tss[i].t[j].period;
				temp=e/p;
				sum=sum+(temp);
				}
			if(sum < 1)
				{
				printf("TASKSET%d SCHEDULABLE\n",i+1);
				sched_count ++;
				//fprintf(fp, "%f\n", );
				}
			// else
			// 	{
			// 	printf("TASKSET%d NOT SCHEDULABLE\n",i+1);
			// 	}			
			}
		else
			{
		
			sum=0;
			n=tss[i].num;
			for(j=0;j<n;j++)
				{
				e=tss[i].t[j].e_time;
				if(tss[i].t[j].deadline < tss[i].t[j].period)
					{
					p=tss[i].t[j].deadline;
					}
				else
					p=tss[i].t[j].period;
				temp=e/p;
				sum=sum+(temp);
				}
			if(sum < 1)
			{
				printf("TASKSET%d SCHEDULABLE\n",i+1);
				sched_count++;
			}
			else
				{
				w=0;
				w_previous=0;
				a=0;
				for(j=0;j<n;j++)
					a=a+tss[i].t[j].e_time;
				do
					{
					w_previous=w;
					w=0;
					for(j=0;j<n;j++)
						{
						value=(double)(a/tss[i].t[j].period);
						w=w+(ceil(value)*tss[i].t[j].e_time);
						}
					a=w;
					}while(w!=w_previous);
				k=0;
				for(j=0;j<n;j++)
					{
					sum=0;
					tim[k]=tss[i].t[j].deadline;
					sum=tss[i].t[j].deadline;
					k++;
					if(sum+tss[i].t[j].period > w)
						continue;
					while(sum<=w)
						{
						sum=sum+tss[i].t[j].period;
						if(sum > w)	
							continue;
						tim[k]=sum;
						k++;
						}
					}
				for(j=0;j<k;j++)
					for(t=j+1;t<k;t++)
						if(tim[j]>tim[t])
							{ 
							tim[k]=tim[j];
							tim[j]=tim[t];
							tim[t]=tim[k];
							}
				n=tss[i].num;
				sum=0;
				for(t=0;t<k;t=t+n)
					{
					for(j=0;j<n;j++)
						{
						sum=sum+tss[i].t[j].e_time;
						l_factor[t+j]=sum;
						}					
					}
				tim[k]=0;
				for(t=0;t<k;t++)
					{
					if(tim[t]==tim[t+1])
						continue;
					u=l_factor[t]/tim[t];
					if(u>1)
						{
						//printf("TASKSET%d NOT SCHEDULABLE\n",i+1);
						break;
						}
						
					}
				if(u<=1)
					{
					printf("TASKSET%d IS SCHEDULABLE\n",i+1);
					sched_count ++;
					}
				
				}
			}

		}
	fprintf(fp, "%d\t%d\n",i,sched_count );
	printf("\nRM ANALYSIS\n");

	fprintf(fp, "RM\nRange\tSched_Count\n");
	sched_count=0;

	printf("------------------------------------------------------------------\n");
	for(i=0;i<200000;i++)
		{
		if(i%5000 == 0 && i!=0)
		{
			fprintf(fp, "%d\t%d\n",i,sched_count );
			sched_count =0;
		}
		printf("------------------------------------------------------------------\n");
		n=tss[i].num;
		if(tss[i].t[n-1].deadline>=tss[i].t[n-1].period)
			{
			branch1:
			sum=0;
			n=tss[i].num;
			for(j=0;j<n;j++)
				{
				e=tss[i].t[j].e_time;
				if(tss[i].t[j].deadline < tss[i].t[j].period)
					{
					p=tss[i].t[j].deadline;
					}
				else
					p=tss[i].t[j].period;
				temp=e/p;
				sum=sum+(temp);
				}
			value=(double)1/n;
			value = pow(2.0, value);
			value= n*( value-1);
			if(sum<value)
				{
				printf("TASKSET%d SCHEDULABLE\n",i+1);
				sched_count ++;
				}
			else
				{
				if(sum<1)
					{
					n=tss[i].num;
					branch2:
					w=0;
					w_previous=0;
					a=0;
					for(j=0;j<n;j++)
						a=a+tss[i].t[j].e_time;
					do
						{w_previous=w;
						w=0;
						for(j=n-1;j>0;j--)
							{
							value=(double)(a/tss[i].t[j].period);
							w=w+(ceil(value)*tss[i].t[j].e_time);
							}
						w=w+tss[i].t[n-1].e_time;
						a=w;
						}while(w!=w_previous);
					if(w<tss[i].t[n-1].deadline)
					{	
						printf("TASKSET%d SCHEDULABLE\n",i+1);
						sched_count++;
					}
					//else	
						//printf("TASKSET%d NOT SCHEDULABLE\n",i+1);
					
					}
				// else
				// 	{
				// 	printf("TASKSET%d NOT SCHEDULABLE\n",i+1);
				// 	}
				}
			}
		else
			{
			n=tss[i].num;
			flag=0;
			for(j=0;j<tss[i].num;j++)
				{
				if(tss[i].t[j].e_time==tss[i].t[j+n].e_time)
					flag++;
				if(tss[i].t[j].deadline==tss[i].t[j+n].deadline)
					flag++;
				if(tss[i].t[j].period==tss[i].t[j+n].period)
					flag++;
				}
			if(flag==(n*3))
				{
				goto branch1;
				}
			else
				{
				for(j=0;j<n;j++)
					{
					f=0;
					count=0;
					e=tss[i].t[j].e_time;
					p=tss[i].t[j].period;
					f=e/p;
					for(k=0;k<n;k++)
						{	if (k==j)
								continue;
							if(tss[i].t[k].period < tss[i].t[j].deadline)
								{
								f=f+((tss[i].t[k].e_time)/(tss[i].t[k].period));
								count++;
								}
							if(tss[i].t[k].period > tss[i].t[j].deadline)
								f=f+((e=tss[i].t[k].e_time)/(p=tss[i].t[j].period));
						}
					value=(double)(1/(count+1));
					value = pow(2.0, value);
					value= (count+1)*( value-1);
					if(f<value)
						{
						continue;
						}
					else
						{
		
						goto branch2;
						}
					}
				}
			}		
		}
	fprintf(fp, "%d\t%d\n",i,sched_count );
	printf("\nDM ANALYSIS\n");
	
	fprintf(fp, "DM\nRange\tSched_Count\n");
	sched_count=0;

	printf("------------------------------------------------------------------\n");
	for(i=0;i<200000;i++)
		{
		if(i%5000 == 0 && i!=0)
		{
			fprintf(fp, "%d\t%d\n",i,sched_count );
			sched_count =0;
		}
		n=tss[i].num;
		if(tss[i].t[(2*n)-1].deadline==tss[i].t[(2*n)-1].period)
			{
			branch3:
			sum=0;
			n=tss[i].num;
			for(j=n;j<(2*n);j++)
				{
				e=tss[i].t[j].e_time;
				if(tss[i].t[j].deadline < tss[i].t[j].period)
					{
					p=tss[i].t[j].deadline;
					}
				else
					p=tss[i].t[j].period;
				temp=e/p;
				sum=sum+(temp);
				}
			value=(double)1/n;
			value = pow(2.0, value);
			value= n*( value-1);
			
			if(sum<value)
				{
				sched_count++;
				printf("TASKSET%d SCHEDULABLE\n",i+1);
				}
			else
				{
				if(sum<1)
					{
					n=tss[i].num;
					
					branch4:
				
					w=0;
					w_previous=0;
					a=0;
					for(j=n;j<(2*n);j++)
						a=a+tss[i].t[j].e_time;
					do
						{w_previous=w;
						w=0;
						if(w>tss[i].t[(2*n)-1].deadline)
							break;
						for(j=(2*n)-1;j>0;j--)
							{
							value=(double)(a/tss[i].t[j].period);
							w=w+(ceil(value)*tss[i].t[j].e_time);
							}
						w=w+tss[i].t[(2*n)-1].e_time;
						a=w;
						}while(w!=w_previous);
					
					if(w<tss[i].t[(2*n)-1].deadline)
						{sched_count++;
						printf("TASKSET%d SCHEDULABLE\n",i+1);
						}
					//else	
						//printf("TASKSET%d NOT SCHEDULABLE\n",i+1);
					
					}
				else
					{
				
					//printf("TASKSET%d NOT SCHEDULABLE\n",i+1);
					}
				}
			}
		else
			{
		
		
			n=tss[i].num;
			flag=0;
			for(j=0;j<tss[i].num;j++)
				{
				if(tss[i].t[j].e_time==tss[i].t[j+n].e_time)
					flag++;
				if(tss[i].t[j].deadline==tss[i].t[j+n].deadline)
					flag++;
				if(tss[i].t[j].period==tss[i].t[j+n].period)
					flag++;
				}
			if(flag==(n*3))
				{
				
				goto branch3;
				}
			else
				{
			
				goto branch4;
				
				}
			}		

		}
fprintf(fp, "%d\t%d\n",i,sched_count );

fclose(fp);
return 0;
}
