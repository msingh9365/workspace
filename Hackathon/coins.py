
n = int(input("Enter Amount:"))
coins = [5,2,1]                 #Denominations of coins
no_list = []                    #no of coins of each denominations                    
for coin in coins:                  
    no_coins = n//coin    
    no_list.append(no_coins)
    n%=coin

for i in range(len(coins)):
    print("No of coins of {} : {}".format(coins[i],no_list[i]))
    
