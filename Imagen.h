#ifndef IMAGEN_H
#define IMAGEN_H

#include <string>
#include <vector>

struct Imagen {
    std::string nombre;
    int ancho;
    int alto;
    std::vector<std::vector<int>> datos;
};

#endif // IMAGEN_H