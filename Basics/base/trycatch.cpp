#include <iostream>
using namespace std;

bool solve(bool Flag)
{
    if(Flag)
    {
        throw runtime_error("Runtime Error");
    }
}

int main() {
    try
    {
        solve(true);
    }
    catch(exception& e)
    {
        cout<<"Error   --->>   "<<e.what()<<endl;
    }
}

//like runtime there are feww more types but if you want to create a own custom messaage then ycheck customcatch.cpp