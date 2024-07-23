#include <iostream>
using namespace std;

class Node
{
    public:
        int data;
        Node* left;
        Node* right;

        Node(int value)
        {
            data =value;
            left = right =nullptr;
        }
};


void printTree(Node* root, int space = 0, int level = 0) {
    if (root == nullptr) {
        return;
    }

    // Increase distance between levels
    space += 4;

    // Process right child first
    printTree(root->right, space, level + 1);

    // Print current node after space count
    cout << endl;
    for (int i = 4; i < space; i++) {
        cout << " ";
    }
    cout << root->data << "\n";

    // Process left child
    printTree(root->left, space, level + 1);
}

int main() {
    
    Node* root =new Node(1);
    root->left =new Node(2);
    root->right =new Node(5);
    root->left->left =new Node(3);
    root->left->right =new Node(4);
    printTree(root);
}