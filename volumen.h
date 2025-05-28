#ifndef VOLUMEN_H
#define VOLUMEN_H

#include <vector>
#include <string>
#include "imagen.h" // Necesario porque Volumen contiene un vector<Imagen>

using namespace std;

// Estructura que representa un volumen 3D como una lista de imágenes 2D.
struct Volumen {
    string base_nombre;            // Prefijo base del conjunto de archivos
    int num_imagenes = 0;          // Cantidad de imágenes que forman el volumen
    int ancho = 0;                 // Ancho promedio de las imágenes
    int alto = 0;                  // Alto promedio de las imágenes
    vector<Imagen> imagenes;       // Lista de imágenes 2D (slices)
    bool cargado = false;          // Indica si el volumen se cargó correctamente
};

// --- Declaraciones de Funciones Asociadas a Volúmenes ---

// Carga un conjunto de imágenes PPM en un objeto Volumen.
bool cargar_volumen(const string& nombre_base, int cantidad, Volumen& volumen);

// Genera una proyección 2D del volumen según una dirección y criterio.
Imagen proyeccion2D(const Volumen& volumen, char direccion, const string& criterio);

// Muestra en consola la información básica del volumen cargado.
void info_volumen(const Volumen& volumen);

// Aplica un criterio (min, max, prom, med) a un vector de valores de píxeles.
int aplicar_criterio(vector<int>& valores, const string& criterio);


#endif // VOLUMEN_H


