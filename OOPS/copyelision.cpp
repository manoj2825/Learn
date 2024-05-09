// C++ program to demonstrate
// the working of copy elision
#include <iostream>
using namespace std;

class GFG {
public:
	void print() { cout << " GFG!"; }
};

int main()
{
	GFG G;
	for (int i = 0; i <= 2; i++) {
		G.print();
		cout << "\n";
	}
	return 0;
}
