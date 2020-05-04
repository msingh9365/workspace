import random

guess = str(random.randint(1000,9999))
print(guess)
n=""
while(n != guess):
    n=input("\nEnter the no.").rstrip()
    for i in range(len(guess)):
        if n[i] == guess[i]:
            print("Modi",end=" ")
        else:
            print("Pappu",end =" ")
   
   

        


