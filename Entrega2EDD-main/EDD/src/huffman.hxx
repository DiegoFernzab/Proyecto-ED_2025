#ifndef HUFFMAN_HXX // o NODO_HUFFMAN_HXX si mantienes ese nombre
#define HUFFMAN_HXX

// --- INCLUDES PRIMERO ---
#include "huffman.h"
#include "NodoHuffman.h"     
#include "imagen.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <functional>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>

using namespace std;

// --- Funciones Auxiliares (Opcional: Podrían ir en un namespace interno) ---

// Estructura para comparar nodos en la cola de prioridad (min-heap por frecuencia)
struct ComparadorNodos {
    bool operator()(NodoHuffman* a, NodoHuffman* b) {
        return a->frecuencia > b->frecuencia;
    }
};

// Función recursiva para generar los códigos Huffman
void generar_codigos_recursivo(NodoHuffman* nodo, string codigo_actual, unordered_map<int, string>& codigos) {
    if (!nodo) return;

    // Si es una hoja (tiene un valor de píxel), guarda el código
    if (nodo->valor != -1) { // Usamos -1 para nodos internos como en el original
        // Asegurarse de no asignar código vacío si solo hay un símbolo
        codigos[nodo->valor] = codigo_actual.empty() ? "0" : codigo_actual;
        return;
    }

    // Recursivamente ir a la izquierda (añadir '0') y derecha (añadir '1')
    generar_codigos_recursivo(nodo->izquierda, codigo_actual + "0", codigos);
    generar_codigos_recursivo(nodo->derecha, codigo_actual + "1", codigos);
}

// Función recursiva para liberar la memoria del árbol Huffman
void limpiar_arbol_huffman(NodoHuffman* nodo) {
    if (!nodo) return;
    limpiar_arbol_huffman(nodo->izquierda);
    limpiar_arbol_huffman(nodo->derecha);
    delete nodo;
}


