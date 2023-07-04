// Binary Search Tree operations in C++

#include <iostream>
#include <chrono>
using namespace std;

struct node
{
    int key;
    struct node *left, *right;
};

// Create a node
struct node *newNode(int item)
{
    struct node *temp = (struct node *)malloc(sizeof(struct node));
    temp->key = item;
    temp->left = temp->right = NULL;
    return temp;
}

// Inorder Traversal
void inorder(struct node *root)
{
    if (root != NULL)
    {
        // Traverse left
        inorder(root->left);

        // Traverse root
        cout << root->key << " -> ";

        // Traverse right
        inorder(root->right);
    }
}

// Insert a node
struct node *insert(struct node *node, int key)
{
    // Return a new node if the tree is empty
    if (node == NULL)
        return newNode(key);

    // Traverse to the right place and insert the node
    if (key < node->key)
        node->left = insert(node->left, key);
    else
        node->right = insert(node->right, key);

    return node;
}

// Find the inorder successor
struct node *minValueNode(struct node *node)
{
    struct node *current = node;

    // Find the leftmost leaf
    while (current && current->left != NULL)
        current = current->left;

    return current;
}

// Deleting a node
struct node *deleteNode(struct node *root, int key)
{
    // Return if the tree is empty
    if (root == NULL)
        return root;

    // Find the node to be deleted
    if (key < root->key)
        root->left = deleteNode(root->left, key);
    else if (key > root->key)
        root->right = deleteNode(root->right, key);
    else
    {
        // If the node is with only one child or no child
        if (root->left == NULL)
        {
            struct node *temp = root->right;
            free(root);
            return temp;
        }
        else if (root->right == NULL)
        {
            struct node *temp = root->left;
            free(root);
            return temp;
        }

        // If the node has two children
        struct node *temp = minValueNode(root->right);

        // Place the inorder successor in position of the node to be deleted
        root->key = temp->key;

        // Delete the inorder successor
        root->right = deleteNode(root->right, temp->key);
    }
    return root;
}

// Find the inorder successor
bool search(struct node *node, int target)
{
    struct node *current = node;

    // Find the leftmost leaf
    bool isItEqual = false;
    while (true)
    {
        if (current == NULL)
        {
            // cout << "none ";
            break;
        }
        else if (current->key == target)
        {
            // cout << "found!";
            return true;
        }
        current = (target < current->key) ? current->left : current->right;
    }

    return isItEqual;
}

// Driver code
int main()
{
    /*
    struct node *root = NULL;
    root = insert(root, 8);
    root = insert(root, 3);
    root = insert(root, 1);
    root = insert(root, 6);
    root = insert(root, 7);
    root = insert(root, 10);
    root = insert(root, 14);
    root = insert(root, 4);
    // cout << "is there 10: " << search(root, 10) << endl;
    */

    int size = 1000000;
    int *dynamicArray = new int[size];
    for (int i = 0; i < size; i++)
    {
        dynamicArray[i] = rand() % size;
    }
    int find = dynamicArray[size / 2]; // use for find and delete

    cout << "start" << endl;
    struct node *root = NULL;
    for (int i = 0; i < size; i++)
    {
        root = insert(root, dynamicArray[i]);
    }
    cout << "end" << endl;

    auto start1 = chrono::high_resolution_clock::now();
    search(root, find);
    // cout << "wow " << search(root, find) << endl;
    auto stop1 = chrono::high_resolution_clock::now();
    auto duration1 = chrono::duration_cast<std::chrono::microseconds>(stop1 - start1);

    auto start2 = chrono::high_resolution_clock::now();
    root = insert(root, 10 / 2);
    auto stop2 = chrono::high_resolution_clock::now();
    auto duration2 = chrono::duration_cast<std::chrono::microseconds>(stop2 - start2);

    auto start3 = chrono::high_resolution_clock::now();
    root = deleteNode(root, find);
    auto stop3 = chrono::high_resolution_clock::now();
    auto duration3 = chrono::duration_cast<std::chrono::microseconds>(stop3 - start3);

    cout << "search: " << (double)duration1.count() << endl
         << "insert: " << (double)duration2.count() << endl
         << "delete: " << (double)duration3.count();

    /*
    cout << "Inorder traversal: ";
    inorder(root);

    cout << "\nAfter deleting 10\n";
    root = deleteNode(root, 10);
    cout << "Inorder traversal: ";
    inorder(root);
    */
}