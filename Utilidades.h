#ifndef UTILIDADES_H
#define UTILIDADES_H

#include "Imagen.h"
#include <vector>
#include <string>

bool leer_pgm(const std::string& nombre_archivo, std::vector<std::vector<int>>& img, int& w, int& h);
bool leer_ppm(const std::string& nombre_archivo, Imagen& img);
bool guardar_pgm(const std::string& nombre_archivo, const std::vector<std::vector<int>>& imagen);
int aplicar_criterio(std::vector<int>& valores, const std::string& criterio);

#endif // UTILIDADES_H