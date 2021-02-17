# Modified-CFS-Scheduler-
WRITE UP FOR QUES 1 - ASSIGNMENT 3

SUYASHI SINGHAL 

2019478 

CSE

Description of my code and implementation of my system call

Basic idea 

-   Basically in this assignment we have to add soft real time requirement to a process whose pid is given in the system call. We need to give x units of timeslice to each process that requires soft real time guarantees. We need to give higher priority to a processes softrealtime requirement compared to the vruntime that is usually considered. 

-   In order to implement this in our kernel we are adding a field rtnice(soft real time value) into the sched_entity struct. It has an initial value as 0. 

-   We are then using a system call of the same name - rtnice in order to update the rtnice value for a given process. 

-   We have also made certain changes in the cfs scheduler code to give more priority to a non zero soft real time value of a process.   

-   In the scheduler - more priority is assigned to a process with soft real time values than vruntime. Moreover, if more than one process has non zero rtnice value, then the one with lesser value of rtnice is given more priority.

1.  Description of my system call - logical and implementation details 

-   The name of my system call is "rtnice". It takes two parameters as input. One is the processID (integer) for which we want to update the rtnice(soft real time value) and the other is the realtime (long) which is the rtnice value to be updated for the given process. 

-   In the system call we use the function find_get_pid() which takes the pid as parameter to get the struct pid. In the pid_task() function , I give the struct pid(obtained from find_get_pid()) and PIDTYPE_PID as the parameters. The function returns the task_struct structure which is then used to update the value of rtnice in struct sched_entity (which is one of the variables in task_struct).

-   In case the find_get_pid() returns NULL, it means the process does not exist. In this case, I print a kernel alert along with the process id that the process does not exist in kernel log using printk(). I also print the error message that "Process ID cannot be negative" on kernel log in case the user enters a negative pid. I then return the -ESRCH which means that the process does not exist. I print the errors on the console using strerror(errno) in the test.c file since the errno is set when the error is returned by the system call. 

-   In  case the process is a valid one i.e find_get_pid() does not return null, I find the task_struct for that process and update the value of rtnice in sched_entity structure as realtime*50000000000. 

-   In case the rtnice value given by the user in the parameter of system call is negative, I do not update the value of rtnice. I print the error "Soft real time value cannot be negative" on the kernel log. I then return the error -EINVAL which stands for invalid arguments. It sets the errno and thus I can print the error on the console by strerror() function using the test.c file. 

-   In case the rtnice value was updated, I print the updated rtnice value on the kernel log and the corresponding pid of the process.

1.  Changes done in the kernel code to write the system call and add the variable rtnice(real time guarantees) in task_struct for every process

1.  In the directory include/linux/ I have edited the file sched.h and added the unsigned rtnice long variable as part of the sched_entity struct of the process using the line 

u64 rtnice; 

1.  In the directory include/linux/  I have also edited the file core.c to add the soft real time requirement rtnice into the sched_entity struct of task_struct and initialize it's value to 0 using the following line - 

p->se.rtnice  =0;

1.  In the directory kernel/sched/ I have edited the file fair.c which is the file for the CFS scheduler in order  to incorporate the soft real time guarantees in the scheduling. I have made the following changes in the below mentioned functions -

1.  update_curr 

Change - 

