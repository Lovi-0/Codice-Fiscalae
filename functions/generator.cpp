#include "generator.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cctype>
#include <chrono>

FiscalCodeGenerator::FiscalCodeGenerator()
    : MONTH_TO_LETTER({
        {1, 'A'}, {2, 'B'}, {3, 'C'}, {4, 'D'}, {5, 'E'}, {6, 'H'},
        {7, 'L'}, {8, 'M'}, {9, 'P'}, {10, 'R'}, {11, 'S'}, {12, 'T'}
        }) {
}

bool FiscalCodeGenerator::startsWithIgnoreCase(const std::string& str, const std::string& prefix) {
    if (prefix.length() > str.length()) 
        return false;

    return std::equal(prefix.begin(), prefix.end(), str.begin(), [](char a, char b) {
        return std::toupper(a) == std::toupper(b); 
    });
}

std::vector<std::string> FiscalCodeGenerator::getSuggestedMunicipalities(const std::string& partial) {
    std::vector<std::string> suggestions;
    if (partial.empty()) 
        return suggestions;

    for (const auto& municipality : municipalities) {
        if (startsWithIgnoreCase(municipality, partial)) {
            suggestions.push_back(municipality);
            if (suggestions.size() >= 10) 
                break;
        }
    }

    return suggestions;
}

std::vector<std::string> FiscalCodeGenerator::getSuggestedProvinces(const std::string& partial) {
    std::vector<std::string> suggestions;
    if (partial.empty()) 
        return suggestions;

    // Normalizza l'input parziale per il confronto
    std::string normalizedPartial = StringHelper::normalizeString(partial);

    for (const auto& province : provinces) {

        // Normalizza la provincia per il confronto
        if (startsWithIgnoreCase(StringHelper::normalizeString(province), normalizedPartial)) {
            suggestions.push_back(province);
            if (suggestions.size() >= 10) 
                break;
        }
    }

    return suggestions;
}

void FiscalCodeGenerator::loadMunicipalCodes(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);

    if (!file)
        throw std::runtime_error("Impossibile aprire il file dei comuni");

    std::string line;
    std::unordered_set<std::string> uniqueProvinces;        // Per evitare duplicati

    while (std::getline(file, line)) {
        size_t firstComma = line.find(',');
        size_t secondComma = line.find(',', firstComma + 1);
        size_t thirdComma = line.find(',', secondComma + 1);
        size_t fourthComma = line.find(',', thirdComma + 1);

        // Controlla che la riga contenga i 5 campi attesi, delimitati da 4 virgole
        if (firstComma != std::string::npos &&
            secondComma != std::string::npos &&
            thirdComma != std::string::npos &&
            fourthComma != std::string::npos) {

            // Estrai i campi separatamente, assegnando ogni colonna a una variabile
            std::string code = line.substr(0, firstComma);
            std::string name = line.substr(firstComma + 1, secondComma - firstComma - 1);
            std::string name_slug = line.substr(secondComma + 1, thirdComma - secondComma - 1);
            std::string name_trans = line.substr(thirdComma + 1, fourthComma - thirdComma - 1);
            std::string province = line.substr(fourthComma + 1);

            // Aggiungi il codice normalizzato alla mappa dei codici comunali
            municipal_codes[StringHelper::normalizeString(name)] = code;

            // Aggiungi il nome del comune alla lista per l'autocompletamento
            municipalities.push_back(name);

            // Normalizza la provincia e aggiungila se non è già presente
            std::string normalizedProvince = StringHelper::normalizeString(province);
            if (uniqueProvinces.insert(normalizedProvince).second) {
                provinces.push_back(province);
            }
        }
    }

    // Ordina le liste per l'autocompletamento
    std::sort(municipalities.begin(), municipalities.end());
    std::sort(provinces.begin(), provinces.end());
}

std::string FiscalCodeGenerator::generateFiscalCode(const InputData& inputData) {

    // Converte i dati da InputData in formato std::string_view
    std::string firstName = inputData.nome;
    std::string surname = inputData.cognome;

    int year = std::stoi(std::string(inputData.data_nascita, 4));
    int month = std::stoi(std::string(inputData.data_nascita + 5, 2));
    int day = std::stoi(std::string(inputData.data_nascita + 8, 2));
    char gender = inputData.sesso_items[inputData.sesso][0];
    std::string municipality = inputData.luogo_nascita;

    // Genera componenti in parallelo
    auto futSurname = std::async(std::launch::async, StringHelper::extractConsonantsVowels, surname, false);
    auto futName = std::async(std::launch::async, StringHelper::extractConsonantsVowels, firstName, true);

    // Mentre nome/cognome vengono processati, gestisci gli altri componenti
    std::string yearCode = StringHelper::formatYear(year);
    char monthCode = MONTH_TO_LETTER.at(month);
    std::string dayCode = StringHelper::formatDay(day, gender);

    // Ottieni il codice del comune
    auto municipalityNorm = StringHelper::normalizeString(municipality);
    auto it = municipal_codes.find(municipalityNorm);

    if (it == municipal_codes.end()) {
        throw std::runtime_error("Comune non trovato: " + municipality);
    }

    // Combina tutti i componenti
    std::string result;
    result.reserve(16);
    result = futSurname.get() + futName.get() + yearCode + monthCode + dayCode + it->second;

    // Aggiungi la lettera di controllo
    result += StringHelper::calculateControlLetter(result);
    StringHelper::toUpper(result);

    return result;
}