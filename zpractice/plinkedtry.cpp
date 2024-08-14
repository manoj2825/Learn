#include <iostream>
#include <vector>
#include <unordered_set>
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

Node* mertwosortedList(Node* list1, Node* list2)
{
    if(list1 ==nullptr) return list2;
    if(list2 ==nullptr) return list1;

    if(list1->data < list2->data)
    {
        list1->next=mertwosortedList(list1->next,list2);
        return list1;
    }
    else
    {
        list2->next=mertwosortedList(list1,list2->next);
        return list2;
    }
}

void makeCircular(Node* head) {
    if (head == nullptr) return;

    Node* temp = head;
    while (temp->next != nullptr) {
        temp = temp->next;
    }
    temp->next = head; // Making the list circular
}

void breakCircular(Node* head) {
    if (head == nullptr) return;

    Node* temp = head;
    while (temp->next != head) {//imortant condition
        temp = temp->next;
    }
    temp->next = nullptr; // Break the circular link
}

bool isCircular(Node* head) {
    if (head == nullptr) return false;

    Node* temp = head->next;
    while (temp != nullptr && temp != head) {
        temp = temp->next;
    }
    return temp == head;
}

void removedup_unsorted(Node* head)
{
    unordered_set<int> seen;
    Node* current = head;
    Node* prev =nullptr;
    while(current!=nullptr)
    {
        if(seen.find(current->data)!=seen.end())
        {
            prev->next = current->next;
            delete current;
        }
        else{
            seen.insert(current->data);
            prev = current;
        }
        current = prev->next;
    }
}

void removedup_sorted(Node* head)
{
    if(head ==nullptr) return;

    Node* current = head;
    while(current != nullptr && current->next != nullptr)
    {   
        if(current->data == current->next->data)
        {
           Node* nextNode = current->next->next;
            delete current->next;
            current->next = nextNode;
        }
        else{
            current=current->next;
        }
    }
}

Node* createListfromVector(const vector<int> v)
{
    if(!v.size()) return nullptr;

    Node* head = new Node(v[0]);
    Node* current =head;
    for(int i=1;i<v.size();++i)
    {
        current->next = new Node(v[i]);
        current=current->next;
    }
    return head;
}

int main() {
    vector<int> v = {9,2,3,4,3,5};
    // Node* head = new Node(v[0]);
    // Node* second = new Node(v[1]);
    // Node* third = new Node(v[2]);
    // Node* four = new Node(v[3]);
    // Node* five = new Node(v[4]);
    // head->next = second;
    // second->next = third;
    // third->next = four;
    // four->next = five;
    Node* head = createListfromVector(v);
    printList(head);

    //Node* middle = middleofList(head);
   // cout<<"Middle of the list : "<<middle->data<<endl;
   // reverseList(&head);
    //printList(head);
    //removedup_sorted(head);
    removedup_unsorted(head);
    printList(head);
    
}