import os
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

def makePath(params, vals, start, end):
	newPath = ''
	for i in range(start, end):
		newPath = newPath + params[i] + '_' + vals[i] + '/'
	return newPath

def getVals(params, vals, index, dataFolder):
	newVals = []
	for i in os.listdir(dataFolder + makePath(params, vals, 0, index)):
		loc = i.rfind('_')
		newVals.append(i[loc+1:])
	newVals.sort()
	return newVals

def checkSuccess(table):
	numSteps = 1600
	lastStep = table[table['Time'] == numSteps]
	for i in lastStep['Robot'].values:
		robotState = lastStep[lastStep['Robot'] == i].squeeze()
		# if robotState['PickedPatternProb'] == robotState['CorrectPatternProb'] and robotState['PickedPatternNum'] <= 4:
		if robotState['PickedPatternProb'] == robotState['CorrectPatternProb']:
			return True
		else:
			return False

dataFolder = 'data/'
path = 'good_basic/bad_constant/side_2/pattern_6/noise_0.49/comm_1/num_robots_10/num_lying_1/range_3/density_1.0/'
params = []
vals = []
for i in path[:-1].split('/'):
	loc = i.rfind('_')
	params.append(i[:loc])
	vals.append(i[loc+1:])

print('Enter 1st parameter to vary')
# param_to_vary_1 = input()

print('Enter 2nd parameter to vary')
# param_to_vary_2 = input()

print('Enter 3rd parameter to vary (row)')
# param_to_vary_3 = input()

print('Enter 4th parameter to vary (column)')
# param_to_vary_4 = input()

param_to_vary_1 = 'num_lying'
param_to_vary_2 = 'good'
param_to_vary_3 = 'density'
param_to_vary_4 = 'range'

index1 = params.index(param_to_vary_1)
index2 = params.index(param_to_vary_2)
index3 = params.index(param_to_vary_3)
index4 = params.index(param_to_vary_4)

vals1 = getVals(params, vals, index1, dataFolder)
vals2 = getVals(params, vals, index2, dataFolder)
vals3 = getVals(params, vals, index3, dataFolder)
vals4 = getVals(params, vals, index4, dataFolder)

# fig = plt.figure()
fig, axs = plt.subplots(len(vals3), len(vals4), sharex=True, sharey=True)
for row in vals3:
	for col in vals4:
		data = np.zeros((len(vals1), len(vals2)))

		for i in vals1:
			for j in vals2:
				vals[index1] = i
				vals[index2] = j
				vals[index3] = row
				vals[index4] = col
				path = dataFolder+makePath(params, vals, 0, len(params))
				success = 0
				for trial in os.listdir(path):
					table = pd.read_table(path+trial)
					if (checkSuccess(table)):
						success = success + 1
					# else:
					# 	print (path + trial)
				data[vals1.index(i), vals2.index(j)] = success*100.0/len(os.listdir(path))
					
					
		np.random.seed(11)
		N = len(vals1)
		ind = np.arange(N)
		width = (0.8)/len(vals2) #0.27    

		rects = []
		for i in range(len(vals2)):
			rects.append(axs[vals3.index(row),vals4.index(col)].bar(ind+width*i, data[:,i], width, color=np.random.rand(3,)))

		axs[vals3.index(row),vals4.index(col)].set_ylabel('Success rate')
		axs[vals3.index(row),vals4.index(col)].set_xticks(ind+width)
		axs[vals3.index(row),vals4.index(col)].set_xlabel(param_to_vary_1)
		axs[vals3.index(row),vals4.index(col)].set_xticklabels( vals1 )
		axs[vals3.index(row),vals4.index(col)].legend( rects, vals2 )
plt.show()
