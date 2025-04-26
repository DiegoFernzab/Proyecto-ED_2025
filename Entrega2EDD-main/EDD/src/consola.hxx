#ifndef CONSOLA_HXX
#define CONSOLA_HXX

#include "consola.h"   // Incluye la declaración de la clase ConsolaInteractiva
#include "imagen.hxx"  // Incluye implementaciones de funciones de imagen (leer_pgm, guardar_pgm, etc.)
#include "volumen.hxx" // Incluye implementaciones de funciones de volumen (cargar_volumen, proyectar_volumen, etc.)
#include "huffman.hxx"
#include "huffman.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>

ConsolaInteractiva::ConsolaInteractiva() {
    comandos["ayuda"] = [this](const vector<string>& args) { this->ayuda(args); };
    comandos["salir"] = [this](const vector<string>& args) { this->salir(args); };
    comandos["clr"] = [this](const vector<string>& args) { this->clr(args); };
    comandos["cargar_imagen"] = [this](const vector<string>& args) { this->cargar_imagen(args); };
    comandos["info_imagen"] = [this](const vector<string>& args) { this->info_imagen(args); };
    comandos["cargar_volumen"] = [this](const vector<string>& args) { this->cargar_volumen(args); };
    comandos["info_volumen"] = [this](const vector<string>& args) { this->info_volumen(args); };
    comandos["proyeccion2D"] = [this](const vector<string>& args) { this->proyeccion2D(args); };
    comandos["codificar_imagen"] = [this](const vector<string>& args) { this->codificar_imagen(args); };
    comandos["decodificar_archivo"] = [this](const vector<string>& args) { this->decodificar_archivo(args); };
    comandos["segmentar"] = [this](const vector<string>& args) { this->segmentar(args); };
}

void ConsolaInteractiva::ejecutar() {
    mostrar_bienvenida(); 

    std::string entrada;
    while (true) {
        std::cout << "$ ";
        if (!getline(std::cin, entrada)) {
            std::cout << "\nSaliendo...\n";
            break;
        }
        if (!entrada.empty()) { 
             procesar_comando(entrada);
        }
    }
}

void ConsolaInteractiva::procesar_comando(const string& entrada) {
    istringstream stream(entrada);
    vector<string> tokens;
    string token;
    while (stream >> token) {
        tokens.push_back(token);
    }

    if (tokens.empty()) return;

    string comando = tokens[0];
    tokens.erase(tokens.begin());

    auto it = comandos.find(comando);
    if (it != comandos.end()) {
        it->second(tokens);
    } else {
        cout << "Error: '" << comando << "' no es un comando reconocido. Usa 'ayuda' para ver los comandos disponibles.\n";
    }
}

void ConsolaInteractiva::ayuda(const vector<string>& args) {
    if (args.empty()) {
        cout << "                     Comandos disponibles:\n";
        cout << "                     :\n";
        for (const auto& cmd : comandos) {
            cout << "  " << cmd.first << " - " << descripcion_comando(cmd.first) << "\n";
        }
        cout << "\nUsa 'ayuda <comando>' para mas detalles.\n";
    } else {
        string comando = args[0];
        auto it = comandos.find(comando);
        if (it != comandos.end()) {
            cout << "Uso de '" << comando << "': " << descripcion_comando(comando, true) << "\n";
        } else {
            cout << "Error: No hay ayuda disponible para '" << comando << "'.\n";
        }
    }
}
void ConsolaInteractiva::salir(const vector<string>&) {
    cout << "Saliendo del sistema...\n";
    exit(0);
}

void ConsolaInteractiva::clr(const vector<string>&) {
    // Limpia la pantalla de la consola (puede variar según el SO)
    #ifdef _WIN32
        system("cls");
    #else
        // Asume Linux, macOS, etc.
        system("clear");
    #endif
}

