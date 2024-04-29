#include <iostream>
#include "manoj.h"

using namespace std;

class base
{


private:

    int val;

    base(int x):val(x){};
    //can ve accesed on here 
    // and friedn function

//create a default constror
public:

    static base create (int x){
        return base(x);
    }


};

int main() {
    
    base obj=base::create(5);

    return 0;
}