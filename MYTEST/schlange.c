#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "schlange.h"
#define ERROR 0xFFFF

static void chkcrush(char* direction, int* posx, int* posy);
//Verifica que no se haya pasado de los bordes y cambia su direccion en caso de choque

static body_t matrix[MAXY+1][MAXX+1]; //Arreglo con el cuerpo de la serpiente
static body_t* apple;  //Puntero a la manzana
static body_t* head;  //Puntero a la cabeza de la serpiente
static char lifes;  //Variable con la cantidad de vidas
static int  score;  //Variable con el puntaje
static char level;  //Variable con la dificultad
static char direction; //Direccion de desplazamiento

int* initsnake(char dificultad)
{
    int counter1,counter2;
    if(dificultad==DIF1 || dificultad==DIF2 || dificultad==DIF3)
    {
      score=0;//Se inicia con 0 puntos
      level=dificultad;
      lifes=3;
      apple=NULL;//Indicamos que no hay manzana
  //Limpiamos toda la matriz en caso de que haya basura
      for(counter1=0;counter1<= MAXY;++counter1)
      {
          for(counter2=0;counter2<= MAXX;++counter2)
          {
            matrix[counter1][counter2].state=DEAD;
            matrix[counter1][counter2].cola=NULL;
          }
      }
      matrix[(MAXY+1)/2][MAXX/2].state = ALIVE; //Creamos la cabeza
      matrix[(MAXY+1)/2][(MAXX/2)-1].state = ALIVE; //Creamos la cola
      head=&matrix[(MAXY+1)/2][MAXX/2]; //Iniciamos el puntero a la cabeza
      matrix[(MAXY+1)/2][MAXX/2].cola = &matrix[(MAXY+1)/2][(MAXX/2)-1]; //Iniciamos el puntero del cuerpo
      direction=KEY_RIGHT;
      printf("Snake started.\n");
      return &score;
    }
    else //Se desea elegir una dificultad IMPOSIBLE
      return NULL;


}

body_t* treat(void)
{
    if(!apple)
    {
        int posx,posy;
        posx = (rand()%16);
        posy = (rand()%16);
        while(matrix[posy][posx].state!=DEAD)
        {
            posx = (rand()%16);
            posy = (rand()%16);
        }
        matrix[posy][posx].state=APPLE; //Debemos indicar que la casilla tiene la manzana
        apple=&matrix[posy][posx]; //Actualizamos el puntero a la manzana
        return (apple);
    }
    else
    {
        return(NULL);
    }
}

void new_direction(char mydirection)
{
  switch (mydirection) {
    case KEY_UP://No puede ser el sentido contrario
                if(direction!=KEY_DOWN)
                      direction=mydirection;
                break;
    case KEY_DOWN://No puede ser el sentido contrario
                if(direction!=KEY_UP)
                      direction=mydirection;
                break;
    case KEY_LEFT://No puede ser el sentido contrario
                if(direction!=KEY_RIGHT)
                      direction=mydirection;
                break;
    case KEY_RIGHT://No puede ser el sentido contrario
                if(direction!=KEY_LEFT)
                      direction=mydirection;
                break;
    default://Es un valor inexistente
                break;
  }
  printf("\ndirection:%d\tlifes:%d\n",direction,lifes);
}

