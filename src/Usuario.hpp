// #pragma once

#ifndef USUARIO_HPP
#define USUARIO_HPP

#include <iostream>
#include <string>

/**
* Usuarios de Almacenamiento:
* Administrador Técnico = 1    Revisa funcionamiento de la máquina, no lee
* Administrador Sistema = 2    Lee el padrón, no los votos, verifica el funcionamiento del sistema
* Auditor = 3                  Solo lee, no escribe
* Usuario Sistema = 4          Tiene permisos de escritura, intermedio entre Cv y Almac
* 
* Usuarios del Centro de Votación
* Administrador Técnico = 5    Revisa funcionamiento de la máquina
* Representante = 6            Habilita la otra máquina de votación (pendiente desición)
* Votante = 7                  Votante
*/

class Usuario {
 private:
  int rol;

 public:
  Usuario(int rol) { 
    this->rol = rol;

  }

  ~Usuario(){ 
  }

  int getPermisos() {
    return rol;
  }
};

#endif