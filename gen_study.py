
# n = (10, 100, 1000, 10000, 25000, 50000, 75000, 100000, 250000, 500000, 750000, 1000000)
n = (10000, 25000)
schedulers = ("fcfs", "rr")
# time_quantum = (100, 250, 500, 1000, 2500, 5000)
time_quantum = (250, 500, 1000)

arguments = []

for n1 in n:
    for n2 in n:
        for n3 in n:
            arguments.append(f"./bin/main {n1} {n2} {n3} fcfs > op.txt\n")

for n1 in n:
    for n2 in n:
        for n3 in n:
            for tq in time_quantum:
                arguments.append(f"./bin/main {n1} {n2} {n3} rr {tq} > op.txt\n")


# arguments.sort()
# print(len(arguments))
# for a in arguments:
#     print(a)



with open("study.sh", "w") as file:
    file.write('#! /bin/sh\n\n')
    file.writelines(arguments)

