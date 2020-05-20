#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
#include <iostream>
#include <ctime>

using namespace std;

#define CRITICAL_SECTION_MODE 0
#define NOT_CRITICAL_SECTION_MODE 1

#define numberOfThreads 3
#define amountOfNumbers 10101110 // 10 101 110

int arr[amountOfNumbers];
CRITICAL_SECTION criricalSection;

void getSum()
{
	int sum = 0;
	for (int i = 0; i < amountOfNumbers; i++)
	{
		sum += arr[i];
	}
	cout << "Sum => " << sum << endl;
}

void getAverage()
{
	double avg = 0;
	for (int i = 0; i < amountOfNumbers; i++)
	{
		avg += arr[i];
	}
	cout << "Avarage => " << avg / amountOfNumbers << endl;
}

void getMaximum()
{
	int max = 0;
	for (int i = 0; i < amountOfNumbers; i++)
	{
		if (max < arr[i])
			max = arr[i];
	}
	cout << "Max => " << max << endl;
}

void generate()
{
	for (int i = 0; i < amountOfNumbers; i++)
	{
		arr[i] = rand() % 100;
	}
}

DWORD WINAPI findSum(LPVOID mode)
{
	if (NOT_CRITICAL_SECTION_MODE)
	{
		getSum();
	}
	else {
		if (!InitializeCriticalSectionAndSpinCount(&criricalSection, 0x00000400))
		{
			return 0;
		}
		EnterCriticalSection(&criricalSection);
		getSum();
		LeaveCriticalSection(&criricalSection);
	}

	return 0;
}

DWORD WINAPI findAverage(LPVOID mode)
{
	if (NOT_CRITICAL_SECTION_MODE)
	{
		getAverage();
	}
	else {
		if (!InitializeCriticalSectionAndSpinCount(&criricalSection, 0x00000400))
		{
			return 0;
		}
		EnterCriticalSection(&criricalSection);
		getAverage();
		LeaveCriticalSection(&criricalSection);
	}

	return 0;
}

DWORD WINAPI findMaximum(LPVOID mode)
{

	if (NOT_CRITICAL_SECTION_MODE)
	{
		getMaximum();
	}
	else {
		if (!InitializeCriticalSectionAndSpinCount(&criricalSection, 0x00000400))
		{
			return 0;
		}
		EnterCriticalSection(&criricalSection);
		getMaximum();
		LeaveCriticalSection(&criricalSection);
	}

	return 0;
}

void DeleteCriticalSection()
{
	DeleteCriticalSection(&criricalSection);
}

int main()
{
	HANDLE hThread[numberOfThreads];
	DWORD ThreadId;
	clock_t start, finish;

	generate();

	cout << "First mode = > use critical section" << endl;
	start = clock();
	//Create threads & execute
	hThread[0] = CreateThread(NULL, 0, findSum, (LPVOID)CRITICAL_SECTION_MODE, 0, &ThreadId);
	hThread[1] = CreateThread(NULL, 0, findAverage, (LPVOID)CRITICAL_SECTION_MODE, 0, &ThreadId);
	hThread[2] = CreateThread(NULL, 0, findMaximum, (LPVOID)CRITICAL_SECTION_MODE, 0, &ThreadId);
	WaitForMultipleObjects(numberOfThreads, hThread, TRUE, INFINITE);

	finish = clock();
	double duration = finish - start;
	cout << "Time: " << duration << endl;

	DeleteCriticalSection();

	for (int i = 0; i < numberOfThreads; i++)
	{
		CloseHandle(hThread[i]);
	}

	cout << "Second mode = > not use critical section" << endl;
	start = clock();

	hThread[0] = CreateThread(NULL, 0, findSum, (LPVOID)NOT_CRITICAL_SECTION_MODE, 0, &ThreadId);
	hThread[1] = CreateThread(NULL, 0, findAverage, (LPVOID)NOT_CRITICAL_SECTION_MODE, 0, &ThreadId);
	hThread[2] = CreateThread(NULL, 0, findMaximum, (LPVOID)NOT_CRITICAL_SECTION_MODE, 0, &ThreadId);
	WaitForMultipleObjects(numberOfThreads, hThread, TRUE, INFINITE);

	finish = clock();
	duration = finish - start;
	cout << "Time: " << duration << endl;

	DeleteCriticalSection();

	for (int i = 0; i < numberOfThreads; i++)
	{
		CloseHandle(hThread[i]);
	}

	return 0;
}