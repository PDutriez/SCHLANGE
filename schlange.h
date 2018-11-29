#ifndef SCHLANGE_H
#define SCHLANGE_H
/*******************************************************************************
* CONSTANTES
*******************************************************************************/
#define MAXX 15
#define MAXY 15
#define MINX 0
#define MINY 0
#define MAX (MAXX+1)*(MAXY+1) //Largo de la serpiente
#define ALIVE 1 //Posibles estado del cuerpo
#define DEAD  0
#define WALL  2
#define FACIL 1 //Todas las dificultades
#define NORMAL 2
#define DIFICIL 3
#define KEY_UP  12  //Como las agujas del reloj
#define KEY_RIGHT  3
#define KEY_DOWN  6
#define KEY_LEFT  9
/******************************************************************************/
//Estructura que representa el cuerpo de la serpiente
typedef struct body{
  char state; //Indica si esta viva o muerta
  signed char x; //Su posicion sobre el eje X en el display
  signed char y; //Su posicion sobre el eje Y en el display
  struct body* cola;
} body_t;
/*******************************************************************************
* FUNCIONES
*******************************************************************************/
//Initsnake crea la serpiente, y setea en nivel con el que inicia el jugador
int* initsnake(char dificultad);

//Treat se encarga de poner una manzana en la posicion correcta, en caso de
// error devuelve NULL, en caso contrario devuelve un puntero a "apple"
body_t* treat(void);

//Moove se encarga de mover la serpiente, analiza todos los casos de choque,
// ademas agranda la serpiente si comio la manzana. Devuelve el valor de la
// direccion en caso de haber cambiado. Si murio o termina el juego, devuelve 0.
char moove(char direction);
//Getbody lo unico que hace es devolver la direccion del arreglo del cuerpo
body_t* getbody(void);
#endif
