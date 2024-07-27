#include <iostream>
#include <map>
using namespace std;
class Node
{
    public:
        int data;
        Node* next;
};

void printList(Node* head)
{
    while(head!=nullptr)
    {
        cout<<head->data<<endl;
        head=head->next;
    }
}


bool solve(string  one , string two)
{
    map<char,int> map_one;
    map<char,int> map_two;

    for(char c:one)
    {
        map_one[c]++;
    }

    for(char c:two)
    {
        map_one[c]++;
    }

    for(auto i =map_one.begin();i!=map_one.end();++i)
    {

        char c = i->first;
        int count= i->second;

        if (map_two.find(c) == map_two.end() || map_two[c] < count) {
            return false;
        }
    }
    return true;
}





int main() {
    
    Node* head = new Node();
    Node* second = new Node();
    Node* third = new Node();

    head->data=10;
    head->next =second;

    second->data=20;
    second->next =third;

    third->data=30;
    third->next =nullptr;

   //printList(head);


    string str1 ="aab";
    string str2 ="aab";

    if(solve(str1,str2))
    {
        cout<<"true"<<endl;
    }
    else
    {
        cout<<"false"<<endl;
    }


}