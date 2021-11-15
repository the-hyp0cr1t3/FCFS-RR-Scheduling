# OS Assignment

## Compilation Instructions

To compile code on a linux machine run:

```sh
make clean # To clear all output directories
make # Compiling to object files and binaries
```

or

```sh
make rebuild # To clear all output directories and then compile object
```
## Execution Instructions

1. To run with default arguments run:

```sh
bin/main # Runs fcfs scheduling algorithm with default values for workloads
```

2. To run with First Come First Serve (FCFS) scheduling algorithm:
```sh
bin/main fcfs # Runs with default values for workloads
bin/main fcfs n1 n2 n3 # Runs with given values of workloads
```

3. To run with Round Robin (RR) scheduling algorithm:
```sh
bin/main rr # Runs with default values for workloads and time quantum
bin/main rr tq # Runs with time quantum tq and default values of workloads
bin/main rr tq n1 n2 n3 # Runs with time quantum tq and given workloads
```