#include <iostream>


using namespace std;

class Book
{
    public:
        string title;
        string author;
        int edition;
        float* Rate;
        int Ratecounter;

        Book(string _title, string _author, int _edition):title(_title),author(_author),edition(_edition)
        {
            Ratecounter =2;
            Rate =new float[Ratecounter];
            Rate[0]=1.0;
            Rate[1]=2.0;
        }

        ~Book();
};

Book::~Book()
{
    delete[] Rate;
    Rate =nullptr;
}

int main() {
    
    Book book1("Gone bois","manoj",2024);
    Book book2("Gone bois version 2","manoj",2024);
    Book book3(book2);
}