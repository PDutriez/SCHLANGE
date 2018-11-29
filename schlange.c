#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "disdrv.h"
#include "schlange.h"

static void chkcrush(char* direction); //Verifica que no se haya pasado de los bordes y sigo misma

static body_t matrix[MAXY][MAXX]; //Arreglo con el cuerpo de la serpiente
static body_t apple;  //Flag que simboliza la existencia de la manzana (0 = no apple)
static char lifes;  //Variable con la cantidad de vidas
static int  score;  //Variable con el puntaje
static char level;  //Variable con la dificultad

int* initsnake(char dificultad)
{
  int counter1,counter2;
  score=0;
  level = dificultad;
  lifes=3;
  apple.state=DEAD;
  for(counter1=0;counter1 < MAX;++counter1)
  {
      for(couter2=0;counter2< MAX;+counter2)
      {
        matrix[counter1][counter2].state=DEAD;
        matrix[counter1][counter2].x=MINX;
        matrix[counter1][counter2].y=MINY;
        matrix[counter1][counter2].cola=NULL;
      }
  }
  cuerpo[(MAXX+1)/2][MAXY/2].state = ALIVE;
  cuerpo[(MAXX+1)/2][MAXY/2].x = (MAXX+1)/2;
  cuerpo[(MAXX+1)/2][MAXY/2].y = MAXY/2;
  cuerpo[(MAXX+1)/2+1][MAXY/2].state = ALIVE;
  cuerpo[(MAXX+1)/2+1][MAXY/2].x = (MAXX+1)/2+1;
  cuerpo[(MAXX+1)/2+1][MAXY/2].y = MAXY/2;

  return(&score);

}
body_t* treat(void)
{
  if(apple.state==DEAD)
  {
      unsigned char  counter1=0;
      apple.x = (rand()%16);
      apple.y = (rand()%16);
      while(matrix[apple.x][apple.y].STATE==ALIVE)
      {
          if(matrix[apple.x][apple.y].x==apple.x)
                apple.x = (rand()%16);
          if(matrix[apple.x][apple.y].y==apple.y)
                apple.y = (rand()%16);
      }
      apple.state=ALIVE;  //Ahora debemos avisar que ya hay una manzana en juego
      return (&apple);
  }
  else
  {
      return(NULL);
  }
}

char moove(char direction)
{
  body_t copy1, copy2; //Creamos los REMANENTES para mover la serpiente
  int counter=0;
  copy1=cuerpo[counter]; //Primero movemos la cabeza que seguramente esta viva

  switch (direction) {  //Debemos analizar hacia que direccion se desplaza
    case KEY_UP:
                  --cuerpo[counter].y;
                  break;
    case KEY_DOWN:
                  ++cuerpo[counter].y;
                  break;
    case KEY_LEFT:
                  ++cuerpo[counter].x;
                  break;
    case KEY_RIGHT:
                  --cuerpo[counter].x;
                  break;
  }
  chkcrush(&direction); //Verificamos si hubo choques, si choco cambia sola la direccion
  if(lifes)
  {
    while(cuerpo[++counter].state==ALIVE) //No tiene sentido mover algo muerto
    {
      copy2=cuerpo[counter];//Creamos el remanente del que vamos a mover
      cuerpo[counter]=copy1;
      copy1=copy2; //Necesitamos crear una copia del remanente
    }
    if((apple.state==ALIVE)&&((cuerpo[0].x==apple.x)&&(cuerpo[0].y==apple.y)))
    {
        cuerpo[counter]=copy1;//Agrego un bloque en la ultima posicion de la serpiente
        apple.state=DEAD;  //Activo el FLAG de que no hay manzana
        score+=(5*level);

    }
    return (direction);
  }
  else
      return 0;

}
static void chkcrush(char* direction) //Verifica que no se haya pasado de los bordes y sigo misma
{
//Primero analisamos el caso en el que se chocase con una pared
    if(cuerpo[0].x < MINX ||cuerpo[0].x > MAXX)  //Analisis sobre las paredes horizontales
    {
        --lifes;
        if(cuerpo[0].x < MINX) //Analizamos si se paso hacia la IZQUIERDA
            cuerpo[0].x=MINX;
        else if (cuerpo[0].x > MAXX) //Analizamos si se paso hacia la DERECHA
            cuerpo[0].x=MAXX;

        if(cuerpo[0].y<=MAXY/2)  //Lo mandamos hacia abajo
            {
                ++cuerpo[0].y;
                *direction=KEY_DOWN;
            }
        else
            {
                --cuerpo[0].y;  //Lo mandamos hacia arriba
                *direction=KEY_UP;
            }
    }
    if(cuerpo[0].y < MINY ||cuerpo[0].y > MAXY)  //Analisis sobre las paredes verticales
    {
        --lifes;
        if(cuerpo[0].y < MINY) //Analizamos si se paso hacia la IZQUIERDA
            cuerpo[0].y=MINY;
        else if (cuerpo[0].y > MAXY) //Analizamos si se paso hacia la DERECHA
            cuerpo[0].y=MAXY;

        if(cuerpo[0].x<=MAXX/2)  //Lo mandamos hacia abajo
            {
              ++cuerpo[0].x;
              *direction=KEY_LEFT;
            }
        else
            {
              --cuerpo[0].x;  //Lo mandamos hacia arriba
              *direction=KEY_LEFT;
            }
    }

//Ahora debemos analisar si se choco consigo misma
    if(lifes)
    {
        unsigned char  crash=0,counter;
        for(counter=1;(cuerpo[counter].state==ALIVE)&&(!crash);++counter)
        {   //Comparamos una por una las posiciones
            if((cuerpo[counter].x==cuerpo[0].x)&&(cuerpo[counter].y==cuerpo[0].y)&&(cuerpo[counter].state==cuerpo[0].state))
            {
                --lifes;
                crash=1;
            }
        }
        if(crash) //Si choco, cortamos todo ese resto de la serpiente
        {
            while(cuerpo[counter].state==ALIVE)
            {
                cuerpo[counter++].state=DEAD;
            }
        }
    }
}

body_t* getbody(void)
{
  return(cuerpo);
}
