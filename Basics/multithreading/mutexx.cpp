#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

mutex mtx;
void function1(char Symbol)
{
	lock_guard<mutex> lock(mtx);
	for (int i = 0; i < 100; i++)
	{
		cout << Symbol;
	}
	this_thread::sleep_for(3000ms);
	cout << endl;
}

void function2()
{
	lock_guard<mutex> lock(mtx);
	for (int i = 0; i < 100; i++)
	{
		cout << 'O';
	}
}


int main()
{
	//function1('+');
	//function1('-');
	thread one(function1, '+');
	thread two(function2);

	one.join();
	two.join();
}