#include <stdio.h>
#include <string.h>
#include "common_operations.h"
#define MAX_LINE 256
#define MAX_SCORES 3 //Cantidad de jugadores que quiero en la lista

/*void main(void)//MAIN DE PRUEBA
{
  char prueba=chkscore("score.txt",295);
  printf("POSICION:%d\n",prueba);
  if(prueba&&prueba!=LOOSER)
  {
    writescore("score.txt","PEPE",295);
    printf("NEW HIGH SCORE\n");
  }
  else
  {
    printf("LOOSER\n");
  }
}*/
char chkscore(char* topscores,int score)
{
    char position=1;
    FILE* archivo=fopen(topscores, "r");
    if(archivo==NULL)
    {
      printf("\nNo se pudo abrir el archivo\n");
      return 0;//Indicamos que no existe el archivo
    }
    else
    {
      char line[MAX_LINE];
      int counter;
      while (fgets(line,sizeof(line),archivo) != NULL)
      {//Primero buscamos el PUNTAJE
//Empezamos en 1 para saltear la enumeracion y buscamos el SCORE del renglon
            for(counter=1;((line[counter]<'0')||(line[counter]>'9'))&&counter<MAX_LINE;++counter);
            printf("%s\n",line);
            if(score<=conver2int(&line[counter]))
            {
              ++position;//Implica que es menor al puntaje actual
            }
      }
      fclose(archivo);
      return position;//Devolvemos la posicion del jugador indicado
    }
}

int writescore(char* topscores,char* name,int score)
{
  int position=chkscore(topscores,score);
  if(position&&position!=LOOSER)
  {
      FILE* archivo=fopen(topscores, "r");
      if(archivo==NULL)
      {
          printf("\nNo se pudo abrir el archivo\n");
          return 0;//Indicamos que no existe el archivo
      }
      else
      {
        FILE* copia=fopen("copia.txt","w");
        if(copia==NULL)
        {
            printf("\nNo se pudo cambiar la tabla\n");
            fclose(archivo);
            return 0;
        }
        else
        {
            int line_counter;
            char lines[MAX_SCORES+1][MAX_LINE];
            for(line_counter=0;line_counter<=MAX_SCORES;++line_counter)
                  strcpy(lines[line_counter],"\0");//Limpiamos TODO

            for(line_counter=0;(!feof(archivo))&&line_counter<MAX_SCORES;++line_counter)
            {//Verificamos que no se haya terminado el archivo
              fgets(lines[line_counter], MAX_LINE, archivo);//Copiamos el renglon
            }

            for( ;position<=line_counter;--line_counter)
            {//Corremos todos uno hacia abajo
              if(lines[line_counter-1][0]!='\0')//¿SE PUEDE COPIAR "LA NADA"?
              {
                strcpy(lines[line_counter],lines[line_counter-1]);
                ++lines[line_counter][0];//Aumentamos el indice
              }
            }

            sprintf(lines[position-1],"%d %s %d\n",position,name,score);//Copiamos el nuevo PUNTAJE

            for(line_counter=0;(line_counter<MAX_SCORES)&&lines[line_counter][0]!='\0';++line_counter)
                fprintf(copia, "%s", lines[line_counter]);//Copiamos el archivo

            fclose(archivo);//No te olvides de cerrrar lo que abris
            fclose(copia);
            remove(topscores);// remove the original file
            rename("copia.txt", topscores);// rename the temporary file to original name
            return 1;
        }
    }
  }
  return 0; //No se puede cambiar el archivo
}

int conver2int(char* string)
{
    int counter;//Contador de cifras
    int num = 0;//Numero a devolver
    for (counter=0;(string[counter]>='0')&&(string[counter]<='9');++counter)
    {//siempre verifico que sea un numero
        num = (string[counter]-'0') + num*10;//Actualizo el numero
    }

    return num;
}
