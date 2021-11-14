import subprocess
from tqdm import tqdm

n = (10000, 25000, 50000, 75000, 100000, 250000, 500000, 750000, 1000000) # 9 items
time_quantum = (100, 250, 500, 1000, 2500, 5000) # 6 items

arguments = []
ntuples = []

for n1 in n:
    for n2 in n:
        for n3 in n:
            ntuples.append((n1, n2, n3))

print(f"ntuples size: {len(ntuples)}")

for n1, n2, n3 in ntuples:
    arguments.append(f"./bin/main fcfs {n1} {n2} {n3}  > op.txt\n")

for n1, n2, n3 in ntuples:
    for tq in time_quantum:
        arguments.append(f"./bin/main rr {tq} {n1} {n2} {n3}  > op.txt\n")


print(f"Number of Experiments: {len(arguments)}")

for i in range(10):
    for args in tqdm(arguments):
        # print(f"Running: {args}")
        subprocess.call(args, shell=True)

