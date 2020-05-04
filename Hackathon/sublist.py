def sublist(arr,i,sub):
    if i == len(arr):
        #if len(sub)!=0:
        print(sub)
    else:
        sublist(arr,i+1,sub)
        sublist(arr,i+1,sub+[arr[i]])
        return
l=[2,4,5,3]
sub = []
sublist(l,0,sub)
