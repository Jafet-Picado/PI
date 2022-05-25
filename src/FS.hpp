#ifndef FS_HPP
#define FS_HPP

#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
#include <chrono>

#include "Usuario.hpp"
#include "Verificador.hpp"

#define SIZE_UNIDAD 128  //Tamano de cada vector de la matriz unidad

//Restriccion: bloque debe ser potencia de 2 y menor que SIZE_UNIDAD.
#define SIZE_BLOQUE 128  //Tamano de cada bloque de la unidad

struct entradaDirectorio{
  int bloque = -1;
  std::string nombre = "";
  std::string fecha = "";
  int puntero = -1; //Esto es para saber en qué posición se escribió la última vez
  std::string permisos = "0000";
  std::string permisosPrint = "-rwxrwxrwx"; // Estos son solo para printear en imprimirUnidad(1)
  bool abierto = false;
  bool isDirectory = false;
};

struct voto{
  std::string ID;
  int voto;
};

class FS{
  // vars
 private:
  
  char unidad[SIZE_UNIDAD][SIZE_UNIDAD];  // El disco debe ser de 100 espacios. // Creo que esto inicializa la matriz con "-" 
  std::vector<entradaDirectorio> directorio;
  std::vector<int> FAT; // -1 para el "x" en la tabla fat y debe ser de tamaño 100
  std::string directorioActual;
  int posicionActual = 0;
  std::fstream archivo;
  Verificador* verificador;

  // methods
 public: 
  FS();
  ~FS();
  void crear(std::string nombre, Usuario* user);
  bool abrir(std::string nombre, Usuario *user);
  void escribir(std::string nombre, std::string texto/*Cambiado de string texto a char por el momento*/, int posicion, Usuario *user);
  
  //void FS::escribirTexto(std::string nombre, std::string texto, int posicion);

  std::string leer(std::string, Usuario *user);
  void agregar(std::string nombre, std::string entrada, Usuario *user);
  void imprimirUnidad(int tipo);
  void eliminar(std::string nombre, Usuario *user);
  void cerrar(std::string nombre, Usuario *user);
  void imprimirArchivo(std::string, Usuario *user);

  char leerCaracter(int I, int J);
  void escribirCaracter(int I, int J, char caracter);
  int unidadSize();
  std::string leerPadron(std::string nombre, Usuario* user);

 private:
  void imprimirMatriz();
  void imprimirDirectorio(int tipo);
  int buscarDirectorio(std::string nombre);
  int buscarEspacio();
};

FS::FS() {
  verificador = new Verificador();
  for (int indexI = 0; indexI < SIZE_UNIDAD; indexI++) {
    for (int indexJ = 0; indexJ < SIZE_UNIDAD; indexJ++) {
      unidad[indexI][indexJ] = '-';
    }
  }
  //-2 Significa posicion vacia. -1 End Of File, Numero positivo puntero a posicion.
  //FAT.resize(SIZE_UNIDAD*SIZE_UNIDAD/*100*/, -2);
  FAT.resize((SIZE_UNIDAD*SIZE_UNIDAD)/SIZE_BLOQUE, -2); 
  verificador = new Verificador();
}

FS::~FS() {
}
  
void FS::crear(std::string nombre, Usuario* user){
  if (verificador->verificarPermisos(user, "0700", 2)) {
    entradaDirectorio temp;
    temp.nombre = nombre;
    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    std::string date(std::ctime(&end_time));
    temp.fecha = date;
    verificador->asignarPermisos(false, temp.permisos, temp.permisosPrint, "0735");
    directorio.push_back(temp);
  } else {
    std::cout << "No tiene permisos para esta acción" << std::endl;
  }
}

bool FS::abrir(std::string nombre, Usuario *user) {
  bool aux = false;
  //if(user->getPermisos() != 5 ){
    for(int i = 0; i < directorio.size(); i++){
      if(directorio[i].nombre == nombre && !directorio[i].abierto){
        directorio[i].abierto = true;
        aux = true;
        break;
      }
    }
  //}
  return aux;
}

int FS::buscarDirectorio(std::string nombre) {
  // retorna la posicion en la que se encuentra el directorio o -1 si no existe
  int posicionDirectorio = 0;
  int encontroArchivo = 0;
  while (posicionDirectorio < directorio.size() && encontroArchivo == 0) {
    if (directorio[posicionDirectorio].nombre == nombre) {
      encontroArchivo = 1;
      break;
    }
    posicionDirectorio++;
  }
  if (encontroArchivo == 0) {
    posicionDirectorio = -1;
  }
  return posicionDirectorio;
}

int FS::buscarEspacio() {
  int indexF = 0;
  int encontroEspacio = 0;
  while (indexF < FAT.size()) {
    if (FAT[indexF] == -2) {
      encontroEspacio = 1;
      break;
    }
    indexF++;
  }
  if (encontroEspacio == 0) {
    std::cerr<<"\033[31mError, ya no hay mas espacio\n";
    return -1;
  }else{
    return indexF;
  }
}

