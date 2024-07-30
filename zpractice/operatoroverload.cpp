#include <iostream>
using namespace std;

class Printer
{
    public:
        int modelname;
        int paperleft;

        Printer(int _modelname,int _paperleft):modelname(_modelname),paperleft(_paperleft){}

        friend ostream& operator<<(ostream& COUT,Printer& obj);

        Printer operator+(Printer& other)
        {
            return Printer(this->modelname,this->paperleft+other.paperleft);
        }
};

ostream& operator<< (ostream& COUT,Printer& obj)
{
    COUT<<"model number : "<<obj.modelname<<"   ,";
    COUT<<"paperleft : "<<obj.paperleft<<endl;
    return COUT;
}

int main() {
    
    Printer p1(2024,1000);
    Printer p2(2022,500);

    Printer p3 = p1+p2;
    cout<<p3;

}