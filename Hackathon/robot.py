dict={}
x = input("Input :").split()
while(x):
    
    dict[x[0].upper()] = int(x[1])
    x = input("Input :").split()

cord = [0,0]

for i in dict.keys():
    if i == 'L':
        cord[0] -= dict[i]
    elif i == 'R':
        cord[0] += dict[i]
    elif i == 'T':
        cord[1] += dict[i]
    else:
        cord[1] -= dict[i]

print("{},{}".format(cord[0],cord[1]))
