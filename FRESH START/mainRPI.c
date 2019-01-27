#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include<pthread.h>
#include <semaphore.h>
#include "termlib.h"
#include "joydrv.h"
#include "disdrv.h"
#include "schlange.h"

typedef enum {END,MENU,PLAY} actions_t;
#define JOY_THRESHOLD 40     // 10-100 Joystick threshold (sensitivity)
int modul(int a);	//Funcion que devuelve el modulo de un numero
void* printfile(char* nombre); //Funcion que imprime un archivo (error:NULL)
char changedir(jcoord_t joy_coordinates2,char mydirection);//SET DIRECTION
static void charge_matrix(void)


static char* difnames[3]={"Facil  ","Normal ","Dificil"};
static jcoord_t joy_coordinates1;  //Variable para los valores del joystick
static jswitch_t joy_switch1=J_NOPRESS; //Variable para el switcher
static char dificultad=FACIL,direction=0; //Varable que indica la direccion de la serpiente
static int i,j;
static body_t** snake;  //Puntero a la serpiente
static body_t** apple;  //Puntero a la manzana
static int* score;
static pthread_mutex_t lock;
static sem_t  semaforo; //semaforo del juego


void* mainsnake()
{
  snake=initsnake(dificultad); //Creamos la serpiente y seteamos la dificultad
  sem_wait(&semaforo);
  printf("Hay algun problema?");
  while(juego) //Si se termino el juego, no se sigue haciendo esto
  {
//      if(pausa) //Debemos esperar ya que algo mas importante esta pasando
//          while(pausa);

      usleep(200000/dificultad);
      apple=treat();
      if(apple)
      {
          display_write(GET_X(),GET_Y(),D_ON);
          printf("SCORE = %d\n",*score);
      }
      direction=changedir(joy_coordinates1,direction);

      snake=getbody(); //debemos reiniciarla porque el puntero se ha movido
      direction=moove(direction);

      if(direction)
      {
            for(;(*snake)->state==ALIVE;++snake)
            {
              display_write(snake->x,snake->y,D_ON);
            }
            --snake; //atrazamos el puntero para borrar el ultimo LED

            display_update();					// Actualizamos el display
            display_write(snake->x,snake->y,D_OFF);
      }
      else
      {
            printf("Juego terminado\n");
            juego=0;
      }
  }
    return NULL;
}

void* mymenu()
{
    printfile("menu.txt");
    printf("Seleccione una dificultad: Se modifica horizontalmente\n");
    printf("%s",difnames[dificultad-1]);
    while(joy_switch1==J_NOPRESS)
    {
        usleep(150000);
        direction=changedir(joy_coordinates1,0);
//        printf("direccion:%d\tdificultad:%d\n",direction,dificultad);

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

          printf("%s",difnames[dificultad-1]);
          direction=0;
        }
    }
    display_clear();			// Clears Display buffer and Hardware Display
    direction=KEY_RIGHT;
    sem_post(&semaforo);
    return NULL;
}

void* myjoystick()
{
    clrscr();

    joy_init();					// Initialize Joystick Hardware
    display_init();				// Initialize Display Hardware


    set_joy_axis(JOY_ROTATE);	// Set joystick axis orientaton

    set_display_axis(NORMAL);	// Set display axis orientation

    set_joy_direction(J_INV_TRUE,J_INV_TRUE);// Invert y joystick axis direction

    for(i=0;i<16;i++)			// All Dots on (write to display buffer)
      for(j=0;j<16;j++)
        display_write(i,j,D_ON);
    display_update();			// Send display buffer to display

/*************** Wait until joystick switch is presed to start ***************/
/*
      while(joy_switch1==J_NOPRESS)
    	{
    		joystick_update();						 // Read joystick Hardware
    		joy_switch1=joystick_get_switch_value(); // And get switch value
    	}
*/
/******************************************************************************/
    forever{
      usleep(5000);
      joystick_update();         // Read joystick Hardware

      joy_coordinates1=joystick_get_coord();  //And get coordinate values
      joy_switch1=joystick_get_switch_value(); // And get switch value
      if(joy_switch1==J_PRESS)
      {
        if(pausa==0)
            pausa=1;
        else
            {
              pausa=0;
            }
      }
    }
    return NULL;
}

int main() {

    srand(time(NULL));
    pthread_t tid1,tid2,tid3,tid4;
    if ( sem_init(&semaforo, 0, 0) != 0 )
		{
			printf("No se pudo crear el semaforo.\n");
		}
    pthread_create(&tid1,NULL,mainsnake,NULL);
    pthread_create(&tid2,NULL,mymenu,NULL);
    pthread_create(&tid3,NULL,myjoystick,NULL);

    pthread_join(tid1,NULL); //Thread del juego
    pthread_join(tid2,NULL); //Thread de los menus
    pthread_join(tid3,NULL); //Thread del joystick

    pthread_mutex_destroy(&lock);

    return (EXIT_SUCCESS);
}

int modul(int a)
{
  if(a>=0)
  return  (a);

  else
  return (-a);
}
void* printfile(char* nombre)
{
    int c;
    FILE* puntero=fopen(nombre, "r");
    if(puntero==NULL)
    {
          printf("No se pudo abrir el archivo.\n");

    }
    else
    {
          while((c=getc(puntero)) != EOF)
          {
                if(feof(puntero))
                    break;
                putchar(c);
          }
    }
    fclose(puntero);
    return(puntero);
}

char changedir(jcoord_t joy_coordinates2,char mydirection)
{
  //  Convert analog to digital based on simetrical threshold for both axis
  //  (-) -----------||dead zone||----------- (+)     <-- Joystcick position
  //Solo se elige una direccion entre la X y la Y
        if(modul(joy_coordinates2.x)>modul(joy_coordinates2.y))
              joy_coordinates2.y=0;
        else if(modul(joy_coordinates2.x)<modul(joy_coordinates2.y))
              joy_coordinates2.x=0;
  //Debemos verificar que este fuera de la dead y distinto a la direccion actual
        if((joy_coordinates2.x > JOY_THRESHOLD)&&(mydirection!=KEY_RIGHT))
              mydirection=KEY_LEFT;
        else if((joy_coordinates2.x < -JOY_THRESHOLD)&&(mydirection!=KEY_LEFT))
              mydirection=KEY_RIGHT;

        if((joy_coordinates2.y > JOY_THRESHOLD)&&(mydirection!=KEY_UP))
              mydirection=KEY_DOWN;
        else if((joy_coordinates2.y < -JOY_THRESHOLD)&&(mydirection!=KEY_DOWN))
              mydirection=KEY_UP;

    return mydirection;
}
/*SI QUERES HACER MUTEX:
pthread_mutex_lock(&lock);
...
pthread_mutex_unlock(&lock);
*/

static void charge_matrix(void)
{
  int c1,c2;
  for(c1=0;c1<=MAX_X;++c1)
  {
      for(c2=0;c2<=MAX_Y;++c2)
      {
        display_write(c1,c2,matrix[c1][c2]);
      }
  }
  display_update();
}
