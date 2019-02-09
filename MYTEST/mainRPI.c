/*******************************************************************************
* LIBRERIAS UTLIZIDAS
*******************************************************************************/
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include<pthread.h>
#include <semaphore.h>
#include "./RPI_LIB/termlib.h"
#include "./RPI_LIB/joydrv.h"
#include "./RPI_LIB/disdrv.h"
#include "libaudio.h"
#include "schlange.h"
#include "common_operations.h"
#include "mydisplay.h"
/*******************************************************************************
* DEFINICIONES
*******************************************************************************/
typedef enum {END,MENU,PAUSA,PLAY,SCORE} actions_t;//VALORES DEL SEMAFORO GRAL.
#define DIR_INIT 0 //Valor inicial de la variable STATICA "DIRECTION" (y otras...)
#define JOY_THRESHOLD 40 // 10-100 Joystick threshold (sensitivity)
#define UN_SEG 1000000 //Cantidad de microsegundos en 1 segundo (para USLEEP)
#define ALLMUSIC 2 //Cantidad de canciones
static int modul(int a);	//Funcion que devuelve el modulo de un numero
static void* printfile(char* nombre); //Funcion que imprime un archivo (error:NULL)
static char changedir(jcoord_t joy_coordinates2);//SET DIRECTION
static void* set_RPI(int position); //Cambia la direccion de DISPLAY y JOYSTICK
static void playsong(char* song); //Inicia una cancion
static char diflevels[3][8]={"FACIL","NORMAL","DIFICIL"};
static char music[ALLMUSIC][24]={"./Music/SaintSeiya.wav","./Music/Megaman2.wav"};
static jcoord_t joy_coordinates1;  //Variable para los valores del joystick
static jswitch_t joy_switch1=J_NOPRESS; //Variable para el switcher
static char gaming,dificultad,direction; //Varable que indica la direccion de la serpiente
static body_t* snake;  //Puntero a la serpiente
static body_t* apple;  //Puntero a la manzana
static int* score; //Puntero al puntaje
static bool change_music; //Flag para cambiar la musica
static pthread_mutex_t lock; //CANDADO
static sem_t  semaforo1,semaforo2; //semaforos del juego


