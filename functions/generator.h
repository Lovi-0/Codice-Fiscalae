#ifndef GENERATOR_H
#define GENERATOR_H

#include <unordered_map>
#include <string>
#include <future>
#include "stringHelper.hpp"

struct InputData {
    char nome[128] = "";
    char cognome[128] = "";
    char data_nascita[11] = "";                     // Formato: YYYY-MM-DD
    char luogo_nascita[128] = "";
    char provincia[3] = "";
    int sesso = 0;                                  // 0 per maschio, 1 per femmina
    const char* sesso_items[2] = { "M", "F" };
    std::string risultato;
};

class FiscalCodeGenerator {
private:
    std::unordered_map<std::string, std::string> municipal_codes;
    std::vector<std::string> municipalities;        // Lista completa dei comuni
    std::vector<std::string> provinces;             // Lista completa delle province
    const std::unordered_map<int, char> MONTH_TO_LETTER;

public:
    FiscalCodeGenerator();
    void loadMunicipalCodes(const std::string& filename);
    std::string generateFiscalCode(const InputData& inputData);
    std::vector<std::string> getSuggestedMunicipalities(const std::string& partial);
    std::vector<std::string> getSuggestedProvinces(const std::string& partial);
    static bool startsWithIgnoreCase(const std::string& str, const std::string& prefix);
};

#endif