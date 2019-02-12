#ifndef SCHLANGE_H
#define SCHLANGE_H
/*******************************************************************************
* CONSTANTES
*******************************************************************************/
typedef enum {DEAD, ALIVE, APPLE, WALL} state_t; //Posibles estado
typedef enum {DIF1=1,DIF2,DIF3} levels_t; //Todas las dificultades
#define KEY_UP  12  //Como las agujas del reloj
#define KEY_RIGHT  3
#define KEY_DOWN  6
#define KEY_LEFT  9
#define MINY 0
#define MAXY 15
#define MINX 0
#define MAXX 15
/******************************************************************************/
typedef struct body{
  char state; //Indica si esta viva o muerta
  struct body* cola; //Si es una serpiente apunta a la parte trasera
} body_t;
/*******************************************************************************
* FUNCIONES
*******************************************************************************/
//Initsnake crea la serpiente, limpia la matriz, reinicia el SCORE y setea el
// nivel con el que inicia el jugador, ademas carga el puntaje del juego a un
// puntero recibido. Devuelve la posicion del puntero a la cabeza que se
// actualiza solo (Por eso se lo pasa una sola vez)
int* initsnake(char dificultad);

//Treat se encarga de poner una manzana en la posicion correcta, en caso de
// error devuelve NULL, en caso contrario devuelve un puntero a "apple"
body_t* treat(void);

//NEW_DIRECTION se encarga de modificar la direccion donde gira la serpiente.
// Recibe la nueva direccion y no devuelve nada.
void new_direction(char mydirection);
//Moove se encarga de mover la serpiente, analiza todos los casos de choque,
// ademas agranda la serpiente si comio la manzana. Devuelve el valor de la
// direccion en caso de haber cambiado. Si murio o termina el juego, devuelve 0.
body_t* moove(void);

//GET_POSITION devuelve la posicion de una casilla dentro de la matriz del Juego
// en caso de error devuelve 0xFFFF que es un valor fuera de rango de la matriz
unsigned int get_position(body_t* slot, char eje);
#endif
