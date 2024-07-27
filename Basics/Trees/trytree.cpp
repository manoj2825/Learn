#include <iostream>
#include <vector>
#include <queue>
#define nullptr NULL

using namespace std;

class Node
{
    public:
        int data;
        Node* left;
        Node* right;

        Node(int val):data(val),left(NULL),right(NULL){}
};

void printTree(Node* root)
{
    if(root == NULL)
    {
        return;
    }

    printTree(root->left);
    cout<<root->data<<" ";
    printTree(root->right);
    
}

void levelOrder(Node* root)
{
    if(root ==nullptr)
    {
        return;
    }

    //lets create a queue
    queue<Node*> q;
    q.push(root);
    q.push(NULL);
    //q.push(nullptr);//to print the line
    while (!q.empty())
    {
        Node *current = q.front();
        q.pop();
        if (current == NULL)
        {
            cout << endl;
            if (q.empty())
            {
                break;
            }
            else
            {
                q.push(NULL);
            }
        }
        else
        {
            cout << current->data << " ";
            if (current->left != nullptr)
            {
                q.push(current->left);
            }
            if (current->right != nullptr)
            {
                q.push(current->right);
            }
        }
    }
}

int main() {

    vector<int> arr ={1,2,3,4,5};

    Node* root =new Node(arr[0]);
    root->left =new Node(arr[1]);
    root->right= new Node(arr[4]);
    root->left->left= new Node(arr[2]);
    root->left->right= new Node(arr[3]);

   // printTree(root);//inorder left root right
   levelOrder(root);//BFS
}