#include <iostream>
#include <thread>
#include <chrono>
#include <semaphore>

using namespace std;

//counting_semaphore<3> countingsemaphore(3);
counting_semaphore<> countingsemaphore(3);


void worker(int id)
{

    cout<<"thread id : "<<id<<"waiting to enter"<<endl;
    countingsemaphore.acquire();
    cout<<"thread id : "<<id<<"entered"<<endl;
    this_thread::sleep_for(100ms);
    countingsemaphore.release();

}

int main() {
    thread threads[5];

    for(int i=0;i<5;i++)
    {
        threads[i]=thread(worker,i);
    }

    for(auto& t:threads)
    {
        t.join();
    }
}