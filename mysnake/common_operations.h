#ifndef COMMON_OPERATIONS_H
#define COMMON_OPERATIONS_H

#define LOOSER 4 //Valor que devuelve CHKSCORE si no fue mejor que nadie
/*******************************************************************************
* FUNCIONES
*******************************************************************************/
char chkscore(char* topscores,int score);
//CHKSCORE se fija en el archivo dado si el score dado entra en la tabla,
// devuelve 0 si el archivo no existe, devuelve 1,2 o 3 si esta dentro de la
// tabla, si devuelve 4 implica que el puntaje no estaria dentro de la tabla

int writescore(char* topscores,char* name,int score);
//WRITESCORE escribe un archivo nuevo con el mismo nombre pero con la tabla
// actualizada.Topscore:"-txt";Name:Nombre del jugador;Score:Puntaje

int conver2int(char* string);
//CONVER2INT convierte un string de numeros en un entero que es el valor que
// devuelve.


#endif
