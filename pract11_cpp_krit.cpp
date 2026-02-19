#include <iostream>
#include <windows.h>
#include <conio.h>
#include <string>
#include <stdlib.h>

using namespace std;

#define MAX_LOGS 1000
#define MAX_TEXT 128

string messages[3] = {
	"Вошел в систему",
	"Вышел из системы",
	"Ошибка входа в систему"
};

struct LogRecord {
	DWORD IDT = 0;
	int priority = 0;
	DWORD tick = 0;

	char message[MAX_TEXT] = "";
};

struct LogBuffer {
	LogRecord records[MAX_LOGS];
	LONG index = 0;
};

DWORD Tid[4] = { 1, 2, 3, 4 };
HANDLE T[4];

CRITICAL_SECTION cs;

LogBuffer buffer;

void Logger() {
	srand(time(NULL));
	DWORD tickStart = GetTickCount64();
	while (buffer.index <= MAX_LOGS - 1) {
		EnterCriticalSection(&cs);
		DWORD tickSeichas = GetTickCount64();
		DWORD tick = tickSeichas - tickStart;
		int id = rand() % 3;
		LogRecord record;
		record.tick = tick;
		record.IDT = GetCurrentThreadId();
		record.priority = GetThreadPriority(GetCurrentThread());
		string mess = "[" + to_string(record.IDT) + ", " + to_string(record.priority) + "] - " + messages[id] + " (" + to_string(record.tick) + "ms)." + to_string(buffer.index);
		strcpy_s(record.message, mess.c_str());
		
		if (buffer.index <= MAX_LOGS - 1) {
			buffer.records[buffer.index] = record;
			buffer.index++;
		}
		LeaveCriticalSection(&cs);

		int time = rand() % 100;
		Sleep(time < 10 ? 10 : time);
	}
}

void Seeker() {
	while (buffer.index <= MAX_LOGS - 1) {
		EnterCriticalSection(&cs);

		int i = buffer.index - 1;
		int printed = 0;

		while (i != 0)
		{
			cout << buffer.records[i].IDT << " " << buffer.records[i].priority << " " << buffer.records[i].tick << " [" << buffer.records[i].message << "]" << endl;
			printed++;

			if (printed == 5) {
				cout << endl << endl;
				break;
			}

			i--;
			 
			if (i == 0) {
				cout << endl << endl;
			}
		}

		LeaveCriticalSection(&cs);

		Sleep(100);
	}
}

int main()
{
	setlocale(LC_ALL, "ru");

	InitializeCriticalSection(&cs);

	T[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Logger, NULL, NULL, &Tid[0]);
	if (T[0] == NULL) {
		return GetLastError();
	}
	T[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Logger, NULL, NULL, &Tid[1]);
	if (T[1] == NULL) {
		return GetLastError();
	}
	T[2] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Logger, NULL, NULL, &Tid[2]);
	if (T[2] == NULL) {
		return GetLastError();
	}
	T[3] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Seeker, NULL, NULL, &Tid[3]);
	if (T[3] == NULL) {
		return GetLastError();
	}

	if (!SetThreadPriority(T[0], THREAD_PRIORITY_HIGHEST)) {
		cout << "Ошибка";
	}
	if (!SetThreadPriority(T[1], THREAD_PRIORITY_NORMAL)) {
		cout << "Ошибка";
	}
	if (!SetThreadPriority(T[2], THREAD_PRIORITY_BELOW_NORMAL)) {
		cout << "Ошибка";
	}
	if (!SetThreadPriority(T[3], THREAD_PRIORITY_IDLE)) {
		cout << "Ошибка";
	}

	WaitForMultipleObjects(4, T, TRUE, INFINITE);

	DeleteCriticalSection(&cs);

	CloseHandle(T[0]);
	CloseHandle(T[1]);
	CloseHandle(T[2]);
	CloseHandle(T[3]);

	return 0;
}