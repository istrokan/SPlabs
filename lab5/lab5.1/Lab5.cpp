#include <stdio.h>
#include <Windows.h>
#include <iostream>

using namespace std;

int main()
{
	HANDLE myMutex = CreateMutex(
		0,
		0,
		TEXT("MutexName")
	);
	DWORD result = WaitForSingleObject(myMutex, 0);

	if (result != WAIT_OBJECT_0)
	{
		cout << "Application is already running" << endl;

		return 0;
	}
	else
	{
		cout << "It`s running" << endl;

		while (1) {}
	}

	return 0;
}