- Currently running: Shared Memory
- Finished: 
- Waiting Queue: Not required, just run unfinished tasks sequentially (mod 3 iteration)


Idea: 

4 memory spaces, 1 byte each

- Done -C1
- Done -C2
- Done -C3
- Running (Mutex)


Processor State (Local to each process)
- Currently Running Process
- Process ID


Mutexes: Required. Unavoidable. Just accept it.


## FAQs

1. Why not create the worker thread in the monitor thread? That way you could just join the worker in the monitor; and join the monitor in the main method. 

A. In this case, mutexes get fucked up and the monitoring purpose is not served. The code will become more of a shithole than it is presently. Monitoring requires checking infinitely till the process completes. Too much bt, take lite. 

