#include "FS.h"
#include "Usuario.h"
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
  fs->crear("a.txt", user);
  //fs->agregar("a.txt", "Hola profe", user);
  fs->escribir("a.txt", "Hola profe", 0, user);
  fs->agregar("a.txt", " como esta", user);
  fs->agregar("a.txt", " bien y usted", user);
  /*s
  fs->crear("b.txt", user);
  fs->agregar("b.txt", "Espero que este bien el trabajo", user);
  fs->imprimirUnidad(1);
  fs->eliminar("a.txt", user);
  std::cout << fs->leer("b.txt", user) << std::endl;
  */
  fs->imprimirUnidad(1);
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
