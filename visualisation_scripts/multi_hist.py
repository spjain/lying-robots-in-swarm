import os
import matplotlib.pyplot as plt
from csv import reader, writer
import numpy as np

num_trials = 100
num_liars = 10
data = np.zeros((num_liars,3))
for i in range(1,num_liars+1):
    for j in range(1,4):
        success = 0
        for k in range(1,num_trials+1):
            for line in open('num_lying_'+str(i)+'/'+str(j)+'/trial'+str(k)+'.dat','r'):
                num = int(line)
                if num<1500:
                    success = success+1
                break
        data[i-1,j-1] = success*100.0/num_trials

N = num_liars
ind = np.arange(N)
width = 0.27     

fig = plt.figure()
ax = fig.add_subplot(111)

yvals = [4, 9, 2]
rects1 = ax.bar(ind, data[:,0], width, color='r')
zvals = [1,2,3]
rects2 = ax.bar(ind+width, data[:,1], width, color='g')
kvals = [11,12,13]
rects3 = ax.bar(ind+width*2, data[:,2], width, color='b')

ax.set_ylabel('Success rate')
ax.set_xticks(ind+width)
ax.set_xlabel('Number of Liars')
ax.set_xticklabels( ('1', '2', '3', '4', '5', '6', '7', '8', '9', '10') )
ax.legend( (rects1[0], rects2[0], rects3[0]), ('1', '2', '3') )

plt.show()
