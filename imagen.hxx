#ifndef IMAGEN_HXX
#define IMAGEN_HXX

#include "imagen.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <numeric>

using namespace std;

// Leer imagen PGM (escala de grises)
bool leer_pgm(const string& nombre_archivo, vector<vector<int>>& img, int& w, int& h) {
    ifstream archivo(nombre_archivo);
		if (!archivo) return false;

		string tipo;
		archivo >> tipo;// Lee el encabezado (debe ser "P2")
		if (tipo != "P2") return false;

		archivo >> w >> h; // Lee dimensiones de la imagen
		int max_val;
		archivo >> max_val;// Lee el valor máximo de gris (normalmente 255)

		img.assign(h, vector<int>(w));// Asigna espacio en la matriz de píxeles
		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w; j++) {
				archivo >> img[i][j];// Carga cada píxel en la matriz
			}
		}

		return true;
}

// Leer imagen PPM (color, convertido a escala de grises)
bool leer_ppm(const string& nombre_archivo, Imagen& img) {
    ifstream archivo(nombre_archivo);
		if (!archivo) {
			cout << "Error: No se pudo abrir " << nombre_archivo << "\n";
			return false;
		}

		string tipo;
		archivo >> tipo;
		if (tipo != "P3") {//encabezado de archivos ppm
			cout << "Error: Formato incorrecto en " << nombre_archivo << " (Se esperaba P3)\n";
			return false;
		}

		archivo >> img.ancho >> img.alto;
		int max_val;
		archivo >> max_val;

		cout << "Leyendo " << nombre_archivo << " - Ancho: " << img.ancho << ", Alto: " << img.alto << "\n";

		img.nombre = nombre_archivo;
		img.datos.assign(img.alto, vector<int>(img.ancho * 3)); // RGB tiene 3 valores por pixel

		for (int i = 0; i < img.alto; i++) {
			for (int j = 0; j < img.ancho * 3; j++) {
				archivo >> img.datos[i][j];
			}
		}

		return true;
}

// Guardar imagen en formato PGM
bool guardar_pgm(const string& nombre_archivo, const vector<vector<int>>& imagen) {
    ofstream archivo(nombre_archivo);
    if (!archivo) return false;

    int w = imagen[0].size();
    int h = imagen.size();

    archivo << "P2\n";//Escribe el encabezado del archivo PGM
    archivo << w << " " << h << "\n";
    archivo << "255\n";// Escribe el valor máximo de gris

    for (const auto& fila : imagen) {
        for (int pixel : fila) {
            archivo << pixel << " ";// Escribe cada píxel
        }
        archivo << "\n";
    }

    return true;
}


int mediana_optimizada(vector<int>& valores) {
    const auto mid = valores.begin() + valores.size() / 2;
    nth_element(valores.begin(), mid, valores.end());

    if (valores.size() % 2 == 1) {
        return *mid;
    } else {
        const auto left_mid = valores.begin() + (valores.size() - 1) / 2;
        nth_element(valores.begin(), left_mid, valores.end());
        int a = *left_mid;
        nth_element(left_mid + 1, left_mid + 1, valores.end());
        return (a + *(left_mid + 1)) / 2;
    }
}

#endif
