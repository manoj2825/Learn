#include <iostream>
using namespace std;

class abstractclass
{
    public:
        virtual void method1()=0;
        void display()
        {
            cout<<"hello"<<endl;
        }
};

class Interface1
{
  public:
        virtual void method2()=0;
};
class Interface2
{
  public:
        virtual void method3()=0;
};

class derived : public abstractclass,public Interface1,public Interface2
{
    public:
        void method1() override{}
         void method2() override{}
          void method3() override{}
};

int main() {
    derived obj;

}