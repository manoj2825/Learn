#include <iostream>
#include <thread>
#include <chrono>
#include <semaphore>

using namespace std;

counting_semaphore<3> semaphore;


void worker(int id)
{

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