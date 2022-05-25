#ifndef VERIFICADOR_H
#define VERIFICADOR_H

#include "FS.h"
#include <iostream>
#include <string>

#include "Usuario.h"

class Verificador {
 public:
  Verificador();

  // @brief Verifica si user tiene permiso de hacer una acción en el archivo
  // @param user el tipo de usuario que hace la solicitud
  // @param permisos los permisos del archivo / método en cuestión que el user quiere usar
  // @return true si el user tiene permisos, false si el user no tiene permisos.
  bool verificarPermisos(Usuario *user, std::string permisos, int necesario);

  // @brief Verifica si user tiene permiso de hacer una acción en el archivo
  // @param perm valor de los permisos a cambiar
  // @param permPrint valor de los permisos en letras.
  // @param permisos los permisos que se le quieren asignar al archivo
  void asignarPermisos(bool isDirectory, std::string& perm, std::string& permPrint, std::string permisos);
};

// Los permisos llevan el formato read-write-execute

Verificador::Verificador() {
}

bool Verificador::verificarPermisos(Usuario *user, std::string permisos, int necesario) {
  std::string temp(1, permisos[user->getPermisos()]);
  int rol = std::stoi(temp); // tiene los permisos de ese rol en concreto
  // Honestamente no supe como usar los números como binarios para esto
  int a = rol & necesario;
  if (a == necesario) {
    return true;
  } else {
    return false;
  }
}

void Verificador::asignarPermisos(bool isDirectory, std::string& perm, std::string& permPrint, std::string permisos) {
  std::string texto;
  std::string numeral;
  int rol;
  if (isDirectory) {
    perm = "1";
    permPrint = "d";
  } else {
    perm = "0";
    permPrint = "-";
  }
  for (int i = 1; i < permisos.size(); ++i) {
    std::string temp(1, permisos[i]);
    rol = std::stoi(temp);
    switch (rol) {
      case 7:
        perm += "7";
        permPrint += "rwx";
        break;
      case 6:
        perm += "6";
        permPrint += "rw-";
        break;
      case 5:
        perm += "5";
        permPrint += "r-x";
        break;
      case 4:
        perm += "4";
        permPrint += "r--";
        break;
      case 3:
        perm += "3";
        permPrint += "-wx";
        break;
      case 2:
        perm += "2";
        permPrint += "-w-";
        break;
      case 1:
        perm += "1";
        permPrint += "--x";
        break;
      case 0:
        perm += "0";
        permPrint += "---";
        break;
    }
  }
}

#endif