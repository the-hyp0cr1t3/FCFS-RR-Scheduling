import subprocess
from tqdm import tqdm

n = (10, 100, 1000, 10000, 25000, 50000, 75000, 100000, 250000, 500000, 750000, 1000000)
# n = (10000, 25000)
schedulers = ("fcfs", "rr")
# time_quantum = (100, 250, 500, 1000, 2500, 5000)
time_quantum = (250,)

arguments = []

for n1 in n:
    for n2 in n:
        for n3 in n:
            arguments.append(f"./bin/main {n1} {n2} {n3} fcfs > op.txt")

for n1 in n:
    for n2 in n:
        for n3 in n:
            for tq in time_quantum:
                arguments.append(f"./bin/main {n1} {n2} {n3} rr {tq} > op.txt")



for args in tqdm(arguments):
    subprocess.call(args, shell=True)

# Alterantively write to a shell script and execute
# with open("study.sh", "w") as file:
#     file.write('#! /bin/sh\n\n')
#     file.writelines(arguments)

