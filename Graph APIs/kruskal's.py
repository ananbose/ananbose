'''
There are n cities labeled from 1 to n. You are given the integer n and an array connections where connections[i] = [xi, yi, costi] indicates that the cost of connecting city xi and city yi (bidirectional connection) is costi.

Return the minimum cost to connect all the n cities such that there is at least one path between each pair of cities. If it is impossible to connect all the n cities, return -1,

The cost is the sum of the connections' costs used.
'''

import heapq
class UnionFind:
    def __init__(self, n):
        self.parent = list(range(n))
        print("PARENT",self.parent)
        self.rank = [0] * n

    def find(self, x):
        print("x", x)
        if self.parent[x] != x:
            self.parent[x] = self.find(self.parent[x])
        return self.parent[x]

    def union(self, x, y):
        px, py = self.find(x), self.find(y)
        if px != py:
            if self.rank[px] > self.rank[py]:
                self.parent[py] = px
            elif self.rank[py] > self.rank[px]:
                self.parent[px] = py
            else:
                self.parent[py] = px
                self.rank[px] += 1

    def is_connected(self, x, y):
        return self.find(x) == self.find(y)
class Solution:
    def minimumCost(self, n: int, connections: List[List[int]]) -> int:
        minHeap =[]
        for i in connections:
            heapq.heappush(minHeap,(i[2],i))
        uf = UnionFind(n)
        tot_cost = 0
        mst =[]
        while(minHeap and len(mst)<n-1):
            element = heapq.heappop(minHeap)[1]
            print(element)
            if not(uf.is_connected(element[0]-1,element[1]-1)):
                tot_cost = tot_cost + element[2]
                mst.append([element[0], element[1]])
                uf.union(element[0]-1, element[1]-1)
        print(mst)
        if len(mst)<n-1:
            return -1
        else:
            return tot_cost