![](https://lh4.googleusercontent.com/TrQ_73Vf3QYPgBs_xzKZEe_dtwX3Wcu05dIMXnodZQL-2xjpu1SaYZ_UQD67O2maYwgHsjcu8WqABI_IeHrkSZm__ua4j_JTmbU1x-XuMmp0I4xIwEAZSCxUpQnn_rcpCRSr7spn)

Logic - If a process has a non zero value of rtnice, then we update the rtnice value. If the rtnice value is greater than delta_exec(time for which process executed) then the rtnice value is decremented by delta_exec. Otherwise it is set to 0.

In case rtnice is 0 already, then scheduling occurs according to vruntime and hence vruntime value is updated. Only one of the rtnice or vruntime value is updated.

1.  entity_before - 

Change - 

![](https://lh5.googleusercontent.com/AQkelzIwWwotmPJov170E1zm1pziAhVt3a-zb1NQsUm_H6kYWMMlKijr6Z4K-xiF5uVGMPI1-tAguKhW-6eNn-NMo8twXCbBSi-IABG9QWQHb1CzrVmBFVEvIlqBOkMSyauUvNm8)

Logic - In this function we are giving more priority to a process which has a smaller rtnice value(non zero) as compared to rtnice value of the other process (given both have non zero rtnice values). If one of them them has rtnice value as 0 then the other gets more priority as it has a nonzero soft real time requirement.

Changes for adding system call -

1.  I have made a directory modifyCFS in the linux directory which stores my system call and makefile. I added two files into the directory. The  first is my system call i.e rtnice.c and the other is the Makefile. The makefile contains a single line i.e - 

obj-y := rtnice.o

*The directory also contains the actual system call c file i.e rtnice.c

1.  In the Makefile inside the linux directory I have made a change in one of the lines as follows - 

core-y += kernel/ certs/ mm/ fs/ ipc/ security/ crypto/ block/ modifyCFS/

I added the modifyCFS/ into the above line so that the files in the directory modifyCFS can also be compiled during compilation.

1.  In the directory  arch/x86/syscalls/  I have edited the file syscalls_64.tbl and added the following line at the end - 

440  common  modifyCFS  sys_rtnice

Here, 

440 is the system call number 

common is the type of system call 

modifyCFS stores the system call and corresponding Makefile

sys_rtnice is the name of the system call.

1.  In the directory include/linux/ I have edited the file syscalls.h and added the following line at the end - 

asmlinkage long sys_rtnice (pid_t processID, long realtime);

1.  Finally after making all the changes I compiled the kernel, installed it and rebooted the virtual box to add my system call. 

I used the following commands  - 

sudo make localmodconfig 

sudo make 

sudo make modules_install install 

sudo make install 

shutdown -r now

Testing the system call

Description of my test.c 

I have added two test functions in test.c The user can choose on of them by entering either 1 or 2 

1.  Test function 1 

Description of code 

-   The function first prompts the user to enter two values of rtnice for process 1 and process 2 respectively. 

-   Process 1 is parent and process 2 is child process. 

-   If the values of rtnice are negative then the syscall returns -1 and print "invalid arguments error".

-   We first fork a child process . In the parent process(process 1), we print the pid of the parent and it's rtnice value entered by the user. We then call the system call rtnice which takes two arguments. First argument is the pid of the process 1 that we get by using getpid()and the second argument is soft_rtnice1 entered by user. We then print the value returned by the system call. In case it returns -1, we print the corresponding error using strerror() function. 

-   Similarly we print the pid and rtnice value of the process 2 (child) and call the system call for the child within the parent itself. First argument is the pid of the process 2 that we get by using pid variable and the second argument is soft_rtnice2 entered by user. We then print the value returned by the system call. In case it returns -1, we print the corresponding error using strerror() function. 

-   After that within the parent we execute a large function (large_factorial) measure its time using clock_gettime() function. When the large function completes execution, we print the pid of the parent and the time taken  by it. Moreover the parent waits for the child to exit. 

-   Within the child process we once again execute a large function which takes around 4 times more time as compared to that in parent (Since we have entered a larger value of the parameter) Again using clock_grttime() function we measure the time of execution and then print the pid of the child and the time taken  by it.

Input the user should give 

-   The system call takes rtnice value and pid as user inputs. 

-   In case of pid, if pid of a process that does not exist is entered, then the system call returns -1 and prints the error "No such process" using strerror(). Moreover if a negative pid is entered by the user, then an error is displayed on the kernel log.  

-   The user should give a non zero rtnice long as input. 

-   In case the user enters a string input (i.e non integral input) for rtnice value , then the error has been handled and we display "I/O error" (errno = 5) . 

-   Also, negative value of rtnice is not allowed to be entered. If the user enters a negative value of rtnice for either process 1 or process 2, then the corresponding system call returns -1 and displays the error for EINVAL - "Invalid arguments" using strerror().

-   Hence the user has to enter an integer pid and long rtnice. The rest is handled by the system calls which returns 0 if the values entered are valid. Else, it returns -1 and sets the errno for the corresponding error which we then display on the terminal.

Expected output

1.  Comparing processes with and without soft real time requirements - In the given screenshot output , we can see that when the child and parent are given equal non zero rtnice value each ( soft_rtnice1=10 and soft_rtnice =10), the time of execution for each process is less than in the case when both parent and child have 0 as their rtnice value. This means that the time taken to execute the processes is more without the soft real time requirements as compared to when processes are given soft real time requirements.

![](https://lh6.googleusercontent.com/GFLxxufX0cgpI6r1iDjUNLsi0OXQHM_npa5col2Zc76RYUQjZhIzjYd491aTsU9MHZQDfTRtCRXbUMpNYVLYSnVH-DT1wTEDT31u4KbYcg4iJRssgK07qAku6bwvPmqi0-7Dva51)

1.  When process 1 is given more priority - In the given output , we have given more priority to the process 1 (parent) as compared to process 2 (child) since rtnice value for the process 1 (soft_rtnice1 =1 )is lesser than that in process 2(soft_rtnice2 = 1000). Hence the parent completes it's execution first as compared to the child. This will also happen when both are given 0 rtnice values as execution time of process 1 is less than that of process 2 in general.

![](https://lh3.googleusercontent.com/mzq_LlIeuieglhoN4A0WzUS0Yw5sWqmlx_RMmvPjDC6tQa1Smodh-n0uC__I6K0lqgeGcDiBL5SZ0JtEQahr213VjVd04fj0_VCRBN514JAfoO_VvdMrcRXMD-3v_L_nva3uMp61)

1.  When process 2 is given more priority - In the given output  , we have given more priority to the Process 2 (child) as compared to Process 1 (parent) since rtnice value for the Process 1 is more than that in Process 1. Hence the child completes it's execution first.

Even tough the time of execution of child is 4 time than that in parent (as a function that takes 4 times more time is executed) , the child completes it's execution first, since it has more priority. 

![](https://lh4.googleusercontent.com/fXi7PatL8JUIZQOez_D2d0BY8oc_DbzA4M8srB1GRjkdk2BOzfyjHCZX0bNnUlgoVXRzn-2eSYkN9pgOKc2vB_AlB6zNSyTtzQQhM-HBihoY5enxQSHAWowNFETd6fkP6UiPKf0r)

Error handling 

1.  When rtnice value is entered as a string instead of long -  In the given output , I have entered a string value in the rtnice variable instead of long. Hence an error is displayed on the terminal  " I/O error" using strerror() function. "EIO" is the error macro for the same(errno = 5).

![](https://lh6.googleusercontent.com/dot3YM_g9oJks4JOQl_Y-pYGkH9mCU3LfX8qOc-Z4ncBy-L13TvvdqDhgc22K9jfJrXDm5Bmun3FS-N8M81mSJWVfcS1KJg_DS_d5HDK934UFKmxWpnngdw_U9lActYB1_vbLC5Q)

![](https://lh5.googleusercontent.com/n6kJHZOhmf4bLK0cn7WA7ir-likWyDQxly-kbDIt-36__t7AzYgin6VKbnzDUtp2VUt2AoOXEyMeT77B6SXXQ0LP8IYAx4LKyLXc0V6CWC8sghgWj3a3cgiofQKhJuGfkYsiAx89)

![](https://lh4.googleusercontent.com/x1BTRPPek8dX8FxCngCQEkOu2etM5suiDf-j9laW_xKfYabonqhG5LkNaS6V1uF_04J_l6UWjPNK7-WY-GBrf499FzDJe1-XiWHcnqen2LJeKEatWhcVOZdEVG71552a6t5PtHX1)

1.  When a negative rtnice value is given by the user -

In the given output, we have entered a negative value for the rtnice variable of the process 2. Hence when the system call executes for process 2, it returns -1 and prints the error on the terminal "Invalid arguments". This is because in case of negative rtnice values, the system call returns -EINVAL which sets the errno and hence we print the error using strerror() function. 

![](https://lh3.googleusercontent.com/XDm7XoDyvpzlumWVibUiL99AgnNjXslFbT005pGYZviQKAD0zBR8WR5d_eWb-gnCvEACpJxc7vl2qHNn_q5aYP6ptGm_SC8sxMUGnnUI2zZtyCYAVhojBcK8S4j4IOaOdjbd6cr-)

1.  When the pid entered in the input of system call does not exist - 

In this case I have altered the test.c file to show that the error for negative pid has been handled in system call. 

![](https://lh3.googleusercontent.com/PxfRS4csOr2iTMKxAD-mrdAXwXlqtm51xL6ygYeIsVOHCYNpTaHWL2TaIx0fkfn0Tgf2uukTXqV5b95vjusFbE2SNTmFg0DZ_izcX8qnpUjiCBQARDl5EO_YG7Ka1JuJWFcEYaPP)

1.  Error handling for fork() in first test function 

![](https://lh6.googleusercontent.com/BMSE5TsDeDBQ1yclvVWz4deDCRdJxzVywRLEgjwtX1Zuc80nUgApq3XazfKqsIcGwYhN2a4NAYopuNZZUJLkQr8XITMxk1cYueIzO_yPNmEk1UCVJXmCDW_P1BSJ2WmJc0fiO8XT)

1.  Error handling for waitpid() function in test function 1 - 

![](https://lh6.googleusercontent.com/y3bxUydw7u0c6Cace4BdWtxx1S_ecM654jui5U0z5LOAGeaOjUGTlrS_j96Tj6WSMmp-y5RToUrx5FI-Fl8-PhvxXJvNT-IMjv-EhmJA8ynDrDquCnAnkonBQjgEWODFKATnDD7U)

1.  Test function 2 

Description of code 

-   In the second test function I have first printed the execution time for a given large function for 5 processes without using soft real time requirements. I have used a while loop that forks 5 child processes and prints the time of execution for each process. The time of execution is calculated using the clock_gettime() function and displayed on the terminal. 

-   After this, we print the execution time for the same large function for 5 processes that have been given different non zero soft real time requirements. I have used a while loop that forks 5 child processes and prints the time of execution for each process. Within the while loop the system call rtnice is called for each process to set the soft real time requirements. The time of execution is calculated using the clock_gettime() function and displayed on the terminal for each process. 

-   We have handled the error for fork() in test.c and rest of the errors have been handled in the system call as displayed above.

Input the user should give 

-   The system call takes rtnice value and pid as user inputs. 

-   In case of pid, if pid of a process that does not exist is entered, then the system call returns -1 and prints the error "No such process" using strerror(). Moreover if a negative pid is entered by the user, then an error is displayed on the kernel log.  

-   The user should give a non zero rtnice long as input. 

-   In case the user enters a string input (i.e non integral input) for rtnice value , then the error has been handled and we display "I/O error" (errno = 5) . 

-   Also, negative value of rtnice is not allowed to be entered. If the user enters a negative value of rtnice for either process 1 or process 2, then the corresponding system call returns -1 and displays the error for EINVAL - "Invalid arguments" using strerror().

-   Hence the user has to enter an integer pid and long rtnice. The rest is handled by the system calls which returns 0 if the values entered are valid. Else, it returns -1 and sets the errno for the corresponding error which we then display on the terminal. 

-   In this test case, we have hardcoded the test case. Hence the user does not need to enter anything into the terminal.

Expected Output 

In the output screenshot given below, we can see that the execution time when soft real time requirements are set is much smaller as compared to when soft real time requirements aren't set.

![](https://lh4.googleusercontent.com/meV4kN_uydkGNaPBXUsP9e3BJx10DfncBy4YtEpxzTt-AovgfKZfgB5G5uOY7CP8YMPx17478lPZTvTo9yj9RL0-3TGYrNLlsvHT1KY2g2TsJO0f36nioPrOX-YJ1RKshBd42Spz)

Error Handling 

1.  We have shown all the error handling done in test case 1

2.  Error handling for fork()

![](https://lh4.googleusercontent.com/EX0Jbl_ttN0kIODkVGyP-fDkmC4QQF_hRlKUj8tiNumTq6OoxAcUG0NZJKfjslG-CfrMuh2PuzPbgBKPbdoFIyfCY_KtGYMPf8lW8cY60PcpCZ7IECn5wTGwNnmKFXlzNTmrAI6I)

Generation of diff 

-   I have generated the diff of my linux directory with the original linux directory. 

-   I have two directories in /usr/src - my_Scheduler which stores the modified code of linux-5.9.1 in which I have added my system call rtnice and original which stores the original code of linux-5.9.1

-   Before generating diff I used the command sudo make distclean in my_Scheduler/linux-5.9.1/ directory. 

-   After that I generated the diff using the command - 

diff    -Naur    original/linux-5.9.1/     my_Scheduler/linux-5.9.1/    >   diff.txt
