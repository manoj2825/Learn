#include <iostream>
using namespace std;

void insamestring(string& str)
{
    int j=0;
    for(int i=0;i<str.length();++i)
    {
        if(str[i]!=' ')
        {
            str[j++]=str[i];
        }
    }
    str.resize(j);
}

string usingnewstring(string& str)
{
    string result;

    for(char ch: str)
    {
        if(ch!=' ')
        {
            result+=ch;
        }
    }
    return result;
}

int main() {
    string str = "asf as d";

    string ans =usingnewstring(str);
    cout<<"'"<<ans<<"'"<<endl;

    insamestring(str);
    cout<<"'"<<str<<"'"<<endl;

}
    