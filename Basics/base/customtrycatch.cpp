#include <iostream>
#include <exception>

using namespace std;

class MyCustomError : public exception
{
    private:
    const char* message;

    public:
        MyCustomError(const char* _msg)
        {
            this->message = _msg;
        }

        //overide the what() method to show the respective string

        virtual const char* what() const noexcept override
        {
            return message;
        }

};

void functionToTrow()
{
    throw MyCustomError("Hello from hell !");
}

int main() {
    try
    {
        functionToTrow();
    }
    catch(const MyCustomError& e)
    {
        cout<<"Finally caught an custom exception : " << e.what() << '\n';
    }
    
}