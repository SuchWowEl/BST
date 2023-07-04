#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>

using namespace std;

// Create a node
class BTreeNode
{
public:
    bool leaf;
    vector<pair<int, int>> keys;
    vector<BTreeNode *> child;

    BTreeNode(bool isLeaf = false)
    {
        leaf = isLeaf;
    }
};

// Tree
class BTree
{
public:
    BTreeNode *root;

    BTree()
    {
        root = new BTreeNode(true);
    }

    // Insert node
    void insert(pair<int, int> k)
    {
        BTreeNode *rootNode = root;
        if (rootNode->keys.size() == 3)
        {
            BTreeNode *temp = new BTreeNode();
            root = temp;
            temp->child.push_back(rootNode);
            splitChild(temp, 0);
            insertNonFull(temp, k);
        }
        else
        {
            insertNonFull(rootNode, k);
        }
    }

    // Insert non-full
    void insertNonFull(BTreeNode *x, pair<int, int> k)
    {
        int i = x->keys.size() - 1;
        if (x->leaf)
        {
            x->keys.push_back(k);
            sort(x->keys.begin(), x->keys.end());
        }
        else
        {
            while (i >= 0 && k.first < x->keys[i].first)
            {
                i--;
            }
            i++;
            if (x->child[i]->keys.size() == 3)
            {
                splitChild(x, i);
                if (k.first > x->keys[i].first)
                {
                    i++;
                }
            }
            insertNonFull(x->child[i], k);
        }
    }

    // Split the child
    void splitChild(BTreeNode *x, int i)
    {
        BTreeNode *y = x->child[i];
        BTreeNode *z = new BTreeNode(y->leaf);
        x->child.insert(x->child.begin() + i + 1, z);
        x->keys.insert(x->keys.begin() + i, y->keys[1]);
        z->keys = vector<pair<int, int>>(y->keys.begin() + 2, y->keys.end());
        y->keys = vector<pair<int, int>>(y->keys.begin(), y->keys.begin() + 1);
        if (!y->leaf)
        {
            z->child = vector<BTreeNode *>(y->child.begin() + 2, y->child.end());
            y->child = vector<BTreeNode *>(y->child.begin(), y->child.begin() + 2);
        }
    }

    // Print the tree
    void printTree(BTreeNode *x, int l = 0)
    {
        cout << "Level " << l << " " << x->keys.size() << ": ";
        for (const auto &key : x->keys)
        {
            cout << "(" << key.first << ", " << key.second << ") ";
        }
        cout << endl;
        l++;
        if (!x->leaf)
        {
            for (auto child : x->child)
            {
                printTree(child, l);
            }
        }
    }

    // Search for a key in the tree
    bool searchKey(BTreeNode *x, int key)
    {
        int i = 0;
        while (i < x->keys.size() && key > x->keys[i].first)
        {
            i++;
        }

        if (i < x->keys.size() && key == x->keys[i].first)
        {
            return true;
        }
        else if (x->leaf)
        {
            return false;
        }
        else
        {
            return searchKey(x->child[i], key);
        }
    }

    // Remove a key from the tree
    void removeKey(BTreeNode *x, int key)
    {
        int i = 0;
        while (i < x->keys.size() && key > x->keys[i].first)
        {
            i++;
        }

        if (i < x->keys.size() && key == x->keys[i].first)
        {
            // Key found in the current node
            if (x->leaf)
            {
                // Key is in a leaf node, simply remove it
                x->keys.erase(x->keys.begin() + i);
            }
            else
            {
                // Key is in an internal node
                BTreeNode *predChild = x->child[i];
                BTreeNode *succChild = x->child[i + 1];

                if (predChild->keys.size() >= 2)
                {
                    // If the predecessor child has enough keys, find the predecessor and replace the key
                    pair<int, int> predKey = findPredecessor(predChild);
                    x->keys[i] = predKey;
                    removeKey(predChild, predKey.first);
                }
                else if (succChild->keys.size() >= 2)
                {
                    // If the successor child has enough keys, find the successor and replace the key
                    pair<int, int> succKey = findSuccessor(succChild);
                    x->keys[i] = succKey;
                    removeKey(succChild, succKey.first);
                }
                else
                {
                    // Both predecessor and successor children have only 1 key
                    // Merge the key and its two children
                    mergeNodes(x, i);
                    removeKey(predChild, key);
                }
            }
        }
        else
        {
            // Key not found in the current node
            if (x->leaf)
            {
                // Key is not in the tree
                cout << "Key " << key << " not found in the tree." << endl;
                return;
            }

            BTreeNode *childNode = x->child[i];
            if (childNode->keys.size() < 2)
            {
                // Ensure the child node has at least 2 keys before descending into it
                if (i > 0 && x->child[i - 1]->keys.size() >= 2)
                {
                    // Borrow a key from the left sibling
                    borrowFromLeftSibling(x, i);
                }
                else if (i < x->child.size() - 1 && x->child[i + 1]->keys.size() >= 2)
                {
                    // Borrow a key from the right sibling
                    borrowFromRightSibling(x, i);
                }
                else
                {
                    // Merge with the left or right sibling
                    if (i > 0)
                    {
                        mergeNodes(x, i - 1);
                        childNode = x->child[i - 1];
                    }
                    else
                    {
                        mergeNodes(x, i);
                    }
                }
            }

            removeKey(childNode, key);
        }
    }

