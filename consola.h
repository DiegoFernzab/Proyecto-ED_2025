#ifndef CONSOLA_H
#define CONSOLA_H

#include <string>           // Para usar std::string
#include <vector>           // Para usar std::vector
#include <unordered_map>    // Para usar std::unordered_map
#include <functional>       // Para usar std::function

// Incluir las definiciones de las estructuras que usa la consola
#include "imagen.h"         // Contiene la definición de 'struct Imagen'
#include "volumen.h"        // Contiene la definición de 'struct Volumen'

// Usar el espacio de nombres std para no tener que escribir std:: constantemente
using namespace std;

// --- Declaración de la Clase ConsolaInteractiva ---
class ConsolaInteractiva {
private:
    
    unordered_map<string, function<void(const vector<string>&)>> comandos;

    Imagen imagen_actual;

    Volumen volumen_actual;

public:
    
    ConsolaInteractiva();


    void ejecutar();

private:
    // --- Métodos Privados (Auxiliares y de Comandos) ---

    // Procesa una línea de entrada del usuario para identificar y ejecutar un comando
    void procesar_comando(const string& entrada);

    // Implementación del comando 'ayuda'
    void ayuda(const vector<string>& args);

    // Implementación del comando 'salir'
    void salir(const vector<string>& args); // No necesita const si llama a exit()

    // Implementación del comando 'cargar_imagen'
    void cargar_imagen(const vector<string>& args);

    // Implementación del comando 'info_imagen'
    void info_imagen(const vector<string>& args); // No necesita 'args' si no los usa

    // Implementación del comando 'cargar_volumen'
    void cargar_volumen(const vector<string>& args);

    // Implementación del comando 'info_volumen'
    void info_volumen(const vector<string>& args); // No necesita 'args' si no los usa

    // Implementación del comando 'proyeccion2D'
    void proyeccion2D(const vector<string>& args);

    // Implementación del comando 'codificar_imagen'
    void codificar_imagen(const vector<string>& args);

    // Implementación del comando 'decodificar_archivo'
    void decodificar_archivo(const vector<string>& args);

    // Implementación del comando 'segmentar'
    void segmentar(const vector<string>& args);

    // Implementación del comando 'clr' (limpiar pantalla)
    void clr(const vector<string>& args); // No necesita 'args' si no los usa

    // Devuelve la descripción de un comando (corta o detallada)
    string descripcion_comando(const string& comando, bool detallado = false);

    void mostrar_bienvenida(); 
};

#endif // CONSOLA_H