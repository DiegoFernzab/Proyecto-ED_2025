#ifndef SEGMENTACION_H
#define SEGMENTACION_H

#include "imagen.h"     // Necesitamos la struct Imagen
#include <vector>
#include <string>

// Estructura simple para representar una semilla
struct Seed {
    int x;
    int y;
    int label;
};
// --- DECLARACIÓN DE LA FUNCIÓN PRINCIPAL DE SEGMENTACIÓN ---

/**
 * @brief Segmenta una imagen utilizando el algoritmo de Dijkstra desde múltiples semillas.
 *
 * @param imagen_entrada La imagen PGM original a segmentar.
 * @param semillas Un vector con las semillas (posición y etiqueta) a utilizar.
 * @param imagen_salida La imagen donde se guardará el resultado de la segmentación (píxeles con etiquetas).
 * @return true si la segmentación fue exitosa, false en caso de error (p.ej., semillas inválidas).
 */
bool segmentar_imagen_dijkstra(const Imagen& imagen_entrada,
                             const std::vector<Seed>& semillas,
                             Imagen& imagen_salida);

#endif // SEGMENTACION_H