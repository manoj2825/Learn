#include <iostream>
#include <string>
#include <cmath>
using namespace std;

int main() {
    
    int num=1234;

    //1.naive approach
    int temp=num;
    int count =0;
        while(temp)
        {
            temp=temp/10;
            count++;
        }
        cout<<count<<endl;

    //2.convert to string
    int count_ =to_string(num).length();
    cout<<count_<<endl;

    //3.log method
    int count__ =log10(num)+1;
    cout<<count__<<endl;
}