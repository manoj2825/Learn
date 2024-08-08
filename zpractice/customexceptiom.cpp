#include <iostream>
#include <climits>
#include <vector>
#include <exception>

using namespace std;

class CustomExcep:public exception
{
    private:

    const char* msg;

    public:

   CustomExcep (const char* message)
    {
        this->msg=message;
    }

    virtual const char* what()const noexcept override
    {
        return msg;
    }

};

void functiontothrow()
{
    throw CustomExcep(" hello from custom");
}

int main()
{
    try
    {
        functiontothrow();
    }
    catch(const CustomExcep& e)
    {
        cout<<"catched : "<<e.what()<<endl;
    }
    
}