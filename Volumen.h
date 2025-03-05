#ifndef VOLUMEN_H
#define VOLUMEN_H

#include "Imagen.h"
#include <vector>
#include <string>

struct Volumen {
    std::string nombre_base;
    int num_imagenes;
    int ancho;
    int alto;
    std::vector<Imagen> imagenes;
    bool cargado = false;
};

#endif // VOLUMEN_H