void FS::escribir(std::string nombre, std::string texto, int posicion, Usuario *user) {
  if (verificador->verificarPermisos(user, "0700", 2)) {
    if (posicion >= (SIZE_UNIDAD*SIZE_UNIDAD)/SIZE_BLOQUE) {
      std::cerr<<"\033[31mError, se intento acceder a una posicion no valida\n" 
      << nombre << std::endl;
    }else{
      int posicionDirectorio = buscarDirectorio(nombre);
      if (posicionDirectorio == -1) {
        std::cerr<<"\033[31mError, el directorio no existe\n";
        return;
      }
      if (FAT[posicion] != -2) {
        posicion = buscarEspacio();
        if (posicion == -1) {
          return;
        }
      }
      if (directorio[posicionDirectorio].bloque == -1) {
        directorio[posicionDirectorio].bloque = posicion;
        directorio[posicionDirectorio].puntero = posicion;
      }
      int contadorTexto = 0;
      int posicionAux = posicion;
      while (contadorTexto < texto.size()) {
        FAT[directorio[posicionDirectorio].puntero] = posicionAux;
        directorio[posicionDirectorio].puntero = posicionAux;  //Se actualiza puntero
        FAT[posicionAux] = -1;  //Nuevo end of file.

        int indexI = 0; int indexJ = 0;    
        int inicioBloque = SIZE_BLOQUE * posicionAux;
        while (inicioBloque >= SIZE_UNIDAD) {
          inicioBloque = inicioBloque - SIZE_UNIDAD;
          indexI++;
        }
        indexJ += inicioBloque;
        int finBloque = indexJ + SIZE_BLOQUE;
        while (indexJ < finBloque) {
          if (contadorTexto < texto.size()) {
            char caracter = texto[contadorTexto++];
            unidad[indexI][indexJ] = caracter;
            indexJ++;             
          }else{
            break;
          }
        }
        if (contadorTexto < texto.size()) {
          posicionAux = buscarEspacio();
          if (posicionAux == -1) {
            return;
          }
        }
      }
      this->posicionActual = posicion;
    }
  } else {
    std::cout << "No tiene permisos para esta acción" << std::endl;
  }
}

std::string FS::leer(std::string nombre, Usuario *user) {
  std::string to_return;
  if (verificador->verificarPermisos(user, "0700", 2)) {
    if(this->abrir(nombre, user)){
      int posicion;
      int encontroDirectorio = 0;
      for (int index = 0; index < directorio.size(); ++index) {
        if (directorio[index].nombre == nombre) {
          posicion = directorio[index].bloque;
          encontroDirectorio = 1;
          break;
        }
      }
      if (encontroDirectorio == 1) {
        int posicionAux = posicion;
        while (posicionAux != -1) {
          int posicionUnidad = posicionAux;
          int indexI = 0;
          int indexJ = 0;
          while (posicionUnidad != 0) {
            if (posicionUnidad>=SIZE_UNIDAD) {
              indexI++;
              posicionUnidad = posicionUnidad - SIZE_UNIDAD;
            }
            if (posicionUnidad<SIZE_UNIDAD) {
              indexJ++;
              posicionUnidad = posicionUnidad - 1;
            }
          }
          to_return += unidad[indexI][indexJ];
          posicion = posicionAux;
          posicionAux = FAT[posicion];
        }
      }else{
        std::cerr<<"\033[31mError, el directorio "<< nombre << " no existe\n";
      }
    }
  } else {
    std::cout << "No tiene permisos para esta acción" << std::endl;
  }
  return to_return;
}

void FS::agregar(std::string nombre, std::string entrada, Usuario *user) {
  if (verificador->verificarPermisos(user, "0700", 2)) {
    int posicion = buscarEspacio();
    if (posicion == -1) {
      return;
    }else{
      escribir(nombre, entrada, posicion, user);
    }
  } else {
    std::cout << "No tiene permisos para esta acción" << std::endl;
  }
}

void FS::imprimirDirectorio(int tipo){
  std::cout << "\033[32mDirectorio:\033[0m" << std::endl;
    for (int i = 0; i < directorio.size(); ++i) {
      if (tipo == 1) {
        std::cout << directorio[i].permisosPrint << "\t";
      }
      std::cout << directorio[i].nombre << "\t";
      std::cout << directorio[i].bloque;
      if (tipo == 1) {
        std::cout << "\t" << directorio[i].fecha;
      }
    }
    std::cout << std::endl; // Fin impresión del directorio
}

