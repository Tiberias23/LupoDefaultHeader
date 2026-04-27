//
// Created by lupo on 25.11.25.
//
#include <lupo_default_header.hpp>
#include <iostream>
#include <string>

#ifdef _WIN32
#define NOMINMAX
#define byte win_byte_override
#include <windows.h>
#undef byte
#include <fcntl.h>
#include <io.h>

/**
 * @brief Aktiviert die Verwendung von ANSI-Farbcodes in der Windows-Konsole.
 */
void activate_ansi_escape_on_windows() {
    int result;
    result = _setmode(_fileno(stdout), _O_TEXT);
    if (result == -1) {
        std::cerr << "Error setting Mode for Win" << std::endl;
    }
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
        return;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode)) {
        std::cerr << "Warnung: ANSI-Farben werden eventuell nicht unterstuetzt.\n";
    }
}

void sleep_seconds(int s) {
    Sleep(s * 1000); // die Sleep funktion erwartet ms nicht s
}

void sleep_milliseconds(int ms) {
    Sleep(ms);
}

void clear_screen() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD count;
    DWORD cellCount;

    if (hOut == INVALID_HANDLE_VALUE)
        return;

    if (!GetConsoleScreenBufferInfo(hOut, &csbi))
        return;
    cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    if (!FillConsoleOutputCharacter(hOut, ' ', cellCount, {0, 0}, &count))
        return;
    if (!FillConsoleOutputAttribute(hOut, csbi.wAttributes, cellCount, {0, 0}, &count))
        return;

    SetConsoleCursorPosition(hOut, {0, 0});
}

void setTerminalTitle(const std::string& title) {
    SetConsoleTitle(title.c_str()); // winapi
}
#else
#include <unistd.h>

void activate_ansi_escape_on_windows() {
    // Unter Linux oder anderen Systemen ist keine Aktion nötig
}

void sleep_seconds(const int s) {
    sleep(s); // Sekunden
}

void sleep_milliseconds(const unsigned int ms) {
    sleep(ms/1000);
}

void clear_screen() {
    std::cout << "\033[2J\033[H";
}

void setTerminalTitle(const std::string& title) {
    std::cout << "\033]0;" << title << "\007" << std::flush;
}
#endif
