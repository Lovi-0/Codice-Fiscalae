#ifndef STRING_HELPER_HPP
#define STRING_HELPER_HPP

#include <array>
#include <string>
#include <string_view>
#include <unordered_set>
#include <algorithm>
#include <cctype>
#include <locale>

namespace StringHelper {
    using sv = std::string_view;

    const std::unordered_set<char> VOWELS = { 'A', 'E', 'I', 'O', 'U' };
    const std::array<int, 36> EVEN_VALUES = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,  // 0-9
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25  // A-Z
    };
    const std::array<int, 36> ODD_VALUES = {
        1, 0, 5, 7, 9, 13, 15, 17, 19, 21,  // 0-9
        1, 0, 5, 7, 9, 13, 15, 17, 19, 21, 2, 4, 18, 20, 11, 3, 6, 8, 12, 14, 16, 10, 22, 25, 24, 23  // A-Z
    };


    inline int getCharIndex(char c) {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
        return 0;
    }

    inline std::string toUpper(const std::string& input) {
        std::string result = input;

        std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
            return std::toupper(c);
        });

        return result;
    }

    inline bool isVowel(char c) {
        return VOWELS.count(std::toupper(c)) > 0;
    }

    inline std::string normalizeString(std::string_view input) {
        std::string result;
        result.reserve(input.length());
        size_t start = 0;

        // Salta gli spazi iniziali
        while (start < input.length() && std::isspace(static_cast<unsigned char>(input[start]))) {
            ++start;
        }

        size_t end = input.length();

        // Salta gli spazi finali
        while (end > start && std::isspace(static_cast<unsigned char>(input[end - 1]))) {
            --end;
        }

        // Converti in maiuscolo durante la copia
        for (size_t i = start; i < end; ++i) {
            result += static_cast<char>(std::toupper(static_cast<unsigned char>(input[i])));
        }

        return result;
    }

    inline std::string extractConsonantsVowels(sv input, bool isName) {
        std::string consonants;
        std::string vowels;
        consonants.reserve(input.length());
        vowels.reserve(input.length());

        // Normalizza e estrai consonanti e vocali
        for (char c : input) {
            c = std::toupper(c);

            if (std::isalpha(c)) {
                if (!isVowel(c)) {
                    consonants += c;
                }
                else {
                    vowels += c;
                }
            }
        }

        std::string result;
        result.reserve(3);

        if (isName && consonants.length() >= 4) {
            // Per il nome con 4+ consonanti: prima, terza e quarta consonante
            result = std::string{ consonants[0], consonants[2], consonants[3] };
        }
        else {

            // Per cognome o nome con meno di 4 consonanti
            if (consonants.length() >= 3) {

                // Se ci sono almeno 3 consonanti, prendi le prime 3
                result = consonants.substr(0, 3);
            }
            else {

                // Altrimenti prendi tutte le consonanti disponibili
                result = consonants;

                // Aggiungi le vocali necessarie
                result += vowels.substr(0, 3 - consonants.length());

                // Se ancora mancano caratteri, aggiungi 'X'
                while (result.length() < 3) {
                    result += 'X';
                }
            }
        }

        return result;
    }

    // Calcola la lettera di controllo
    inline char calculateControlLetter(sv fiscalCode) {
        int sum = 0;

        // Processa i caratteri in posizione dispari (indice 0-based)
        for (size_t i = 0; i < fiscalCode.length(); i += 2) {
            int idx = getCharIndex(fiscalCode[i]);
            sum += ODD_VALUES[idx];
        }

        // Processa i caratteri in posizione pari (indice 0-based)
        for (size_t i = 1; i < fiscalCode.length(); i += 2) {
            int idx = getCharIndex(fiscalCode[i]);
            sum += EVEN_VALUES[idx];
        }

        // Il resto della divisione per 26 determina la lettera di controllo
        return 'A' + (sum % 26);
    }

    inline std::string formatDay(int day, char gender) {
        std::string dayCode = std::to_string(gender == 'M' ? day : day + 40);
        return (dayCode.length() == 1) ? "0" + dayCode : dayCode;
    }

    inline std::string formatYear(int year) {
        return std::to_string(year).substr(2, 2);
    }

    inline bool IsValidDate(const char* date_str) {

        // Controlla la lunghezza e il formato base
        if (strlen(date_str) != 10 || date_str[4] != '-' || date_str[7] != '-') {
            return false;
        }

        int year, month, day;
        if (sscanf_s(date_str, "%4d-%2d-%2d", &year, &month, &day) != 3) {
            return false;  // formato non valido
        }

        // Controlla i limiti di mese e giorno
        if (month < 1 || month > 12 || day < 1 || day > 31) {
            return false;
        }

        // Controllo sui giorni del mese
        int days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))) {
            days_in_month[1] = 29;  // Anno bisestile
        }

        return day <= days_in_month[month - 1];
    }
}

#endif