void ConsolaInteractiva::cargar_imagen(const std::vector<std::string>& args) {
    if (args.size() != 1) {
        std::cout << "Error: Uso incorrecto. " << descripcion_comando("cargar_imagen", true) << "\n";
        return;
    }

    std::string nombre_imagen = args[0];
    vector<vector<int>> datos_leidos; 
    int w, h;

    if (!::leer_pgm(nombre_imagen, datos_leidos, w, h)) { 
        std::cout << "Error: La imagen '" << nombre_imagen << "' no ha podido ser cargada.\n";
        imagen_actual = {}; // Limpiar imagen actual
    } else {
        // --- ASIGNACIÓN CORRECTA ---
        imagen_actual.nombre = nombre_imagen; // Asignar nombre
        imagen_actual.ancho = w;              // Asignar ancho leído
        imagen_actual.alto = h;               // Asignar alto leído
        imagen_actual.datos = datos_leidos;   // Asignar datos leídos

        // Ahora sí, imprimir la info correcta
        std::cout << "La imagen '" << imagen_actual.nombre << "' ha sido cargada ("
                  << imagen_actual.ancho << "x" << imagen_actual.alto << ").\n";
    }
}

void ConsolaInteractiva::cargar_volumen(const std::vector<std::string>& args) {
    if (args.size() != 2) {
       std::cout << "Error: Uso incorrecto. " << descripcion_comando("cargar_volumen", true) << "\n";
       return;
   }
   std::string nombre_base = args[0];
   int n_im = 0;
   try {
       n_im = std::stoi(args[1]);
   } catch (const std::invalid_argument& e) {
       std::cout << "Error: El número de imágenes debe ser un entero.\n";
       return;
   } catch (const std::out_of_range& e) {
        std::cout << "Error: El número de imágenes está fuera de rango.\n";
       return;
   }

   if (n_im <= 0) {
       std::cout << "Error: El número de imágenes debe ser positivo.\n";
       return;
   }

   Volumen temp_volumen; // Volumen temporal
   // Llamar a la función externa definida en volumen.hxx
   if (!::cargar_volumen(nombre_base, n_im, temp_volumen)) {
       std::cout << "Error: El volumen con base '" << nombre_base << "' y " << n_im << " imágenes no pudo ser cargado.\n";
       volumen_actual = {}; // Limpiar el volumen actual
       volumen_actual.cargado = false;
   } else {
       volumen_actual = temp_volumen; // Asignar el volumen cargado
       volumen_actual.cargado = true; // Marcar como cargado
        std::cout << "Volumen '" << volumen_actual.base_nombre<< "' cargado ("
                 << volumen_actual.num_imagenes << " imágenes, "
                 << volumen_actual.ancho << "x" << volumen_actual.alto << ").\n";
   }
}

void ConsolaInteractiva::info_volumen(const std::vector<std::string>& args) {
    if (!volumen_actual.cargado) {
       std::cout << "Error: No hay un volumen cargado en memoria.\n";
   } else {
       // Llamar a la función externa si existe, o imprimir directamente
       ::info_volumen(volumen_actual); // Asume que info_volumen(const Volumen&) está en volumen.hxx
       
   }
}


void ConsolaInteractiva::proyeccion2D(const std::vector<std::string>& args) {
    if (args.size() != 3) {
        std::cout << "Error: Uso incorrecto. " << descripcion_comando("proyeccion2D", true) << "\n";
        return;
    }
     if (!volumen_actual.cargado) {
        std::cout << "Error: No hay un volumen cargado para proyectar.\n";
        return;
    }

    std::string direccion_str = args[0];
    std::string criterio = args[1];
    std::string nombre_archivo_salida = args[2];

    // Validar dirección
    if (direccion_str.length() != 1 || (direccion_str[0] != 'x' && direccion_str[0] != 'y' && direccion_str[0] != 'z')) {
         std::cout << "Error: La dirección debe ser 'x', 'y' o 'z'.\n";
        return;
    }
    char direccion = direccion_str[0];

    // Validar criterio (podría hacerse también dentro de la función de proyección)
    if (criterio != "minimo" && criterio != "maximo" && criterio != "promedio" && criterio != "mediana") {
         std::cout << "Error: El criterio debe ser 'minimo', 'maximo', 'promedio' o 'mediana'.\n";
        return;
    }


    // Llamar a la función externa de volumen.hxx para calcular la proyección, usando la firma de volumen.h
    // Renombré ::proyectar_volumen a ::proyeccion2D para coincidir con volumen.h
    Imagen imagen_proyectada = ::proyeccion2D(volumen_actual, direccion, criterio);

    if (imagen_proyectada.datos.empty()) {
         std::cout << "Error: No se pudo generar la proyección 2D (quizás el volumen está vacío o hubo un error interno).\n";
    } else {
        // Llamar a la función externa de imagen.hxx para guardar, pasando el vector de datos
        if (::guardar_pgm(nombre_archivo_salida, imagen_proyectada.datos)) {
            std::cout << "La proyección 2D (dirección '" << direccion << "', criterio '" << criterio
                      << "') ha sido generada y guardada en '" << nombre_archivo_salida << "'.\n";
        } else {
            std::cout << "Error: La proyección 2D se generó pero no pudo ser guardada en '"
                      << nombre_archivo_salida << "'.\n";
        }
    }
}

