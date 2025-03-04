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


using namespace std;
namespace fs = std::filesystem;

struct Imagen {
	string nombre;
	int ancho;
	int alto;
	vector<vector<int>> datos;
};

struct Volumen {
	string nombre_base;
	int num_imagenes;
	int ancho;
	int alto ;
	vector<Imagen> imagenes;
	bool cargado = false;
};


class ConsolaInteractiva {
private:
	unordered_map<string, function<void(const vector<string>&)>> comandos;
	Imagen imagen_actual;
	Volumen volumen_actual;

public:
	ConsolaInteractiva() {
		comandos["ayuda"] = [this](const  vector<string>& args) {
			this->ayuda(args);
		};
		comandos["salir"] = [this](const vector<string>& args) {
			this->salir(args);
		};
		comandos["cargar_imagen"] = [this](const vector<string>& args) {
			this->cargar_imagen(args);
		};
		comandos["clr"] = [this](const vector<string>& args) {
			this->clr(args);
		};
		comandos["info_imagen"] = [this](const vector<string>& args) {
			this->info_imagen(args);
		};
		comandos["proyeccion2D"] = [this](const vector<string>& args) {
			this->proyeccion2D(args);
		};
		comandos["decodificar_archivo"] = [this](const vector<string>& args) {
			this->decodificar_archivo(args);
		};
		comandos["cargar_volumen"] = [this](const vector<string>& args) {
			this->cargar_volumen(args);
		};
		comandos["info_volumen"] = [this](const vector<string>& args) {
			this->info_volumen(args);
		};
		comandos["codificar_imagen"] = [this](const vector<string>& args) {
			this->codificar_imagen(args);
		};
		comandos["segmentar"] = [this](const vector<string>& args) {
			this->segmentar(args);
		};
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
		vector<vector<int>> datos;
		int w, h;

		if (!leer_pgm(nombre_imagen, datos, w, h)) {
			cout << "Error: La imagen " << nombre_imagen << " no ha podido ser cargada.\n";
			return;
		}

		imagen_actual = {nombre_imagen, w, h, datos};  //  Se almacena en `imagen_actual`
		cout << "La imagen " << nombre_imagen << " ha sido cargada.\n";
	}
	void info_imagen(const vector<string>&) {
		if (imagen_actual.nombre.empty()) {
			cout << "Error: No hay una imagen cargada en memoria.\n";
		} else {
			cout << "Imagen cargada en memoria: " << imagen_actual.nombre
			     << ", ancho: " << imagen_actual.ancho
			     << ", alto: " << imagen_actual.alto << ".\n";
		}
	}

	bool leer_pgm(const string& nombre_archivo, vector<vector<int>>& img, int& w, int& h) {
		ifstream archivo(nombre_archivo);
		if (!archivo) return false;

		string tipo;
		archivo >> tipo;
		if (tipo != "P2") return false;

		archivo >> w >> h;
		int max_val;
		archivo >> max_val;

		img.assign(h, vector<int>(w));
		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w; j++) {
				archivo >> img[i][j];
			}
		}

		return true;
	}


	void proyeccion2D(const vector<string>& args) {
    if (!volumen_actual.cargado) {
        cout << "Error: El volumen aún no ha sido cargado en memoria.\n";
        return;
    }

    if (args.size() != 3) {
        cout << "Error: Uso incorrecto. Sintaxis: proyeccion2D <direccion> <criterio> <nombre_archivo.pgm>\n";
        return;
    }

    string direccion = args[0];
    string criterio = args[1];
    string nombre_archivo = args[2];

    int w = volumen_actual.ancho;
    int h = volumen_actual.alto;
    int d = volumen_actual.num_imagenes;
    
    vector<vector<int>> proyeccion;

    if (direccion == "x") {
        proyeccion.assign(h, vector<int>(d, 0));
        for (int k = 0; k < d; k++) {
            for (int j = 0; j < h; j++) {
                vector<int> valores;
                for (int i = 0; i < w; i++) {
                    valores.push_back(volumen_actual.imagenes[k].datos[j][i]);
                }
                proyeccion[j][k] = aplicar_criterio(valores, criterio);
            }
        }
    } 
    else if (direccion == "y") {
        proyeccion.assign(w, vector<int>(d, 0));
        for (int k = 0; k < d; k++) {
            for (int i = 0; i < w; i++) {
                vector<int> valores;
                for (int j = 0; j < h; j++) {
                    valores.push_back(volumen_actual.imagenes[k].datos[j][i]);
                }
                proyeccion[i][k] = aplicar_criterio(valores, criterio);
            }
        }
    } 
    else if (direccion == "z") {
        proyeccion.assign(h, vector<int>(w, 0));
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                vector<int> valores;
                for (int k = 0; k < d; k++) {
                    valores.push_back(volumen_actual.imagenes[k].datos[j][i]);
                }
                proyeccion[j][i] = aplicar_criterio(valores, criterio);
            }
        }
    } 
    else {
        cout << "Error: Dirección no válida. Usa 'x', 'y' o 'z'.\n";
        return;
    }

    if (!guardar_pgm(nombre_archivo, proyeccion)) {
        cout << "Error: La proyección 2D no pudo ser generada.\n";
    } else {
        cout << "La proyección 2D del volumen ha sido generada y almacenada en el archivo " << nombre_archivo << ".\n";
    }

}

