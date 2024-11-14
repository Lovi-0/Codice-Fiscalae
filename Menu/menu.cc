#include "../Header Files/menu.hh" 
using namespace ImGui;

#include "../Header Files/globals.hh"
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui.h"

static void HelpMarker(const char* desc);
void CenterText(const char* text);

#include <string>
#include <cstring>
#include <sstream>
#include <cctype>
#include "../functions/generator.h"

InputData input_data;

void ui::renderMenu() {
    static FiscalCodeGenerator generator;

    static bool initialized = false;

    static std::vector<std::string> municipalitySuggestions;
    static std::vector<std::string> provinceSuggestions;

    static bool showMunicipalityList = false;
    static bool showProvinceList = false;

    static int selectedMunicipality = -1;
    static int selectedProvince = -1;

    float width_label = 120.0f;
    float width_input = 200.0f;

    if (!initialized) {
        generator.loadMunicipalCodes("./data/comuni.csv");
        initialized = true;
    }

    if (!globals.active) 
        return;
    
    ImGui::SetNextWindowPos(ImVec2(window_pos.x, window_pos.y), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(window_size.x, window_size.y));
    ImGui::SetNextWindowBgAlpha(1.0f);

    ImGui::Begin(window_title, &globals.active, window_flags);
    {
        // Titolo centrato
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Generatore Codice Fiscale").x) * 0.5f);
        ImGui::Text("Generatore Codice Fiscale");
        ImGui::Separator();
        ImGui::Spacing();

        // Nome
        ImGui::Text("Nome:");
        ImGui::SameLine(width_label);
        ImGui::PushItemWidth(width_input);
        ImGui::InputText("##nome", input_data.cognome, IM_ARRAYSIZE(input_data.cognome));
        ImGui::PopItemWidth();
        ImGui::Spacing();

        // Cognome
        ImGui::Text("Cognome:");
        ImGui::SameLine(width_label);
        ImGui::PushItemWidth(width_input);
        ImGui::InputText("##cognome", input_data.nome, IM_ARRAYSIZE(input_data.nome));
        ImGui::PopItemWidth();
        ImGui::Spacing();

        // Data di nascita
        ImGui::Text("Data di nascita:");
        ImGui::SameLine(width_label);
        ImGui::PushItemWidth(width_input);
        ImGui::InputText("##data", input_data.data_nascita, IM_ARRAYSIZE(input_data.data_nascita));
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Formato: YYYY-MM-GG");
        ImGui::PopItemWidth();
        ImGui::Spacing();

        // Validazione della data e messaggio di errore
        if (strlen(input_data.data_nascita) > 0 && !StringHelper::IsValidDate(input_data.data_nascita)) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Data non valida!");
        }

        // Luogo di nascita con autocompletamento
        ImGui::Text("Luogo di nascita:");
        ImGui::SameLine(width_label);
        ImGui::PushItemWidth(width_input);
        if (ImGui::InputText("##luogo", input_data.luogo_nascita, IM_ARRAYSIZE(input_data.luogo_nascita))) {
            municipalitySuggestions = generator.getSuggestedMunicipalities(input_data.luogo_nascita);
            showMunicipalityList = !municipalitySuggestions.empty();
            selectedMunicipality = -1;
        }

        // Lista suggerimenti comuni
        if (showMunicipalityList && !municipalitySuggestions.empty()) {
            ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
            //ImGui::SetNextWindowSize(ImVec2(width_input, std::min(municipalitySuggestions.size() * ImGui::GetTextLineHeightWithSpacing(), 200.0f)));

            if (ImGui::Begin("##municipalitySuggestions", nullptr,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_Tooltip)) {

                for (int i = 0; i < municipalitySuggestions.size(); i++) {
                    if (ImGui::Selectable(municipalitySuggestions[i].c_str(), selectedMunicipality == i)) {
                        strncpy_s(input_data.luogo_nascita, municipalitySuggestions[i].c_str(), sizeof(input_data.luogo_nascita) - 1);
                        showMunicipalityList = false;
                    }
                }
            }
            ImGui::End();
        }
        ImGui::PopItemWidth();
        ImGui::Spacing();

        // Provincia con autocompletamento
        ImGui::Text("Provincia:");
        ImGui::SameLine(width_label);
        ImGui::PushItemWidth(width_input);
        if (ImGui::InputText("##provincia", input_data.provincia, IM_ARRAYSIZE(input_data.provincia))) {
            provinceSuggestions = generator.getSuggestedProvinces(input_data.provincia);
            showProvinceList = !provinceSuggestions.empty();
            selectedProvince = -1;
        }

        // Lista suggerimenti province
        if (showProvinceList && !provinceSuggestions.empty()) {
            ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
            //ImGui::SetNextWindowSize(ImVec2(width_input, std::min(provinceSuggestions.size() * ImGui::GetTextLineHeightWithSpacing(), 200.0f)));

            if (ImGui::Begin("##provinceSuggestions", nullptr,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_Tooltip)) {

                for (int i = 0; i < provinceSuggestions.size(); i++) {
                    if (ImGui::Selectable(provinceSuggestions[i].c_str(), selectedProvince == i)) {
                        strncpy_s(input_data.provincia, provinceSuggestions[i].c_str(), sizeof(input_data.provincia) - 1);
                        showProvinceList = false;
                    }
                }
            }
            ImGui::End();
        }
        ImGui::PopItemWidth();
        ImGui::Spacing();

        // Sesso (Combo Box)
        ImGui::Text("Sesso:");
        ImGui::SameLine(width_label);
        ImGui::PushItemWidth(70);
        ImGui::Combo("##sesso", &input_data.sesso, input_data.sesso_items, IM_ARRAYSIZE(input_data.sesso_items));
        ImGui::PopItemWidth();
        ImGui::Spacing();
        ImGui::Spacing();

        // Pulsante Calcola centrato
        float button_width = 120.0f;
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);

        
        bool validData = true;
        std::string errorMessage = "";
       
        if (ImGui::Button("Calcola", ImVec2(button_width, 30))) {
            
            // Validation: Check if all required fields are filled
            if (strlen(input_data.nome) == 0) {
                validData = false;
                errorMessage += "Nome mancante.\n";
            }
            if (strlen(input_data.cognome) == 0) {
                validData = false;
                errorMessage += "Cognome mancante.\n";
            }
            if (strlen(input_data.data_nascita) == 0 || !StringHelper::IsValidDate(input_data.data_nascita)) {
                validData = false;
                errorMessage += "Data di nascita mancante o non valida.\n";
            }
            if (strlen(input_data.luogo_nascita) == 0) {
                validData = false;
                errorMessage += "Luogo di nascita mancante.\n";
            }
            if (strlen(input_data.provincia) == 0) {
                validData = false;
                errorMessage += "Provincia mancante.\n";
            }
            if (input_data.sesso < 0 || input_data.sesso >= IM_ARRAYSIZE(input_data.sesso_items)) {
                validData = false;
                errorMessage += "Sesso mancante.\n";
            }

            //ImGui::Spacing();
            //ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", errorMessage.c_str());

            if (validData) {
                std::string risultato = generator.generateFiscalCode(input_data);
                input_data.risultato = risultato;
            }
            else {
                input_data.risultato = "";
            }
        }

        // Mostra il risultato se presente
        if (!input_data.risultato.empty()) {
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::TextWrapped("Risultato: %s", input_data.risultato.c_str());
        }
    }
    ImGui::End();
}