    // Find the predecessor key in the subtree
    pair<int, int> findPredecessor(BTreeNode *x)
    {
        if (x->leaf)
        {
            return x->keys.back();
        }
        else
        {
            return findPredecessor(x->child.back());
        }
    }

    // Find the successor key in the subtree
    pair<int, int> findSuccessor(BTreeNode *x)
    {
        if (x->leaf)
        {
            return x->keys.front();
        }
        else
        {
            return findSuccessor(x->child.front());
        }
    }

    // Borrow a key from the left sibling
    void borrowFromLeftSibling(BTreeNode *x, int idx)
    {
        BTreeNode *childNode = x->child[idx];
        BTreeNode *leftSibling = x->child[idx - 1];

        childNode->keys.insert(childNode->keys.begin(), x->keys[idx - 1]);
        x->keys[idx - 1] = leftSibling->keys.back();
        leftSibling->keys.pop_back();

        if (!childNode->leaf)
        {
            childNode->child.insert(childNode->child.begin(), leftSibling->child.back());
            leftSibling->child.pop_back();
        }
    }

    // Borrow a key from the right sibling
    void borrowFromRightSibling(BTreeNode *x, int idx)
    {
        BTreeNode *childNode = x->child[idx];
        BTreeNode *rightSibling = x->child[idx + 1];

        childNode->keys.push_back(x->keys[idx]);
        x->keys[idx] = rightSibling->keys.front();
        rightSibling->keys.erase(rightSibling->keys.begin());

        if (!childNode->leaf)
        {
            childNode->child.push_back(rightSibling->child.front());
            rightSibling->child.erase(rightSibling->child.begin());
        }
    }

    // Merge the current node with its left or right sibling
    void mergeNodes(BTreeNode *x, int idx)
    {
        BTreeNode *leftChild = x->child[idx];
        BTreeNode *rightChild = x->child[idx + 1];

        leftChild->keys.push_back(x->keys[idx]);
        leftChild->keys.insert(leftChild->keys.end(), rightChild->keys.begin(), rightChild->keys.end());

        if (!leftChild->leaf)
        {
            leftChild->child.insert(leftChild->child.end(), rightChild->child.begin(), rightChild->child.end());
        }

        x->keys.erase(x->keys.begin() + idx);
        x->child.erase(x->child.begin() + idx + 1);

        delete rightChild;
    }
};

int main()
{
    BTree bTree;
    int arr[] = {100, 500, 1000, 5000, 10000, 20000, 50000, 100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000};
    for (int i = 0; i < sizeof(arr) / sizeof(int); i++)
    {
        // vector<int> arr = {4, 6, 12, 15, 3, 5};
        int size = arr[i];
        int *dynamicArray = new int[size];
        for (int i = 0; i < size; i++)
        {
            dynamicArray[i] = rand() % size;
        }
        int find = dynamicArray[size / 2];

        for (int i = 0; i < size; i++)
        {
            bTree.insert(make_pair(dynamicArray[i], dynamicArray[i]));
            // bTree.printTree(bTree.root);
            // cout << "-------------------------------------------------" << endl;
        }
        cout << endl
             << "-----------------------" << arr[i] << "--------------------------" << endl;
        auto start1 = chrono::high_resolution_clock::now();
        // bTree.searchKey(root, find);
        bTree.searchKey(bTree.root, find);
        // cout << "wow " << search(root, find) << endl;
        auto stop1 = chrono::high_resolution_clock::now();
        auto duration1 = chrono::duration_cast<std::chrono::microseconds>(stop1 - start1);

        auto start2 = chrono::high_resolution_clock::now();
        bTree.insert(make_pair(size / 2, size / 2));
        // root = insertNode(root, size / 2);
        auto stop2 = chrono::high_resolution_clock::now();
        auto duration2 = chrono::duration_cast<std::chrono::microseconds>(stop2 - start2);

        auto start3 = chrono::high_resolution_clock::now();
        bTree.removeKey(bTree.root, find);
        // root = deleteNode(root, find);
        auto stop3 = chrono::high_resolution_clock::now();
        auto duration3 = chrono::duration_cast<std::chrono::microseconds>(stop3 - start3);

        cout << "search: " << (double)duration1.count() << endl
             << "insert: " << (double)duration2.count() << endl
             << "delete: " << (double)duration3.count();

        delete dynamicArray;
    }

    return 0;
}
