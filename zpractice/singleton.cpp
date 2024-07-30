#include <iostream>
using namespace std;

class Singleton
{
    private:
        static Singleton* instance;
        int data;
        Singleton(int _data):data(_data){}
    
    public:
        static Singleton* getinstance(int _data)
        {
            if(instance ==nullptr)
            {
                instance = new Singleton(_data);
            }
            return instance;
        }
        int getdata()
        {
            return data;
        }
};

//very important thing
Singleton* Singleton::instance = nullptr;

int main() {
    Singleton* s1 = Singleton::getinstance(100);
    Singleton* s2 = Singleton::getinstance(200);
    cout<<s1->getdata()<<endl;
    cout<<s2->getdata();
}