void ui::init(LPDIRECT3DDEVICE9 device) {
    dev = device;
    ui::RunStyle();

    if (window_pos.x == 0) {
        RECT screen_rect{};
        GetWindowRect(GetDesktopWindow(), &screen_rect);
        screen_res = ImVec2(float(screen_rect.right), float(screen_rect.bottom));
        window_pos = (screen_res - window_size) * 0.5f;
    }
}

void ui::RunStyle(){
    ImGuiStyle& style = ImGui::GetStyle();

    style.Colors[ImGuiCol_TitleBgActive] = ImColor(0, 0, 0, 155);
    style.Colors[ImGuiCol_WindowBg] = ImColor(color.all[0], color.all[1], color.all[2], color.all[3]);
    style.Colors[ImGuiCol_ChildBg] = ImColor(5, 5, 5, 255);
    style.Colors[ImGuiCol_Button] = ImColor(color.all[0], color.all[1], color.all[2], color.all[3]);
    style.Colors[ImGuiCol_ButtonActive] = ImColor(0, 0, 0, 255);
    style.Colors[ImGuiCol_ButtonHovered] = ImColor(color.all[0], color.all[1], color.all[2], color.all[3]);
    style.Colors[ImGuiCol_CheckMark] = ImColor(color.all[0], color.all[1], color.all[2], color.all[3]);
    style.Colors[ImGuiCol_FrameBg] = ImColor(30, 30, 30, 205);
    style.Colors[ImGuiCol_FrameBgHovered] = ImColor(35, 35, 35, 205);
    style.Colors[ImGuiCol_FrameBgActive] = ImColor(color.all[0], color.all[1], color.all[2], color.all[3]);
    style.Colors[ImGuiCol_SliderGrab] = ImColor(color.all[0], color.all[1], color.all[2], color.all[3]);
    style.Colors[ImGuiCol_SliderGrabActive] = ImColor(color.all[0], color.all[1], color.all[2], color.all[3]);
    style.Colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);
}

static void HelpMarker(const char* desc){
    ImGui::TextDisabled("(?)");

    if (ImGui::IsItemHovered()){
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void CenterText(const char* text){
    if (text == nullptr)
        return;

    ImGui::Spacing();
    ImGui::SameLine((ImGui::GetContentRegionAvail().x / 2) - (ImGui::CalcTextSize(text).x / 2));
    ImGui::Text(text);
    ImGui::Spacing();
}