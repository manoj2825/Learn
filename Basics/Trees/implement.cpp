#include <iostream>
#include <queue>
#define nullptr NULL
using namespace std;
class Node
{
    public:
        int data;
        Node* left;
        Node* right;

        Node(int value):data(value),left(nullptr),right(nullptr){}
};

void printTree(Node * root)//inorder 
{
    if(root == nullptr)
    {
        return;
    }

    printTree(root->left);
    cout<<root->data<< " ";
    printTree(root->right);

}
void inOrder(Node * root)//inOrder 
{
    if(root == nullptr)
    {
        return;
    }

    inOrder(root->left);
    cout<<root->data<< " ";
    inOrder(root->right);

}
void preOrder(Node * root)//preOrder 
{
    if(root == nullptr)
    {
        return;
    }

    cout<<root->data<< " ";
    preOrder(root->left);
    preOrder(root->right);

}
void postOrder(Node * root)//postOrder 
{
    if(root == nullptr)
    {
        return;
    }

    postOrder(root->left);
    postOrder(root->right);
    cout<<root->data<< " ";

}

void levelOrder(Node* root)
{
    //lets create a queue
    if(root == nullptr) return;

    queue<Node *> q;
    q.push(root);
    q.push(nullptr);//for endline
    while(!q.empty())
    {
        Node* current =q.front();
        q.pop();
        if(current ==NULL)
        {
            cout<<endl;//first null
            if(q.empty())
            {
                break;
            }
            else{
                q.push(NULL);
            }
        }
        else
        {
            cout<<current->data<<" ";
            if(current->left != NULL) q.push(current->left);
            if(current->right != NULL) q.push(current->right);
        }
    }

}

int main() {


//       1
//      / \
//     2   5
//    / \
//   3   4

    Node* root =new Node(1);
    root->left =new Node(2);
    root->right =new Node(5);
    root->left->left=new Node(3);
    root->left->right =new Node(4);

    //printTree(root);
    // inOrder(root);
    // cout<<endl;
    // preOrder(root);
    // cout<<endl;
    // postOrder(root);//DFS
    // cout<<endl;
    levelOrder(root);//BFS
}
    