#include <iostream>
using namespace std;
class Node
{
    public:
        int data;
        Node* next;
        Node(int val);
};

Node::Node(int val)
{
    data = val;
    next = nullptr;
}

void printList(Node* head)
{
    while(head != nullptr)
    {
        cout<<head->data<<" ";
        head=head->next;
    }
    cout<<endl;
}

Node* middleofList(Node* head)
{
    Node* slow = head;
    Node* fast = head;

    while(fast!=nullptr && fast->next!=nullptr)
    {
        slow =slow->next;
        fast = fast->next->next;
    }
    return slow;
}


void reverseList(Node** head)
{
    Node* prev = nullptr;
    Node* current =*head;
     Node* nextNode = nullptr;
    while(current!=nullptr)
    {
        nextNode = current->next;
        current->next = prev;
        prev = current;
        current =nextNode;
    }
    *head = prev;
}

void insertFirst(Node** head , int value)
{
    Node* newNode = new Node(value);

    newNode->next = *head;
    *head = newNode;
}

void insertEnd(Node** head ,int value)
{
    Node* newNode = new Node(value);

    if(head ==nullptr)
    {
        *head = newNode;
        return;
    }

    Node* last =*head;
    while(last != nullptr)
    {
        last= last->next;
    }

    last->next = newNode;
}

void insertAfter(Node* prev,int value)
{
    if(prev ==nullptr)
    {
        cout<<"cannot be null"<<endl;
        return;
    }

    Node* newNode = new Node(value);
    newNode->next = prev->next;
    prev->next = newNode;
}

int main() {
    
    Node* head = new Node(1);
    Node* second = new Node(2);
    Node* third = new Node(3);
    Node* four = new Node(4);
    Node* five = new Node(5);
    head->next = second;
    second->next = third;
    third->next = four;
    four->next = five;
    printList(head);
    Node* middle = middleofList(head);
    cout<<"Middle of the list : "<<middle->data<<endl;
    reverseList(&head);
    printList(head);
}