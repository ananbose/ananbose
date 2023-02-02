// DFS algorithm in C++

#include <iostream>
#include <map>
#include<vector>
using namespace std;

class Graph {
    //public:
    map<int, vector<int>> adj_list;
    map<int,bool> visited; 

    public:
    void addEdge(int src, int dest);
    void DFS(int vertex);
};


// Add edges
void Graph::addEdge(int src, int dest) {
  adj_list[src].push_back(dest);
}

// DFS algorithm
void Graph::DFS(int vertex) {
  visited[vertex] = true;

  cout << vertex << " ";

  for (auto neighbour : adj_list[vertex]) {
    if (visited[neighbour] == false) DFS(neighbour);
  }
}

int main() {
  //Graph g(4);
  Graph g;
  g.addEdge(0, 1);
  g.addEdge(0, 2);
  g.addEdge(1, 2);
  g.addEdge(2, 3);

  g.DFS(0);

  return 0;
}