body_t* moove(void)
{
  int posx,posy;
  body_t* next;//Puntero utlizado para desplazar la serpiente
  body_t* last;//Puntero utilizado para actualizar la serpiente
  posx=get_position(head,'x');//Posicion en X de la cabeza
  posy=get_position(head,'y');//Posicion en Y de la cabeza

  switch (direction) {  //Analizamos hacia que direccion se desplaza
    case KEY_UP:
                  --posy;
                  break;
    case KEY_DOWN:
                  ++posy;
                  break;
    case KEY_LEFT:
                  --posx;
                  break;
    case KEY_RIGHT:
                  ++posx;
                  break;
  }
  chkcrush(&direction,&posx,&posy);//Verificamos si hubo choques, si choco cambia sola la direccion

  if(lifes>0)
  {
    matrix[posy][posx].state=ALIVE;//Seteamos la cabeza
    matrix[posy][posx].cola= head;//Actualizamos el puntero del cuerpo
    head=&matrix[posy][posx];//Actualizamos el puntero a head

    if(head==apple)
    {
        apple=NULL;  //Activo el FLAG de que no hay manzana
        score+=(5*level);

    }
    else //Si no hay manzana el movimiento es el clasico
    {
        next=head;
        while(next->cola!=NULL) //Debo buscar la cola par destruirla
        {
	    last=next;
            next=next->cola;
        }
        last->cola=NULL;//Actualizamos la cola de la serpiente
        next->state=DEAD;//Destruimos la ultima casilla
    }
	return (head);
  }
  else
  	return NULL;
}

static void chkcrush(char* direction, int* posx, int* posy)
{
//Primero analisamos el caso en el que se chocase con una pared
    if(*posx < MINX || *posx > MAXX)//Analisis sobre las paredes horizontales
    {
        --lifes;
        if(*posx < MINX) //Analizamos si se paso hacia la IZQUIERDA
            *posx=MINX;
        else if (*posx > MAXX) //Analizamos si se paso hacia la DERECHA
            *posx=MAXX;

        if(*posy<MAXY/2)  //Lo mandamos hacia abajo
            {
                ++(*posy);
                *direction=KEY_DOWN;
            }
        else
            {
                --(*posy);  //Lo mandamos hacia arriba
                *direction=KEY_UP;
            }
    }
    if(*posy < MINY ||*posy > MAXY)//Analisis sobre las paredes verticales
    {
        --lifes;
        if((*posy) < MINY) //Analizamos si se paso hacia la IZQUIERDA
            *posy=MINY;
        else if ((*posy) > MAXY) //Analizamos si se paso hacia la DERECHA
            *posy=MAXY;

        if((*posx)<MAXX/2)  //Lo mandamos hacia abajo
            {
              ++(*posx);
              *direction=KEY_RIGHT;
            }
        else
            {
              --(*posx);  //Lo mandamos hacia arriba
              *direction=KEY_LEFT;
            }
    }

//Ahora debemos analisar si se choco consigo misma
    if(lifes)
    {
        body_t* delete1,* delete2; //Creamos un puntero para borrar la cola

        if(matrix[*posy][*posx].state==ALIVE)
        {
            --lifes;
            delete1=head;
	    while(delete1->cola!=(&matrix[*posy][*posx]))//Buscamos la celula que apunta a contra la del choque
			delete1=delete1->cola;

	    delete2=delete1;
	    delete1=delete1->cola;//Colocamos DELETE1 en la celula chocada
	    delete2->cola=NULL;//Cortamos la coneccion con el choque

            if(delete1->cola==NULL)//Si es justo el ultimo, solo se borra ese
            {
                delete1->state=DEAD;
            }

            while(delete1->cola!=NULL)//En caso de que no sea el ultimo, borro todos
            {
                delete2=delete1;
                delete1->state=DEAD;
                delete1=delete1->cola;
                delete2->cola=NULL;

            }
	    delete1->state=DEAD;//Borramos la ultima celula
        }

    }
}

unsigned int get_position(body_t* slot, char eje)
{
    unsigned int position=ERROR;

    switch (eje) {
      case 'X': case 'x'://Nos da la columna de una casilla en una matriz
                          position=((slot-matrix[0])%(MAXX+1));
                          break;
      case 'Y': case 'y'://Nos da la fila de una casilla en una matriz
                          position=((slot-matrix[0])/(MAXY+1));
                          break;
      default: break; //Si no es ninguno, no hay que cambiar el valor
    }

    return position;
}
