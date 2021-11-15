# OS Assignment

## Compilation Instructions

To compile code on a linux machine run:

```sh
make # To clear all output directories and then compile object
```
## Execution Instructions

1. To run with default arguments run:

```sh
bin/main # Runs fcfs scheduling algorithm with default values for workloads n1 = n2 = n3 = 1e6
```

2. To run with First Come First Serve (FCFS) scheduling algorithm:
```sh
bin/main fcfs # Runs with default values for workloads n1 = n2 = n3 = 1e6
bin/main fcfs n1 n2 n3 # Runs with given values of workloads
```

3. To run with Round Robin (RR) scheduling algorithm:
```sh
bin/main rr # Runs with default values for workloads n1 = n2 = n3 = 1e6 and time quantum tq = 1e3
bin/main rr tq # Runs with time quantum tq and default values of workloads n1 = n2 = n3 = 1e6 
bin/main rr tq n1 n2 n3 # Runs with time quantum tq and given workloads
```

### Contributors
|Name|ID|
|-|-|
| Nandan H R        | 2019A7PS0164H |
| Jeevan Jyot Singh | 2019A7PS0172H |
| Ankita Behera     | 2019A7PS0075H |
| Anuradha Pandey   | 2019A7PS0265H |
| Omkar Pitale      | 2019A7PS0083H |
| Aditya Chopra     | 2019A7PS0178H |
| Durba Satpathi    | 2019A7PS0972H |
| Hriday G          | 2019A7PS1212H |
