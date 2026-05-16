//
// Created by lupo on 25.11.25.
//
#include <lupo_default_header.hpp>
#include <iostream>
#include <string>
#include <vector>

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

void activate_ansi_escape_on_windows() {
    // Unter Linux oder anderen Systemen ist keine Aktion nötig
}

void clear_screen() {
    std::cout << "\033[2J\033[H";
}

void setTerminalTitle(const std::string& title) {
    std::cout << "\033]0;" << title << "\007" << std::flush; // Flush the cout stream
}

#endif

void printTable(
    const std::vector<std::vector<std::string>>& data,
    const bool firstRowIsHeader,
    const bool rowSeparators
) {

    // constructs a struct that sets the console encoding to UTF-8 and sets it back at decunstruction
#ifdef _WIN32
    struct CPGuard {
        UINT prev;
        CPGuard() {
	        prev = GetConsoleOutputCP(); 
        	SetConsoleOutputCP(CP_UTF8);
        }
        ~CPGuard() {
	        SetConsoleOutputCP(prev);
        }
    } cpGuard;
#endif

    if (data.empty()) return;

    auto utf8Len = [&](const std::string& s) -> size_t {
        size_t len = 0;
        for (unsigned char c : s)
            if ((c & 0xC0) != 0x80) ++len;
        return len;
    };

    // Anzahl Spalten (Maximum über alle Zeilen)
    size_t numCols = 0;
    for (const auto& row : data)
        numCols = std::max(numCols, row.size());

    // Spaltenbreiten berechnen
    std::vector<size_t> colWidths(numCols, 0);
    for (const auto& row : data)
        for (size_t i = 0; i < row.size(); ++i)
            colWidths[i] = std::max(colWidths[i], utf8Len(row[i]));

    // Horizontale Linie zeichnen
    auto hLine = [&](const std::string& l, const std::string& m, const std::string& r) {
        std::cout << l;
        for (size_t i = 0; i < numCols; ++i) {
            for (size_t j = 0; j < colWidths[i] + 2; ++j)
                std::cout << "─";
            std::cout << (i < numCols - 1 ? m : r);
        }
        std::cout << "\n";
    };

    // Zeile ausgeben
    auto printRow = [&](const std::vector<std::string>& row) -> void {
        std::cout << "│";
        for (size_t i = 0; i < numCols; ++i) {
            const std::string& cell = (i < row.size()) ? row[i] : "";
            std::cout << " " << cell << std::string(colWidths[i] - utf8Len(cell), ' ') << " │";
        }
        std::cout << "\n";
    };

    hLine("┌", "┬", "┐");

    for (size_t r = 0; r < data.size(); ++r) {
        printRow(data[r]);
        std::flush(std::cout);

        const bool isHeader = (r == 0 && firstRowIsHeader);
        const bool isLast   = (r == data.size() - 1);

        if (isLast)
            hLine("└", "┴", "┘");
        else if (isHeader || rowSeparators)
            hLine("├", "┼", "┤");
    }
}
