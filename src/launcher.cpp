#include <iostream>
#include <string>
#include <filesystem>
#ifdef _WIN32
#include <windows.h>
#endif

int main() {
    namespace fs = std::filesystem;
    fs::path exe = fs::current_path() / "build" / "catbomb.exe";
    if (!fs::exists(exe)) {
        std::cerr << "Executable not found: " << exe.string() << "\n";
        std::cerr << "Please run build_windows.ps1 or start_game.sh to build the game first." << "\n";
        return 1;
    }

#ifdef _WIN32
    std::string cmd = exe.string();
    std::vector<char> writable(cmd.begin(), cmd.end());
    writable.push_back('\0');

    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);
    if (!CreateProcessA(nullptr, writable.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
        std::cerr << "Failed to launch game (CreateProcess error)." << std::endl;
        return 2;
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
#else
    int r = std::system(exe.string().c_str());
    return r;
#endif

    return 0;
}
