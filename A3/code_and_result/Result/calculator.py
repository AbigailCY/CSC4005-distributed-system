from sympy import *
import random as ra
import numpy as np
import matplotlib.pyplot as plt
import math
import xlwt
from numpy.linalg import *
from scipy import stats
from scipy import io
import pandas as pd
from sklearn import preprocessing
from datetime import datetime as dt
from functools import reduce

# 如何写excel的例子
workbook = xlwt.Workbook(encoding = 'utf-8')
worksheet1 = workbook.add_sheet('result')
worksheet1.write_merge(
0,
0,
2,
5,
'num_bodies')
worksheet1.write(1,0,'method')
worksheet1.write(1,1,'process')
worksheet1.write(1,2,'200')
worksheet1.write(1,3,'800')
worksheet1.write(1,4,'2000')
worksheet1.write(1,5,'5000')
filename="/Users/rolling/Desktop/CSC4005/A3/code_and_result/Result/seq.txt"
filename1="/Users/rolling/Desktop/CSC4005/A3/code_and_result/Result/mpi.txt"
filename2="/Users/rolling/Desktop/CSC4005/A3/code_and_result/Result/pth.txt"
filename3="/Users/rolling/Desktop/CSC4005/A3/code_and_result/Result/openmp.txt"
mpi=[]
thd = []
num=[]
runtime=[]
writting_num=0
with open(filename, 'r') as file_to_read:
    while True:
        lines=file_to_read.readline()
        # print(lines)
        if not lines:
            break
        if len(lines)>10:
            pos=lines.find(":")+1
            posb=0
            pos1=[]
            pos2=[]
            for i in range(3):
                # print(pos)
                pos1.append(lines.find(":",pos+1))
                pos2.append(lines.find(";",posb+1))
                pos=lines.find(":",pos+1)
                posb=lines.find(";",pos+1)
            pos2[2]=len(lines)
            mpi.append(lines[:10])
            thd.append(1)
            num.append((int)(lines[pos1[0]+2:pos2[0]]))
            runtime.append((float)(lines[pos1[1]+2:pos2[1]]))
            # runtime.append(lines[pos1[2]+2:pos2[2]-1])
#     print(mpi)
#     print(thd)
#     print(num)
#     print(runtime)

with open(filename1, 'r') as file_to_read:
    while True:
        lines=file_to_read.readline()
        print(lines)
        if not lines:
            break
        if len(lines)>10:
            pos=lines.find(":")+1
            posb=0
            pos1=[]
            pos2=[]
            for i in range(4):
                # print(pos)
                pos1.append(lines.find(":",pos+1))
                pos2.append(lines.find(";",posb+1))
                pos=lines.find(":",pos+1)
                posb=lines.find(";",pos+1)
            pos2[3]=len(lines)
            mpi.append(lines[:3])
            thd.append((int)(lines[pos1[0]+2:pos2[0]]))
            num.append((int)(lines[pos1[1]+2:pos2[1]]))
            runtime.append((float)(lines[pos1[2]+2:pos2[2]-1]))

with open(filename2, 'r') as file_to_read:
    while True:
        lines=file_to_read.readline()
        print(lines)
        if not lines:
            break
        if len(lines)>10:
            pos=lines.find(":")+1
            posb=0
            pos1=[]
            pos2=[]
            for i in range(4):
                # print(pos)
                pos1.append(lines.find(":",pos+1))
                pos2.append(lines.find(";",posb+1))
                pos=lines.find(":",pos+1)
                posb=lines.find(";",pos+1)
            pos2[3]=len(lines)
            mpi.append(lines[:7])
            thd.append((int)(lines[pos1[0]+2:pos2[0]]))
            num.append((int)(lines[pos1[1]+2:pos2[1]]))
            runtime.append((float)(lines[pos1[2]+2:pos2[2]-1]))
with open(filename3, 'r') as file_to_read:
    while True:
        lines=file_to_read.readline()
        print(lines)
        if not lines:
            break
        if len(lines)>10:
            pos=lines.find(":")+1
            posb=0
            pos1=[]
            pos2=[]
            for i in range(4):
                # print(pos)
                pos1.append(lines.find(":",pos+1))
                pos2.append(lines.find(";",posb+1))
                pos=lines.find(":",pos+1)
                posb=lines.find(";",pos+1)
            pos2[3]=len(lines)
            mpi.append(lines[:6])
            thd.append((int)(lines[pos1[0]+2:pos2[0]]))
            num.append((int)(lines[pos1[1]+2:pos2[1]]))
            runtime.append((float)(lines[pos1[2]+2:pos2[2]-1]))
    print(mpi)
    print(thd)
    print(num)
    print(runtime)
# mpi = np.array(mpi)
# thd = np.array(thd)
# num = np.array(num)
# runtime = np.array(runtime)
# df = pd.DataFrame({'200':1.,
#                 '800':pd.Timestamp('20190114'),
#                 '2000':np.array([3]*4,dtype='int32'),
#                 '':pd.Categorical(["test","train","test","train"]),
#                 'F':'foo',
#                 'G':pd.Series([1,2,3,4])
# }, intex = [mpi, thd])
for i in range(len(mpi)):
    worksheet1.write(i+2,0,mpi[i])
    worksheet1.write(i+2,1,thd[i])
    if num[i]==200:
        worksheet1.write(i+2,2,runtime[i])
    elif num[i]==800:
        worksheet1.write(i+2,3,runtime[i])
    elif num[i]==2000:
        worksheet1.write(i+2,4,runtime[i])
    else:
        worksheet1.write(i+2,5,runtime[i])
    # writting_num=len(mpi_d)+5-3
workbook.save('/Users/rolling/Desktop/CSC4005/A3/code_and_result/Result/result.xls')







