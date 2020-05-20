#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
#include <locale.h>
#include <iostream>
#include <ctime>

using namespace std;

#define numberOfThreads 10
#define initSemaphoreCount 3
#define maxSemaphoreCount 3

HANDLE semaphore;

void showError() {
	setlocale(LC_ALL, "Russian");
	cout << "Something went wrong" << endl;

	LPVOID eMessage;
	DWORD eCode = GetLastError();

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
		eCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&eMessage, 0, NULL);
	char* errorMessage = (char*)eMessage;
	wprintf(L"ERROR was generated: failed with 0x%x code\n", eCode);
	wprintf(L"ERROR formatted message: %s\n", errorMessage);
}


DWORD WINAPI ExecutedByThread(LPVOID lpParam)
{
	DWORD state, bytesWritten;
	HANDLE file;
	clock_t start, finish;
	char* string = new char[10];
	bool run = true;
	const int second = 1000;
	state = WaitForSingleObject(semaphore, 3);

	while (run == true)
	{
		if (state == WAIT_OBJECT_0)
		{
			start = clock();

			Sleep(3 * second);
			cout << "Thread =>" << GetCurrentThreadId() << " is in critical section" << endl;

			//Create file
			file = CreateFileA("result.txt", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			finish = clock();

			//count time
			double duration = finish - start;

			SetFilePointer(file, 0, NULL, FILE_END);
			//Write a formatted string to character string buffer
			sprintf(string, "%f", duration);

			//Write result to the file
			WriteFile(file, string, sizeof(string), &bytesWritten, NULL);

			if (file == INVALID_HANDLE_VALUE)
			{
				showError();
			}
			run = false;
		
			Sleep(3 * second);

			cout << "Thread =>" << GetCurrentThreadId() << " is not in critical section anymore" << endl;

			if (!ReleaseSemaphore(semaphore, 1, NULL))
			{
				showError();
			}
		}
	}
	return 0;
}

int main()
{
	HANDLE threads[numberOfThreads];
	DWORD id;

	semaphore = CreateSemaphore(
		NULL,			
		initSemaphoreCount,	  
		maxSemaphoreCount,	  
		NULL				
	);

	if (semaphore == NULL)
	{
		showError();

		return 1;
	}

	//Create threads
	for (int i = 0; i < numberOfThreads; i++)
	{
		threads[i] = CreateThread(NULL, 0, ExecutedByThread, NULL, 0, &id);

		if (threads[i] == NULL)
		{
			showError();

			return 1;
		}
	}

	WaitForMultipleObjects(numberOfThreads, threads, TRUE, INFINITE);

	for (int i = 0; i < numberOfThreads; i++)
	{
		CloseHandle(threads[i]);
	}

	CloseHandle(semaphore);

	return 0;
}