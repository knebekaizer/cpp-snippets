import sys

W = 530
mySet = (101, 111, 121, 130, 151, 160, 190)

T = sorted(mySet)
print T
N = len(T)


E = [[-1] * N] * (W+1)
for w in range(1, W+1):
	# the best fit starting from n
	for n in range(0,N):
		for k in range(n,N):
			if T[k] <= w:
			#	print w, n, k
				E[w][n] = k

for w in range(1, 10):
	for n in range(0,N):
		print E[w][n],
	print
sys.exit()
				
def fit(w, n):
	print w, n
	if w < 0: return
	if E[w][n] >= 0 and E[w][n] != E[w][n-1]:
		fit(w-E[w][n], n)
	else:
		fit(w, n-1)

fit(N, W)