NodoHuffman* construir_arbol_huffman(const unordered_map<int, int>& frecuencias_ocurrencias) {
    priority_queue<NodoHuffman*, vector<NodoHuffman*>, ComparadorNodos> cola_prioridad;

   // Crear nodos hoja SOLO para valores con frecuencia > 0
   for (const auto& par : frecuencias_ocurrencias) {
       if (par.second > 0) {
            cola_prioridad.push(new NodoHuffman(par.first, par.second));
       }
   }

    if (cola_prioridad.empty()) {
        return nullptr; // No hay datos para codificar
    }
    // Caso especial: Si solo hay un tipo de píxel
    else if (cola_prioridad.size() == 1) {
        NodoHuffman* unico = cola_prioridad.top();
        cola_prioridad.pop();
        NodoHuffman* raiz_ficticia = new NodoHuffman(-1, unico->frecuencia);
        raiz_ficticia->izquierda = unico;
        cola_prioridad.push(raiz_ficticia);
    }

   // Combinar nodos hasta que solo quede la raíz
   while (cola_prioridad.size() > 1) {
       NodoHuffman* izquierda = cola_prioridad.top(); cola_prioridad.pop();
       NodoHuffman* derecha = cola_prioridad.top(); cola_prioridad.pop();

       NodoHuffman* nodo_padre = new NodoHuffman(-1, izquierda->frecuencia + derecha->frecuencia);
       nodo_padre->izquierda = izquierda;
       nodo_padre->derecha = derecha;
       cola_prioridad.push(nodo_padre);
   }

   return cola_prioridad.top(); // Devolver la raíz
}
// --- Implementación de codificar_huffman (Formato Binario PDF) ---
bool codificar_huffman(const Imagen& imagen_entrada, const string& archivo_salida) {
    if (imagen_entrada.datos.empty() || imagen_entrada.ancho <= 0 || imagen_entrada.alto <= 0) {
        cerr << "Error: La imagen de entrada está vacía o tiene dimensiones inválidas.\n";
        return false;
    }

    // --- Cabecera Binaria ---
    // Usamos tipos de ancho fijo para claridad y portabilidad
    uint16_t W = static_cast<uint16_t>(imagen_entrada.ancho);
    uint16_t H = static_cast<uint16_t>(imagen_entrada.alto);
    // Asumimos M=255 según la descripción PGM (máximo valor posible)
    uint8_t M = 255;

    // --- Tabla de Frecuencias (F0 a FM) ---
    // Necesitamos tamaño M+1 (256 entradas si M=255)
    // Usamos uint64_t para unsigned long (8 bytes)
    vector<uint64_t> tabla_frecuencias_completa(static_cast<size_t>(M) + 1, 0);
    unordered_map<int, int> frecuencias_ocurrencias; // Para construir el árbol (solo las > 0)

    int max_pixel_val_encontrado = 0;
    for (const auto& fila : imagen_entrada.datos) {
        for (int pixel : fila) {
            if (pixel >= 0 && pixel <= M) { // Validar rango
                tabla_frecuencias_completa[pixel]++;
                frecuencias_ocurrencias[pixel]++; // También llenar mapa para árbol
                 if (pixel > max_pixel_val_encontrado) max_pixel_val_encontrado = pixel;
            } else {
                cerr << "Error: Valor de píxel fuera de rango (0-" << (int)M << "): " << pixel << endl;
                // Podrías retornar false o manejarlo de otra forma
            }
        }
    }

    // Opcional: Podrías reajustar M al máximo encontrado si es menor a 255, pero
    // el PDF parece implicar que M es fijo (ej. 255) y la tabla siempre tiene M+1 entradas.
    // Nos quedamos con M=255.

     if (frecuencias_ocurrencias.empty()) {
        cerr << "Error: No se encontraron píxeles válidos en la imagen para codificar.\n";
        return false;
    }

    // --- Construir Árbol y Códigos ---
    NodoHuffman* raiz = construir_arbol_huffman(frecuencias_ocurrencias);
     if (!raiz) {
        cerr << "Error: No se pudo construir el árbol de Huffman.\n";
        return false;
    }
    unordered_map<int, string> codigos_huffman;
    generar_codigos_recursivo(raiz, "", codigos_huffman);

    // --- Escribir Archivo Binario ---
    ofstream archivo_salida_stream(archivo_salida, ios::binary | ios::trunc); // Modo binario y truncar si existe
    if (!archivo_salida_stream) {
        cerr << "Error: No se pudo crear el archivo de salida binario '" << archivo_salida << "'.\n";
        limpiar_arbol_huffman(raiz);
        return false;
    }

    // Escribir W, H, M
    archivo_salida_stream.write(reinterpret_cast<const char*>(&W), sizeof(W));
    archivo_salida_stream.write(reinterpret_cast<const char*>(&H), sizeof(H));
    archivo_salida_stream.write(reinterpret_cast<const char*>(&M), sizeof(M));

    // Escribir Tabla de Frecuencias Completa (F0...FM)
    archivo_salida_stream.write(reinterpret_cast<const char*>(tabla_frecuencias_completa.data()),
                                tabla_frecuencias_completa.size() * sizeof(uint64_t));

    // --- Escribir Datos Codificados ---
    string bits_codificados_str;
    long long total_pixeles = (long long)W * H;
    long long pixeles_procesados = 0;

    for (const auto& fila : imagen_entrada.datos) {
        for (int pixel : fila) {
            if (pixeles_procesados < total_pixeles && pixel >= 0 && pixel <= M) {
                // Buscar código. Si un píxel válido no está (raro si tabla freqs está bien), manejar error.
                auto it = codigos_huffman.find(pixel);
                if (it != codigos_huffman.end()){
                    bits_codificados_str += it->second;
                } else {
                     cerr << "Error: No se encontró código Huffman para el píxel " << pixel << endl;
                     // Manejar este error improbable
                }
                pixeles_procesados++;
            }
        }
    }

    // Convertir bits a bytes (el padding es implícito en el último byte)
    char buffer = 0;
    int bit_count = 0;
    for (char bit : bits_codificados_str) {
        buffer = (buffer << 1) | (bit - '0'); // Añadir bit al buffer
        bit_count++;
        if (bit_count == 8) { // Si el buffer (byte) está lleno
            archivo_salida_stream.write(&buffer, 1); // Escribir byte
            buffer = 0; // Resetear buffer
            bit_count = 0; // Resetear contador
        }
    }
    // Escribir el último byte si quedó incompleto (con padding de ceros a la derecha)
    if (bit_count > 0) {
        buffer <<= (8 - bit_count); // Alinear los bits a la izquierda, ceros a la derecha
        archivo_salida_stream.write(&buffer, 1);
    }

    // --- Finalizar ---
    archivo_salida_stream.close();
    limpiar_arbol_huffman(raiz);

    return true; // Éxito
}



