// Dentro de NodoHuffman.h
#ifndef NodoHuffman_h
#define NodoHuffman_h
struct NodoHuffman {
    int valor;
    int frecuencia;
    NodoHuffman* izquierda;
    NodoHuffman* derecha;

    // Constructor CORREGIDO: inicializa TODOS los miembros
    NodoHuffman(int val, int freq) :
        valor(val),
        frecuencia(freq),
        izquierda(nullptr), // <-- Añadir inicialización
        derecha(nullptr)    // <-- Añadir inicialización

        {};
};
#endif
