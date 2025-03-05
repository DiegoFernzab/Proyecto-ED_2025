#include "ConsolaInteractiva.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <numeric>
using namespace std;

ConsolaInteractiva::ConsolaInteractiva() {
    comandos["ayuda"] = [this](const vector<string>& args) { this->ayuda(args); };
    comandos["salir"] = [this](const vector<string>& args) { this->salir(args); };
    comandos["cargar_imagen"] = [this](const vector<string>& args) { this->cargar_imagen(args); };
    comandos["info_imagen"] = [this](const vector<string>& args) { this->info_imagen(args); };
    comandos["proyeccion2D"] = [this](const vector<string>& args) { this->proyeccion2D(args); };
    comandos["decodificar_archivo"] = [this](const vector<string>& args) { this->decodificar_archivo(args); };
    comandos["cargar_volumen"] = [this](const vector<string>& args) { this->cargar_volumen(args); };
    comandos["info_volumen"] = [this](const vector<string>& args) { this->info_volumen(args); };
    comandos["codificar_imagen"] = [this](const vector<string>& args) { this->codificar_imagen(args); };
    comandos["segmentar"] = [this](const vector<string>& args) { this->segmentar(args); };
    comandos["clr"] = [this](const vector<string>& args) { this->clr(args); };
}

void ConsolaInteractiva::ejecutar() {
    string entrada;
    while (true) {
        cout << "$ ";
        if (!getline(cin, entrada)) {
            cout << "\nSaliendo...\n";
            break;
        }
        procesar_comando(entrada);
    }
}

void ConsolaInteractiva::procesar_comando(const string& entrada) {
    istringstream stream(entrada);
    vector<string> tokens;
    string token;
    while (stream >> token) {
        tokens.push_back(token);
    }
    if (tokens.empty()) return;
    string comando = tokens[0];
    tokens.erase(tokens.begin());
    auto it = comandos.find(comando);
    if (it != comandos.end()) {
        it->second(tokens);
    } else {
        cout << "Error: '" << comando << "' no es un comando reconocido. Usa 'ayuda' para ver los comandos disponibles.\n";
    }
}

void ConsolaInteractiva::ayuda(const vector<string>&) {
    cout << "Comandos disponibles:\n";
    for (const auto& cmd : comandos) {
        cout << "  " << cmd.first << "\n";
    }
}