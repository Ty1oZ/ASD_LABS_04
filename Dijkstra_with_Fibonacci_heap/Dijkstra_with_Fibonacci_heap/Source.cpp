#include <iostream>
#include <vector>
#include <unordered_map>
#include <limits>
#include <cmath>
using namespace std;

const int INF = numeric_limits<int>::max();

// Node of the Fibonacci heap
struct Node {
    int key;            // Node Key
    int vertex;         // The vertex of the graph associated with the node
    int degree;         // Number of child nodes
    Node* parent;       // Pointer to the parent node
    Node* child;        // Pointer to a child node
    Node* left;         // Pointer to the left node
    Node* right;        // Pointer to the right node
    bool mark;          // Node marker for cascading cropping

    // Node Constructor
    Node(int v, int k) : vertex(v), key(k), degree(0), parent(nullptr), child(nullptr), left(this), right(this), mark(false) {}
};

// Fibonacci heap
class FibonacciHeap {
private:
    Node* minNode;
    int nodeCount;     // Total number of nodes in the heap

    // Combining two doubly linked node lists
    void mergeLists(Node* a, Node* b) {
        // Checking for empty lists
        if (!a || !b) return;
        Node* temp = a->right;
        a->right = b->right;
        a->right->left = a;
        b->right = temp;
        b->right->left = b;
    }

    // Consolidating nodes in a heap to restore heap properties
    void consolidate() {
        // Maximum degree of nodes
        int maxDegree = static_cast<int>(log2(nodeCount)) + 1;
        // Table for nodes of each degree
        vector<Node*> degreeTable(maxDegree, nullptr);

        // Vector of root nodes
        vector<Node*> roots;
        // Starting from the minimum node
        Node* current = minNode;
        if (current) {
            do {
                roots.push_back(current);
                current = current->right;
            } while (current != minNode);
        }

        // Combining nodes with the same degree
        for (Node* root : roots) {
            Node* x = root;
            int degree = x->degree;

            while (degreeTable[degree]) {
                Node* y = degreeTable[degree];
                // Node exchange to maintain the heap property
                if (x->key > y->key) swap(x, y);
                linkNodes(y, x);
                degreeTable[degree] = nullptr;
                degree++;
            }
            degreeTable[degree] = x;
        }

        minNode = nullptr;
        for (Node* node : degreeTable) {
            if (node) {
                if (!minNode || node->key < minNode->key) {
                    minNode = node;
                }
            }
        }
    }

    // Linking two nodes (y becomes a child node of x)
    void linkNodes(Node* y, Node* x) {
        y->left->right = y->right;
        y->right->left = y->left;
        y->parent = x;
        y->left = y->right = y;
        y->mark = false;

        if (!x->child) {
            x->child = y;
        }
        else {
            // Combining y with the current children of x
            mergeLists(x->child, y);
        }
        x->degree++;
    }

    // Pruning a node from its parent
    void cut(Node* x, Node* y) {
        // If x is the only child, delete the reference to the child
        if (x->right == x) {
            y->child = nullptr;
        }
        else {
            x->right->left = x->left;
            x->left->right = x->right;
            if (y->child == x) {
                y->child = x->right;
            }
        }
        y->degree--;

        // Removing x from the list of children and creating a new single - node list
        x->left = x->right = x;
        x->parent = nullptr;
        x->mark = false;
        // Adding x to the root list
        mergeLists(minNode, x);
    }

    // Cascading pruning to restore heap properties
    void cascadingCut(Node* y) {
        Node* z = y->parent;
        if (z) {
            if (!y->mark) {
                y->mark = true;
            }
            else {
                // If the marker is already installed, we perform cropping
                cut(y, z);
                // Recursive call for the parent node
                cascadingCut(z);
            }
        }
    }

public:
    FibonacciHeap() : minNode(nullptr), nodeCount(0) {}

    // Inserting a new node
    Node* insert(int vertex, int key) {
        Node* newNode = new Node(vertex, key);
        // If the heap is empty, the new node becomes minimal
        if (!minNode) {
            minNode = newNode;
        }
        else {
            // Otherwise, add a new node to the root list
            mergeLists(minNode, newNode);
            if (key < minNode->key) {
                // Updating the minimum node if the new key is smaller
                minNode = newNode;
            }
        }
        nodeCount++;
        return newNode;
    }

