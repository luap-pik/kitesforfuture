import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import matplotlib
matplotlib.use('qt5agg')

from itertools import count
import pandas as pd 
import numpy as np
from collections import deque
from io import StringIO
import argparse
 
# create parser
parser = argparse.ArgumentParser()
 
# add arguments to the parser
parser.add_argument("--fname", default=r'out.txt', type=np.str)
parser.add_argument("--nrows", default=100, type=np.int)
parser.add_argument("--fullscreen", default=0, type=np.int, choices=[0, 1])
parser.add_argument('-v','--vars', nargs='+', default=None)

# parse the arguments
args = parser.parse_args()

print(args)

ncols = 10
variables = ["var " + str(i) for i in range(ncols)]

if not args.vars is None:
	for i, v in enumerate(args.vars):
		variables[i] = v


fig, ax = plt.subplots() 
if args.fullscreen == 1:
	fig.canvas.manager.full_screen_toggle() # toggle fullscreen mode


index = count()


def animate(i):
	with open(args.fname, 'r') as f:
	    q = deque(f, args.nrows)  # replace 2 with n (lines read at the end)

	df =  pd.read_csv(StringIO(''.join(q)), header=None, names=variables)
	#print(df.tail())

	plt.cla()
	df.plot(ax=ax, ls="-", marker="o", ms=2, lw=1)
	plt.xlabel('time')
	plt.title(i)
	plt.gcf().autofmt_xdate()
	plt.tight_layout()

ani = FuncAnimation(plt.gcf(), animate, 500)

plt.tight_layout()
plt.show()
