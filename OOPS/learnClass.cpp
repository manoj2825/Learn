#include <iostream>
using namespace std;

class car
{   
    //class is just the blueprint it doesnt take any memory
    //memory will be allocated after creating the object
    public:
        //default will be private
        //but in strcture default will be public
        car(){}
        //constructor

};

int main() {

    //create obj without dynamic
    //car obj;

    //with new keyword
    car* obj =new car();
}