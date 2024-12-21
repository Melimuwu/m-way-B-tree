#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

// 定義樹節點結構
struct Node {
    vector<int> keys;
    vector<Node*> children;
    bool isLeaf;

    Node(bool leaf) : isLeaf(leaf) {}
};

// M-way 搜尋樹類別
class MWayTree {
private:
    Node* root;
    int m;

    void splitChild(Node* parent, size_t index);
    void insertNonFull(Node* node, int key);
    void remove(Node* node, int key);

public:
    MWayTree(int m) : root(nullptr), m(m) {}
    void insert(int key);
    void remove(int key);
    void visualizeTree(const string& filename);
    void treeToArray(Node* node, vector<int>& arr, size_t index);
    Node* getRoot() { return root; }
};

// B-tree 類別
class BTree {
private:
    Node* root;
    int degree;

    void splitChild(Node* parent, size_t index);
    void insertNonFull(Node* node, int key);
    void remove(Node* node, int key);

public:
    BTree(int degree) : root(nullptr), degree(degree) {}
    void insert(int key);
    void remove(int key);
    void visualizeTree(const string& filename);
    void treeToArray(Node* node, vector<int>& arr, size_t index);
    Node* getRoot() { return root; }
};

// MWayTree 類別的函式定義
void MWayTree::splitChild(Node* parent, size_t index) {
    Node* child = parent->children[index];
    Node* newChild = new Node(child->isLeaf);
    newChild->keys.assign(child->keys.begin() + m / 2, child->keys.end());
    child->keys.resize(m / 2);

    if (!child->isLeaf) {
        newChild->children.assign(child->children.begin() + m / 2, child->children.end());
        child->children.resize(m / 2);
    }

    parent->children.insert(parent->children.begin() + index + 1, newChild);
    parent->keys.insert(parent->keys.begin() + index, child->keys[m / 2 - 1]);
    child->keys.pop_back();
}

void MWayTree::insertNonFull(Node* node, int key) {
    int i = node->keys.size() - 1;
    if (node->isLeaf) {
        node->keys.push_back(0);
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
    }
    else {
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        i++;
        if (node->children[i]->keys.size() == m - 1) {
            splitChild(node, i);
            if (key > node->keys[i]) {
                i++;
            }
        }
        insertNonFull(node->children[i], key);
    }
}

void MWayTree::remove(Node* node, int key) {
    // 刪除節點的邏輯
}

void MWayTree::insert(int key) {
    if (root == nullptr) {
        root = new Node(true);
        root->keys.push_back(key);
    }
    else {
        if (root->keys.size() == m - 1) {
            Node* newRoot = new Node(false);
            newRoot->children.push_back(root);
            splitChild(newRoot, 0);
            insertNonFull(newRoot, key);
            root = newRoot;
        }
        else {
            insertNonFull(root, key);
        }
    }
}

void MWayTree::remove(int key) {
    if (root != nullptr) {
        remove(root, key);
        if (root->keys.empty()) {
            Node* oldRoot = root;
            if (root->isLeaf) {
                root = nullptr;
            }
            else {
                root = root->children[0];
            }
            delete oldRoot;
        }
    }
}

// BTree 類別的函式定義
void BTree::splitChild(Node* parent, size_t index) {
    Node* child = parent->children[index];
    Node* newChild = new Node(child->isLeaf);
    newChild->keys.assign(child->keys.begin() + degree, child->keys.end());
    child->keys.resize(degree - 1);

    if (!child->isLeaf) {
        newChild->children.assign(child->children.begin() + degree, child->children.end());
        child->children.resize(degree);
    }

    parent->children.insert(parent->children.begin() + index + 1, newChild);
    parent->keys.insert(parent->keys.begin() + index, child->keys[degree - 1]);
    child->keys.pop_back();
}

