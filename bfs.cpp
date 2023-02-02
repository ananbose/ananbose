#include <map>
#include <vector>
#include <queue>
#include <iostream>

using namespace std;

map<int, vector<int>> adjacencyList;
map<int, bool> visited;

// add an edge from node u to node v
void addEdge(int u, int v) {
    adjacencyList[u].push_back(v);
}

void BFS(int startNode) {
    queue <int> q;
    visited[startNode]=true;
    q.push(startNode);
    while(!q.empty()) {
        int ele=q.front();
        q.pop();
        for (auto i:adjacencyList[ele]){
            if (!visited[i]) {q.push(i);visited[i]=true;}
        }

    }
}

int main() {
    addEdge(1, 2);
    addEdge(1, 3);
    addEdge(2, 4);
    addEdge(2, 5);

    cout << "BFS starting from node 1: ";
    BFS(1);
    cout << endl;
    return 0;
}