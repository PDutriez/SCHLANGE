#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include "disdrv.h"
#include "mydisplay.h"
#define IND_ERA 26
#define IND_AUP 27
#define IND_ADN 28
#define IND_NUM 29
#define IND_LET 'A'

static char display[MAX_Y+1][MAX_X+1];
static char alphabet [][WIDTH]={A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,ERASE,ARROWUP,ARROWDOWN,N0,N1,N2,N3,N4,N5,N6,N7,N8,N9};


int main() //MAIN DE PRUEBA
{
      display_clear();
      display_init();
      set_display_axis(NORMAL);
        clearmatrix();

     printfrase("HOLA ALE");


}
char collet(int columna, int fila, char letra, int collet)
{
    int counter, index;
    if(columna<=MAX_X && fila<=MAX_Y && collet<WIDTH)
    {//No se puede escribir fuera de la matriz
          index=myindex(letra);
          if(index!=ERROR)
          {
                for(counter=0; (counter<8)&&(fila+counter<=MAX_Y); ++counter)
                {
                    if((alphabet[index][collet])&(1<<counter))
                    {
                        display[fila+counter][columna]=D_ON;
                    }
                    else
                    {
                        display[fila+counter][columna]=D_OFF;
                    }
                }
                return 1;
          }
          else//Caso en el que el caracter deseado no existe
                return 0;
    }
    else
      return 0;
}

void rotar(char direction, char times)
{
  int counter,counterx,countery;
  for(counter=0;counter<times;++counter) //Repetimos tantas veces se pida
  {
    switch (direction) {
      case 'L': case 'l':
              for(counterx=1;counterx<=MAX_X;++counterx)
              {
                  for(countery=0;countery<MAX_Y;++countery)
                  { //Primero copiamos la columna hacia la izquierda
                    display[countery][counterx-1]=display[countery][counterx];
                    display[countery][counterx]=D_OFF; //Limpiamos la columna
                  }
              }
              break;

      case 'R': case 'r':
              for(counterx=MAX_X;counterx>=1;--counterx)
              {
                  for(countery=0;countery<=MAX_Y;++countery)
                  { //Primero copiamos la columna hacia la derecha
                    display[countery][counterx]=display[countery][counterx-1];
                    display[countery][counterx-1]=D_OFF; //Limpiamos la columna
                  }
              }
              break;
      case 'U': case 'u':
              for(countery=1;countery<=MAX_Y;++countery)
              {
                  for(counterx=0;counterx<=MAX_X;++counterx)
                  { //Primero copiamos la columna hacia la izquierda
                    display[countery-1][counterx]=display[countery][counterx];
                    display[countery][counterx]=D_OFF; //Limpiamos la columna
                  }
              }
              break;

      case 'D': case 'd':
              for(countery=MAX_Y;countery>=1;--countery)
              {
                  for(counterx=0;counterx<=MAX_X;++counterx)
                  { //Primero copiamos la columna hacia la derecha
                    display[countery][counterx]=display[countery-1][counterx];
                    display[countery-1][counterx]=D_OFF; //Limpiamos la columna
                  }
              }
              break;
    }
  }
}

int printchar(char letra,int x,int y)
{
  if((x<=MAX_X && x>=MIN_X)&&(y<=MAX_Y && y>=MIN_Y)&&(myindex(letra)!=ERROR))
  {
      int counter;
      for(counter=0;counter<WIDTH;++counter)
      {
          collet(x+counter,y,letra,counter);
      }
      return 1;
  }
  else
      return 0;
}
//SE PODRIA AGREGAR VELOCIDAD

void printfrase(char* frase)
{
    unsigned int c1, c2, c3;
    for(c1=0;frase[c1]!=0;++c1)//Primero cargamos cada letra de la frase
    {
        printf("%d %c\n",frase[c1],frase[c1]);
        for(c2=0;c2<WIDTH;++c2)//Cargamos todas las columnas de la letra
        {
            collet(4,MAX_X,frase[c1],c2);//Imprimimos la columna de la letra
            matrix2display();
            rotar('L',1);
            usleep(85000);
        }
        rotar('L',1);
    }
    //Una vez cargada toda la frase debemos rotar hasta que haya pasado todo
    for(c3=0;c3<MAX_X;++c3)
    {
        matrix2display();
        rotar('L',1);
        usleep(85000);
    }
}
int togglematrix(int const x1,int const x2,int const y1,int const y2)
{
   int posx,posy;
   if((x1<=x2 && y1<=y2)||(x1<MIN_X || x2>MAX_X)||(y1<MIN_Y || y2>MAX_Y))
   {
       for(posx=x1;posx<=x2;++posx)
       {
          for(posy=y1;posy<=y2;++posy)
          {
              display[posx][posy]^=D_ON;
          }
       }
        return 1;
   }
   else
        return 0;
}
int myindex(char letra)
{
      int index
      if(letra<='9' && letra>='0')//Lo convertimos en indice para el arreglo
      {
          index=letra-IND_NUM;//Indice para los numeros
      }
      else if(letra<='Z' && letra>='A')
      {
          index=letra-IND_LET;//Indice para las letras
      }
      else//Casos especiales y "OUT OF RANGE"
      {
          switch (letra)
          {
              case ' ': index=IND_ERA; break;
              case '>': index=IND_AUP; break;
              case '<': index=IND_ADN; break;
              default: index=0xFFFF; break;
          }
      }
      return index;
}
void matrix2display(void)
{
    int c1,c2;
    for(c1=0;c1<=MAX_X;++c1)
    {
        for(c2=0;c2<=MAX_Y;++c2)
        {
          display_write(c1,c2,display[c1][c2]);
        }
    }
    display_update();
}

void clearmatrix(void)
{
    int c1,c2;
    for(c1=0;c1<=MAX_X;++c1)
    {
        for(c2=0;c2<=MAX_Y;++c2)
        {
          display[c1][c2]=D_OFF;
        }
    }
}