int aplicar_criterio(vector<int>& valores, const string& criterio) {
    if (valores.empty()) return 0;

    if (criterio == "minimo") {
        return *min_element(valores.begin(), valores.end());
    } 
    else if (criterio == "maximo") {
        return *max_element(valores.begin(), valores.end());
    } 
    else if (criterio == "promedio") {
        return accumulate(valores.begin(), valores.end(), 0) / valores.size();
    } 
    else if (criterio == "mediana") {
        sort(valores.begin(), valores.end());
        return valores[valores.size() / 2];
    } 
    else {
        cout << "Error: Criterio no válido. Usa 'minimo', 'maximo', 'promedio' o 'mediana'.\n";
        return 0;
    }
}

bool guardar_pgm(const string& nombre_archivo, const vector<vector<int>>& imagen) {
    ofstream archivo(nombre_archivo);
    if (!archivo) return false;

    int w = imagen[0].size();
    int h = imagen.size();

    archivo << "P2\n";
    archivo << w << " " << h << "\n";
    archivo << "255\n";

    for (const auto& fila : imagen) {
        for (int pixel : fila) {
            archivo << pixel << " ";
        }
        archivo << "\n";
    }

    return true;
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

	}
	void cargar_volumen(const vector<string>& args) {
        if (args.size() != 2) {
            cout << "Error: Uso incorrecto. Sintaxis: cargar_volumen <nombre_base> <n_im>\n";
            return;
        }

        string nombre_base = args[0];
        string carpeta_base = nombre_base + "-ppm";
        string carpeta_alternativa = nombre_base + "_ppm";

        if (!fs::exists(carpeta_base) && !fs::exists(carpeta_alternativa)) {
            cout << "Error: No se encontró la carpeta correspondiente para " << nombre_base << ".\n";
            return;
        }

        string carpeta = fs::exists(carpeta_base) ? carpeta_base : carpeta_alternativa;

        int n_im;
        try {
            n_im = stoi(args[1]);
            if (n_im < 1 || n_im > 99) {
                cout << "Error: n_im debe estar entre 1 y 99.\n";
                return;
            }
        } catch (...) {
            cout << "Error: n_im debe ser un número válido.\n";
            return;
        }

        vector<string> archivos_encontrados;
        for (const auto& entry : fs::directory_iterator(carpeta)) {
            string filename = entry.path().filename().string();
            if ((filename.find(nombre_base) == 0) && (filename.size() > nombre_base.size() + 2)) {  
                if (filename.substr(filename.size() - 4) == ".ppm") {  
                    archivos_encontrados.push_back(entry.path().string());
                }
            }
        }

        sort(archivos_encontrados.begin(), archivos_encontrados.end());

        if (archivos_encontrados.size() != n_im) {
            cout << "Error: No se encontraron " << n_im << " imágenes con el prefijo '" << nombre_base 
                 << "' en la carpeta '" << carpeta << "'.\n";
            volumen_actual.cargado = false;
            return;
        }

        volumen_actual.nombre_base = nombre_base;
        volumen_actual.num_imagenes = n_im;
        volumen_actual.imagenes.clear();
        volumen_actual.cargado = true;

        cout << "Cargando volumen desde carpeta: " << carpeta << " con " << n_im << " imágenes...\n";
        int suma_ancho = 0, suma_alto = 0;

        for (const string& img : archivos_encontrados) {
            Imagen nueva_imagen;
            if (leer_ppm(img, nueva_imagen)) {  
                volumen_actual.imagenes.push_back(nueva_imagen);
                suma_ancho += nueva_imagen.ancho;
                suma_alto += nueva_imagen.alto;
                cout << "  Cargando " << img << " (Ancho: " << nueva_imagen.ancho << ", Alto: " << nueva_imagen.alto << ")...\n";
            }
        }

        volumen_actual.ancho = suma_ancho / n_im;
        volumen_actual.alto = suma_alto / n_im;

        cout << "✅ Volumen cargado correctamente desde '" << carpeta << "'.\n";
    }


	void info_volumen(const vector<string>& args) {
		if (!volumen_actual.cargado) {
			cout << "No hay un volumen cargado en memoria." << endl;
			return;
		}

		cout << "Volumen cargado en memoria: " << volumen_actual.nombre_base
		     << ", tamaño: " << volumen_actual.num_imagenes
		     << ", ancho: " << volumen_actual.ancho
		     << ", alto: " << volumen_actual.alto << "." << endl;
	}

	bool leer_ppm(const string& nombre_archivo, Imagen& img) {
		ifstream archivo(nombre_archivo);
		if (!archivo) {
			cout << "Error: No se pudo abrir " << nombre_archivo << "\n";
			return false;
		}

		string tipo;
		archivo >> tipo;
		if (tipo != "P3") {
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
			return detallado ? "Uso: ayuda [comando]\nMuestra los comandos disponibles o informacion detallada de un comando especifico." : "Muestra la lista de comandos disponibles o informacion de un comando especifico.";
		}
		if (comando == "salir") {
			return detallado ? "Uso: salir\nCierra la consola interactiva y termina la ejecucion del programa." : "Finaliza la ejecucion del programa.";
		}
		if (comando == "cargar_imagen") {
			return detallado ? "Uso: cargar_imagen nombre_imagen.pgm\nCarga una imagen en memoria. Si ya hay una imagen cargada, sera reemplazada." : "Carga en memoria una imagen PGM.";
		}
		if (comando == "clr") {
			return detallado ? "Uso: limpiar pantalla\nLimpia pantalla" : "limpiar pantalla de la consola.";
		}
		if (comando == "info_imagen") {
			return detallado ? "Uso: info_imagen\nMuestra informacion de la imagen cargada (nombre, ancho y alto).": "Muestra informacion de la imagen cargada.";
		}
		if (comando == "proyeccion2D") {
			return detallado ? "Uso: proyeccion2D\nGenera una proyeccion 2D de la imagen cargada y la guarda en el archivo nombre_archivo.pgm.": "Proyecta una imagen 2D";
		}
		if (comando == "decodificar_archivo") {
			return detallado ? "Uso: decodificar_archivo\nRealiza la decodificaciC3n de un archivo .huf y lo guarda como un archivo .pgm.": "Proyecta una imagen 2D";
		}
		if (comando == "cargar_volumen") {
			return detallado ? "Uso: cargar_volumen\nCarga un volumen de imagenes en memoria." : "Carga un volumen de imagenes.";
		}
		if (comando == "info_volumen") {
			return detallado ? "Uso: info_volumen\nMuestra informacion del volumen de imC!genes cargado en memoria." : "Muestra informacion del volumen.";
		}
		if (comando == "codificar_imagen") {
			return detallado ? "Uso: codificar_imagen\nCodifica una imagen en formato .huf utilizando el algoritmo de Huffman." : "Codifica una imagen.";
		}
		if (comando == "segmentar") {
			return detallado ? "Uso: segmentar\nSegmenta una imagen en base a semillas y grafos." : "Segmenta una imagen.";
		}
		return "Descripcion no disponible.";
	}
};

int main() {
	ConsolaInteractiva consola;
	consola.ejecutar();
	return 0;
};