    // Extracting a node with a minimum key
    Node* extractMin() {
        if (!minNode) return nullptr;

        Node* oldMin = minNode;
        if (oldMin->child) {
            Node* child = oldMin->child;
            do {
                // Removing the link to the parent node from children
                child->parent = nullptr;
                child = child->right;
            } while (child != oldMin->child);

            // Combining children with the root list
            mergeLists(minNode, oldMin->child);
        }

        oldMin->left->right = oldMin->right;
        oldMin->right->left = oldMin->left;

        if (oldMin == oldMin->right) {
            // If there is a single node, the heap becomes empty// If there is a single node, the heap becomes empty
            minNode = nullptr;
        }
        else {
            minNode = oldMin->right;
            consolidate();
        }

        nodeCount--;
        return oldMin;
    }

    // Reducing the node's key
    void decreaseKey(Node* node, int newKey) {
        if (newKey > node->key) {
            cout << "Error: the new key is larger than the current one" << endl;
            return;
        }

        node->key = newKey;
        Node* parent = node->parent;

        if (parent && node->key < parent->key) {
            cut(node, parent);
            cascadingCut(parent);
        }

        if (node->key < minNode->key) {
            minNode = node;
        }
    }

    bool isEmpty() {
        return minNode == nullptr;
    }
};

// Dijkstra's algorithm for finding the shortest path in a graph
void dijkstra(const vector<vector<int>>& matrix, int source) {
    int n = matrix.size();
    vector<int> dist(n, INF);            // Distances to the vertices
    vector<Node*> nodes(n, nullptr);     // Pointers to nodes
    FibonacciHeap heap;

    // Initialization
    dist[source] = 0;
    nodes[source] = heap.insert(source, 0);

    for (int i = 0; i < n; ++i) {
        if (i != source) {
            nodes[i] = heap.insert(i, INF);
        }
    }

    while (!heap.isEmpty()) {
        Node* minNode = heap.extractMin();
        int u = minNode->vertex;

        for (int v = 0; v < n; ++v) {
            if (matrix[u][v] != INF && dist[u] != INF && dist[u] + matrix[u][v] < dist[v]) {
                dist[v] = dist[u] + matrix[u][v];
                heap.decreaseKey(nodes[v], dist[v]);
            }
        }

        delete minNode;
    }

    // Output of results
    cout << "FROM VERTEX " << source << ":\n";
    for (int i = 0; i < n; ++i) {
        if (dist[i] == INF) {
            cout << "vertex " << i << ": Unattainable\n";
        }
        else {
            cout << "to vertex " << i << " minimum distance: " << dist[i] << '\n';
        }
    }
}

int main() {
    // The adjacency matrix of a graph for 9 vertices
    vector<vector<int>> matrix = {
        {0, 3, INF, INF, INF, INF, INF, 10, INF},
        {3, 0, 4, 5, 2, INF, INF, INF, INF},
        {INF, 4, 0, 6, INF, INF, INF, INF, INF},
        {INF, 5, 6, 0, 8, INF, 7, INF, INF},
        {INF, 2, INF, 8, 0, 5, INF, 9, INF},
        {INF, INF, 4, INF, 5, 0, 4, 8, 5},
        {INF, INF, INF, 7, INF, 4, 0, INF, 6},
        {10, INF, INF, INF, 9, 8, INF, 0, 9},
        {INF, INF, INF, INF, INF, 5, 6, 9, 0}
    };

   /* Example 2
   vector<vector<int>> matrix = {
       {INF, 7, 9, INF, INF, 14},
       {7, INF, 10, 15, INF, INF},
       {9, 10, INF, 11, INF, 2},
       {INF, 15, 11, INF, 6, INF},
       {INF, INF, INF, 6, INF, 9},
       {14, INF, 2, INF, 9, INF}
   };
   */

    int source = 2; // The initial vertex
    dijkstra(matrix, source);

    return 0;
}