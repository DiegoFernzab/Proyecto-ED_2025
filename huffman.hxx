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
#include <cstdint>

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


bool codificar_huffman(const Imagen& imagen_entrada, const string& archivo_salida) {


    // ***** IMPRESIÓN DE DEBUG INICIO *****
    cout << "[DEBUG Codificador Inicio] Recibida imagen: "
         << imagen_entrada.ancho << "x" << imagen_entrada.alto
         << ", Nombre: '" << imagen_entrada.nombre << "'"
         << ", Datos empty? " << (imagen_entrada.datos.empty() ? "SI" : "NO") << endl;
    if (!imagen_entrada.datos.empty()) {
        cout << "[DEBUG Codificador Inicio] Tamaño primera fila (datos[0]): " << imagen_entrada.datos[0].size() << endl;
    } else {
         cout << "[DEBUG Codificador Inicio] El vector de datos está vacío." << endl;
    }
    // ************************************


    // --- Calcular Frecuencias ---
    unordered_map<int, int> frecuencias;
    if (imagen_entrada.datos.empty() || imagen_entrada.alto <= 0 || imagen_entrada.ancho <= 0) {
        cerr << "Error en codificador: La imagen de entrada parece vacía o inválida.\n";
        return false;
    }
     try { // Calcular frecuencias con chequeos
        for (int i = 0; i < imagen_entrada.alto; ++i) {
             if (i >= imagen_entrada.datos.size()) { cerr << "Error en codificador: Fila " << i << " fuera de límites.\n"; return false; }
             const auto& fila = imagen_entrada.datos[i];
             if (fila.size() != imagen_entrada.ancho) { /* Opcional: manejar ancho inconsistente */ }
             for (int j = 0; j < imagen_entrada.ancho; ++j) {
                 if (j >= fila.size()){ cerr << "Error en codificador: Columna " << j << " fuera de límites en fila " << i << ".\n"; return false; }
                 int pixel = fila[j];
                 frecuencias[pixel]++;
             }
         }
     } catch (const std::exception& e) {
         cerr << "Excepción durante el cálculo de frecuencias: " << e.what() << endl; return false;
     }


    // ***** IMPRESIÓN DE DEBUG FRECUENCIAS *****
    cout << "[DEBUG Codificador] Tamaño de frecuencias calculado: " << frecuencias.size() << endl;
    if (frecuencias.empty()) {
        cout << "[DEBUG Codificador] ¡Advertencia! El mapa de frecuencias está vacío. No se puede codificar." << endl;
        return false; // No se puede codificar si no hay símbolos
    }
    // Opcional: Imprimir algunos pares si el tamaño > 0
    // cout << "[DEBUG Codificador] Ejemplo freqs: Valor=" << frecuencias.begin()->first << " Freq=" << frecuencias.begin()->second << endl;


    // --- Construir Árbol de Huffman ---
    priority_queue<NodoHuffman*, vector<NodoHuffman*>, ComparadorNodos> cola_prioridad;
    for (auto const& [valor, freq] : frecuencias) {
        cola_prioridad.push(new NodoHuffman(valor, freq));
    }
     if (cola_prioridad.size() == 1) { // Caso único símbolo
         NodoHuffman* unico_nodo = cola_prioridad.top(); cola_prioridad.pop();
         NodoHuffman* raiz_ficticia = new NodoHuffman(-1, unico_nodo->frecuencia);
         raiz_ficticia->izquierda = unico_nodo; cola_prioridad.push(raiz_ficticia);
     }
    while (cola_prioridad.size() > 1) { // Construcción normal
        NodoHuffman* izquierdo = cola_prioridad.top(); cola_prioridad.pop();
        NodoHuffman* derecho = cola_prioridad.top(); cola_prioridad.pop();
        int freq_total = izquierdo->frecuencia + derecho->frecuencia;
        NodoHuffman* padre = new NodoHuffman(-1, freq_total);
        padre->izquierda = izquierdo; padre->derecha = derecho;
        cola_prioridad.push(padre);
    }
    NodoHuffman* raiz = cola_prioridad.top();


    // --- Generar Códigos Huffman ---
    unordered_map<int, string> codigos_huffman;
    generar_codigos_recursivo(raiz, "", codigos_huffman);
    //debug 
    if (codigos_huffman.count(0)) { // Asegúrate de que el pixel 0 existe
        cout << "[DEBUG Codificador] Código para pixel 0: '" << codigos_huffman.at(0) << "'" << endl;
   } else {
        cout << "[DEBUG Codificador] Pixel 0 no encontrado en códigos." << endl;
   }

     if (frecuencias.size() == 1 && codigos_huffman.empty()) { // Revisión caso único símbolo
         codigos_huffman[frecuencias.begin()->first] = "0";
         cout << "[DEBUG Codificador] Generado código '0' para único símbolo." << endl;
     } else if (codigos_huffman.empty()) { // Fallo si hay freqs pero no códigos
         cerr << "Error: No se generaron códigos Huffman aunque había frecuencias.\n";
         limpiar_arbol_huffman(raiz); return false;
     }


    // --- Abrir Archivo de Salida ---
    ofstream archivo_salida_stream(archivo_salida, ios::binary);
    if (!archivo_salida_stream) {
        cerr << "Error: No se pudo crear el archivo de salida binario '" << archivo_salida << "'.\n";
        limpiar_arbol_huffman(raiz); return false;
    }

    // --- Escribir Encabezado (Dimensiones y M) ---
    uint16_t W = imagen_entrada.ancho;
    uint16_t H = imagen_entrada.alto;
    uint8_t M = 255;
    archivo_salida_stream.write(reinterpret_cast<const char*>(&W), sizeof(uint16_t));
    archivo_salida_stream.write(reinterpret_cast<const char*>(&H), sizeof(uint16_t));
    archivo_salida_stream.write(reinterpret_cast<const char*>(&M), sizeof(uint8_t));
    

    for (int i = 0; i <= M; ++i) {
        uint64_t freq = (frecuencias.count(i) > 0) ? frecuencias[i] : 0;
        archivo_salida_stream.write(reinterpret_cast<const char*>(&freq), sizeof(uint64_t));
    }

    // --- Escribir Datos Codificados (Refactorizado - Escritura Directa de Bits) ---
    unsigned char buffer = 0; // Buffer para acumular bits (IMPORTANTE: unsigned char)
    int bits_en_buffer = 0;   // Contador de bits en el buffer actual

    cout << "[DEBUG Codificador] Iniciando escritura directa de bits..." << endl;

    try {
        for (int i = 0; i < H; ++i) {
             const auto& fila = imagen_entrada.datos.at(i); // Usar .at() para chequeo de límites
             for (int j = 0; j < W; ++j) {
                  int pixel = fila.at(j); // Usar .at() para chequeo de límites
                  // Obtener el código Huffman para este píxel
                  const string& codigo = codigos_huffman.at(pixel); // Lanza excepción si no existe

                  // Procesar cada bit del código de este píxel
                  for (char bit_char : codigo) {
                      buffer <<= 1; // Desplaza buffer a la izquierda
                      if (bit_char == '1') {
                          buffer |= 1; // Pone el bit menos significativo a 1
                      }
                      bits_en_buffer++;

                      // Si el buffer está lleno, escribirlo en el archivo
                      if (bits_en_buffer == 8) {
                          archivo_salida_stream.write(reinterpret_cast<const char*>(&buffer), 1);
                          if (!archivo_salida_stream) { // Check for write errors
                               cerr << "Error al escribir byte de datos codificados en el archivo.\n";
                               limpiar_arbol_huffman(raiz); return false;
                           }
                          buffer = 0;          // Resetear buffer
                          bits_en_buffer = 0;  // Resetear contador
                      }
                  } // Fin for bits del código
             } // Fin for columnas (j)
        } // Fin for filas (i)
    } catch (const std::out_of_range& oor) {
        // Captura errores de .at() (pixel sin código o acceso fuera de límites)
        cerr << "Error interno: Se encontró un píxel en los datos que no tiene código Huffman asignado, o hubo acceso fuera de límites. " << oor.what() << endl;
        limpiar_arbol_huffman(raiz); return false;
    } catch (const std::exception& e) { // Captura otras posibles excepciones
        cerr << "Excepción durante la escritura de bits codificados: " << e.what() << endl;
        limpiar_arbol_huffman(raiz); return false;
    }

    // Escribir el último byte si quedó parcialmente lleno (con padding)
    if (bits_en_buffer > 0) {
        buffer <<= (8 - bits_en_buffer); // Mover bits a la izquierda, rellenar con 0s a la derecha (LSBs)
        archivo_salida_stream.write(reinterpret_cast<const char*>(&buffer), 1);
         if (!archivo_salida_stream) { // Check for write errors
              cerr << "Error al escribir último byte de datos codificados en el archivo.\n";
              limpiar_arbol_huffman(raiz); return false;
         }
        cout << "[DEBUG Codificador] Escrito último byte con " << bits_en_buffer << " bits útiles y padding." << endl;
    } else {
         cout << "[DEBUG Codificador] No se necesitó escribir último byte parcial (datos terminaron justo en límite de byte)." << endl;
    }
    // ***************************************************************************


    // --- Limpieza y Cierre ---
    limpiar_arbol_huffman(raiz);
    archivo_salida_stream.close();

    cout << "[DEBUG Codificador] Codificación finalizada exitosamente." << endl;
    return true; // Indicar éxito
}

