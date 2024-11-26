#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>

using namespace std;
//--------------------------------------------------------------------------------------------
// UNION FIND for Alghorithm Kuraskul struct data
// // MakeSet
// Time: O(numOfElements); Space O(1)
void makeSet(vector<int>& parent, vector<int>& size, int numOfElements) {
    parent.resize(numOfElements);
    size.resize(numOfElements, 1);
    for (int i = 0; i < numOfElements; i++) {
        parent[i] = i;
    }
}

// Find
// Time: O(log(n)); Space: O(1)
int find(vector<int>& parent, int node) {
    while (node != parent[node]) {
        parent[node] = parent[parent[node]];
        node = parent[node];
    }
    return node;
}

// Time: O(1); Space: O(1)
void Union(vector<int>& parent, vector<int>& size, int node1, int node2) {
    int root1 = find(parent, node1);
    int root2 = find(parent, node2);

    // already in the same set
    if (root1 == root2) {
        return;
    }

    if (size[root1] > size[root2]) {
        parent[root2] = root1;
        size[root1] += 1;
    }
    else {
        parent[root1] = root2;
        size[root2] += 1;
    }
}
//--------------------------------------------------------------------------------------------
//--------------
bool compareEdges(pair<pair<int, int>, int>& a, pair<pair<int, int>, int>& b) {
    return a.second < b.second;
}
// -------------
//--------------------------------------------------------------------------------------------
// Algorithm Kuraskul
int Kuraskul(int numOfElements, vector<pair<pair<int, int>, int>>& edges) {
    vector<int> parent, size;
    // union find start
    makeSet(parent, size, numOfElements);

    // sorting edges 
    sort(edges.begin(), edges.end(), compareEdges);

    int totalWeight = 0;
    for (auto& edge : edges) {
        int ver1 = edge.first.first;
        int ver2 = edge.first.second;
        int weight = edge.second;

        //if its not a loop then do union
        if (find(parent, ver1) != find(parent, ver2)) {
            Union(parent, size, ver1, ver2);
            totalWeight += weight;
            cout << "Edge included: (" << ver1 << ", " << ver2 << ") with weight " << weight << '\n';
        }
    }
    return totalWeight;
}
//--------------------------------------------------------------------------------------------
vector<pair<pair<int, int>, int>> fill2DVectorFromFile(const string& filename) {
    vector<pair<pair<int, int>, int>> edges;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "File opening error" << endl;
        return edges;
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        int u, v, weight;
        if (iss >> u >> v >> weight) {
            edges.push_back({ {u, v}, weight });
        }
    }

    file.close();
    return edges;
}

void printEdges(vector<pair<pair<int, int>, int>>& edges) {
    cout << "Edges: " << '\n';
    for (auto& edge : edges) {
        cout << "(" << edge.first.first << ", " << edge.first.second << ") = " << edge.second << '\n';
    }
}

int main() {
    int numOfElements = 9;
	string filename = "Matrix.txt";
    vector<pair<pair<int, int>, int>> edges = fill2DVectorFromFile(filename);
    printEdges(edges);

    if (!edges.empty()) {
        int totalWeight = Kuraskul(numOfElements, edges);
        cout << "Total weight: " << totalWeight << '\n';
    }
    else {
        cout << "No edges found or error reading file" << '\n';
    }
	return 0;
}