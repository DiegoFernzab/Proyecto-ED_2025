#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <string>
#include "imagen.h" // Necesitamos la definición de Imagen

using namespace std;



bool codificar_huffman(const Imagen& imagen_entrada, const string& archivo_salida);
bool decodificar_huffman(const string& archivo_entrada, Imagen& imagen_salida);

#endif // HUFFMAN_H
