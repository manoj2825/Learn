#include <iostream>

using namespace std;


namespace manoj
{
class souptika
{
    public:
        int variable;

        souptika()
            {
                cout<<"souptika created in namespcase"<<endl;
            }
        
};
}


class souptika
{
    public:
        int variable;

                souptika()
            {
                cout<<"souptika created outside"<<endl;
            }
};


int main() {
    
    souptika m;

    manoj::souptika s;
}