# 📷 Proyecto de Estructuras de Datos - Entrega #2

## 📚 Curso
**Pontificia Universidad Javeriana**  
Departamento de Ingeniería de Sistemas  
_Estructuras de Datos - 2025_

## 👨‍🏫 Profesor
John Jairo Corredor Franco

## 👨‍💻 Integrantes
- Diego Fernando Zabala  
- Santiago Camargo Trujillo  
- David Andrés Villarreal  

---

## 📌 Descripción General

Este proyecto implementa un sistema de **procesamiento de imágenes en escala de grises** en formato `.pgm`. En esta segunda entrega, se consolidan los **Componentes 1 y 2** del sistema:

1. **Proyección de volúmenes tridimensionales en 2D**.
2. **Compresión y descompresión de imágenes** usando **Codificación Huffman**.

La interacción con el sistema se realiza mediante una **consola interactiva de comandos**, siguiendo el estilo propuesto en el enunciado del proyecto.

---

## ⚙️ Estructura del Proyecto

Entrega2EDD/ ├── src/ # Código fuente principal (archivos .cpp/.h) ├── tests/ # Casos de prueba unitarios y de integración ├── docs/ # Documentación en PDF (Diseño y Plan de Pruebas) ├── assets/ # Imágenes de prueba y resultados (.pgm/.huf) ├── README.md # Este archivo ├── .gitignore # Archivos a ignorar por Git └── Makefile # (opcional) automatiza la compilación



---

## 🧠 Componentes Implementados

### 🧩 Componente 1: Proyección de Imágenes

Simula la generación de radiografías proyectando un **volumen 3D de imágenes** en una imagen 2D mediante distintos criterios: `minimo`, `maximo`, `promedio`, `mediana`.  

**Comandos disponibles:**
- `cargar_imagen <archivo.pgm>`
- `cargar_volumen <nombre_base> <cantidad>`
- `proyeccion2D <direccion> <criterio> <salida.pgm>`
- `info_imagen`
- `info_volumen`

---

### 🧩 Componente 2: Codificación de Imágenes

Implementa la **Codificación y Decodificación Huffman** para imágenes PGM. Utiliza estructuras eficientes como `priority_queue` y árboles binarios.

**Comandos disponibles:**
- `codificar_imagen <salida.huf>`
- `decodificar_archivo <entrada.huf> <salida.pgm>`

---

## 🚀 Instrucciones de Compilación y Ejecución

### 📦 Compilar

Asegúrate de tener `g++` instalado. Luego ejecuta:

```bash
g++ src/*.cpp -o imagenes

▶️ Ejecutar

./imagenes

💻 Interacción
Desde la consola del sistema aparecerá un prompt $ para escribir los comandos. Por ejemplo:
$ cargar_volumen base_01 10
$ proyeccion2D z promedio salida.pgm
$ codificar_imagen salida.huf
$ decodificar_archivo salida.huf salida_dec.pgm

🧪 Pruebas
Las pruebas se encuentran en la carpeta tests/ e incluyen:

Validaciones de comandos de consola

Carga correcta de imágenes y volúmenes

Proyecciones 2D correctas

Compresión y descompresión sin pérdida

Comparación binaria de imágenes originales vs decodificadas

📄 Documentación
En la carpeta docs/ encontrarás:

documento_entrega2.pdf: Documento de diseño detallado con TADs, algoritmos, diagramas y análisis de complejidad y casos de prueba con entradas, salidas esperadas y resultados obtenidos.

✅ Estado de Avance
 Componente 1: Proyecciones volumétricas

 Componente 2: Codificación Huffman

 Interfaz interactiva por consola

 Validación de errores y formatos

 Documentación técnica

 Plan de pruebas estructurado

🧠 Lecciones Aprendidas
Este proyecto reforzó conceptos de:

Diseño e implementación de TADs (Imagen, Volumen, NodoHuffman)

Algoritmos de compresión

Manejo eficiente de estructuras dinámicas

Interacción por consola y manejo de errores

🏁 Próximos pasos
Para la Entrega #3 se incorporará el Componente 3 (segmentación de imágenes) usando algoritmos de grafos como Dijkstra, y se integrará todo en un sistema unificado.

📜 Licencia
Proyecto académico, sin fines comerciales.
