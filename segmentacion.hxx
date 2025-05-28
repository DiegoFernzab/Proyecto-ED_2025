#ifndef SEGMENTACION_HXX
#define SEGMENTACION_HXX

#include "segmentacion.h"
#include <vector>
#include <queue>
#include <cmath>        // Para std::abs
#include <limits>       // Para std::numeric_limits
#include <tuple>        // Para usar en la cola de prioridad
#include <iostream>     // Para mensajes de error/debug

using namespace std;

// --- ADAPTACIÓN DE ESTRUCTURAS Y ALGORITMO ---

// Estado para la cola de prioridad de Dijkstra
// Guarda: costo acumulado, coordenada x, coordenada y, etiqueta de la semilla origen
using EstadoDijkstra = std::tuple<double, int, int, int>; // cost, x, y, label

// Función auxiliar para calcular el costo entre píxeles vecinos
inline double calcular_costo_arista(int intensidad1, int intensidad2) {
    // Costo simple: diferencia absoluta. Podría ser otra métrica.
    return std::abs(intensidad1 - intensidad2);
}

bool segmentar_imagen_dijkstra(const Imagen& imagen_entrada,
                             const std::vector<Seed>& semillas,
                             Imagen& imagen_salida) {

    if (imagen_entrada.datos.empty() || imagen_entrada.alto <= 0 || imagen_entrada.ancho <= 0) {
        cerr << "Error [Segmentacion]: La imagen de entrada está vacía o tiene dimensiones inválidas.\n";
        return false;
    }
    if (semillas.empty()) {
        cerr << "Error [Segmentacion]: No se proporcionaron semillas.\n";
        return false;
    }

    int H = imagen_entrada.alto;
    int W = imagen_entrada.ancho;

    // --- Inicialización de estructuras de datos ---
    // Costo mínimo encontrado para llegar a cada píxel
    vector<vector<double>> min_cost(H, vector<double>(W, numeric_limits<double>::infinity()));
    // Etiqueta de la región asignada a cada píxel (-1 = no asignado)
    vector<vector<int>> region_label(H, vector<int>(W, -1));

    // Cola de prioridad (min-heap basado en el costo)
    priority_queue<EstadoDijkstra, vector<EstadoDijkstra>, greater<EstadoDijkstra>> pq;

    // --- Añadir semillas iniciales a la cola y estructuras ---
    for (const auto& seed : semillas) {
        // Validar coordenadas de la semilla
        if (seed.x < 0 || seed.x >= W || seed.y < 0 || seed.y >= H) {
            cerr << "Error [Segmentacion]: Semilla en (" << seed.x << "," << seed.y << ") está fuera de los límites de la imagen (" << W << "x" << H << ").\n";
            return false; // O podrías simplemente ignorar la semilla inválida
        }
        // Validar etiqueta
        if (seed.label < 1 || seed.label > 255) {
             cerr << "Error [Segmentacion]: Etiqueta de semilla " << seed.label << " inválida (debe ser 1-255).\n";
             return false;
        }

        // Inicializar el píxel de la semilla
        min_cost[seed.y][seed.x] = 0.0;
        region_label[seed.y][seed.x] = seed.label;
        pq.push({0.0, seed.x, seed.y, seed.label});
        //cout << "[DEBUG] Semilla añadida: (" << seed.x << "," << seed.y << "), Label: " << seed.label << endl; // Debug
    }

    // --- Bucle Principal de Dijkstra (Adaptado para Segmentación) ---
    // Direcciones de los vecinos (4-conectividad)
    int dx[] = {0, 1, 0, -1};
    int dy[] = {-1, 0, 1, 0}; // Arriba, Derecha, Abajo, Izquierda

    while (!pq.empty()) {
        auto [current_cost, x, y, current_label] = pq.top();
        pq.pop();

        // Optimización: Si ya encontramos un camino MEJOR (menor costo)
        // hacia este píxel ANTES, ignoramos este estado. Esto es crucial
        // porque un píxel puede ser alcanzado desde múltiples semillas.
        // PERO, como queremos asignar la etiqueta del PRIMERO que llega,
        // el chequeo principal es si ya tiene una etiqueta asignada (!= -1).
        // Este chequeo de costo > min_cost es más para Dijkstra estándar.
        // Lo dejamos por si acaso, pero el chequeo clave es region_label[y][x] != -1
         if (current_cost > min_cost[y][x]) {
             continue;
         }
         // Si ya tiene etiqueta, ya fue "conquistado" por una semilla (la primera que llegó)
         // No hacemos nada más para este píxel desde esta ruta.
         // (Este chequeo se hace implícitamente al verificar region_label[ny][nx] == -1 abajo)


        // Explorar vecinos
        for (int i = 0; i < 4; ++i) {
            int nx = x + dx[i];
            int ny = y + dy[i];

            // Verificar si el vecino está dentro de los límites de la imagen
            if (nx >= 0 && nx < W && ny >= 0 && ny < H) {
                // Solo procesar si el vecino NO ha sido asignado a ninguna región aún
                if (region_label[ny][nx] == -1) {
                    // Calcular costo de la arista (basado en diferencia de intensidad)
                    double cost_edge = calcular_costo_arista(imagen_entrada.datos[y][x], imagen_entrada.datos[ny][nx]);
                    double new_cost = current_cost + cost_edge;

                    // ¡Encontramos el PRIMER camino hacia este vecino!
                    min_cost[ny][nx] = new_cost;
                    region_label[ny][nx] = current_label; // Asignar la etiqueta de la semilla actual
                    pq.push({new_cost, nx, ny, current_label}); // Añadir a la cola para explorar desde él
                }
                // Si ya tiene etiqueta (region_label[ny][nx] != -1), no hacemos nada.
                // La primera semilla que lo alcanzó ya lo reclamó.
            }
        }
    }

    // --- Preparar la imagen de salida ---
    imagen_salida.ancho = W;
    imagen_salida.alto = H;
    imagen_salida.nombre = "segmentada"; // Nombre genérico
    imagen_salida.datos = region_label; // Asignar la matriz de etiquetas

    // Opcional: Manejar píxeles que quedaron sin asignar (-1) si alguna región no se expandió
    for (int r = 0; r < H; ++r) {
        for (int c = 0; c < W; ++c) {
            if (imagen_salida.datos[r][c] == -1) {
                imagen_salida.datos[r][c] = 0; // Asignar etiqueta 0 (o alguna por defecto)
            }
        }
    }

    cout << "[Segmentacion] Proceso completado.\n";
    return true;
}

#endif // SEGMENTACION_HXX