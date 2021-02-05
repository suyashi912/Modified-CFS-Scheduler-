/* Name: Suyashi Singhal
   Roll_Number: 2019478 */

#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>

#define rtnice 440
#define BILLION 1000000000.0
extern int errno; 
int large_factorial(long long int n); 
int tc1(void); 
int tc2(void); 

int main()
{
	int testcase = 0; 
	printf("Choose test case 1 or 2: \n");
	if(scanf("%d", &testcase) != 1 || (testcase != 1 && testcase != 2))
	{
		printf("No such test case found.\n");
		return 0; 
	}
	if(testcase == 1)
	{
		tc1(); 
	}
	if(testcase == 2)
	{
		tc2(); 
	}

	return 0; 
}

int tc1(void )
{
	int pid, wpid; 
	int status; 
	long soft_rtnice1, soft_rtnice2; 
	printf("Enter real time for process 1: ");
	if(scanf("%ld", &soft_rtnice1) != 1)
		{
			printf("Error: %s\n", strerror(EIO));  // EIO (errno 5) is for Input/Output error
			return 0; 
		}
	printf("Enter real time for process 2: ");
	if(scanf("%ld", &soft_rtnice2) != 1)
		{
			printf("Error: %s\n", strerror(EIO));  // EIO (errno 5) is for Input/Output error
			return 0; 
		}
	printf("\n");

	pid = fork(); 
 
	if(pid<0)                     // Error handling for negative pid 
	{
		printf("Error: %s\n", strerror(errno));
		return 0; 
	}
	else
	{
		if(pid > 0) //in parent process 
		{ 
			long int t1 = syscall(rtnice, getpid(), soft_rtnice1); 
			printf("PID of Process 1 (parent) : %d with rtnice value : %ld\n", getpid(), soft_rtnice1);
			printf("System call sys_rtnice returned %ld\n\n", t1);
			if(t1 != 0)
			{
				printf("Error : %s\n", strerror(errno));
				return 0; 
			} 

			long int t2 = syscall(rtnice, pid, soft_rtnice2); 

			printf("PID of Process 2 (child) : %d with rtnice value : %ld\n", pid, soft_rtnice2); 
			printf("System call sys_rtnice returned %ld\n\n", t2);
			if(t2 != 0)
			{
				printf("Error : %s\n", strerror(errno));
				return 0; 
			} 
			for(long i=0; i<1000000000; i++); 
	
		}

		if( pid == 0)     // in child process 
		{
			if(soft_rtnice2 < 0 || soft_rtnice1 < 0)
				return 0; 
			double time_taken = 0; 
			struct timespec start, done; 
			clock_gettime(CLOCK_REALTIME, &start); 
			large_factorial(4000000000); 
			clock_gettime(CLOCK_REALTIME, &done);
			time_taken = time_taken + (double)(done.tv_nsec-start.tv_nsec)/BILLION + (double)(done.tv_sec-start.tv_sec); 
			printf("Process 2 with pid: %d terminated.\n", getpid());
			printf("Time taken by Process 2 : %f \n\n", time_taken);
			exit(0); 
		}
		else    //in parent process
		{
			if(soft_rtnice1 < 0 || soft_rtnice2 < 0)
				return 0;
			double time_taken = 0; 
			struct timespec start, done; 
			clock_gettime(CLOCK_REALTIME, &start);
			large_factorial(1000000000);
			clock_gettime(CLOCK_REALTIME, &done);
			time_taken = time_taken + (double)(done.tv_nsec-start.tv_nsec)/BILLION + (double)(done.tv_sec-start.tv_sec); 
			printf("Process 1 with pid: %d terminated.\n", getpid());
			printf("Time taken by Process 1 : %f \n\n", time_taken);
			
			wpid = waitpid(pid, &status, 0); 
		if(wpid<0)
	 	{
	 		printf("Error: %s\n", strerror(errno));		//Error handling for waitpid()
	 		return 0;  								
	 	}
		}
	}
	return 0; 
}

int tc2(void)
{
	int pid, wpid, status; 
	int count = 0;
	printf("Time taken to run 5 processes without soft real time requirements: \n");

	while(count<5)
	{
		pid = fork(); 
		if(pid<0)
		{
			printf("Error: %s\n", strerror(errno));
		}
		else if(pid == 0)
		{
			double time_taken = 0; 
			struct timespec start, done; 
			clock_gettime(CLOCK_REALTIME, &start);
			large_factorial(200000000);
			clock_gettime(CLOCK_REALTIME, &done);
			time_taken = time_taken + (double)(done.tv_nsec-start.tv_nsec)/BILLION + (double)(done.tv_sec-start.tv_sec); 
			printf("Process %d took : %f seconds. \n", count+1, time_taken);
			exit(0); 
		}
		count++; 
	}

	for(int i=count; i>0; i--)
	{
		wait(NULL); 
	}
	count = 0; 
	printf("Time taken to run 5 processes with soft real time requirements: \n");
	long rtniceval = 0; 
	while(count<5)
	{
		pid = fork(); 
		if(pid<0)
		{
			printf("Error: %s\n", strerror(errno));
		}
		else if(pid == 0)
		{
			rtniceval = 1+count*10; 
			long int t1 = syscall(rtnice, getpid(), rtniceval); 
			if(t1 != 0)
			{
				printf("Error : %s\n", strerror(errno));
				return 0; 
			} 
			double time_taken = 0; 
			struct timespec start, done; 
			clock_gettime(CLOCK_REALTIME, &start);
			large_factorial(200000000);
			clock_gettime(CLOCK_REALTIME, &done);
			time_taken = time_taken + (double)(done.tv_nsec-start.tv_nsec)/BILLION + (double)(done.tv_sec-start.tv_sec); 
			printf("Process %d took : %f seconds. \n", count+1, time_taken);
			exit(0); 
		}

		count++; 
	}
	for(int i=count; i>0; i--)
	{
		wait(NULL); 
	}
}

int large_factorial(long long int n)
{

	long long int fact = 1; 
	long long int count = 1;
	while(count <= n)
	{
		fact *= count; 
		count++; 
	}

	return 0; 
}

