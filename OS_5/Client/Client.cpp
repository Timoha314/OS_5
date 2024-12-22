#include <windows.h>
#include <iostream>
#include <vector>
#include <random>
#include <conio.h>

using namespace std;

int main() {
    HANDLE ReadPipe1 = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE WritePipe2 = GetStdHandle(STD_OUTPUT_HANDLE);

    DWORD bytesRead, bytesWritten;

    long size, N, M;

    // Чтение размера массива, N и M от Server
    ReadFile(ReadPipe1, &size, sizeof(size), &bytesRead, NULL);
    ReadFile(ReadPipe1, &N, sizeof(N), &bytesRead, NULL);
    ReadFile(ReadPipe1, &M, sizeof(M), &bytesRead, NULL);

    cout << "Received from Server:" << endl;
    cout << "Array size: " << size << endl;
    cout << "N: " << N << ", M: " << M << endl;

    // Генерация элементов массива в диапазоне [N, M]
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<long> dis(N, M);

    vector<long> generatedArray(size);
    for (long& val : generatedArray) {
        val = dis(gen);
    }

    cout << "Generated array: ";
    for (long val : generatedArray) {
        cout << val << " ";
    }
    cout << endl;

    // Отправка массива обратно в Server
    WriteFile(WritePipe2, generatedArray.data(), size * sizeof(long), &bytesWritten, NULL);

    cout << "Press 'Q' to exit..." << endl;
    while (1) {}

    return 0;
}
