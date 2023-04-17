'''
You are given an array points representing integer coordinates of some points on a 2D-plane, where points[i] = [xi, yi].

The cost of connecting two points [xi, yi] and [xj, yj] is the manhattan distance between them: |xi - xj| + |yi - yj|, where |val| denotes the absolute value of val.

Return the minimum cost to make all points connected. All points are connected if there is exactly one simple path between any two points.
'''
from typing import List
import heapq

class Solution:
    def minCostConnectPoints(self, points: List[List[int]]) -> int:
        """
        :type points: List[List[int]]
        :rtype: int
        """
        # Initialize variables
        n = len(points)
        minHeap =[]
        def add_edge_to_Heap(v):
            for i in range(n):
                if i==v:
                    continue
                cost = abs(points[v][0]-points[i][0]) + abs(points[v][1]-points[i][1])
                
                heapq.heappush(minHeap, (cost ,i))
        tot_cost = 0
        visited=set()
        visited.add(0)
        add_edge_to_Heap(0)
        while(minHeap and len(visited) < n):
            cost , vertex = heapq.heappop(minHeap)
            if vertex  in visited :
                continue
            if vertex not in visited:
                add_edge_to_Heap(vertex)
                visited.add(vertex)
                tot_cost = tot_cost + cost
        return tot_cost