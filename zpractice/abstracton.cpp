#include <iostream>
using namespace std;

class Phone
{
    public:
        virtual void start()=0;
};

class Android : public Phone
{
    public:
        void start()override
        {
            cout<<"Android satred"<<endl;
        }

};

int main() {
    
    Phone* samsung = new Android();
    samsung->start();
    delete samsung;
    return 0;
}