void ConsolaInteractiva::info_imagen(const vector<string>&) {
    if (imagen_actual.nombre.empty()) {
        cout << "Error: No hay una imagen cargada en memoria.\n";
    } else {
        cout << "Información de la imagen:\n";
        cout << "  Nombre: " << imagen_actual.nombre << "\n";
        cout << "  Dimensiones: " << imagen_actual.ancho << "x" << imagen_actual.alto << "\n";
    }
}


void ConsolaInteractiva::codificar_imagen(const std::vector<std::string>& args) {
    if (args.size() != 1) {
         std::cout << "Error: Uso incorrecto. " << descripcion_comando("codificar_imagen", true) << "\n";
        return;
    }
     if (imagen_actual.nombre.empty()) {
        std::cout << "Error: No hay una imagen cargada en memoria para codificar.\n";
        return;
    }
     std::string archivo_salida_huf = args[0];

     // Llamar a la función externa de huffman.hxx
     // (Asegúrate que el nombre del archivo .hxx sea correcto)
     if (::codificar_huffman(imagen_actual, archivo_salida_huf)) {
          std::cout << "La imagen en memoria ha sido codificada exitosamente y almacenada en '"
                    << archivo_salida_huf << "'.\n";
     } else {
          std::cout << "Error: La imagen no pudo ser codificada en '" << archivo_salida_huf << "'.\n";
     }
}
void ConsolaInteractiva::decodificar_archivo(const std::vector<std::string>& args) {
    if (args.size() != 2) {
        std::cout << "Error: Uso incorrecto. " << descripcion_comando("decodificar_archivo", true) << "\n";
       return;
   }
   std::string archivo_entrada_huf = args[0];
   std::string archivo_salida_pgm = args[1];

   Imagen imagen_decodificada;
   // Llamar a la función externa de huffman.hxx
   // (Asegúrate que el nombre del archivo .hxx sea correcto)
   if(::decodificar_huffman(archivo_entrada_huf, imagen_decodificada)){
       // Guardar la imagen decodificada usando guardar_pgm de imagen.hxx
       if(::guardar_pgm(archivo_salida_pgm, imagen_decodificada.datos)){ // Pasar imagen_decodificada.datos
            std::cout << "Archivo '" << archivo_entrada_huf << "' decodificado y guardado como '" << archivo_salida_pgm << "'.\n";
       } else {
            std::cout << "Error: El archivo fue decodificado pero no se pudo guardar como '" << archivo_salida_pgm << "'.\n";
       }
   } else {
        std::cout << "Error: El archivo '" << archivo_entrada_huf << "' no pudo ser decodificado.\n";
   }
}

