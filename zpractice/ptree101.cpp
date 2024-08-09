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

void inorder(Node* root)
{
    if(root ==nullptr) return;

    inorder(root->left);
    cout<<root->data<< " ";
    inorder(root->right);
}

void preorder(Node* root)
{
    if(root ==nullptr) return;

    cout<<root->data<< " ";
    preorder(root->left);
    preorder(root->right);
}

void postorder(Node* root)
{
    if(root ==nullptr) return;

    postorder(root->left);
    postorder(root->right);
    cout<<root->data<< " ";
}

void levelorder(Node* root)
{
    if(root == nullptr) return;

    queue<Node* >q;
    q.push(root);
    q.push(nullptr);

    while(!q.empty())
    {
        Node* current = q.front();
        q.pop();

        if(current == nullptr)
        {
            cout<<endl;
            if(q.empty())
            {
                break;
            }
            else{
                q.push(nullptr);
            }
        }
        else{
            cout<<current->data<< " ";
            if(current->left != nullptr) q.push(current->left);
            if(current->right != nullptr) q.push(current->right);
        }
    }
}

int countofNodes(Node* root)
{
    if(root == nullptr) return 0;

    int leftcount = countofNodes(root->left);
    int rightcount = countofNodes(root->right);
    return leftcount+rightcount+1;
}

int sumofNodes(Node* root)
{
    if(root == nullptr) return 0;

    int leftsum = sumofNodes(root->left);
    int rightsum = sumofNodes(root->right);
    return leftsum+rightsum+root->data;
}

int maxheight(Node* root)
{
    if(root == nullptr) return 0;

    int lefthieght = maxheight(root->left);
    int righthieght = maxheight(root->right);
    return max(lefthieght,righthieght)+1;
}

int main() {

    vector<int> arr ={1,2,3,4,5};

    Node* root =new Node(arr[0]);
    root->left =new Node(arr[1]);
    root->right= new Node(arr[4]);
    root->left->left= new Node(arr[2]);
    root->left->right= new Node(arr[3]);

    // inorder(root);
    // cout<<endl;
    // preorder(root);
    // cout<<endl;
    // postorder(root);
    // cout<<endl;
    levelorder(root);

    cout<<"Nodes count : "<<countofNodes(root)<<endl;
    cout<<"Nodes sum : "<<sumofNodes(root)<<endl;
    cout<<"Nodes hieght : "<<maxheight(root)<<endl;

}