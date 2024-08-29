#include <iostream>
#include <stack>
using namespace std;

bool isValid(string s) {
        
    stack<char> store;
    
    for(char ch:s)
    {
        if(ch == '(' || ch =='{' || ch =='[')
        {
            store.push(ch);
        }
        else{

            if(store.empty()) return false;
            char top = store.top();
            store.pop();

            if((top == '(' && ch != ')') ||
                (top == '{' && ch != '}') ||
                (top == '[' && ch != ']'))
                {
                    return false;
                }

        }
    }

    return store.empty();
}

int main() {
    string str = "())";
    cout<<(isValid(str) == true?"Valid":"Not Valid")<<endl;
}