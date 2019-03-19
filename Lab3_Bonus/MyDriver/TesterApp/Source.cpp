#include <iostream>
#include <string>
#include <Windows.h>
#include <vector>

#pragma pack(push, 1)
typedef struct _DATA
{
    HANDLE handle;
    ULONG input;
    ULONG output;
    OVERLAPPED inOverlapped;
    OVERLAPPED outOverlapped;
}DATA, *PDATA;
#pragma pack(pop)

std::vector<DATA> gData;

void Create()
{
    auto handle = CreateFile(
        "\\\\.\\MyPipeSymLink",
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
        nullptr
    );

    if (handle == INVALID_HANDLE_VALUE)
    {
        std::wcout << "CreateFile failed! GLE=" << GetLastError() << std::endl;
        throw std::exception("CreateFile failed");
    }

    DATA data = { 0 };
    data.handle = handle;
    data.input = 4821;

    gData.push_back(data);
}

void CleanUp()
{
    for (auto& data : gData)
    {
        CloseHandle(data.handle);
    }
}

void PrintHelp()
{
    std::cout << "You can open 2 or more instances of this program and perform read/write operations\n";
    std::cout << "To force terminating the application you can use ctrl + c. \n";
    std::cout << "Available commands:\n";
    std::cout << "\t>Create - Opens a new pipe instance\n";
    std::cout << "\t>Read [index]- Reads an ULONG from pipe [index]\n";
    std::cout << "\t>Write [index]- Writes an ULONG to pipe [index]\n";
    std::cout << "\t>Cancel [index]- Cancel an io from pipe [index]\n";
    std::cout << "\t>Wait [index]- Waits an io from pipe [index]\n";
    std::cout << "\t>Exit - Clean exit\n";
}

void DoWrite(size_t Index)
{
    DWORD noBytes = 0;

    if (!WriteFile(gData[Index].handle, &gData[Index].input, sizeof(ULONG), &noBytes, &gData[Index].inOverlapped))
    {
        std::cout << "Write file has failed with GLE = " << GetLastError() << "\n";
        return;
    }
    else
    {
        std::cout << "Write file succeded!\n";
    }
}

void DoRead(size_t Index)
{
    DWORD noBytes = 0;

    if (!ReadFile(gData[Index].handle, &gData[Index].output, sizeof(ULONG), &noBytes, &gData[Index].outOverlapped))
    {
        std::cout << "ReadFile file has failed with GLE = " << GetLastError() << "\n";
    }
    else
    {
        std::cout << "Read file succeded!\n";
    }
}

void DoCancel(size_t Index)
{
    if (!CancelIo(gData[Index].handle))
    {
        std::cout << "CancelIo file has failed with GLE = " << GetLastError() << "\n";
    }
    else
    {
        std::cout << "CancelIo file succeded!\n";
    }
}

void DoWait(size_t Index)
{
    DWORD bytes = 0;

    GetOverlappedResult(gData[Index].handle, &gData[Index].inOverlapped, &bytes, true);
    GetOverlappedResult(gData[Index].handle, &gData[Index].outOverlapped, &bytes, true);

    std::cout << "Overlapped result: READ = " << gData[Index].output << " WRITE = " << gData[Index].input << "\n";
}

int main()
{
    std::string command;
    size_t index;

    while (true)
    {
        PrintHelp();
        std::cout << "\n\n\n Your command:\n";

        std::cin >> command;
        if (command == "Read")
        {
            std::cin >> index;
            DoRead(index);
        }
        else if (command == "Write")
        {
            std::cin >> index;
            DoWrite(index);
        }
        else if (command == "Cancel")
        {
            std::cin >> index;
            DoCancel(index);
        }
        else if (command == "Create")
        {
            Create();
        }
        else if (command == "Wait")
        {
            std::cin >> index;
            DoWait(index);
        }
        else if (command == "Exit")
        {
            break;
        }
    }

    CleanUp();
    return 0;
}