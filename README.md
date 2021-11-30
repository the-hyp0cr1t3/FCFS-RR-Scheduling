# FCFS and RR Scheduling
Emulation and analysis of two process scheduling algorithms (FCFS and RR) for different workloads.

## Execution Instructions

Requires a Linux environment.

**Compilation:**
```sh
make # To run the makefile and create the executable ./bin/main
```

**Execution:**
1. To run with default arguments run:

```sh
./bin/main # Runs fcfs scheduling algorithm with default values for workloads n1 = n2 = n3 = 1000
```

2. To run with First Come First Serve (FCFS) scheduling algorithm:
```sh
./bin/main fcfs # Runs with default values for workloads n1 = n2 = n3 = 1000
./bin/main fcfs n1 n2 n3 # Runs with given values of workloads
```

3. To run with Round Robin (RR) scheduling algorithm:
```sh
./bin/main rr # Runs with default values for workloads n1 = n2 = n3 = 1000 and time quantum tq = 1000 us (micro-seconds)
./bin/main rr tq # Runs with time quantum tq (us) and default values of workloads n1 = n2 = n3 = 1000 
./bin/main rr n1 n2 n3 # Runs with default time quantum tq = 1000 us (micro-seconds) and given workloads
./bin/main rr tq n1 n2 n3 # Runs with time quantum tq (us) and given workloads
```

## What does it do?
There is a master process M which spawns 3 child processes C<sub>1</sub>, C<sub>2</sub> and C<sub>3</sub>. 
* C<sub>1</sub> is a compute intensive process. It adds n<sub>1</sub> numbers ranging from 1 to 10<sup>6</sup> and returns the sum to M.
* C<sub>2</sub> is an I/O intensive process. It reads n<sub>2</sub> numbers from a file ranging from 1 to 10<sup>6</sup> and prints them to the console. After printing all numbers, C<sub>2</sub> sends the message *“Done Printing”* to M.
* C<sub>3</sub> is both compute and I/O intensive. It reads n<sub>3</sub> numbers from a file ranging from 1 to 10<sup>6</sup> and also returns their sum to M.

Assuming a uniprocessor environment, the master process M emulates one of the following scheduling algorithms.
* [First Come First Serve (FCFS)](https://en.wikipedia.org/wiki/Scheduling_(computing)#First_come,_first_served)
* [Round Robin (RR)](https://en.wikipedia.org/wiki/Scheduling_(computing)#Round-robin_scheduling)

At any moment only one of the child processes can be awake, utilizing the CPU. M uses shared memory segments with C<sub>1</sub>, C<sub>2</sub> and C<sub>3</sub> to communicate if each of them should sleep or wake up to perform their designated tasks.

C<sub>1</sub>, C<sub>2</sub> and C<sub>3</sub> each have two threads: a worker thread and a monitor thread. The worker thread actually performs the task, while the monitor thread monitors communications from M and accordingly puts the worker thread to sleep or wakes it up.

After all the processes have completed their tasks, the wait time and turnaround time for each process are calculated and printed to console.

## How does it work?
The choice of the scheduling algorithm (FCFS or RR), workloads (n<sub>1</sub>, n<sub>2</sub> and n<sub>3</sub>) and time quantum (for RR) are user input via commandline arguments. The master process M writes to and the child processes C<sub>1</sub>, C<sub>2</sub> and C<sub>3</sub> read from a shared memory segment which stores an integer 1, 2 or 3 — corresponding to the currently scheduled process.

![Architecture flowchart](https://user-images.githubusercontent.com/55075129/142473934-377d0763-3527-4fe5-87b4-bb225615e658.png)

The scheduler (M) writes the currently scheduled process to the shared memory segment and sleeps until the process finshes executing (in the case of FCFS) or for a period of *time quantum* (in the case of RR).

Meanwhile the monitor thread of each process polls this shared memory segment at regular intervals. As soon as it detects that the value matches its process ID, it signals the *turn_lock* semaphore that it is holding. The worker thread, which is busy waiting on this lock, resumes execution from where it previously left off.

Additionally, to guarantee mutual exclusion assuming a uniprocessor environment, the CPU is treated as a resource and there is a *cpu_lock* semaphore which has to be acquired and released by the worker thread after the *turn_lock* has been signalled. C<sub>1</sub>, C<sub>2</sub> and C<sub>3</sub> each have their own *turn_lock*, while the same *cpu_lock* is used by all three of them.

When the worker thread has finished its task, the process writes to shared memory that it is done (the scheduler M will not schedule this process anymore) and returns the result (via a pipe). M writes the next process to shared memory and the process continues till all processes have completed their tasks.
In the case of RR, if a full time quantum elapses and the process has not yet finished executing i.e. the shared memory value for currently scheduled process changes, the monitor thread which is polling this value decrements the value of the *turn_lock*. As a result, the worker thread is made to once again busy wait on the *turn_lock*, pausing execution.


#### Contributors
* Nandan H R       
* Jeevan Jyot Singh
* Ankita Behera    
* Anuradha Pandey  
* Omkar Pitale     
* Aditya Chopra    
* Durba Satpathi   
* Hriday G         
