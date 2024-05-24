#include <iostream>
#include <tuple>
using namespace std;
class dummy
{
    public:
        int val;

        dummy(int x):val(x){}
        // dummy(dummy obj)
        // {

        // }
            dummy(dummy& obj) {
        val = obj.val;
    }

};

int main() {
    dummy test(5);
    dummy test1 =test;
    tuple <int,string,char,int,string ,float> test;

}  