void FS::imprimirMatriz(){
  std::cout << "\033[32mUnidad:\033[0m" << std::endl;
    for (int i = 0; i < SIZE_UNIDAD; ++i) {
      for (int j = 0; j < SIZE_UNIDAD; ++j) {
        std::cout << unidad[i][j] << " ";
      }
      std::cout << std::endl;
    }
}
void FS::imprimirUnidad(int tipo = 0){ // Esto hace que tipo sea opcional
  std::cout << std::endl;
  if (!directorio.empty()) {
    // Este for es para imprimir el directorio
    imprimirDirectorio(tipo);
    
    // Imprimir FAT
    std::cout << "\033[32mFAT:\033[0m" << std::endl;
    int temp = 0;
    int posicionActual = 0;
    for (int i = 0; i < FAT.size(); ++i) { // Imprime la primera parte de una fila de 10, si temp es 10 o se termina, imprime las posiciones
      if (FAT[i] == -1) {
        std::cout << "\033[34mx\033[0m"; //Solo imprime la X de color azul
      } else if (FAT[i] == -2) {
        std::cout << "-";
      } else {
        std::cout << FAT[i];
      }
      ++temp;
      if (temp == 15 || i+1 == FAT.size()) { // temp es para el tamaño de la fila al imprimir por si se quisiera cambiar
        std::cout << std::endl;
        for (int j = 0; j < temp; ++j) { // Imprime las posiciones de memoria correspondientes al fat ya escrito
          std::cout << posicionActual;
          ++posicionActual;
          std::cout << "\t";
        }
        std::cout << std::endl << std::endl;
        temp = 0;
      } else {
        std::cout << "\t";
      }
    }
    std::cout << std::endl;

    // Imprimir la unidad
    imprimirMatriz();
    
  } else { // Si el directorio está vacío
    std::cout << "\033[31mEl directorio está vacío" << std::endl;
  }
}

void FS::eliminar(std::string nombre, Usuario *user) {
  //if(user->getPermisos() == 4){
    int posicion = 0;
    int ultimaPosicion = 0;
    for (int index = 0; index < directorio.size(); index++) {
      if (directorio[index].nombre == nombre) {
        // Borrado de directorio
        posicion = directorio[index].bloque;
        ultimaPosicion = directorio[index].puntero;
        directorio[index] = directorio[directorio.size()-1];
        directorio.resize(directorio.size()-1);
      }
    }
    //Reseteo de espacio en tabla FAT tras liberar directorio
    int posicionAux = posicion;
    while (FAT[posicion] != -1) {
      posicionAux = FAT[posicion];
      // -2 Para posicion libre
      int posicionUnidad = posicion;
      int indexI = 0;
      int indexJ = 0;
      while (posicionUnidad != 0) {
        if (posicionUnidad>=SIZE_UNIDAD) {
          indexI++;
          posicionUnidad -= SIZE_UNIDAD;
        } else {
          indexJ++;
          posicionUnidad--;
        }
      }
      unidad[indexI][indexJ] = '-';
      FAT[posicion] = -2;
      posicion = posicionAux;
    }
    FAT[posicion] = -2;
    int posicionUnidad = posicion;
    int indexI=0; int indexJ=0;
    while (posicionUnidad != 0) {
        if (posicionUnidad>=SIZE_UNIDAD) {
          indexI++;
          posicionUnidad -= SIZE_UNIDAD;
        } else {
          indexJ++;
          posicionUnidad--;
        }
    }
    unidad[indexI][indexJ] = '-';
    //Borrado de unidad de caracteres o string?
  //}
}

void FS::cerrar(std::string nombre, Usuario *user) {
  for(int i = 0; i < directorio.size(); i++){
    if(directorio[i].nombre == nombre && directorio[i].abierto){
      directorio[i].abierto = false;
      break;
    }
  }
}

void FS::imprimirArchivo(std::string nombre, Usuario *user) {
  std::cout << this->leer(nombre, user) << std::endl;
}

char FS::leerCaracter(int I, int J) {
  return unidad[I][J];
}

void FS::escribirCaracter(int I, int J, char caracter) {
  unidad[I][J] = caracter;
}

int unidadSize() {
  return SIZE_UNIDAD;
}

std::string FS::leerPadron(std::string nombre, Usuario* user){
  if (verificador->verificarPermisos(user, "0700", 2)) {
    int posicionDirectorio;
    //Busca la entrada del directorio
    for(int i = 0; i < directorio.size(); i++){
        if(directorio[i].nombre == nombre){
            posicionDirectorio = i;
        }
    }
    //Guardo todos los bloques que utiliza el archivo en un vector
    std::vector<int> bloquesUsados;
    int bloqueActual = directorio[posicionDirectorio].bloque;
    while(bloqueActual != -1){
        bloquesUsados.push_back(bloqueActual);
        bloqueActual = FAT[bloqueActual];
    }
    std::string lectura = "";
    //Recorro todos los bloques utilizados por el archivo
    for(int bloque : bloquesUsados){
        int inicioBloque = bloque*128;
        int finalBloque = ((bloque+1)*128);
        int fila = 0;
        int columna = 0;
        int contador = 0;
        //Obtengo la posición inicial del bloque en la unidad
        while(contador < inicioBloque){
            if(columna == SIZE_UNIDAD){
                columna = 0;
                fila++;
            } else {
                columna++;
                contador++;
            }
        }
        for(int index = inicioBloque; index < finalBloque; index++){
            if(columna == SIZE_UNIDAD){
                columna = 0;
                fila++;
            } else {
                if(unidad[fila][columna] == '$'){
                    lectura += "\n";
                    break;
                } else {
                    lectura += unidad[fila][columna];
                    columna++;
                }
            }
        }
    }
    return lectura;
  } else {
    std::cout << "No tiene permisos para esta acción" << std::endl;
  }
  return "";
}

#endif