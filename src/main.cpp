#include "FS.hpp"
#include "Usuario.hpp"
#include "Empadronador.hpp"
#include <string>
#include <iostream>

// RESTRICCIONES
// 1. bloques son de 1 caracter
// 2. Unidad es una matriz de caracteres
// 3. Tamaño máximo 100 caracteres, por el momento
// 4. Usando FAT
// 5. Usando un struct para el directorio (nombre, bloque inicial, fecha, por el
// momento)

int main() {
  FS* fs = new FS();
  Usuario* user = new Usuario(1);
  fs->crear("votos.txt",user);
  Empadronador empadronador;
  empadronador.cargarPadron(fs, user);
  std::vector<int> votos = {1,2,1,2,1,2,1,2,1,2};
  empadronador.subirVotos(fs,votos,user);
  std::cout<<empadronador.contarVotos(fs, user)<<std::endl;
  //fs->imprimirUnidad();
  //std::cout<<fs->leerPadron("CA-Paraiso.txt",user)<<std::endl;
}

/*  IMPRIMIR UNIDAD al final

Directorio:
"a.dat"   0
"b.dat"   1

FAT
x 2 x
0 1 2
- - -
3 4 5
- - -
6 7 8

UNIDAD
a b 2
_ _ _
_ _ _

*/
