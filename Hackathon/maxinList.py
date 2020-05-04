l=[1,23,43,4,34,6]

n = int(input("Enter the no."))
if n>len(l):
    num =-1
else:
    while(n>0):
        num = max(l)
        l.remove(num)
        n-=1
print(num)
    
