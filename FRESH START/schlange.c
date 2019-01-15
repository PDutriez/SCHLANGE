#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "disdrv.h"
#include "schlange.h"

static void chkcrush(char* direction, int* posx, int* posy);
//Verifica que no se haya pasado de los bordes y cambia su direccion en caso de choque

static body_t matrix[MAX_Y+1][MAX_X+1]; //Arreglo con el cuerpo de la serpiente
static body_t* apple;  //Puntero a la manzana
static body_t* head;  //Puntero a la cabeza de la serpiente
static char lifes;  //Variable con la cantidad de vidas
static int  score;  //Variable con el puntaje
static char level;  //Variable con la dificultad

body_t** initsnake(char dificultad)
{
    int counter1,counter2;
    if(dificultad==DIF1 || dificultad==DIF2 || dificultad==DIF3)
    {
      score=0;//Se inicia con 0 puntos
      level=dificultad;
      lifes=3;
      apple=NULL;//Indicamos que no hay manzana
  //Limpiamos toda la matriz en caso de que haya basura
      for(counter1=0;counter1<= MAX_Y;++counter1)
      {
          for(couter2=0;counter2<= MAX_X;++counter2)
          {
            matrix[counter1][counter2].state=DEAD;
            matrix[counter1][counter2].cola=NULL;
          }
      }
      matrix[(MAX_Y+1)/2][MAX_X/2].state = ALIVE; //Creamos la cabeza
      matrix[(MAX_Y+1)/2+1][MAX_X/2].state = ALIVE; //Creamos la cola
      head=&matrix[(MAX_Y+1)/2][MAX_X/2]; //Iniciamos el puntero a la cabeza
      matrix[(MAX_Y+1)/2][MAX_X/2].cola = &matrix[(MAX_Y+1)/2+1][MAX_X/2]; //Iniciamos el puntero del cuerpo

      return(&head);
    }
    else //Se desea elegir una dificultad IMPOSIBLE
      return NULL;


}

body_t** treat(void)
{
    if(apple)
    {
        int posx,posy;
        posx = (rand()%16);
        posy = (rand()%16);
        while(matrix[posx][posy].state!=DEAD)
        {
            posx = (rand()%16);
            posy = (rand()%16);
        }
        matrix[posx][posy].state=APPLE; //Debemos indicar que la casilla tiene la manzana
        apple=&matrix[posx][posy]; //Actualizamos el puntero a la manzana
        return (&apple);
    }
    else
    {
        return(NULL);
    }
}

char moove(char direction)
{
  int posx,posy;
  body_t* next;//Puntero utlizado para desplazar la serpiente
  posx1=get_position(head,'x');//Posicion en X de la cabeza
  posy1=get_position(head,'y');//Posicion en Y de la cabeza

  switch (direction) {  //Analizamos hacia que direccion se desplaza
    case KEY_UP:
                  --posy;
                  break;
    case KEY_DOWN:
                  ++posy;
                  break;
    case KEY_LEFT:
                  ++posx;
                  break;
    case KEY_RIGHT:
                  --posx;
                  break;
  }
  chkcrush(&direction,&posx,&posy);//Verificamos si hubo choques, si choco cambia sola la direccion


  if(lifes>0)
  {
    matrix[posy][posx].state=ALIVE;//Seteamos la cabeza
    matrix[posy][posx].cola= head;//Actualizamos el puntero del cuerpo
    head=&matrix[posy][posx];//Actualizamos el puntero a head

    if(head=apple)
    {
        apple=NULL;  //Activo el FLAG de que no hay manzana
        score+=(5*level);

    }
    else //Si no hay manzana el movimiento es el clasico
    {
        next=head;
        while(next->cola!=NULL) //Debo buscar la cola par destruirla
        {
            next=next->cola;
        }
        next->state=DEAD;//Destruimos la ultima casilla
    }
    return (direction);
  }
  else
      return 0;

}

static void chkcrush(char* direction, char* posx, char* posy)
{
//Primero analisamos el caso en el que se chocase con una pared
    if(*posx < MIN_X || *posx > MAX_X)  //Analisis sobre las paredes horizontales
    {
        --lifes;
        if(*posx < MIN_X) //Analizamos si se paso hacia la IZQUIERDA
            *posx=MIN_X;
        else if (posx > MAX_X) //Analizamos si se paso hacia la DERECHA
            *posx=MAX_X;

        if(*posy<MAX_Y/2)  //Lo mandamos hacia abajo
            {
                ++(*posy);
                *direction=KEY_DOWN;
            }
        else
            {
                --(*pos_y);  //Lo mandamos hacia arriba
                *direction=KEY_UP;
            }
    }
    if(*posy < MIN_Y ||*posy > MAX_Y)  //Analisis sobre las paredes verticales
    {
        --lifes;
        if((*posy) < MIN_Y) //Analizamos si se paso hacia la IZQUIERDA
            *posy=MIN_Y;
        else if ((*posy) > MAX_Y) //Analizamos si se paso hacia la DERECHA
            *posy=MAX_Y;

        if((*posx)<MAX_X/2)  //Lo mandamos hacia abajo
            {
              ++(*posx);
              *direction=KEY_LEFT;
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
        body_t* delete1, delete2; //Creamos un puntero para borrar la cola

        if(matrix[*posy][*posx].state==ALIVE)
        {
            --lifes;
            delete1=&matrix[*posy][*posx];
            if(delete1->cola==NULL)//Si es justo el ultimo, solo se borra ese
            {
                delete1->state=DEAD;
            }

            while(delete1->cola!=NULL)//En caso de que no sea el ultimo, borro todos
            {
                delete1=delete2;
                delete1->state=DEAD;
                delete1=delete1->cola;
                delete2->cola=NULL;

            }
        }

    }
}

unsigned int get_position(body_t* slot, char eje)
{
    switch (eje) {
      case "X": case "x"://Nos da la columna de una casilla en una matriz
                          return((slot-matrix)%MAX_X);
                          break;
      case "Y": case "y"://Nos da la fila de una casilla en una matriz
                          return((slot-matrix)/MAX_Y);
                          break;
      default://En caso de cargar mal el valor devuelve 0xFF
                          return 0xFFFF;
                          break;
    }
}
