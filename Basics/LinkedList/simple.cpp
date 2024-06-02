#include <iostream>

using namespace std;


class Node
{
    public:
        int data;
        Node* next;
};

void printList(Node* n)
{
    while(n!=nullptr)
    {
        cout<<n->data<<" at address : "<<n->next<<endl;
        n=n->next;
    }
}

int main() {
    

    Node* head =new Node();
    Node* second =new Node();
    Node* third =new Node();

    head->data=1;
    head->next=second;

    second->data=2;
    second->next=third;


    third->data=3;
    third->next=nullptr;
    
    
    printList(head);
}