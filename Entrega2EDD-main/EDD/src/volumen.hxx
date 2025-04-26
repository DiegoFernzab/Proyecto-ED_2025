#ifndef VOLUMEN_HXX
#define VOLUMEN_HXX
#include "volumen.h"
#include "imagen.h"
#include "huffman.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <numeric>
#include <queue>
#include <stdexcept>
#include <cmath>

using namespace std;
namespace fs = std::filesystem;


//CARGAR VOLUMEN ACTUALIZADO
#include <stdexcept> // Necesario para std::stoi y sus excepciones

// Firma correcta que coincide con volumen.h
bool cargar_volumen(const string& nombre_base, int n_im, Volumen& volumen) { // Usa los parámetros correctos

    // --- Validación inicial (n_im ya viene validado desde la consola, pero podemos re-validar si queremos) ---
    if (n_im <= 0) {
        // Opcional: Mostrar error aquí o asumir que la consola ya lo hizo.
        // cout << "Error interno: n_im debe ser positivo.\n";
        return false; // Devolver false en error
    }

    // --- Lógica principal usando el parámetro 'volumen' ---
    volumen.base_nombre = nombre_base; // Modifica el 'volumen' recibido

    string carpeta_base = volumen.base_nombre + "-ppm";
    string carpeta_alternativa = volumen.base_nombre + "_ppm";

    if (!fs::exists(carpeta_base) && !fs::exists(carpeta_alternativa)) {
        cout << "Error: No se encontró la carpeta correspondiente para '" << volumen.base_nombre << "'.\n";
        volumen.cargado = false; // Asegurar estado consistente
        return false; // Devolver false en error
    }

    string carpeta = fs::exists(carpeta_base) ? carpeta_base : carpeta_alternativa;

    vector<string> archivos_encontrados;
    try { // Usar try-catch por si hay problemas con filesystem
        for (const auto& entry : fs::directory_iterator(carpeta)) {
            string filename = entry.path().filename().string();
            // Usar volumen.base_nombre en lugar de volumen_actual.base_nombre
            if ((filename.find(volumen.base_nombre) == 0) && (filename.size() > volumen.base_nombre.size() + 2)) {
                if (filename.substr(filename.size() - 4) == ".ppm") {
                    archivos_encontrados.push_back(entry.path().string());
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        cerr << "Error al acceder al directorio '" << carpeta << "': " << e.what() << endl;
        volumen.cargado = false;
        return false;
    }


    sort(archivos_encontrados.begin(), archivos_encontrados.end());

    if (archivos_encontrados.size() != n_im) {
        cout << "Error: No se encontraron " << n_im << " imágenes PPM con el prefijo '" << volumen.base_nombre
             << "' en la carpeta '" << carpeta << "'. Se encontraron " << archivos_encontrados.size() << ".\n";
        volumen.cargado = false;
        return false; // Devolver false en error
    }

    // --- Cargar las imágenes en el parámetro 'volumen' ---
    volumen.num_imagenes = n_im;
    volumen.imagenes.clear(); // Limpia el vector del 'volumen' recibido
    volumen.cargado = false; // Marcar como no cargado hasta que termine bien

    cout << "Cargando volumen desde carpeta: " << carpeta << " con " << n_im << " imágenes...\n";
    int suma_ancho = 0;
    int suma_alto = 0;
    int imagenes_cargadas_ok = 0;

    for (const string& img_path : archivos_encontrados) {
        Imagen nueva_imagen;
        // Llamar a leer_ppm (asumimos que está definida en imagen.hxx)
        if (leer_ppm(img_path, nueva_imagen)) { // leer_ppm debe modificar nueva_imagen
            volumen.imagenes.push_back(nueva_imagen); // Añadir al 'volumen' recibido
            suma_ancho += nueva_imagen.ancho;
            suma_alto += nueva_imagen.alto;
            imagenes_cargadas_ok++;
             cout << "  Cargando " << img_path << " (Ancho: " << nueva_imagen.ancho << ", Alto: " << nueva_imagen.alto << ")...\n";
        } else {
            // Opcional: decidir si fallar todo o continuar
            cout << "Advertencia: No se pudo cargar la imagen '" << img_path << "'. Omitiendo.\n";
        }
    }

    // Verificar si se cargaron todas las imágenes esperadas
    if (imagenes_cargadas_ok != n_im) {
         cout << "Error: No se pudieron cargar todas las imágenes correctamente.\n";
         volumen.imagenes.clear(); // Limpiar por si acaso
         volumen.cargado = false;
         return false;
    }

    // Calcular dimensiones promedio y actualizar el 'volumen' recibido
    if (n_im > 0) {
        volumen.ancho = suma_ancho / n_im;
        volumen.alto = suma_alto / n_im;
    } else {
        volumen.ancho = 0;
        volumen.alto = 0;
    }

    cout << "✅ Volumen cargado correctamente desde '" << carpeta << "'.\n";
    volumen.cargado = true; // Marcar como cargado en el 'volumen' recibido
    return true; // Devolver true indicando éxito
}


	
void info_volumen(const Volumen& volumen) {
    // Verifica el estado del 'volumen' recibido, no de la variable global
    if (!volumen.cargado) {
        std::cout << "Error: El volumen proporcionado no está cargado en memoria." << std::endl;

    } else {
         // Muestra la información del 'volumen' recibido
        std::cout << "Volumen cargado en memoria: " << volumen.base_nombre
                  << ", tamaño: " << volumen.num_imagenes
                  << ", ancho promedio: " << volumen.ancho // Asumiendo que son promedios
                  << ", alto promedio: " << volumen.alto << "." << std::endl;
    }
}



int aplicar_criterio(vector<int>& valores, const string& criterio) {
    if (valores.empty()) return 0; // Evitar división por cero o acceso inválido

    if (criterio == "minimo") {
        
        return *min_element(valores.begin(), valores.end());
    }
    else if (criterio == "maximo") {
        
        return *max_element(valores.begin(), valores.end());
    }
    else if (criterio == "promedio") {
        double suma = accumulate(valores.begin(), valores.end(), 0.0);
        return static_cast<int>(round(suma / valores.size())); 
    }
    else if (criterio == "mediana") {
        
        return mediana_optimizada(valores); 
    }
    else {
        
        cerr << "Error interno: Criterio de proyección no válido '" << criterio << "'. Usando 0.\n";
        return 0;
    }
}



Imagen proyeccion2D(const Volumen& volumen, char direccion, const string& criterio) {
    Imagen imagen_resultante; 

    
    if (!volumen.cargado || volumen.imagenes.empty() || volumen.ancho <= 0 || volumen.alto <= 0) {
        cerr << "Error: El volumen no está cargado o está vacío.\n";
        return imagen_resultante; 
    }

    int w = volumen.ancho;
    int h = volumen.alto;
    int d = volumen.num_imagenes;

    vector<vector<int>> datos_proyeccion; // Vector para los datos de la imagen resultante

    
    try { 
        if (direccion == 'x') { 
            imagen_resultante.alto = h;
            imagen_resultante.ancho = d;
            datos_proyeccion.assign(h, vector<int>(d, 0));
            for (int k = 0; k < d; ++k) { 
                if (k >= volumen.imagenes.size()) continue; 
                const auto& slice_k = volumen.imagenes[k]; 

                for (int j = 0; j < h; ++j) { 
                    if (j >= slice_k.alto || j >= slice_k.datos.size()) continue;

                    vector<int> valores;
                    valores.reserve(w); 
                    for (int i = 0; i < w; ++i) { 
                        if (i >= slice_k.ancho || i >= slice_k.datos[j].size()) continue;
                        valores.push_back(slice_k.datos[j][i]);
                    }
                    if (!valores.empty()) {
                        datos_proyeccion[j][k] = aplicar_criterio(valores, criterio);
                    }
                }
            }
        }
        else if (direccion == 'y') { // Proyección en el plano XZ (ancho x profundidad)
             imagen_resultante.alto = w; 
             imagen_resultante.ancho = d;
             datos_proyeccion.assign(w, vector<int>(d, 0)); 
              for (int k = 0; k < d; ++k) { 
                 if (k >= volumen.imagenes.size()) continue;
                 const auto& slice_k = volumen.imagenes[k];

                 for (int i = 0; i < w; ++i) {
                      vector<int> valores;
                      valores.reserve(h);
                      for (int j = 0; j < h; ++j) { 
                          if (j >= slice_k.alto || j >= slice_k.datos.size() || i >= slice_k.ancho || i >= slice_k.datos[j].size()) continue;
                          valores.push_back(slice_k.datos[j][i]);
                      }
                     if (!valores.empty()) {
                         datos_proyeccion[i][k] = aplicar_criterio(valores, criterio); 
                     }
                 }
             }
        }
        else if (direccion == 'z') { // Proyección en el plano XY (alto x ancho) - Vista frontal
            imagen_resultante.alto = h;
            imagen_resultante.ancho = w;
            datos_proyeccion.assign(h, vector<int>(w, 0));
            for (int j = 0; j < h; ++j) { 
                for (int i = 0; i < w; ++i) { 
                    vector<int> valores;
                    valores.reserve(d);
                    for (int k = 0; k < d; ++k) { 
                        if (k >= volumen.imagenes.size()) continue;
                        const auto& slice_k = volumen.imagenes[k];
                         if (j >= slice_k.alto || j >= slice_k.datos.size() || i >= slice_k.ancho || i >= slice_k.datos[j].size()) continue;
                        valores.push_back(slice_k.datos[j][i]);
                    }
                     if (!valores.empty()) {
                        datos_proyeccion[j][i] = aplicar_criterio(valores, criterio);
                    }
                }
            }
        }
        else {
            cerr << "Error: Dirección de proyección no válida '" << direccion << "'.\n";
            return imagen_resultante; // Devuelve imagen vacía
        }
    } catch (const std::out_of_range& oor) {
         cerr << "Error de acceso fuera de rango durante la proyección: " << oor.what() << endl;
         return Imagen(); 
    } catch (const std::exception& e) {
         cerr << "Error inesperado durante la proyección: " << e.what() << endl;
         return Imagen(); 
    }


    imagen_resultante.datos = datos_proyeccion;
    
   

    return imagen_resultante;
}
#endif
