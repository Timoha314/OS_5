#include <windows.h>
#include <iostream>
#include <vector>
#include <conio.h>

using namespace std;

int main() {
    HANDLE ReadPipe1, WritePipe1, ReadPipe2, WritePipe2;
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.lpSecurityDescriptor = NULL;
    saAttr.bInheritHandle = TRUE;

    // Создание анонимных каналов
    if (!CreatePipe(&ReadPipe1, &WritePipe1, &saAttr, 0)) {
        cerr << "Error creating channel 1" << endl;
        return 1;
    }

    if (!CreatePipe(&ReadPipe2, &WritePipe2, &saAttr, 0)) {
        cerr << "Error creating channel 2" << endl;
        return 1;
    }

    // Настройка дочернего процесса
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdInput = ReadPipe1;
    si.hStdOutput = WritePipe2;
    si.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    // Ввод данных
    long n;
    cout << "Enter array size (long): ";
    cin >> n;

    long N, M;
    cout << "Enter N and M (N < M): ";
    cin >> N >> M;

    // Запуск процесса Client
    wstring Arg = L"Client.exe";
    if (!CreateProcess(NULL, &Arg[0], NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        cerr << "Error starting Client process" << endl;
        return 1;
    }

    // Передача данных Client'у
    DWORD bytesWritten;
    WriteFile(WritePipe1, &n, sizeof(n), &bytesWritten, NULL);
    WriteFile(WritePipe1, &N, sizeof(N), &bytesWritten, NULL);
    WriteFile(WritePipe1, &M, sizeof(M), &bytesWritten, NULL);

    // Получение данных от Client'а
    vector<long> receivedArray;
    long temp;
    DWORD bytesRead;

    cout << "Waiting for data from Client..." << endl;
    for (long i = 0; i < n; ++i) {
        if (ReadFile(ReadPipe2, &temp, sizeof(temp), &bytesRead, NULL)) {
            receivedArray.push_back(temp);
        }
        else {
            cerr << "Error reading data from Client" << endl;
            break;
        }
    }

    // Вывод полученной информации
    cout << "Received data from Client:" << endl;
    for (long value : receivedArray) {
        cout << value << " ";
    }
    cout << endl;

    // Ожидание нажатия клавиши "Q"
    cout << "Press 'Q' to exit." << endl;
    while (true) {
        if (_kbhit() && _getch() == 'Q') {
            break;
        }
    }

    // Завершение работы
    CloseHandle(ReadPipe1);
    CloseHandle(WritePipe1);
    CloseHandle(ReadPipe2);
    CloseHandle(WritePipe2);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    cout << "Program terminated." << endl;
    return 0;
}