// --- Implementación de decodificar_huffman (Formato Binario PDF) ---
bool decodificar_huffman(const string& archivo_entrada, Imagen& imagen_salida) {
    ifstream archivo_entrada_stream(archivo_entrada, ios::binary);
    if (!archivo_entrada_stream) {
        cerr << "Error: No se pudo abrir el archivo de entrada binario '" << archivo_entrada << "'.\n";
        return false;
    }

    // --- Leer Cabecera Binaria ---
    uint16_t W = 0;
    uint16_t H = 0;
    uint8_t M = 0;

    archivo_entrada_stream.read(reinterpret_cast<char*>(&W), sizeof(W));
    archivo_entrada_stream.read(reinterpret_cast<char*>(&H), sizeof(H));
    archivo_entrada_stream.read(reinterpret_cast<char*>(&M), sizeof(M));

    if (!archivo_entrada_stream) { // Verificar si hubo error leyendo la cabecera
         cerr << "Error: No se pudo leer la cabecera binaria (W, H, M).\n";
         return false;
    }
     if (W <= 0 || H <= 0) {
         cerr << "Error: Dimensiones leídas inválidas (" << W << "x" << H << ").\n";
         return false;
    }

    // --- Leer Tabla de Frecuencias Completa (F0 a FM) ---
    size_t tamano_tabla = static_cast<size_t>(M) + 1;
    vector<uint64_t> tabla_frecuencias_completa(tamano_tabla);

    archivo_entrada_stream.read(reinterpret_cast<char*>(tabla_frecuencias_completa.data()),
                                tamano_tabla * sizeof(uint64_t));

    if (!archivo_entrada_stream) {
         cerr << "Error: No se pudo leer la tabla de frecuencias binaria.\n";
         return false;
    }

    // --- Reconstruir Mapa de Frecuencias (solo las > 0) para el árbol ---
    unordered_map<int, int> frecuencias_ocurrencias;
    for(size_t i = 0; i < tamano_tabla; ++i) {
        if (tabla_frecuencias_completa[i] > 0) {
            // Convertir freqs de uint64_t a int (cuidado con overflow si son gigantes, improbable aquí)
            frecuencias_ocurrencias[static_cast<int>(i)] = static_cast<int>(tabla_frecuencias_completa[i]);
        }
    }

     if (frecuencias_ocurrencias.empty()) {
         cerr << "Advertencia: La tabla de frecuencias leída está vacía.\n";
         // Crear imagen vacía según W y H leídos
         imagen_salida.ancho = W;
         imagen_salida.alto = H;
         imagen_salida.datos.assign(H, vector<int>(W, 0));
         imagen_salida.nombre = archivo_entrada + ".pgm";
         // imagen_salida.cargado = true; // ELIMINADO
         return true; // ¿Considerar éxito?
     }

    // --- Reconstruir Árbol Huffman ---
    NodoHuffman* raiz = construir_arbol_huffman(frecuencias_ocurrencias);
    if (!raiz) {
         cerr << "Error: No se pudo reconstruir el árbol de Huffman desde las frecuencias leídas.\n";
         return false;
    }

    // --- Leer Datos Binarios Restantes y Decodificar ---
    vector<vector<int>> datos_decodificados(H, vector<int>(W));
    NodoHuffman* nodo_actual = raiz;
    int fila = 0, col = 0;
    long long pixeles_necesarios = (long long)W * H;
    long long pixeles_decodificados = 0;
    char byte;

    // Leer byte a byte el resto del archivo
    while (pixeles_decodificados < pixeles_necesarios && archivo_entrada_stream.get(byte)) {
        for (int i = 7; i >= 0; --i) { // Procesar cada bit del byte
            if (pixeles_decodificados >= pixeles_necesarios) break; // Salir si ya tenemos todos

            bool bit_es_uno = ((byte >> i) & 1);

            if (!nodo_actual) { // Chequeo de seguridad
                 cerr << "Error: Nodo actual nulo durante decodificación.\n";
                 limpiar_arbol_huffman(raiz);
                 return false;
            }

            // Moverse en el árbol según el bit
            nodo_actual = bit_es_uno ? nodo_actual->derecha : nodo_actual->izquierda;

            if (!nodo_actual) { // Chequeo después de mover
                cerr << "Error: Se llegó a un nodo nulo después de leer un bit.\n";
                 limpiar_arbol_huffman(raiz);
                 return false;
            }

            // Si llegamos a una hoja (nodo con valor de píxel)
            if (nodo_actual->valor != -1) {
                if (fila < H && col < W) { // Asegurar que estamos dentro de límites
                    datos_decodificados[fila][col] = nodo_actual->valor;
                    pixeles_decodificados++;
                    // Avanzar al siguiente píxel
                    col++;
                    if (col == W) {
                        col = 0;
                        fila++;
                    }
                } else {
                     // Esto no debería pasar si W*H es correcto
                     cerr << "Advertencia: Se intentó escribir fuera de los límites de la imagen decodificada.\n";
                     break; // Salir del bucle de bits
                }
                nodo_actual = raiz; // Volver a la raíz para el siguiente código
            }
        } // Fin for bits
    } // Fin while bytes

    // --- Verificación Final ---
    if (pixeles_decodificados != pixeles_necesarios) {
         cerr << "Error: Se decodificaron " << pixeles_decodificados << " píxeles, pero se esperaban " << pixeles_necesarios << ".\n";
         limpiar_arbol_huffman(raiz);
         return false;
    }

    // --- Guardar Resultados y Limpiar ---
    imagen_salida.ancho = W;
    imagen_salida.alto = H;
    imagen_salida.datos = move(datos_decodificados); // Usar move para eficiencia
    imagen_salida.nombre = archivo_entrada + ".pgm"; // Nombre sugerido
    // imagen_salida.cargado = true; // ¡ELIMINADO! Imagen no tiene 'cargado'.

    limpiar_arbol_huffman(raiz);
    archivo_entrada_stream.close();

    return true; // Éxito
}


#endif // HUFFMAN_HXX