void BTree::insertNonFull(Node* node, int key) {
    int i = node->keys.size() - 1;
    if (node->isLeaf) {
        node->keys.push_back(0);
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
    }
    else {
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        i++;
        if (node->children[i]->keys.size() == 2 * degree - 1) {
            splitChild(node, i);
            if (key > node->keys[i]) {
                i++;
            }
        }
        insertNonFull(node->children[i], key);
    }
}

void BTree::remove(Node* node, int key) {
    // 刪除節點的邏輯
}

void BTree::insert(int key) {
    if (root == nullptr) {
        root = new Node(true);
        root->keys.push_back(key);
    }
    else {
        if (root->keys.size() == 2 * degree - 1) {
            Node* newRoot = new Node(false);
            newRoot->children.push_back(root);
            splitChild(newRoot, 0);
            insertNonFull(newRoot, key);
            root = newRoot;
        }
        else {
            insertNonFull(root, key);
        }
    }
}

void BTree::remove(int key) {
    if (root != nullptr) {
        remove(root, key);
        if (root->keys.empty()) {
            Node* oldRoot = root;
            if (root->isLeaf) {
                root = nullptr;
            }
            else {
                root = root->children[0];
            }
            delete oldRoot;
        }
    }
}

// MWayTree 和 BTree 類別的 treeToArray 方法
void MWayTree::treeToArray(Node* node, vector<int>& arr, size_t index) {
    if (node == nullptr) return;
    if (index >= arr.size()) arr.resize(index + 1, -1);
    arr[index] = node->keys[0];
    for (size_t i = 1; i < node->keys.size(); ++i) {
        arr.push_back(node->keys[i]);
    }
    for (size_t i = 0; i < node->children.size(); ++i) {
        treeToArray(node->children[i], arr, 2 * index + 1 + i);
    }
}

void BTree::treeToArray(Node* node, vector<int>& arr, size_t index) {
    if (node == nullptr) return;
    if (index >= arr.size()) arr.resize(index + 1, -1);
    arr[index] = node->keys[0];
    for (size_t i = 1; i < node->keys.size(); ++i) {
        arr.push_back(node->keys[i]);
    }
    for (size_t i = 0; i < node->children.size(); ++i) {
        treeToArray(node->children[i], arr, 2 * index + 1 + i);
    }
}

// MWayTree 和 BTree 類別的 visualizeTree 方法
void MWayTree::visualizeTree(const string& filename) {
    cout << "digraph G {\n";
    // 這裡應該添加樹的可視化邏輯
    cout << "}\n";
}

void BTree::visualizeTree(const string& filename) {
    cout << "digraph G {\n";
    // 這裡應該添加樹的可視化邏輯
    cout << "}\n";
}

// 主函數
int main() {
    int m, degree;
    cout << "請輸入M-way搜尋樹的M值: ";
    cin >> m;
    cout << "請輸入B-tree的度數: ";
    cin >> degree;

    MWayTree mwayTree(m);
    BTree bTree(degree);

    cout << "請輸入整數值（以空格分隔，輸入-1結束）: ";
    vector<int> keys;
    int key;
    while (cin >> key && key != -1) {
        keys.push_back(key);
    }

    for (int key : keys) {
        mwayTree.insert(key);
        bTree.insert(key);
    }

    vector<int> mwayTreeArray;
    mwayTree.treeToArray(mwayTree.getRoot(), mwayTreeArray, 0);
    cout << "M-Way Tree:" << endl;
    for (int val : mwayTreeArray) {
        if (val != -1) {
            cout << val << " ";
        }
    }
    cout << endl;

    vector<int> bTreeArray;
    bTree.treeToArray(bTree.getRoot(), bTreeArray, 0);
    cout << "B-Tree:" << endl;
    for (int val : bTreeArray) {
        if (val != -1) {
            cout << val << " ";
        }
    }
    cout << endl;

    cout << "M-Way Tree Visualization:" << endl;
    mwayTree.visualizeTree("");

    cout << "B-Tree Visualization:" << endl;
    bTree.visualizeTree("");

    return 0;
}
