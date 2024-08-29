#include <iostream>
using namespace std;

class Box
{
    private:
        int width;
    public:
        Box():width(0){}

        friend void setwidth(Box& b,int w);

        friend class Display;
};

class Display
{
    public:
        void showBox(const Box& b)
        {
            // Display has access to private members of Box
            cout << "Width: " << b.width << endl;
        }
};

void setwidth(Box& b,int w)
{
    b.width = w;
}
int main() {
    
}