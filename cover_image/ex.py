W = 530
P = (101, 101, 111, 111, 121, 121, 130, 141, 151, 160, 170, 180, 180, 190, 200)

A = [[0 for j in range(W+1)] for i in range(len(P)+1)]
for j in range(W+1):
  if (j < P[0]):
    A[1][j] = 0
  else:
    A[1][j] = P[0]

def way(i, j):
  print i, j
  if (i == 1 and A[i][j] == 0):
    return
  if (i == 1 or A[i][j] != A[i-1][j]):
    way(i, j-P[i-1])
    print P[i-1],
  else:
    way(i-1, j)

for i in range(2,len(P)+1):
  for j in range(1,W+1):
    if (j - P[i-1] >= 0):
      if (A[i-1][j] > A[i-1][j-P[i-1]] + P[i-1]):
        A[i][j] = A[i-1][j]
      else:
        A[i][j] = A[i-1][j-P[i-1]] + P[i-1]
    else:
      A[i][j] = A[i-1][j]

way(len(P), W)
