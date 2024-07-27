#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
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


void levelOrder(Node* root)
{
    if(root ==nullptr) return;
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
                break;
            else
                q.push(NULL);
        }
        else
        {
            cout << current->data << " ";
            if (current->left != nullptr) q.push(current->left);
            if (current->right != nullptr) q.push(current->right);
        }
    }
}

int  countofNodes(Node* root)
{
    if(root ==nullptr) return 0;
    int leftcount = countofNodes(root->left);
    int rightcount = countofNodes(root->right);
    return leftcount+rightcount+1;
}

int  sumofNodes(Node* root)
{
    if(root ==nullptr) return 0;
    int leftsum = sumofNodes(root->left);
    int rightsum = sumofNodes(root->right);
    return leftsum+rightsum+root->data;//return the data 
}

int  height(Node* root)
{
    if(root ==nullptr) return 0;
    int leftheight = height(root->left);
    int rightheight = height(root->right);

    return max(leftheight,rightheight)+1;;
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
    cout<<"Count of nodes : "<<countofNodes(root)<<endl;
    cout<<"Count of nodes : "<<sumofNodes(root)<<endl;
    cout<<"height of tree : "<<height(root)<<endl;
}