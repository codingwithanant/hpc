#include <iostream>
#include <vector>
#include <queue>
#include <omp.h>

using namespace std;

class Graph {
    int V;
    vector<vector<int>> adj;

public:
    Graph(int vertices) {
        V = vertices;
        adj.resize(V);
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u); // undirected graph
    }

    // ---------------- PARALLEL BFS ----------------
    void parallelBFS(int source) {
        vector<bool> visited(V, false);
        queue<int> q;

        visited[source] = true;
        q.push(source);

        cout << "Parallel BFS: ";

        while (!q.empty()) {
            int levelSize = q.size();

            vector<int> nodes(levelSize);

            // Extract current level nodes
            for (int i = 0; i < levelSize; i++) {
                nodes[i] = q.front();
                q.pop();
            }

            // Process current level in parallel
            #pragma omp parallel for
            for (int i = 0; i < nodes.size(); i++) {
                int u = nodes[i];

                #pragma omp critical
                cout << u << " ";

                for (int v : adj[u]) {
                    if (!visited[v]) {
                        #pragma omp critical
                        {
                            if (!visited[v]) {
                                visited[v] = true;
                                q.push(v);
                            }
                        }
                    }
                }
            }
        }
        cout << endl;
    }

    // ---------------- PARALLEL DFS ----------------
    void dfsTask(int node, vector<bool> &visited) {
        #pragma omp critical
        {
            if (visited[node]) return;
            visited[node] = true;
            cout << node << " ";
        }

        for (int neighbor : adj[node]) {
            if (!visited[neighbor]) {
                #pragma omp task
                dfsTask(neighbor, visited);
            }
        }
    }

    void parallelDFS(int source) {
        vector<bool> visited(V, false);

        cout << "Parallel DFS: ";

        #pragma omp parallel
        {
            #pragma omp single
            dfsTask(source, visited);
        }

        cout << endl;
    }
};

int main() {
    int V, E;

    cout << "Enter number of vertices: ";
    cin >> V;

    Graph g(V);

    cout << "Enter number of edges: ";
    cin >> E;

    cout << "Enter edges (u v):\n";
    for (int i = 0; i < E; i++) {
        int u, v;
        cin >> u >> v;
        g.addEdge(u, v);
    }

    int start;
    cout << "Enter starting vertex: ";
    cin >> start;

    g.parallelBFS(start);
    g.parallelDFS(start);

    return 0;
}