void* mainsnake()
{
  body_t* applechk;//Variable que verifica si se necesita una nueva manzana

  while(gaming!=END) //Si se termino el juego, no se sigue haciendo esto
  {
      if(gaming==PLAY)//FALTA HACER
      {
        usleep((UN_SEG/5)/dificultad);
        applechk=treat();
        if(applechk)
        {
            apple=applechk;
            display_write(get_position(apple,'y'),get_position(apple,'x'),D_ON);
        }
        snake=moove();
        if(snake)
        {
              while(snake->cola!=NULL)
              {//Imprimimos el CUERPO
                display_write(get_position(snake,'y'),get_position(snake,'x'),D_ON);
                snake=snake->cola;
              }//Imprimimos la COLA
              display_write(get_position(snake,'y'),get_position(snake,'x'),D_ON);
              display_update();
              display_write(get_position(snake,'y'),get_position(snake,'x'),D_OFF);

              //Apagamos la ultima celula
        }
        else//Caso en el que se termino el juego
        {
          usleep(UN_SEG/5);//Para darse cuenta que perdio
          gaming=SCORE;//Indicamos que hay que analizar el score
          sem_post(&semaforo1);//Liberamos el SNAKEBRAIN
        }
        if(joy_switch1==J_PRESS)
              gaming=PAUSA;//Si se apreta, se quiere pausar
      }

  }
    return NULL;
}
/******************************************************************************/
void* snakebrain()
{
  while(gaming!=END)
  {
    if(gaming==PLAY)
    {
      direction=DIR_INIT;
      joy_switch1=J_NOPRESS;
      sem_post(&semaforo2);
      while(gaming==PLAY)
      {
        if(direction)
        {
            new_direction(direction);
        }
        sem_post(&semaforo2);
        sem_wait(&semaforo1);
      }
    }
    if(gaming==SCORE)
    {
      display_clear();
      int position;
      char finalscore[256];
      sprintf(finalscore,"SCORE %d",*score);
      pthread_mutex_lock(&lock);//¡¡ATENCION!! (es x precaucion)
      printfrase(finalscore);
      pthread_mutex_unlock(&lock);
      position=chkscore("score.txt",*score);
      if(position&&position!=LOOSER)
      {
        char name[4]={'A','A','A','\0'};
        int indicator=DIR_INIT;
        clearmatrix();
        printchar('>',indicator,13);
        for(indicator=DIR_INIT;indicator<=2;++indicator)
        printchar(name[indicator],indicator*WIDTH,4);
        matrix2display();
        indicator=DIR_INIT;
        direction=DIR_INIT;
        joy_switch1=J_NOPRESS;
        sem_post(&semaforo2);
        while(joy_switch1==J_NOPRESS)
        {
          if(direction)
          {
            printchar(' ',indicator*WIDTH,13);//Borro la flecha
            switch (direction) {
              case KEY_LEFT:  --indicator; break;
              case KEY_RIGHT: ++indicator; break;
              case KEY_UP:    ++name[indicator]; break;
              case KEY_DOWN:  --name[indicator]; break;
            }
            //Analizamos si se fue del rango el INDICADOR
            if(indicator>2)
                indicator=2;
            else if(indicator<0)
                indicator=0;
            //Analisamos si se fue de rango la letra
            if(name[indicator]>'Z')
                name[indicator]='Z';
            else if(name[indicator]<'A')
                name[indicator]='A';
            //Actualizamos todo, total se va a cargar todo de una
            printchar('>',indicator*WIDTH,13);
            printchar(name[indicator],indicator*WIDTH,4);
            matrix2display();

          }
          sem_post(&semaforo2);
          sem_wait(&semaforo1);//Esta aca para que analice el switch
        }
        display_clear();
        clearmatrix();
        writescore("score.txt",name,*score);
        printfile("score.txt");
      }
      gaming=MENU;
    }
  }
  return NULL;
}
/******************************************************************************/
void* mymenu()
{
//Damos la opcion de elegir jugar ROTATE o NORMAL (Solo al iniciar el juego)
    clearmatrix();
    printchar('<',5,13);
    matrix2display();
    direction=DIR_INIT;
    joy_switch1=J_NOPRESS;
    sem_post(&semaforo2);//En caso de que se haya trabado el semaforo
    while(joy_switch1==J_NOPRESS)
    {
      if(direction)
      {
        display_clear();
        if(direction==KEY_LEFT)
              set_RPI(NORMAL);
        else if(direction==KEY_RIGHT)
              set_RPI(ROTATE);
        matrix2display();//Al cambiar la rotacion no es necesario cambiar nada
      }
      sem_post(&semaforo2);
      sem_wait(&semaforo1);
    }
    sem_post(&semaforo2);//Al salir habilito el joystick de nuevo
//Esta parte se encarga del MENU PRINCIPAL y el de PAUSA
    while(gaming!=END)
    {
      if(gaming==MENU)//MENU PRINCIPAL
      {

          int x,y;//Variables para ubicarse en el MENU PRINCIPAL
          x=0;//Empezamos con la PRIMER COLUMNA
          y=0;//Empezamos con el PRIMER RENGLON
          display_clear();
          display_update();
          clearmatrix();//Limpiamos la MATRIZ
          printchar('P',x+1,y);//PRIMER COLUMNA,PRIMER RENGLON
          printchar('C',x+9,y);//SEGUNDA COLUMNA,PRIMER RENGLON
          printchar('S',x+1,y+8);//PRIMER COLUMNA,SEGUNDO RENGLON
          printchar('E',x+9,y+8);//SEGUNDA COLUMNA,SEGUNDO RENGLON
          togglematrix(x,x+7,y,y+7);//Se selecciona uno por DEFECTO
          matrix2display();
          direction=DIR_INIT;
          joy_switch1=J_NOPRESS;
          printf("direction:%d switch:%d\n",direction,joy_switch1);
          while(joy_switch1==J_NOPRESS)
          {
            if(direction)
            {
                togglematrix(x,x+7,y,y+7);//Cambiamos el viejo
                switch (direction) {
                  case KEY_UP:  y=0; break;
                  case KEY_DOWN: y=8; break;
                  case KEY_LEFT: x=0; break;
                  case KEY_RIGHT: x=8; break;
                }
                togglematrix(x,x+7,y,y+7);//Ponemos el nuevo
                matrix2display();
                direction=DIR_INIT;
            }
            sem_post(&semaforo2);
            sem_wait(&semaforo1);//Esta aca para que analice el switch
          }
//ANALISIS DE LO SELECCIONADO
          display_clear();//BORRO EL MENU PRINCIPAL
          clearmatrix();
          if(x==0)//PRIMERO COLUMNA
          {
              if(y==0)//PRIMER COLUMNA,PRIMER RENGLON
              {
                dificultad=DIF1;
                pthread_mutex_lock(&lock);//¡¡ATENCION!! (es x precaucion)
                printfrase(diflevels[dificultad-1]);
                pthread_mutex_unlock(&lock);
                direction=DIR_INIT;
                joy_switch1=J_NOPRESS;
                sem_post(&semaforo2);//Habilito el joystick de nuevo
                while(joy_switch1==J_NOPRESS)
                {
                    if(direction)
                    {
                      switch (direction) {
                        case KEY_RIGHT: ++dificultad; break;
                        case KEY_LEFT:  --dificultad; break;
                      }
                      if(dificultad>3)
                          dificultad=3;
                      if(dificultad<1)
                          dificultad=1;

                      pthread_mutex_lock(&lock);//¡¡ATENCION!! (es x precaucion)
                      printfrase(diflevels[dificultad-1]);
                      pthread_mutex_unlock(&lock);

                      direction=DIR_INIT;
                    }
                    sem_post(&semaforo2);
                    sem_wait(&semaforo1);//Esta aca para que analice el switch
                }
                display_clear();// Clears Display buffer and Hardware Display
                clearmatrix();//Limpiamos TEXTO
                score=initsnake(dificultad);
                gaming=PLAY;
                joy_switch1=J_NOPRESS;
                sem_post(&semaforo2);//Al salir habilito el joystick de nuevo
              }
              else//PRIMER COLUMNA,SEGUNDO RENGLON
              {
                pthread_mutex_lock(&lock);//¡¡ATENCION!! (es x precaucion)
                printfile("score.txt");
                pthread_mutex_unlock(&lock);
              }
          }
          else//SEGUNDA COLUMNA
          {
              if(y==0)//SEGUNDA COLUMNA,PRIMER RENGLON
              {
                if(!change_music)//En caso de que no se haya bajado el FLAG
                  change_music=true;
              }
              else//SEGUNDA COLUMNA,SEGUNDO RENGLON
              {
                gaming=END;
              }
          }
      }
      if(gaming==PAUSA)//MENU DE PAUSA (obvio no?)
      {
          display_clear();//BORRO LA MATRIZ
          clearmatrix();
          int x;//Variable para ubicarse en el MENU de PAUSA
          x=0;//Empezamos en la PRIMERA COLUMNA
          printchar('P',x+1,3);
          printchar('M',x+9,3);
          togglematrix(x,x+7,0,15);
          matrix2display();
          direction=DIR_INIT;
          joy_switch1=J_NOPRESS;
          sem_post(&semaforo2);//Habilito el joystick de nuevo
          while(joy_switch1==J_NOPRESS)
          {
            if(direction)
            {
                togglematrix(x,x+7,0,15);//Cambiamos el viejo
                switch (direction) {
                  case KEY_LEFT: x=0; break;
                  case KEY_RIGHT: x=8; break;
                }
                togglematrix(x,x+7,0,15);//Ponemos el nuevo
                matrix2display();
            }
            sem_post(&semaforo2);
            sem_wait(&semaforo1);//Esta aca para que analice el switch
          }
//ANALISIS DE LO SELECCIONADO
          display_clear();//Borro el MENU de PAUSA
          clearmatrix();
          if(x==0)//PRIMER COLUMNA
          {
              gaming=PLAY;
              display_write(get_position(apple,'y'),get_position(apple,'x'),D_ON);
              sem_post(&semaforo2);//Habilito el joystick de nuevo
              joy_switch1=J_NOPRESS;
          }
          else//SEGUNDA COLUMNA
          {
              gaming=MENU;
          }
      }
    }

    return NULL;
}
/******************************************************************************/
void* myjoystick()
{//Se encarga unicamente de actualizar el valor del JOYSTICK
    int counter;
    counter=0;
    while(gaming!=END)
    {
      usleep(UN_SEG/4);//Lo que tarda en volver el JOYSTICK
      joystick_update();         // Read joystick Hardware

      joy_coordinates1=joystick_get_coord();  //And get coordinate values
      joy_switch1=joystick_get_switch_value(); // And get switch value
      direction=changedir(joy_coordinates1);
      if((joy_switch1==J_PRESS||(direction)))
      {
          printf("ENTRAMOS AL SEMAFORO %d\n",counter);
          ++counter;
          printf("direction:%d switch:%d",direction,joy_switch1);
          sem_post(&semaforo1);//Indicamos que hubo una accion
          sem_wait(&semaforo2);//Esperamos al analisis
          printf("SALIMOS DEL SEMAFORO\n\n");
      }
    }
    return NULL;
}
/******************************************************************************/
void* playmusic()
{
    int ind_music=DIR_INIT;//Indicador de musica
    init_sound(); // Init sound driver
    playsong(music[ind_music]);
    while(gaming!=END)
    {
      if(change_music==true)
      {
        ++ind_music;
        if(ind_music>ALLMUSIC)
            ind_music=DIR_INIT;

        if(ind_music==ALLMUSIC)//Caso sin musica
            stop_sound();
        else
            playsong(music[ind_music]);

        change_music=false;
      }
      if(player_status()==FINISHED)
      {
        printf(MAGENTA_TEXT "CANCION TERMINADA\n" WHITE_TEXT);
         playsong(music[ind_music]);
      }
    }
  return NULL;
}
/******************************************************************************/
int main() {

    srand(time(NULL));
    set_RPI(NORMAL);
    gaming=MENU;
    direction=DIR_INIT;
    dificultad=DIF1;
    pthread_t tid1,tid2,tid3,tid4,tid5;
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\nNo se pudo creal el mutex.\n");
        return 1;
    }
    if ( sem_init(&semaforo1, 0, 0) != 0 )
		{
			printf("\nNo se pudo crear el semaforo.\n");
      return 1;
		}
    if ( sem_init(&semaforo2, 0, 0) != 0 )
		{
			printf("\nNo se pudo crear el semaforo.\n");
      return 1;
		}
    pthread_create(&tid1,NULL,mainsnake,NULL);
    pthread_create(&tid2,NULL,mymenu,NULL);
    pthread_create(&tid3,NULL,myjoystick,NULL);
    pthread_create(&tid4,NULL,snakebrain,NULL);
    pthread_create(&tid5,NULL,playmusic,NULL);
    pthread_join(tid1,NULL); //Thread del juego
    pthread_join(tid2,NULL); //Thread de los menus
    pthread_join(tid3,NULL); //Thread del joystick
    pthread_join(tid4,NULL); //Thread de los datos del juego
    pthread_join(tid5,NULL);
    return (EXIT_SUCCESS);
}

