#ifndef EMPADRONADOR_HPP
#define EMPADRONADOR_HPP

#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "FS.hpp"

class Empadronador{
	//vars
 private:
	std::vector<int> almacenVotos;
 public: 
  Empadronador();
	// Carga el padrón con los n archivos de la carpeta Padron
	bool cargarPadron(FS* fs, Usuario *user);
	// Simula la llegada de 10 votos en distintos padrones
	void simularVotos(FS *fs, Usuario *user);
  // Actualiza el valor de que "nombre" en "padron" votó
  bool actualizarPadron(FS *fs, std::string padron, std::string datos, Usuario *user);
  void subirVotos(FS *fs, std::vector<int> votos, Usuario* user);
  std::string contarVotos(FS* fs, Usuario* user);
  // Sube un vector de 10 votos
};

Empadronador::Empadronador() {

}

// @brief carga los contenidos de la carpeta ./Padron/ en la unidad, todos los archivos representan el padrón de cada cantón
// @param *fs el filesystem en donde se carga el padrón
// @return true si se carga exitosamente todo el contenido de la carpeta y si alguno falla devuelve false
bool Empadronador::cargarPadron(FS* fs, Usuario *user) {
  int contador = 0;
  std::fstream lista;
  std::string nombrePadron; // Nombre del archivo a leer y a crear
  std::fstream archivo;
  std::string linea;
  lista.open("../Padron/ListaPadrones.txt");
  if (lista.is_open()) {
    while (getline(lista, nombrePadron)) { // Recorre el path y "it" tiene el nombre de un archivo
      archivo.open("../Padron/" + nombrePadron);
      if (archivo.is_open()) {
        fs->crear(nombrePadron, user);
        while (getline(archivo, linea)) {
		linea += "$";
        fs->agregar(nombrePadron, linea, user);
        //fs->agregar(nombrePadron, "$", user); // Esto internamente significa \n EOL
        }
        archivo.close();
        ++contador;
      } else {
        std::cout << "\033[31mNo se pudo cargar el archivo " << nombrePadron << std::endl;
        return false;
      }
    }
  }
  
	if (contador > 0) {
		std::cout << "\033[32mSe cargaron correctamente " << contador << " archivos al padron\033[0m" << std::endl;
		return true;
	} else {
		std::cout << "\033[31mNo se cargo ningun archivo al padron" << std::endl;
		return false;
	}
}

// @brief Simula la llegada de votos en el sistema y como se cargan a la unidad
// @param *fs el filesystem con el padrón
void Empadronador::simularVotos(FS *fs, Usuario *user) {
	std::fstream archivo;
	std::string linea;
	std::string padron;
	std::string datos;
	archivo.open("../Votantes.txt");
	if (archivo.is_open()) {
		while (getline(archivo, linea)) {
			padron = linea.substr(0, linea.find(" "));
			std::cout << padron << std::endl;
			linea.erase(0, linea.rfind(" ")+1); // linea ahora contiene los datos de la persona que votó
			std::cout << linea << std::endl;
			actualizarPadron(fs, padron, linea, user);
			// Digamos que aquí se vota y después se encripta el voto. bla bla bla
		}
		archivo.close();
    // De aquí en adelante simula que llegaron N votos en un paquete para subir a la unidad
		archivo.open("../Votos.txt");
		std::string votos = "";
		if (archivo.is_open()) {
			while (getline(archivo, linea)) {
				almacenVotos.push_back(stoi(linea));
			}
		} else {
			std::cout << "\033[31mNo se pudo abrir el archivo Votos.txt" << std::endl;
		}
	} else {
		std::cout << "\033[31mNo se pudo abrir el archivo Votantes.txt" << std::endl;
	}
}

// @brief Actualiza el valor que indica si una persona votó o no en el padrón, le asigna 1 si ya votó
// @param *fs el filesystem en donde está el padrón
// @param padron es el nombre del archivo en donde se ubica el nombre de la persona a actualizar
// @param datos, contiene "nombre1 nombre2 apellido1 apellido2 cédula" de la persona que votó
// @return Devuelve true si logró encontrar a la persona y actualizó el valor en el padrón, false si no
bool Empadronador::actualizarPadron(FS *fs, std::string padron, std::string datos, Usuario *user) {
	
  // Elimina los nombres de datos, deja solamente la cedula
  int tamano_de_datos = datos.size();  
  for (int i = 0; i < (tamano_de_datos - 9); i++) {
    datos.erase(datos.begin());
  }

  // Recorre la unidad
  int size = unidadSize();
  for (int i = 0; i < size*size; i++) {
    // Los digitos de la unidad siempre pertenecen a la cedula
    if (isdigit(fs->leerCaracter((i-(i%40))/40, i%40 /* esta aritmetica complicada simplemente itera la unidad */))) {
      std::string cedula = "";
      for (int j = 0; j < 9; j++) {
        cedula += fs->leerCaracter((i-(i%40))/40, i%40);
        i++;
      }
      i++;
      // Compara la cedula actual con la que se ensta buscando
      if (datos.compare(cedula) == 0) {
        // Actualiza el valor del caracter
        fs->escribirCaracter((i-(i%40))/40, i%40, '1');
        return true;
      }
    }
  }
	return false;
}

// @brief Carga en la unidad los N votos que lleguen en el paquete (vector)
// @param *fs el filesystem en donde se deben cargar los votos
// @param votos, vector que contiene N cantidad de votos para subir a la unidad
void Empadronador::subirVotos(FS *fs, std::vector<int> votos, Usuario* user) {
	std::string voto = "";
	for(int v : votos){
		voto += std::to_string(v)+" ";
	}
	fs->agregar("votos.txt", voto, user);
}

std::string Empadronador::contarVotos(FS* fs, Usuario* user){
	std::string votos = "";
	fs->abrir("votos.txt",user);
	std::string aux = fs->leer("votos.txt",user);
	std::cout<<"Prueba leer: "<<aux<<std::endl;
	std::vector<char> partidos;
	for(char x : aux){
		if (!(std::find(partidos.begin(), partidos.end(), x) != partidos.end())) {
			partidos.push_back(x);	
		}
	}
	for(char x : partidos){
		int contador = 0;
		for(char y : aux){
			if(x == y){
				contador++;
			}
		}
		votos += "Partido #"+std::to_string(x)+" = "+std::to_string(contador)+" votos\n";
	}
	return votos;
}

#endif