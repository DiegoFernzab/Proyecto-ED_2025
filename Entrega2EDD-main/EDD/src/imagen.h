#ifndef IMAGEN_H
#define IMAGEN_H

#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>

using namespace std;

// Estructura que representa una imagen PGM o PPM.
struct Imagen {
    string nombre;                 // Nombre del archivo de la imagen
    int ancho;                     // Número de columnas (pixeles por fila)
    int alto;                      // Número de filas (pixeles por columna)
    vector<vector<int>> datos;    // Matriz de datos de imagen (valores de intensidad o RGB)

    // --- CONSTRUCTORES AÑADIDOS ---

    Imagen() : ancho(0), alto(0) {
    }

    
    Imagen(const string& _nombre, int _ancho, int _alto)
        : nombre(_nombre), ancho(_ancho), alto(_alto) {
        
    }

    
    Imagen(const string& _nombre, int _ancho, int _alto, vector<vector<int>> _datos)
        : nombre(_nombre), ancho(_ancho), alto(_alto), datos(move(_datos)) {
    }


};

// Funciones para leer y guardar imágenes
//constructor por paranetros y ek constructor vacio 
bool leer_pgm(const string& nombre_archivo, vector<vector<int>>& img, int& w, int& h);
bool leer_ppm(const string& nombre_archivo, Imagen& img);
bool guardar_pgm(const string& nombre_archivo, const vector<vector<int>>& imagen);
int mediana_optimizada(vector<int>& valores);

#endif
