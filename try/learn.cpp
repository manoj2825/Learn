#include <iostream>
#include <tuple>
using namespace std;

class test
{
    public:
    test();
};

test::test()
{
    cout<<"test created\n";
}

int main() {
   
  test t1;

  int x=173;

  cout<<x/10<<" "<<x%10<<endl;

  int* value = new int(3);
  cout<<*value<<endl;

}  