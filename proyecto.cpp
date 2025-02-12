#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cstdlib>
#include <fstream>

using namespace std;

class ConsolaInteractiva {
private:
    unordered_map<string, function<void(const vector<string>&)>> comandos;
    string imagen_cargada;
    int ancho,alto;
public:
    ConsolaInteractiva() {
        comandos["ayuda"] = [this](const  vector<string>& args) { this->ayuda(args); };
        comandos["salir"] = [this](const vector<string>& args) { this->salir(args);};
        comandos["cargar_imagen"] = [this](const vector<string>& args) { this->cargar_imagen(args);};
        comandos["clr"] = [this](const vector<string>& args) { this->clr(args);};
        comandos["info_imagen"] = [this](const vector<string>& args) { this->info_imagen(args);};
        comandos["proyeccion2D"] = [this](const vector<string>& args) { this->proyeccion2D(args);};
        comandos["decodificar_archivo"] = [this](const vector<string>& args) { this->decodificar_archivo(args);};
        comandos["cargar_volumen"] = [this](const vector<string>& args) { this->cargar_volumen(args); };
        comandos["info_volumen"] = [this](const vector<string>& args) { this->info_volumen(args); };
        comandos["codificar_imagen"] = [this](const vector<string>& args) { this->codificar_imagen(args); };
        comandos["segmentar"] = [this](const vector<string>& args) { this->segmentar(args); };
    }

    void ejecutar() {
        string entrada;
        while (true) {
            cout << "$ ";
            if (!getline(cin, entrada)) {
                cout << "\nSaliendo...\n";
                break;
            }
            procesar_comando(entrada);
        }
    }

private:
    void procesar_comando(const string& entrada) {
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

    void ayuda(const vector<string>& args) {
        if (args.empty()) {
            cout << "Comandos disponibles:\n";
            for (const auto& cmd : comandos) {
                cout << "  " << cmd.first << " - " << descripcion_comando(cmd.first) << "\n";
            }
            cout << "\nUsa 'ayuda <comando>' para más detalles.\n";
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
      void salir(const vector<string>&) {
            cout << "Saliendo del sistema...\n";
            exit(0);
          }
      void cargar_imagen(const vector<string>& args) {
           if (args.size() != 1) {
            cout << "Error: Uso incorrecto. Sintaxis: cargar_imagen nombre_imagen.pgm\n";
            return;
        }

        string nombre_imagen = args[0];

        if (!leer_dimensiones_pgm(nombre_imagen, ancho, alto)) {
            cout << "Error: La imagen " << nombre_imagen << " no ha podido ser cargada.\n";
            return;
        }

        imagen_cargada = nombre_imagen;
        cout << "La imagen " << nombre_imagen << " ha sido cargada.\n";
        
    }
    void info_imagen(const vector<string>&) {
        if (imagen_cargada.empty()) {
            cout << "Error: No hay una imagen cargada en memoria.\n";
        } else {
            cout << "Imagen cargada en memoria: " << imagen_cargada << ", ancho: " << ancho << ", alto: " << alto << ".\n";
        }
    }

     bool leer_dimensiones_pgm(const string& nombre_archivo, int &ancho, int &alto) {
        ifstream archivo(nombre_archivo);
        if (!archivo) return false;

        string tipo;
        archivo >> tipo;
        if (tipo != "P2" && tipo != "P5") return false;

        archivo >> ancho >> alto;
        return true;
    }
      void proyeccion2D(const vector<string>& args) {
              if (args.size() != 3) {
                  cout << "Error: Uso incorrecto. Sintaxis: proyeccion2D direccion criterio nombre_archivo.pgm\n";
                  return;
              }
      
              if (imagen_cargada.empty()) {
                  cout << "Error: El volumen aún no ha sido cargado en memoria.\n";
                  return;
              }
      
             
          }
    void decodificar_archivo(const vector<string>& args) {
        if (args.size() != 2) {
            cout << "Error: Uso incorrecto. Sintaxis: decodificar_archivo nombre_archivo.huf nombre_imagen.pgm\n";
            return;
        }

        string archivo_entrada = args[0];
        string archivo_salida = args[1];

       
        ifstream archivo(archivo_entrada);
        if (!archivo) {
            cout << "Error: El archivo " << archivo_entrada << " no ha podido ser decodificado.\n";
            return;
        }
\
        }void cargar_volumen(const vector<string>& args) {
            cout << "(Cargando volumen en memoria)\n";
        }
        
        void info_volumen(const vector<string>& args) {
            cout << "(Mostrando información del volumen cargado)\n";
        }
        
        void codificar_imagen(const vector<string>& args) {
            cout << "(Codificando imagen en formato .huf)\n";
        }
        
        void segmentar(const vector<string>& args) {
            cout << "(Segmentando imagen en base a semillas y grafos)\n";
        }
        void clr(const vector <string>&) {
            system("clear");
        }


    string descripcion_comando(const string& comando, bool detallado = false) {
        if (comando == "ayuda") {
            return detallado ? "Uso: ayuda [comando]\nMuestra los comandos disponibles o información detallada de un comando específico." : "Muestra la lista de comandos disponibles o información de un comando específico.";
        }
      if (comando == "salir") {
            return detallado ? "Uso: salir\nCierra la consola interactiva y termina la ejecución del programa." : "Finaliza la ejecución del programa.";
      }
      if (comando == "cargar_imagen") {
            return detallado ? "Uso: cargar_imagen nombre_imagen.pgm\nCarga una imagen en memoria. Si ya hay una imagen cargada, será reemplazada." : "Carga en memoria una imagen PGM.";
      }
      if (comando == "clr") {
            return detallado ? "Uso: limpiar pantalla\nLimpia pantalla" : "limpiar pantalla de la consola.";
      }
      if (comando == "info_imagen") {
            return detallado ? "Uso: info_imagen\nMuestra información de la imagen cargada (nombre, ancho y alto).": "Muestra información de la imagen cargada.";
      }
      if (comando == "proyeccion2D") {
            return detallado ? "Uso: proyeccion2D\nGenera una proyección 2D de la imagen cargada y la guarda en el archivo nombre_archivo.pgm.": "Proyecta una imagen 2D";
      }
      if (comando == "decodificar_archivo") {
            return detallado ? "Uso: decodificar_archivo\nRealiza la decodificación de un archivo .huf y lo guarda como un archivo .pgm.": "Proyecta una imagen 2D";
      }
      if (comando == "cargar_volumen") {
            return detallado ? "Uso: cargar_volumen\nCarga un volumen de imágenes en memoria." : "Carga un volumen de imágenes.";
        }
      if (comando == "info_volumen") {
          return detallado ? "Uso: info_volumen\nMuestra información del volumen de imágenes cargado en memoria." : "Muestra información del volumen.";
      }
      if (comando == "codificar_imagen") {
          return detallado ? "Uso: codificar_imagen\nCodifica una imagen en formato .huf utilizando el algoritmo de Huffman." : "Codifica una imagen.";
      }
      if (comando == "segmentar") {
          return detallado ? "Uso: segmentar\nSegmenta una imagen en base a semillas y grafos." : "Segmenta una imagen.";
      }
        return "Descripción no disponible.";
    }
};

int main() {
    ConsolaInteractiva consola;
    consola.ejecutar();
    return 0;
}