import subprocess
from itertools import combinations
import os
# PARAMS
program = "multithreaded"
NTIMES = 5000
N = 8
PATH = "testdir"
STRING = "HELLO WORLD"
PRINT_MISMATCHES = True #Print mismatches
DELETE_FILES = True # Delete files
KEEP_PROBLEMATIC_FILES = False #Keep the files with mismatches if DELETE_FILES is True
# =======
to_test = []
dont_delete = set()
errors = {}
n_errors = 0
error_count = 0
total = 0
print("Consistency checker by Justin Ruaya\n=============================")
pthread = "-pthread" if (program == "multithreaded") else ""
subprocess.call(f"gcc {program}.c -g -o {program} {pthread}", shell=True)
for i in range(1,NTIMES+1):    
    subprocess.call(f"./{program} {N} \"{PATH}\" \"{STRING}\" > {program}_{i}.txt", shell=True)
    original = open(f"{program}_{i}.txt", "r")
    lines = []

    for line in original.readlines():
        lines.append(line[4:].strip())
    if len(lines) == 0:
        print(f"[Error] {i} is empty! Perhaps a segfault?")
        total+=1
        n_errors+=1
        error_count+=1
        continue
    lines.sort()
    original = open(f"{program}_{i}.txt", "w")
    for line in lines:
        original.write(f"{line}\n")
    original.close()
    to_test.append(f"{program}_{i}")
    print(f"[Test {i}] Exported {program}_{i}.txt")
print("\n----------------------------\n")

for (x,y) in list(combinations(to_test, 2)):
    total += 1
    errors[f"{x}-{y}"] = 0 
    A = open(f"{x}.txt", "r").readlines()
    B = open(f"{y}.txt", "r").readlines()
    for i in range(max(len(A), len(B))):
        str1 = A[i] if i < len(A) else "<EMPTY STRING>"
        str2 = B[i] if i < len(B) else "<EMPTY STRING>"
        if str1 != str2:
            errors[f"{x}-{y}"]+= 1 
            if PRINT_MISMATCHES:
                print(f"[Error] LINE {i+1} on {x}-{y}: \n{str1}{str2}")
            if KEEP_PROBLEMATIC_FILES:
                dont_delete.add(x)
                dont_delete.add(y)
    if errors[f"{x}-{y}"] == 0:
        print(f"{x}-{y}: Horray! Perfect match. :)")
    print("---")
print("Summary:")
for (key, val) in errors.items():
    n_errors+=(1 if val > 0 else 0)
    error_count += val
    print(f"{key}: {val} errors")
print(f"Errors: {error_count}\nFinal score: {total-n_errors}/{total} ({round(((total-n_errors)/total*100), 2)}%)")
if DELETE_FILES:
    to_delete = "EXCEPT problematic files" if KEEP_PROBLEMATIC_FILES else ""
    print(f"Deleting files {to_delete}...")
    for str in to_test:
        if KEEP_PROBLEMATIC_FILES and str in dont_delete:
            continue
        os.remove(f"{str}.txt")