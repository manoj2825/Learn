#include <iostream>
using namespace std;

class Singleton
{
    private:
        static Singleton* instance;
        int data;
        Singleton(int _data):data(_data){}

        //private destructor
        ~Singleton()
        {
            cout<<"destructed"<<endl;
        }
    
    public:
        static Singleton* getinstance(int _data)
        {
            if(instance ==nullptr)
            {
                instance = new Singleton(_data);
            }
            return instance;
        }
        static void destroyInstance()
        {
            if(instance != nullptr)
            {
                delete instance;
                instance =nullptr;
            }
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

    Singleton::destroyInstance();
}