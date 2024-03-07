
import pandas as pd
import matplotlib.pyplot as plt


s = "/home/hate-walkin/explo-results/explo/3000_large"
fabc = s + "/output_abc.txt"
fabc_tree = s + "/output_abc_tree.txt"
fbpso_tree = s + "/output_bpso_tree.txt"

abc = []
abc_tree = []
bpso_tree = []


def func(s,ls):
	file = open(s,'r')
	cnt = 0
	for i in file:
		if(cnt == 0):
			cnt += 1
		elif(cnt == 1):
			s = i.split('\n')
			s = s[0].split(':')
			s = s[1].split(',')
			s = s[0].split(' ')
			util = int(s[1])
			cnt += 1
		else:
			cnt = 0
			s = i.split('\n')
			s = s[0].split(':')
			s = s[1].split(',')
			s = s[0].split(' ')
			s = s[1:]
			ls.append([util,len(s)])
	file.close()







func(fabc,abc)
func(fabc_tree,abc_tree)
func(fbpso_tree,bpso_tree)


xabc = []
yabc = []
xabc_tree = []
yabc_tree = []
xbpso_tree = []
ybpso_tree = []

def g(ls,t,l):
	n = len(ls)
	j = 0
	for i in range(1,400000):
		while(j < len(ls) and ls[j][0] < i):
			j += 1
			n -= 1
		l.append(i)
		t.append(n)		


g(abc,yabc,xabc)
g(abc_tree,yabc_tree,xabc_tree)
g(bpso_tree,ybpso_tree,xbpso_tree)

plt.xlabel('Minimum - Utility')
plt.ylabel('Number of HUIs')
plt.title('Number of HUIs vs Minimum Utility (3000_large)')



plt.plot(xabc,yabc,label = 'ABC-HUIM')
plt.plot(xabc_tree,yabc_tree,label = 'ABC-TREE')
plt.plot(xbpso_tree,ybpso_tree,label = 'BPSO-TREE')
plt.legend()
plt.grid()
plt.savefig(s + "/3000_large.png", dpi=300)	

