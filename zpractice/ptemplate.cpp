#include <iostream>
using namespace std;
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

template<typename T>

T add(T a, T b)
{
    return a+b;
}

template<>
float add(float a, float b)
{
    return 2.5;
}

int main() {
    
    //int ans = add(5,4);
    //cout<<ans<<endl;
    print(1,51,3);

    Test<int> init(3);

    cout<<add(3,4)<<endl;
    cout<<add(3.5f,4.5f)<<endl;

}