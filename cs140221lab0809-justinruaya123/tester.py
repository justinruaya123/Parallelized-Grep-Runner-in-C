inp = input().split()
counter = 0
for i in inp: 
    print(len(i), end='')
    if len(i) != 3:
        print(len(i), i)
    else:
        counter += 1
print(counter)