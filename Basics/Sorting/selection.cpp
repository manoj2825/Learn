#include <iostream>
using namespace std;

void solve(int arr[], int size)
{
	for(int i =0;i<size-1;i++)
	{
		int min_ind=i;
		for(int j=i+1; j<size -1;j++)
		{
			if(arr[j]<arr[min_ind])
			{
			min_ind = j;
			}
		}
        if(min_ind != i)
        {
		int temp = arr[min_ind];
		arr[min_ind]=arr[i];
		arr[i] = temp;
        }
	}
}

int main() {
    int arr[3]={3,2,5};
    solve(arr,3);

    for(int i=0;i<3;i++)
    {
        cout<<arr[i]<<"\t";
    }
}