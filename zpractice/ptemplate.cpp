#include <iostream>
using namespace std;
template<typename T>

T add(T a,T b)
{
    return a+b;
}

void print()
{
    cout<<endl;
}

template<typename T, typename...Args>
void print(T first,Args...args)
{
    cout<<first<< " ";
    print(args...);
}

template<typename T>
class Test{

    public:
        T val;
        Test(T data);

};
template<typename T>
Test<T>::Test(T data)
{
    val = data;
}
int main() {
    
    //int ans = add(5,4);
    //cout<<ans<<endl;
    print(1,51,3);

    Test<int> init(3);

}