#include <iostream>
using namespace std;


struct Printer

{   
    int model;
    string color;
    int paperLeft;

    Printer(int _model,string _color,int _paperLeft)
    {
        model=_model;
        color=_color;
        paperLeft=_paperLeft;
    }

};

ostream& operator << (ostream& COUT,Printer& printerobject)
{
    COUT<<"Printer Model Year : "<<printerobject.model<<endl;
    COUT<<"Printer Color Type : "<<printerobject.color<<endl;
    COUT<<"Printer Paper Left : "<<printerobject.paperLeft<<endl;

    return COUT;
}

int main() {
    
    Printer printer1 = Printer(2024,"Color",1000);

    cout<<printer1;
    
}