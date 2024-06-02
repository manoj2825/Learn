#include <iostream>
using namespace std;
class shape
{
    public:
        virtual void  draw()=0;
};

class circle :public shape
{
    public:
        void draw() override
        {
            cout<<"circle"<<endl;
        }
};

class sqaure :public shape
{
    public:
        void draw() override
        {
            cout<<"sqaure"<<endl;
        }
};



int main() {
    
    shape* m = new circle();
    m->draw();
}