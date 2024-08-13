#include <iostream>
using namespace std;

class Test
{
    public:
        int data;   // | * * * * |
        int* val;   // | * * * * |
        char one;   // | * * * * | extra 3 bytes to have padding so the total size is 12 bytes

        Test(int x,int y, char s)
        {
            data = x;
            *val = y;
            one = s;
        }
 };

 class base
 {
    public:
        int test;
        char c;
 };

//to avoid padding 
#pragma pack(push, 1)
class PackedClass {
public:
    char a;   // 1 byte
    int b;    // 4 bytes
    char c;   // 1 byte


};
// Restore previous packing
#pragma pack(pop)

int main() {
    
    cout<<"int size           : "<<sizeof(int)<<endl;
    cout<<"int* size          : "<<sizeof(int*)<<endl;
    cout<<"size of Test class : "<<sizeof(Test)<<endl;
    cout<<"size of Base class : "<<sizeof(base)<<endl;
    cout<<"size of PackedClass class : "<<sizeof(PackedClass)<<endl;

    Test t(1,2,'m');
    cout<<sizeof(t)<<endl;
}