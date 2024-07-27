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

int main() {
    
    Printer one = Printer(2024,"Color",1000);
    
}