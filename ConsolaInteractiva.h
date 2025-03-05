#ifndef CONSOLA_INTERACTIVA_H
#define CONSOLA_INTERACTIVA_H

#include "Imagen.h"
#include "Volumen.h"
#include "Utilidades.h"
#include <unordered_map>
#include <functional>
#include <vector>
#include <string>

class ConsolaInteractiva {
private:
    std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> comandos;
    Imagen imagen_actual;
    Volumen volumen_actual;
    void procesar_comando(const std::string& entrada);
    void ayuda(const std::vector<std::string>& args);
    void salir(const std::vector<std::string>& args);
    void cargar_imagen(const std::vector<std::string>& args);
    void info_imagen(const std::vector<std::string>& args);
    void proyeccion2D(const std::vector<std::string>& args);
    void decodificar_archivo(const std::vector<std::string>& args);
    void cargar_volumen(const std::vector<std::string>& args);
    void info_volumen(const std::vector<std::string>& args);
    void codificar_imagen(const std::vector<std::string>& args);
    void segmentar(const std::vector<std::string>& args);
    void clr(const std::vector<std::string>& args);

public:
    ConsolaInteractiva();
    void ejecutar();
};

#endif // CONSOLA_INTERACTIVA_H