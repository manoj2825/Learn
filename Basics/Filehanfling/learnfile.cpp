#include <iostream>
#include <fstream>
#include <string>

using namespace std;

//to write to file

int main() {
    
    fstream Myfile;

    Myfile.open("manoj.txt",ios::out);

    if(Myfile.is_open())
    {
        cout<<"Opened"<<endl;
        for(int i=0;i<10;i++)
        {
        Myfile<<"Manoj is noob : ";
        Myfile<<i<<endl;
        }

        Myfile.close();
    }

    Myfile.open("manoj.txt",ios::in);//read mode

    if(Myfile.is_open())
    {
        string line;
        while(getline(Myfile,line))
        {
            cout<<line<<endl;
        }
        Myfile.close();
    }
}