void ConsolaInteractiva::segmentar(const vector<string>& args) {
    cout << "(Segmentando imagen... aún no implementado)\n";
}
// --- Implementación de la ayuda detallada (usando las descripciones originales) ---
std::string ConsolaInteractiva::descripcion_comando(const std::string& comando, bool detallado) {
    if (comando == "ayuda") {
        return detallado ? "Uso: ayuda [comando]\nMuestra los comandos disponibles o informacion detallada de un comando especifico." : "Muestra la lista de comandos o ayuda específica.";
    }
    if (comando == "salir") {
        return detallado ? "Uso: salir\nCierra la consola interactiva y termina la ejecucion del programa." : "Finaliza la ejecucion del programa.";
    }
    if (comando == "cargar_imagen") {
        return detallado ? "Uso: cargar_imagen <nombre_imagen.pgm>\nCarga una imagen PGM en memoria. Si ya hay una imagen cargada, sera reemplazada." : "Carga en memoria una imagen PGM.";
    }
    if (comando == "clr") {
        return detallado ? "Uso: clr\nLimpia la pantalla de la consola." : "Limpia la pantalla de la consola.";
    }
    if (comando == "info_imagen") {
        return detallado ? "Uso: info_imagen\nMuestra informacion de la imagen cargada (nombre, ancho y alto)." : "Muestra informacion de la imagen cargada.";
    }
    if (comando == "proyeccion2D") {
        return detallado ? "Uso: proyeccion2D <x|y|z> <minimo|maximo|promedio|mediana> <nombre_archivo_salida.pgm>\nGenera una proyeccion 2D del volumen cargado usando un criterio y la guarda en un archivo PGM." : "Proyecta el volumen 3D a una imagen 2D.";
    }
    if (comando == "decodificar_archivo") {
        return detallado ? "Uso: decodificar_archivo <nombre_archivo.huf> <nombre_imagen_salida.pgm>\nRealiza la decodificacion de un archivo .huf y lo guarda como un archivo .pgm." : "Decodifica un archivo Huffman (.huf).";
    }
    if (comando == "cargar_volumen") {
        return detallado ? "Uso: cargar_volumen <nombre_base> <n_imagenes>\nCarga un volumen de n_imagenes PGM/PPM desde archivos que comienzan con nombre_base (ej: base_01.ppm, base_02.ppm...). Busca en carpetas '<nombre_base>-ppm' o '<nombre_base>_ppm'." : "Carga un volumen de imagenes (slices).";
    }
    if (comando == "info_volumen") {
        return detallado ? "Uso: info_volumen\nMuestra informacion del volumen de imagenes cargado en memoria (nombre base, numero de slices, dimensiones promedio)." : "Muestra informacion del volumen cargado.";
    }
    if (comando == "codificar_imagen") {
        return detallado ? "Uso: codificar_imagen <nombre_archivo_salida.huf>\nCodifica la imagen PGM cargada en memoria a formato Huffman y la guarda en el archivo .huf especificado." : "Codifica la imagen actual a formato Huffman.";
    }
    if (comando == "segmentar") {
        return detallado ? "Uso: segmentar [opciones...]\n(No implementado) Segmenta la imagen cargada en memoria." : "(No implementado) Segmenta la imagen.";
    }
    return "Descripcion no disponible."; // Caso por defecto
}

void ConsolaInteractiva::mostrar_bienvenida() {
    // Puedes ajustar el ancho según tu preferencia
    const int ancho_total = 60;
    std::string linea_horizontal(ancho_total, '=');
    std::string titulo = "Bienvenido al Procesador de Imagenes v1.0";
    std::string espacio_titulo((ancho_total - 2 - titulo.length()) / 2, ' '); // Calcular espacio para centrar

    std::cout << linea_horizontal << std::endl;
    std::cout << "|" << espacio_titulo << titulo << espacio_titulo
              // Ajuste por si el ancho es impar
              << (titulo.length() % 2 != ancho_total % 2 ? " " : "") << "|" << std::endl;
    std::cout << linea_horizontal << std::endl;
    std::cout << std::endl; // Línea en blanco

    
    ayuda({}); // Llama a tu función ayuda para listar los comandos

    std::cout << std::endl; // Línea en blanco
    std::cout << "Escribe 'salir' para terminar." << std::endl;
    std::cout << linea_horizontal << std::endl; // Línea final opcional
}


#endif // CONSOLA_HXX
