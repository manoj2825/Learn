#include <iostream>
#include <stack>
#include <string>

using namespace std;


bool solve(const string& str)
{
    stack<char> s;

    for(char ch: str)
    {
        if(ch=='['|| ch=='{' || ch=='(')
        {
            s.push(ch);
        }
        else if (ch==']'|| ch=='}' || ch==')')
        {
            if(s.empty())
            {
                return false;
            }

            char top =s.top();
            if((ch ==']' && top!='[')||(ch =='}' && top!='{')||(ch ==')' && top!='(')) return false;

            s.pop();
        }
        
    }

}

int main() {

    string str = "(){}[]";

    if(solve(str))
        cout<<"balanced"<<endl;
    else   
        cout<<"not balanced"<<endl;
}