bool decodificar_huffman(const string& archivo_entrada, Imagen& imagen_salida) {

    ifstream archivo_entrada_stream(archivo_entrada, ios::binary);
    if (!archivo_entrada_stream) {
        cerr << "Error: No se pudo abrir el archivo de entrada binario '" << archivo_entrada << "'.\n";
        return false;
    }

    // --- Leer Encabezado ---
    uint16_t W, H;
    uint8_t M;
    archivo_entrada_stream.read(reinterpret_cast<char*>(&W), sizeof(uint16_t));
    archivo_entrada_stream.read(reinterpret_cast<char*>(&H), sizeof(uint16_t));
    archivo_entrada_stream.read(reinterpret_cast<char*>(&M), sizeof(uint8_t));
    

    // --- Leer Tabla de Frecuencias ---
    unordered_map<int, int> frecuencias;
for (int i = 0; i <= M; ++i) {
    uint64_t frecuencia;
    if (!archivo_entrada_stream.read(reinterpret_cast<char*>(&frecuencia), sizeof(uint64_t))) {
        cerr << "Error al leer la frecuencia de la intensidad " << i << endl;
        archivo_entrada_stream.close();
        return false;
    }
    if (frecuencia > 0) {
        frecuencias[i] = static_cast<int>(frecuencia);
    }
}

    // --- Reconstruir Árbol de Huffman ---
    priority_queue<NodoHuffman*, vector<NodoHuffman*>, ComparadorNodos> cola_prioridad;
    for (auto const& [valor, freq] : frecuencias) {
        // Asegurarse que la frecuencia sea positiva para la construcción del árbol
        if (freq > 0) {
            cola_prioridad.push(new NodoHuffman(valor, freq));
        }
    }

    // Si después de filtrar frecuencias <= 0, la cola está vacía o tiene un solo nodo con freq 0...
     if (cola_prioridad.empty()) {
         cerr << "Error: No hay símbolos con frecuencia positiva para construir el árbol.\n";
         archivo_entrada_stream.close();
         return false;
     }

    // Caso especial: solo un símbolo con frecuencia positiva
    if (cola_prioridad.size() == 1) {
        NodoHuffman* unico_nodo = cola_prioridad.top(); cola_prioridad.pop();
        NodoHuffman* raiz_ficticia = new NodoHuffman(-1, unico_nodo->frecuencia);
        raiz_ficticia->izquierda = unico_nodo;
        cola_prioridad.push(raiz_ficticia);
    }

    // Construcción normal del árbol
    while (cola_prioridad.size() > 1) {
        NodoHuffman* izquierdo = cola_prioridad.top(); cola_prioridad.pop();
        NodoHuffman* derecho = cola_prioridad.top(); cola_prioridad.pop();
        // Validar que los nodos extraídos no sean nulos (defensivo)
        if (!izquierdo || !derecho) {
             cerr << "Error interno: Se extrajo un nodo nulo de la cola de prioridad." << endl;
             // Limpiar memoria de los nodos restantes... (complejo)
             archivo_entrada_stream.close(); return false;
        }
        int freq_total = izquierdo->frecuencia + derecho->frecuencia;
        NodoHuffman* padre = new NodoHuffman(-1, freq_total);
        padre->izquierda = izquierdo;
        padre->derecha = derecho;
        cola_prioridad.push(padre);
    }

    // Obtener la raíz (ya validamos que la cola no estaba vacía antes del while)
    NodoHuffman* raiz = cola_prioridad.top();


    // --- Decodificar Datos ---
    vector<vector<int>> datos_decodificados;
    try {
         datos_decodificados.assign(H, vector<int>(W));
    } catch (const std::bad_alloc& e) {
         cerr << "Error: No se pudo asignar memoria para imagen decodificada (" << H << "x" << W << "): " << e.what() << endl;
         limpiar_arbol_huffman(raiz); archivo_entrada_stream.close(); return false;
    } catch (const std::exception& e) { // Captura más general
        cerr << "Error inesperado al asignar memoria (" << H << "x" << W << "): " << e.what() << endl;
        limpiar_arbol_huffman(raiz); archivo_entrada_stream.close(); return false;
    }


    NodoHuffman* nodo_actual = raiz;
    int pixeles_decodificados = 0;
    const int pixeles_necesarios = W * H;
    int fila = 0, col = 0;
    char byte_buffer; // Leer en char

    // --- Bucle Principal de Lectura y Decodificación ---
    while (archivo_entrada_stream.read(&byte_buffer, 1) && pixeles_decodificados < pixeles_necesarios) {
        unsigned char u_byte = static_cast<unsigned char>(byte_buffer); // Tratar como unsigned para bits

        // --- Bucle Interno de Bits ---
        for (int bit_pos = 7; bit_pos >= 0; --bit_pos) {
            bool bit = (u_byte >> bit_pos) & 1;

            // --- Avanzar en el árbol ---
            // Añadir chequeo extra: si nodo_actual es hoja, no debería moverse más
            // if (nodo_actual && !(nodo_actual->izquierda == nullptr && nodo_actual->derecha == nullptr)) {
                 nodo_actual = bit ? nodo_actual->derecha : nodo_actual->izquierda;
            // } // Este chequeo extra puede ser problemático si la raíz es hoja (imagen de 1 símbolo)

            // --- Chequeo de Puntero Nulo ---
            if (!nodo_actual) {
                 cerr << "Error: Travesía del árbol inválida (puntero nulo).\n";
                 limpiar_arbol_huffman(raiz); archivo_entrada_stream.close(); return false;
            }

            // --- Procesar si es Hoja ---
            if (nodo_actual->izquierda == nullptr && nodo_actual->derecha == nullptr) { // Es Hoja?
                 if (pixeles_decodificados < pixeles_necesarios) { // Aún se necesitan píxeles?
                     if (fila < H && col < W) { // Dentro de límites?
                         datos_decodificados[fila][col] = nodo_actual->valor;
                         pixeles_decodificados++;

                         // Avanzar coordenadas
                         col++;
                         if (col == W) { col = 0; fila++; }
                     } else {
                         // Error crítico si las coordenadas se salen antes de terminar
                         cerr << "Error interno: Coordenadas (" << fila << "," << col << ") fuera de limites.\n";
                         limpiar_arbol_huffman(raiz); archivo_entrada_stream.close(); return false;
                     }
                 }
                 // --- Resetear nodo a la raíz ---
                 nodo_actual = raiz;

                 // --- Salir del bucle de bits si ya se decodificaron todos ---
                 if (pixeles_decodificados >= pixeles_necesarios) {
                     break; // Salir del for (bit_pos...)
                 }
            } // Fin if Hoja
        } // Fin for bits
    } // Fin while bytes

    // --- Verificación Final ---
    if (pixeles_decodificados != pixeles_necesarios) {
         // Podría ser un error o simplemente fin de archivo prematuro
         cerr << "Error/Advertencia: Se decodificaron " << pixeles_decodificados << " píxeles, pero se esperaban " << pixeles_necesarios << ". Archivo podría estar incompleto o corrupto.\n";
         // Decidir si retornar false o permitir resultado parcial. Retornar false es más seguro.
         limpiar_arbol_huffman(raiz); archivo_entrada_stream.close(); return false;
    }

    // --- Guardar Resultados y Limpiar ---
    imagen_salida.ancho = W;
    imagen_salida.alto = H;
    imagen_salida.datos = move(datos_decodificados);
    imagen_salida.nombre = ""; // Limpiar nombre o poner uno genérico

    limpiar_arbol_huffman(raiz);
    archivo_entrada_stream.close();

    
    return true; // Indicar éxito
}

#endif // HUFFMAN_HXX

