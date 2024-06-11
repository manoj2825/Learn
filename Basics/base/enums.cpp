#include <iostream>
using namespace std;

//normal enum
enum Day
{
    Monday = 1,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday,
    Sunday
};

//scoped enum or enum class
//main difference  is to avoid name conflict
//becaue it used :: operator so the you can access very carefully

enum class Month
{
    Jan = 1,
    Feb,
    Mar,
    Apr,
    May,
    June,
    July,
    Aug,
    Sept,
    Oct,
    Nov,
    Dec
};

int main() {
    
    Day currentday = Monday; //normal enum

    cout<<currentday<<endl;

   // Month currentMonth =Jan; you cannot access like this 
   Month currentMonth = Month::Jan; 

    // Directly printing scoped enum value will not work as intended
    // cout << "Current month (enum class): " << currentMonth << endl;  // This will cause a compilation error

    // To print the value of a scoped enum, you need to cast it to its underlying type
    cout << "Current month (enum class): " << static_cast<int>(currentMonth) << endl;
}