//
// Created by lupo on 25.11.25.
//

#ifndef SCHUL_UEBUNGEN_LUPO_DEFAULT_HEADER_H
#define SCHUL_UEBUNGEN_LUPO_DEFAULT_HEADER_H
#include <string>
#include <chrono>
#include <thread>
#include <vector>

/**
 * @brief Aktiviert die Verwendung von ANSI-Farbcodes in der Windows-Konsole.
 * @return Void
 */
void activate_ansi_escape_on_windows();

/**
 * @brief Sleeps the program for a given amount of seconds
 * @param s the amount of seconds to sleep
 */
inline void sleep_seconds(const unsigned int s) {
    std::this_thread::sleep_for(std::chrono::seconds(s));
}

/**
 * @brief Sleeps the program for a given amount of milliseconds
 * @param ms the amount of milliseconds
 */
inline void sleep_milliseconds(const unsigned int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}


/**
 * @brief Löscht den Bildschirm der Konsole (Cross platform).
 */
void clear_screen();


/**
 * @brief set the console title to a given string, this is just a nice to have feature to make it look a bit nicer
 * @param title the titel to set for the console window
 */
void setTerminalTitle(const std::string& title);

/**
 * @brief Prints a table to the console, the table is given as a two-dimensional vector of strings
 * @param data the data in a two-dimensional vector
 * @param firstRowIsHeader if true, the first row will be printed as a header and separated from the rest of the table with a horizontal line
 * @param rowSeparators if true, every row will be separated with a horizontal line, this ignores the firstRowIsHeader, because then the header will be separated from the rest of the table with a horizontal line anyway
 */
void printTable( const std::vector<std::vector<std::string>>& data, bool firstRowIsHeader = true, bool rowSeparators = false);

//----------------------------------------------------------------------------------------------------------------------
// Hier wird eine Möglichkeit implementiert, um Strings in die Binärdatei einzubetten
//----------------------------------------------------------------------------------------------------------------------

// Hide a Message ore something in the binary
#ifdef _WIN32
#pragma section(".custom", read)
#define EMBED_STRING_IMPL2(id, str)                                                                                    \
    __declspec(allocate(".custom")) static volatile const char _emb_##id[] = str;                                      \
    __declspec(allocate(".custom")) static volatile const char _emb_pad_##id[4] = {0, 0, 0, 0}
#elif defined(__APPLE__)
#define EMBED_STRING_IMPL2(id, str)                                                                                    \
    static const char _emb_##id[] __attribute__((section("__DATA,.custom"), used)) = str;                              \
    static const char _emb_pad_##id[4] __attribute__((section("__DATA,.custom"), used)) = {0, 0, 0, 0}
#else
#define EMBED_STRING_IMPL2(id, str)                                                                                    \
    static const char _emb_##id[] __attribute__((section(".note.custom"), used)) = str;                                \
    static const char _emb_pad_##id[4] __attribute__((section(".note.custom"), used)) = {0, 0, 0, 0}
#endif

#define EMBED_STRING_IMPL(id, str) EMBED_STRING_IMPL2(id, str)
#define EMBED_STRING(str) EMBED_STRING_IMPL(__COUNTER__, str)

#endif // SCHUL_UEBUNGEN_LUPO_DEFAULT_HEADER_H