/*****************************************************************************/
/*                       FUNCIONCES EXTRAS PARA LA RPI                       */
/*****************************************************************************/
int modul(int a)
{
  if(a>=0)
  return  (a);

  else
  return (-a);
}

void* printfile(char* nombre)
{
    char line[256];
    FILE* puntero=fopen(nombre, "r");
    if(puntero==NULL)
    {
          printf("No se pudo abrir el archivo.\n");

    }
    else
    {
          while(fgets(line,sizeof(line),puntero) != NULL)
          {
              printfrase(line);
          }
    }
    fclose(puntero);
    return(puntero);
}

char changedir(jcoord_t joy_coordinates2)
{
        char mydirection;
  //  Convert analog to digital based on simetrical threshold for both axis
  //  (-) -----------||dead zone||----------- (+)     <-- Joystcick position
  //Solo se elige una direccion entre la X y la Y
        if(modul(joy_coordinates2.x)>modul(joy_coordinates2.y))
        {
            if((joy_coordinates2.x > JOY_THRESHOLD))
                  mydirection=KEY_RIGHT;
            else if((joy_coordinates2.x < -JOY_THRESHOLD))
                  mydirection=KEY_LEFT;
            else//Implica que estamos dentro del THRESHOLD
                  mydirection=0;
        }
        else if(modul(joy_coordinates2.x)<modul(joy_coordinates2.y))
        {
            if((joy_coordinates2.y > JOY_THRESHOLD))
                  mydirection=KEY_DOWN;
            else if((joy_coordinates2.y < -JOY_THRESHOLD))
                  mydirection=KEY_UP;
            else//Implica que estamos dentro del THRESHOLD
                  mydirection=0;
        }
        else //Implica que ambos valores son iguales (¿Es posible?)
            mydirection=0;

    return mydirection;
}
/*SI QUERES HACER MUTEX:
pthread_mutex_lock(&lock);
...
pthread_mutex_unlock(&lock);
*/
void* set_RPI(int position)
{
  joy_init();					// Initialize Joystick Hardware
  display_init();				// Initialize Display Hardware
  if(position==NORMAL || position==ROTATE)
  {


    if(position==NORMAL)
    {
      set_joy_axis(JOY_NORMAL);
      set_display_axis(NORMAL);
      printf("set NORMAL\n");
    }
    else
    {
      set_joy_axis(JOY_ROTATE);
      set_display_axis(ROTATE);
      printf("set ROTATE");
    }
    set_joy_direction(J_INV_FALSE,J_INV_TRUE);// Invert Y joystick axis direction

    return NULL;
  }
  else
    return (void*)1;

}

void playsong(char* song)
{
  stop_sound();//Detenemos la musica si se esta reproduciendo
  if(player_status()==READY || player_status()==FINISHED)
	{

		printf(GREEN_TEXT "Playing: %s \n",song); 	printf(WHITE_TEXT);

		set_file_to_play(song);      // Load file

		play_sound(); 					  // Play File (Non Blocking)


	}
	else
	{

		printf(RED_TEXT "No se inicializo la MUSICA\n" WHITE_TEXT );

		exit(-1);
	}
}
