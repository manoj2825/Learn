#include <iostream>

using namespace std;

class Node
{
    public:
        int value;
        Node* next;
};

void printList(Node* n)
{
    while(n!=nullptr)
    {
    cout<<n->value;
    if(n->next != nullptr)
    {
        cout<<"->";
    }
    n=n->next;
    }
}

void insertAtFront(Node** head, int newValue)//when you recieve the address you need to receive pointer
{
    //lets solve in 3 steps
    //1. Lets create a new node and assign the value
    Node* newNode = new Node();
    newNode->value=newValue;

    //2.point this next to head
    newNode->next=*head;//assign pointer to the pointer

    //3.make this as the head
    *head =newNode;
}

void insertAtEnd(Node** head ,int newValue)
{
    //lets solve in 4 steps
    //1. Lets create a new node and assign the value
    Node* newNode = new Node();
    newNode->value=newValue;
    newNode->next=NULL;

    //2.if the list is empty newNode will be head
    if(head==NULL)
    {
        *head=newNode;
        return;
    }

    //3.find the last node by traversing the list
    Node* last =*head;//create a temp last
    while(last->next!=NULL)
    {
        last=last->next;
    }//now we found the last node which points to NULL

    //4.assign the last next to new node
    last->next=newNode;

}


void insertAfter(Node* prev,int newValue)
{
    //1.lets check prev is null
    //prev cannot be null
    if(prev ==NULL)
    {
        cout<<"Previous cannot be NULL"<<endl;
        return;
    }

    //2.prepare the node
    Node* newNode =new Node();
    newNode->value=newValue;


    //3.assignment of the nodes
    newNode->next=prev->next;
    prev->next=newNode;
}

Node* reverseList(Node* head)
{
    if(head == nullptr) return nullptr;

    Node* prev= nullptr;
    Node* current = head;
    while(current!=nullptr)
    {
        Node* nextNode = current->next;
        current->next = prev;
        prev = current;
        current = nextNode; 
    }
    return prev;
}

void convertToCircular(Node* head) {
    if (head == nullptr) return;

    Node* current = head;
    while (current->next != nullptr) {
        current = current->next;
    }
    // 'current' now points to the last node of the SLL

    // Make the last node circular
    current->next = head;
}

void printCircularList(Node* head, int nodesToShow = 10) {
    if (head == nullptr) return;

    Node* current = head;
    int count = 0;
    do {
        std::cout << current->value;
        if (current->next != head) {
            std::cout << " -> ";
        }
        current = current->next;
        count++;
    } while (current != head && count < nodesToShow);
    std::cout << std::endl;
}

int main() {
    
    Node* head =new Node();
    Node* second =new Node();
    Node* third =new Node();

    head->value=1;
    head->next=second;

    second->value=2;
    second->next=third;

    third->value=3;
    third->next=nullptr;

   // printList(head);

    //insert at beginning

   // insertAtFront(&head,-3);//pass the address of the current head and tha value to be inserted

    //insertAtFront(&head,-2);
    //insertAtFront(&head,-1);

    insertAtEnd(&head,4);
   // insertAfter(head,5);
    printList(head);
    Node* reverse = reverseList(head);
    cout<<endl;
    printList(reverse);
    convertToCircular(head);
    printCircularList(head);
    printList(head);
}

