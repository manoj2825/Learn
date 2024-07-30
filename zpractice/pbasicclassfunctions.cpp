#include <iostream>
using namespace std;

class Base
{
    public:
        int* data;
        int value;

        Base(int _data,int _value):value(_value)
        {
            data = new int;
            *data = _data;
            cout<<"constructed"<<endl;
        }

        //impemnting custom copy constructor
        Base(const Base& other)
        {
            data = new int;
            *data = *(other.data);
            value = other.value;
        }

        Base& operator=(const Base& other)
        {
            if(this == &other)
                return *this;   //to handle conditions like b1=b1;
            
            delete data;        //delete pervioaly allocateed mempry just to handle proper memroty management 

            data = new int;
            *data = *(other.data);
            value = other.value;
            return *this;
        }
        //move constructor

        Base(Base&& other)
        {
            data=other.data;
            value = other.value;
            other.data =nullptr;
            cout<< " move constructed"<<endl;
        }

        //move assignment operator
        Base& operator=(Base&& other) noexcept   //for b4 = move(b3);
        {
            if(this ==&other)
                return *this;
            
            delete data;
            data = other.data;
            value = other.value;

            other.data = nullptr;
            cout<<"move assigned"<<endl;
            return *this;
        }

        void display();
        friend ostream &operator<<(ostream &COUT,const Base& obj);
        ~Base();
};

Base :: ~Base()     //no resturn type for destor and constror defined outsied the calss
{
    delete data;
    cout<<"destructed"<<endl;
}

void Base::display()
{
    cout<< "data : "<<*data<<" , ";
    cout<< "value : "<<value<<endl;
}

ostream &operator<<(ostream &COUT,const Base& obj)   //since << operator used ostrean which is not part of the 
                                                    //class you need to defined outside the class and cannot use this
{
    if (obj.data) {
        COUT << "data : " << *(obj.data) << " , ";
    } else {
        COUT << "data : nullptr , ";
    }
    COUT << "value : " << obj.value << endl;
    return COUT;
}

// ostream& operator<< (ostream& COUT,Base& obj)
// {
//     COUT<< "data : "<<*(obj.data)<<" , ";
//     COUT<< "value : "<<obj.value<<endl;

//     return COUT;
// } 
int main() {
    
    // Base b1(5,2);
    // //b1.display();   
    // cout<<b1;
    // Base b2=b1;      here cpp provodes default copy construot which performs shallo copy of the objecy
    // cout<<b2;

    Base b1(5,2);
    //b1.display();
    //cout<<b1;
    // Base b2=b1;
    // cout<<b2;
    // Base b3(5,10);
    // b3=b2;      //copy assignment operator

    Base b4 = move(b1);
    cout<<b4;
    cout<<b1;
}