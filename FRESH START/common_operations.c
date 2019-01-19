#include <stdio.h>
#include <string.h>
#include "common_operations.h"
#define MAX_LINE 256
#define LOOSER 4 //Valor que devuelve CHKSCORE si no fue mejor que nadie

void main(void)
{
  char prueba=chkscore("score.txt",33);
  printf("POSICION:%d\n",prueba);
  if(prueba&&prueba!=LOOSER)
  {
    writescore("score.txt","SABR",33);
  }
  else
  {
    printf("LOOSER\n");
  }
}
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
            for(counter=1;(line[counter]<'0')||(line[counter]>'9');++counter);
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
          char line_counter=0,line[MAX_LINE],copyline[MAX_LINE];
          while ((!feof(archivo))&&line_counter<LOOSER)
          {//Verificamos que no se haya terminado el archivo
            strcpy(line, "\0");//Limpio el "copia-lineas"
            fgets(line, MAX_LINE, archivo);
            if ((!feof(archivo))&&line_counter<LOOSER)//Si todavia no se termino...
            {
                line_counter++;//Aumentamos el contador de lineas
                if (line_counter != position)
                {
                    fprintf(copia, "%s", line);
                }
                else
                {
                    strcpy(copyline,line);
                    sprintf(line,"%d %s %d\n",line_counter,name,score);
                    fprintf(copia, "%s", line);
                    ++copyline[0];
                    fprintf(copia, "%s", copyline);
                }
                if(line_counter!=line[0])
                {
                    ++line[0];
                    fprintf(copia, "%s", line);
                }
            }
          }
          fclose(archivo);//No te olvides de cerrrar lo que abris
          fclose(copia);
          remove(topscores);// remove the original file
          rename("copia.txt", topscores);// rename the temporary file to original name
          return 1;
      }
  }
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
