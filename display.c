#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include<pthread.h>
#include "termlib.h"
#include "joydrv.h"
#include "disdrv.h"
#include "schlange.h"

#define JOY_THRESHOLD 40     // 10-100 Joystick threshold (sensitivity)
int modul(int a);	//Funcion que devuelve el modulo de un numero
int main()
{
  FILE* menu; //Puntero al archivo del menu
  jcoord_t joy_coordinates1;  //Variable para los valores del joystick
  jswitch_t joy_switch1=J_NOPRESS; //Variable para el switcher
  char c, direction=KEY_RIGHT; //Varable que indica la direccion de la serpiente
  int i,j;
  body_t* snake;  //Puntero a la serpiente
  body_t* apple;  //Puntero a la manzana
  int* score;

  clrscr();

  joy_init();					// Initialize Joystick Hardware
	display_init();				// Initialize Display Hardware


	set_joy_axis(JOY_ROTATE);	// Set joystick axis orientaton

	set_display_axis(NORMAL);	// Set display axis orientation

	set_joy_direction(J_INV_TRUE,J_INV_TRUE); // Invert y joystick axis direction

  for(i=0;i<16;i++)			// All Dots on (write to display buffer)
		for(j=0;j<16;j++)
			display_write(i,j,D_ON);

	display_update();			// Send display buffer to display

  menu=fopen("menu.txt", "r");
  if(menu==NULL)
  {
        printf("No se pudo abrir el menu.\n");

  }
  else
  {
        while((c=getc(menu)) != EOF)
        {
              if(feof(menu))
                  break;
              putchar(c);
        }
  }
  fclose(menu);


/*************** Wait until joystick switch is presed to start ***************/

	while(joy_switch1==J_NOPRESS)
	{
		joystick_update();						 // Read joystick Hardware
		joy_switch1=joystick_get_switch_value(); // And get switch value
	}

  display_clear();			// Clears Display buffer and Hardware Display

  score=initsnake(2); //Creamos la serpiente
  snake=getbody();  //Conseguimos la ubicacion de la serpiente
/******************************************************************************/
  forever
  {
      usleep(200000);
      apple=treat();
      if(apple)
      {
          display_write(apple->x,apple->y,D_ON);
          printf("SCORE = %d\n",*score);
      }

    	joystick_update();         // Read joystick Hardware

    	joy_coordinates1=joystick_get_coord();  //And get coordinate values

    //  Convert analog to digital based on simetrical threshold for both axis
    //  (-) -----------||dead zone||----------- (+)     <-- Joystcick position
    //  Advance forward or backward (Meaning: || --> Thresholds , ----- active zone)

      if(modul(joy_coordinates1.x)>modul(joy_coordinates1.y)) //Solo se elige una direccion
            joy_coordinates1.y=0;
      else if(modul(joy_coordinates1.x)<modul(joy_coordinates1.y))
            joy_coordinates1.x=0;
    //Debemos verificar que este fuera de la dead y distinto a la direccion actual
      if((joy_coordinates1.x > JOY_THRESHOLD)&&(direction!=KEY_RIGHT))
    				direction=KEY_LEFT;
    	else if((joy_coordinates1.x < -JOY_THRESHOLD)&&(direction!=KEY_LEFT))
    				direction=KEY_RIGHT;

    	if((joy_coordinates1.y > JOY_THRESHOLD)&&(direction!=KEY_UP))
    				direction=KEY_DOWN;
    	else if((joy_coordinates1.y < -JOY_THRESHOLD)&&(direction!=KEY_DOWN))
    				direction=KEY_UP;

      snake=getbody();
      direction=moove(direction);
      if(direction)
      {
            for(;snake->state==ALIVE;++snake)
            {
              display_write(snake->x,snake->y,D_ON);
            }
            --snake;

            display_update();					// Reflect changes on display matrix
            display_write(snake->x,snake->y,D_OFF);
      }
      else
      {
            printf("Juego terminado\n");
      }
  }
}

int modul(int a)
{
  if(a>=0)
  return  (a);

  else
  return (-a);
}
