/*******************************************************************************
 * Liberías utilizadas
 ******************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h> 
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h> 
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdbool.h>


/*******************************************************************************
 * Definiciones
 ******************************************************************************/
#define SCREEN_MAX 738
#define SCREEN_W 738
#define SCREEN_H 738
#define SQUARE_SIZE 41

#define PLAY 1
#define MENU 2
#define PAUSE 6
#define GAME_OVER 4
#define END 5
#define HIGHSCORE 6
#define MUSIC 7
static char gaming;

static ALLEGRO_DISPLAY * display=NULL;
// Punteros a imagenes modo facil
static ALLEGRO_BITMAP *snake_easy=NULL; //puntero a imagen de serpiente
static ALLEGRO_BITMAP *fruit_easy=NULL;   //puntero a imagen de fruta
static ALLEGRO_BITMAP *back_easy=NULL;    //puntero a imagen de piso
static ALLEGRO_BITMAP *wall_easy=NULL;    //puntero a imagen de pared
// Punteros a imagenes modo normal
static ALLEGRO_BITMAP *snake_medium=NULL; //puntero a imagen de serpiente
static ALLEGRO_BITMAP *fruit_medium=NULL;   //puntero a imagen de fruta
static ALLEGRO_BITMAP *back_medium=NULL;    //puntero a imagen de piso
static ALLEGRO_BITMAP *wall_medium=NULL;    //puntero a imagen de pared
// Punteros a imagenes modo dificil
static ALLEGRO_BITMAP *snake_hard=NULL; //puntero a imagen de serpiente
static ALLEGRO_BITMAP *fruit_hard=NULL;   //puntero a imagen de fruta
static ALLEGRO_BITMAP *back_hard=NULL;    //puntero a imagen de piso
static ALLEGRO_BITMAP *wall_hard=NULL;    //puntero a imagen de pared
//Sigo estableciendo los punteros
static ALLEGRO_EVENT_QUEUE * event_queue= NULL;   //continuo incializando punteros para usar en imagnes
static ALLEGRO_FONT *font = NULL;
static ALLEGRO_FONT *title = NULL;
static ALLEGRO_COLOR txtcolor;
//Punteros a canciones y audios
static ALLEGRO_SAMPLE *main_theme=NULL;
static ALLEGRO_SAMPLE *music_SS=NULL;
static ALLEGRO_SAMPLE *music_T=NULL;
static ALLEGRO_SAMPLE *music_Z=NULL;        
static ALLEGRO_SAMPLE *music_MM=NULL;
static ALLEGRO_SAMPLE *snake_end =NULL;
//Fondos y menus
static ALLEGRO_BITMAP *main_menu=NULL;
static ALLEGRO_BITMAP *pause_menu=NULL;
        
int menu (void);
int pause (void);
int music (void);
int game_over (void);
/*******************************************************************************
 * Inicialización de allegro                                      
 ******************************************************************************/

int init_all(void)         
{
    
    // Inicializamos los ADDONS necesarios
    
    if(!al_init()) 
        { //Inicializamos allegro
                printf("failed to initialize allegro!\n");
                return 1;
        }
    
    if(!al_init_image_addon()) 
    {   // Instalamos el manejo de imágenes
        printf("failed to initialize image addon !\n");
        return 1;
    }
    al_init_font_addon();    
        // Instalamos el manejo de fuentes
    
    if(!al_init_ttf_addon())
    {   // Instalamos el manejo de textos
        printf("failed to initialize text addon !\n");
        return 1;
    }
    
    if(!al_install_keyboard()) 
    {   // Istalamos el teclado
	printf("failed to initialize the keyboard!\n");
	return 1;
    }
    
    if(!al_install_audio())
    {   // Instalamos el audio
      fprintf(stderr, "failed to initialize audio!\n");
      return -1;
    }
    if(!al_init_acodec_addon())
    {
      fprintf(stderr, "failed to initialize audio codecs!\n");
      return -1;
    }

    if (!al_reserve_samples(1))
    {
       fprintf(stderr, "failed to reserve samples!\n");
       return -1;
    }
    //Inicializamos imagenes y punteros correspondientes, 4 por cada modo
    
    /***************************************************************************
    * Imagenes modo facil                                     
    ***************************************************************************/
    snake_easy= al_load_bitmap("snake_easy.png");
    if(!snake_easy) 
    {
        printf("failed to load snake image !\n");
        return 1;
    }
    
  
     fruit_easy=al_load_bitmap("fruit_easy.png");  
    if(!fruit_easy) 
    {
        printf("failed to load fruit image !\n");
        al_destroy_bitmap(snake_easy);
        return 1;
    }
    back_easy=al_load_bitmap("grass_easy.png");
         
    if(!back_easy) 
    {
        printf("failed to load floor image !\n");
        al_destroy_bitmap(snake_easy);
        al_destroy_bitmap(fruit_easy);
        return 1;
    }
    
    wall_easy=al_load_bitmap("wall_easy.png");
    if(!wall_easy) 
    {
        printf("failed to load wall image !\n");
     
        al_destroy_bitmap(snake_easy);
        al_destroy_bitmap(fruit_easy);
        al_destroy_bitmap(back_easy);
        return 1;
    }
    /***************************************************************************
    * Imagenes modo normal                                    
    ***************************************************************************/
    snake_medium= al_load_bitmap("snake_medium.png");
    if(!snake_medium) 
    {
        printf("failed to load snake image !\n");
        al_destroy_bitmap(snake_easy);
        al_destroy_bitmap(fruit_easy);
        al_destroy_bitmap(back_easy);
        al_destroy_bitmap(wall_easy);
        return 1;
    }
    
  
     fruit_medium=al_load_bitmap("fruit_medium.png");  
    if(!fruit_medium) 
    {
        printf("failed to load fruit image !\n");
        al_destroy_bitmap(snake_easy);
        al_destroy_bitmap(fruit_easy);
        al_destroy_bitmap(back_easy);
        al_destroy_bitmap(wall_easy);
        al_destroy_bitmap(snake_medium);
        return 1;
    }
    back_medium=al_load_bitmap("grass_medium.png");
         
    if(!back_medium) 
    {
        printf("failed to load floor image !\n");
        al_destroy_bitmap(snake_easy);
        al_destroy_bitmap(fruit_easy);
        al_destroy_bitmap(back_easy);
        al_destroy_bitmap(wall_easy);
        al_destroy_bitmap(snake_medium);
        al_destroy_bitmap(fruit_medium);
        return 1;
    }
    
    wall_medium=al_load_bitmap("wall_medium.png");
    if(!wall_medium) 
    {
        printf("failed to load wall image !\n");
     
        al_destroy_bitmap(snake_easy);
        al_destroy_bitmap(fruit_easy);
        al_destroy_bitmap(back_easy);
        al_destroy_bitmap(wall_easy);
        al_destroy_bitmap(snake_medium);
        al_destroy_bitmap(fruit_medium);
        al_destroy_bitmap(back_medium);
        return 1;
    }
    /***************************************************************************
    * Imagenes modo dificil                                   
    ***************************************************************************/
    snake_hard= al_load_bitmap("snake_hard.png");
    if(!snake_hard) 
    {
        printf("failed to load snake image !\n");
        al_destroy_bitmap(snake_easy);
        al_destroy_bitmap(fruit_easy);
        al_destroy_bitmap(back_easy);
        al_destroy_bitmap(wall_easy);
        al_destroy_bitmap(snake_medium);
        al_destroy_bitmap(fruit_medium);
        al_destroy_bitmap(back_medium);
        al_destroy_bitmap(wall_medium);
        return 1;
    }
    
  
     fruit_hard=al_load_bitmap("fruit_hard.png");  
    if(!fruit_hard) 
    {
        printf("failed to load fruit image !\n");
        al_destroy_bitmap(snake_easy);
        al_destroy_bitmap(fruit_easy);
        al_destroy_bitmap(back_easy);
        al_destroy_bitmap(wall_easy);
        al_destroy_bitmap(snake_medium);
        al_destroy_bitmap(fruit_medium);
        al_destroy_bitmap(back_medium);
        al_destroy_bitmap(wall_medium);
        al_destroy_bitmap(snake_hard);
        return 1;
    }
    back_hard=al_load_bitmap("grass_hard.png");
         
    if(!back_hard) 
    {
        printf("failed to load floor image !\n");
        al_destroy_bitmap(snake_easy);
        al_destroy_bitmap(fruit_easy);
        al_destroy_bitmap(back_easy);
        al_destroy_bitmap(wall_easy);
        al_destroy_bitmap(snake_hard);
        al_destroy_bitmap(fruit_hard);
        return 1;
    }
    
    wall_hard=al_load_bitmap("wall_hard.png");
    if(!wall_hard) 
    {
        printf("failed to load wall image !\n");
     
        al_destroy_bitmap(snake_easy);
        al_destroy_bitmap(fruit_easy);
        al_destroy_bitmap(back_easy);
        al_destroy_bitmap(wall_easy);
        al_destroy_bitmap(snake_medium);
        al_destroy_bitmap(fruit_medium);
        al_destroy_bitmap(back_medium);
        al_destroy_bitmap(wall_medium);
        al_destroy_bitmap(snake_hard);
        al_destroy_bitmap(fruit_hard);
        al_destroy_bitmap(back_hard);
        return 1;
    }
    
    txtcolor= al_map_rgb(0,15,0); //defino el color del texto
    
    //Inicializo fuentes para el texto
    
    font=al_load_font("phil_lyrics.TTF",-30,0);
    if(!font) 
    {
        printf("failed to load font !\n");
        al_destroy_bitmap(snake_easy);
        al_destroy_bitmap(fruit_easy);
        al_destroy_bitmap(back_easy);
        al_destroy_bitmap(wall_easy);
        al_destroy_bitmap(snake_medium);
        al_destroy_bitmap(fruit_medium);
        al_destroy_bitmap(back_medium);
        al_destroy_bitmap(wall_medium);
        al_destroy_bitmap(snake_hard);
        al_destroy_bitmap(fruit_hard);
        al_destroy_bitmap(back_hard);
        al_destroy_bitmap(wall_hard);
        return 1;
    }
   title=al_load_font("phil_lyrics.TTF",-(SQUARE_SIZE*2),0);
    if(!font) 
    {
        printf("failed to load font !\n");
        al_destroy_bitmap(snake_easy);
        al_destroy_bitmap(fruit_easy);
        al_destroy_bitmap(back_easy);
        al_destroy_bitmap(wall_easy);
        al_destroy_bitmap(snake_medium);
        al_destroy_bitmap(fruit_medium);
        al_destroy_bitmap(back_medium);
        al_destroy_bitmap(wall_medium);
        al_destroy_bitmap(snake_hard);
        al_destroy_bitmap(fruit_hard);
        al_destroy_bitmap(back_hard);
        al_destroy_bitmap(wall_hard);
        al_destroy_font(font);
        return 1;
    }
    
    //Inicializo display
    display=al_create_display(SCREEN_W,SCREEN_H); // Inicializo display
    if(!display)  //pregunto si display se inicializo correctamente
    {
        al_destroy_bitmap(snake_easy);
        al_destroy_bitmap(fruit_easy);
        al_destroy_bitmap(back_easy);
        al_destroy_bitmap(wall_easy);
        al_destroy_bitmap(snake_medium);
        al_destroy_bitmap(fruit_medium);
        al_destroy_bitmap(back_medium);
        al_destroy_bitmap(wall_medium);
        al_destroy_bitmap(snake_hard);
        al_destroy_bitmap(fruit_hard);
        al_destroy_bitmap(back_hard);
        al_destroy_bitmap(wall_hard);
        al_destroy_font(font);
        al_destroy_font(title);
        printf("failed to create display!\n");
        return 1;
    }

    /***************************************************************************
    * Audio del juego                                  
    ***************************************************************************/
    
     music_SS = al_load_sample( "saint_seiya.wav" );
 
   if (!music_SS){
      printf( "Audio clip sample not loaded!\n" );
            al_destroy_bitmap(snake_easy);
            al_destroy_bitmap(fruit_easy);
            al_destroy_bitmap(back_easy);
            al_destroy_bitmap(wall_easy);
            al_destroy_bitmap(snake_medium);
            al_destroy_bitmap(fruit_medium);
            al_destroy_bitmap(back_medium);
            al_destroy_bitmap(wall_medium);
            al_destroy_bitmap(snake_hard);
            al_destroy_bitmap(fruit_hard);
            al_destroy_bitmap(back_hard);
            al_destroy_bitmap(wall_hard);
            al_destroy_font(font);
            al_destroy_font(title);
            al_destroy_display(display);
      return 1;
   }
   
   music_T = al_load_sample( "tetris.wav" );
   if (!music_T){
      printf( "Audio clip sample not loaded!\n" );
            al_destroy_bitmap(snake_easy);
            al_destroy_bitmap(fruit_easy);
            al_destroy_bitmap(back_easy);
            al_destroy_bitmap(wall_easy);
            al_destroy_bitmap(snake_medium);
            al_destroy_bitmap(fruit_medium);
            al_destroy_bitmap(back_medium);
            al_destroy_bitmap(wall_medium);
            al_destroy_bitmap(snake_hard);
            al_destroy_bitmap(fruit_hard);
            al_destroy_bitmap(back_hard);
            al_destroy_bitmap(wall_hard);
            al_destroy_font(font);
            al_destroy_font(title);
            al_destroy_display(display);
            al_destroy_sample (music_SS);
      return 1;
   }
   
   music_Z = al_load_sample( "zelda.wav" );
   if (!music_Z){
      printf( "Audio clip sample not loaded!\n" );
            al_destroy_bitmap(snake_easy);
            al_destroy_bitmap(fruit_easy);
            al_destroy_bitmap(back_easy);
            al_destroy_bitmap(wall_easy);
            al_destroy_bitmap(snake_medium);
            al_destroy_bitmap(fruit_medium);
            al_destroy_bitmap(back_medium);
            al_destroy_bitmap(wall_medium);
            al_destroy_bitmap(snake_hard);
            al_destroy_bitmap(fruit_hard);
            al_destroy_bitmap(back_hard);
            al_destroy_bitmap(wall_hard);
            al_destroy_font(font);
            al_destroy_font(title);
            al_destroy_display(display);
            al_destroy_sample (music_SS);
            al_destroy_sample (music_T);
      return 1;
   }
   music_MM = al_load_sample( "megaman.wav" );
   if (!music_MM){
      printf( "Audio clip sample not loaded!\n" );
            al_destroy_bitmap(snake_easy);
            al_destroy_bitmap(fruit_easy);
            al_destroy_bitmap(back_easy);
            al_destroy_bitmap(wall_easy);
            al_destroy_bitmap(snake_medium);
            al_destroy_bitmap(fruit_medium);
            al_destroy_bitmap(back_medium);
            al_destroy_bitmap(wall_medium);
            al_destroy_bitmap(snake_hard);
            al_destroy_bitmap(fruit_hard);
            al_destroy_bitmap(back_hard);
            al_destroy_bitmap(wall_hard);
            al_destroy_font(font);
            al_destroy_font(title);
            al_destroy_display(display);
            al_destroy_sample (music_SS);
            al_destroy_sample (music_T);
            al_destroy_sample (music_Z);
      return 1;
   }
   snake_end = al_load_sample( "snake.wav" );
   if (!snake_end){
      printf( "Audio clip sample not loaded!\n" );
            al_destroy_bitmap(snake_easy);
            al_destroy_bitmap(fruit_easy);
            al_destroy_bitmap(back_easy);
            al_destroy_bitmap(wall_easy);
            al_destroy_bitmap(snake_medium);
            al_destroy_bitmap(fruit_medium);
            al_destroy_bitmap(back_medium);
            al_destroy_bitmap(wall_medium);
            al_destroy_bitmap(snake_hard);
            al_destroy_bitmap(fruit_hard);
            al_destroy_bitmap(back_hard);
            al_destroy_bitmap(wall_hard);
            al_destroy_font(font);
            al_destroy_font(title);
            al_destroy_display(display);
            al_destroy_sample (music_SS);
            al_destroy_sample (music_T);
            al_destroy_sample (music_Z);
             al_destroy_sample (music_MM);
      return 1;
   }
   // Cargo los menus y fondos
   
   main_menu=al_load_bitmap("Menu.png");  
    if(!main_menu) 
    {
            al_destroy_bitmap(snake_easy);
            al_destroy_bitmap(fruit_easy);
            al_destroy_bitmap(back_easy);
            al_destroy_bitmap(wall_easy);
            al_destroy_bitmap(snake_medium);
            al_destroy_bitmap(fruit_medium);
            al_destroy_bitmap(back_medium);
            al_destroy_bitmap(wall_medium);
            al_destroy_bitmap(snake_hard);
            al_destroy_bitmap(fruit_hard);
            al_destroy_bitmap(back_hard);
            al_destroy_bitmap(wall_hard);
            al_destroy_font(font);
            al_destroy_font(title);
            al_destroy_display(display);
            al_destroy_sample (music_SS);
            al_destroy_sample (music_T);
            al_destroy_sample (music_Z);
            al_destroy_sample (music_MM);
            al_destroy_sample (snake_end);
            printf("failed to create menu image!\n");
            return 1;
    }
    pause_menu=al_load_bitmap("Pause_menu.png");  
    if(!pause_menu) 
     {
            al_destroy_bitmap(snake_easy);
            al_destroy_bitmap(fruit_easy);
            al_destroy_bitmap(back_easy);
            al_destroy_bitmap(wall_easy);
            al_destroy_bitmap(snake_medium);
            al_destroy_bitmap(fruit_medium);
            al_destroy_bitmap(back_medium);
            al_destroy_bitmap(wall_medium);
            al_destroy_bitmap(snake_hard);
            al_destroy_bitmap(fruit_hard);
            al_destroy_bitmap(back_hard);
            al_destroy_bitmap(wall_hard);
            al_destroy_font(font);
            al_destroy_font(title);
            al_destroy_display(display);
            al_destroy_sample (music_SS);
            al_destroy_sample (music_T);
            al_destroy_sample (music_Z);
            al_destroy_sample (music_MM);
            al_destroy_sample (snake_end);
            al_destroy_bitmap(main_menu);
            printf("failed to create pause image!\n");
            return 1;
	}
     
     
    //Inicializo cola de eventos
    event_queue = al_create_event_queue(); // inicializo cola de eventos
     if(!event_queue)  //pregunto si cola de eventos se inicializo correctamente
        {
            al_destroy_bitmap(snake_easy);
            al_destroy_bitmap(fruit_easy);
            al_destroy_bitmap(back_easy);
            al_destroy_bitmap(wall_easy);
            al_destroy_bitmap(snake_medium);
            al_destroy_bitmap(fruit_medium);
            al_destroy_bitmap(back_medium);
            al_destroy_bitmap(wall_medium);
            al_destroy_bitmap(snake_hard);
            al_destroy_bitmap(fruit_hard);
            al_destroy_bitmap(back_hard);
            al_destroy_bitmap(wall_hard);
            al_destroy_font(font);
            al_destroy_font(title);
            al_destroy_display(display);
            al_destroy_sample (music_SS);
            al_destroy_sample (music_T);
            al_destroy_sample (music_Z);
            al_destroy_sample (music_MM);
            al_destroy_sample (snake_end);
            al_destroy_bitmap(main_menu);
            al_destroy_bitmap(pause_menu);
            printf("failed to create event_queue!\n");
            return 1;
	}
    return 0;
}
/*******************************************************************************
 * The fool's world
 ******************************************************************************/
void main (void)
{
    
    init_all ();
    ALLEGRO_EVENT eve;
    int locker = 0;
    gaming = MENU;
    al_play_sample(music_Z, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);
    while (1)
    {   
        /*if(al_get_next_event(event_queue, &eve))
        {*/
            if (gaming == MENU)
            {
                
                while (gaming == MENU)
                {
                    menu ();
                    al_register_event_source(event_queue, al_get_keyboard_event_source()); 
                    if(eve.type == ALLEGRO_EVENT_KEY_CHAR) //si detecto que se presiono una tecla
                    {   
                        if(al_get_next_event(event_queue, &eve))
                        {
                            switch(eve.keyboard.keycode)     //evaluo que tecla, definiendo 'letra' a su respectivo valor char
                            {
                                case ALLEGRO_KEY_S:   
                                {
                                    gaming = PLAY;
                                    break;
                                }
                                case ALLEGRO_KEY_H:    
                                {
                                    gaming = HIGHSCORE;
                                    break;
                                }
                                case ALLEGRO_KEY_M:    
                                {
                                    gaming = MUSIC;
                                    break;
                                }
                                case ALLEGRO_KEY_Q:   
                                {
                                    gaming =END;
                                    break;
                                }
                                default:
                                {
                                    break;
                                }
                            }
                        }
                    }
                }

            }
            if (gaming == PAUSE)
            {
                while (gaming == PAUSE)
                {
                    pause ();
                    if(eve.type == ALLEGRO_EVENT_KEY_CHAR) //si detecto que se presiono una tecla
                    {

                        switch(eve.keyboard.keycode)     //evaluo que tecla, definiendo 'letra' a su respectivo valor char
                        {
                            case ALLEGRO_KEY_M:   
                            {
                                gaming = MENU;
                                break;
                            }
                            case ALLEGRO_KEY_R:    
                            {
                                gaming = PLAY;
                                break;
                            }
                            default:
                            {
                                break;
                            }
                        }                               
                    }
                }

            }
            if (gaming == PLAY)
            {
                menu ();
                if(eve.type == ALLEGRO_EVENT_KEY_CHAR) //si detecto que se presiono una tecla
                {

                    switch(eve.keyboard.keycode)     //evaluo que tecla, definiendo 'letra' a su respectivo valor char
                    {
                        case ALLEGRO_KEY_W:   
                        {
                            printf ("toque la w");
                            break;
                        }
                        case ALLEGRO_KEY_A:    
                        {
                            printf ("toque la a");
                            break;
                        }
                        case ALLEGRO_KEY_S:    
                        {
                            printf ("toque la s");
                            break;
                        }
                        case ALLEGRO_KEY_D:   
                        {
                            printf ("toque la d");
                            break;
                        }
                        case ALLEGRO_KEY_P:   
                        {
                            gaming =PAUSE;
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }                               
                }
            }
            if (gaming == MUSIC)
            {
                while (gaming == MUSIC)
                {
                    music ();
                    al_register_event_source(event_queue, al_get_keyboard_event_source()); 
                    if(eve.type == ALLEGRO_EVENT_KEY_CHAR) //si detecto que se presiono una tecla
                    {

                        switch(eve.keyboard.keycode)     //evaluo que tecla, definiendo 'letra' a su respectivo valor char
                        {
                            case ALLEGRO_KEY_M:   
                            {
                                main_theme = music_MM;
                                al_play_sample(music_MM, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);
                                break;
                            }
                            case ALLEGRO_KEY_Z:    
                            {
                                main_theme = music_Z;
                                al_play_sample(music_Z, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);
                                break;
                            }
                            case ALLEGRO_KEY_T:    
                            {
                                main_theme = music_T;
                                al_play_sample(music_T, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);
                                break;
                            }
                            case ALLEGRO_KEY_S:   
                            {
                                main_theme = music_SS;
                                al_play_sample(music_SS, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);
                                break;
                            }
                            case ALLEGRO_KEY_B:   
                            {
                                gaming =MENU;
                                break;
                            }
                            default:
                            {
                                break;
                            }
                        }                               
                    }
                }

            }
            if (gaming = GAME_OVER)
            {
                 
                game_over ();
                al_stop_samples();
                al_play_sample(snake_end, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);
                gaming = MENU;
                
                
            }
            
    }
    
}

/*******************************************************************************
 * Menu principal
 ******************************************************************************/
int menu (void)
{

    al_set_target_backbuffer(display);       //hago un backbuffer y lo pongo como target
    al_clear_to_color(al_map_rgb(0,0,0));   //limpio backbuffer
    
    //Dibujo las letras del menu
    al_draw_bitmap(main_menu,0,0,0);
    al_draw_textf(title,txtcolor,SQUARE_SIZE*8,0,0,"SNAKE");        
    al_draw_textf(font,txtcolor,SQUARE_SIZE,SQUARE_SIZE*7,0,"Press 'S' to start");
    al_draw_textf(font,txtcolor,SQUARE_SIZE,SQUARE_SIZE*9,0,"Press 'H' for highscores");
    al_draw_textf(font,txtcolor,SQUARE_SIZE,SQUARE_SIZE*11,0,"Press 'M' to change the music");
    al_draw_textf(font,txtcolor,SQUARE_SIZE,SQUARE_SIZE*13,0,"Press 'Q' to quit");
    al_flip_display();      //actualizo display con el backbuffer
    
    /* UN SOLO ALEGRO EVENT EN EL MAIN
     * MENU: SOLO IMPRIME MENU (LO MISMO PAUSA Y GAME OVER)
     * Variable GAMING Indica que se imprime
     * Dependiendo valor de la variable (IF y adentro metele un SWITCH)
     * CUANDO VEAS QUE ANDA MENU Y MUSICA, PONES UN TIMER QUE IMRPIMA 2 PALABRAS, DESDE AHI MENU PAUSA
     */
    
    
	return 0;
} 
/*******************************************************************************
 * Menu de cambio de musica
 ******************************************************************************/
int music (void)
{
   
    al_set_target_backbuffer(display);       //hago un backbuffer y lo pongo como target
    al_clear_to_color(al_map_rgb(0,0,0));   //limpio backbuffer
    
    //Dibujo las letras del menu
    al_draw_bitmap(pause_menu,0,0,0);
    al_draw_textf(title,txtcolor,SQUARE_SIZE*4,0,0,"MUSIC");        
    al_draw_textf(font,txtcolor,SQUARE_SIZE,SQUARE_SIZE*3,0,"'M' for Megaman theme");
    al_draw_textf(font,txtcolor,SQUARE_SIZE,SQUARE_SIZE*5,0,"'T' for Tetris theme");
    al_draw_textf(font,txtcolor,SQUARE_SIZE,SQUARE_SIZE*7,0,"'S' for Saint Seiya theme");
    al_draw_textf(font,txtcolor,SQUARE_SIZE,SQUARE_SIZE*9,0,"'Z' for Zelda theme");
    al_draw_textf(font,txtcolor,SQUARE_SIZE,SQUARE_SIZE*11,0,"'B' to go back to menu");
    al_flip_display();      //actualizo display con el backbuffer
    
    
    /* UN SOLO ALEGRO EVENT EN EL MAIN
     * MENU: SOLO IMPRIME MENU (LO MISMO PAUSA Y GAME OVER)
     * Variable GAMING Indica que se imprime
     * Dependiendo valor de la variable (IF y adentro metele un SWITCH)
     * CUANDO VEAS QUE ANDA MENU Y MUSICA, PONES UN TIMER QUE IMRPIMA 2 PALABRAS, DESDE AHI MENU PAUSA
     */
    
    return 0;
} 
/*******************************************************************************
 * Menu de pausa                                    
 ******************************************************************************/

int pause (void)
{
    ALLEGRO_EVENT Pausing;                   //declaro un evento de allegro
    al_set_target_backbuffer(display);       //hago un backbuffer y lo pongo como target
    al_clear_to_color(al_map_rgb(0,0,0));    //limpio backbuffer

    al_draw_bitmap(pause_menu,0,0,0);
    al_draw_textf(title,txtcolor,SQUARE_SIZE*4,0,0,"PAUSE");        
    /*
    al_draw_textf(font,txtcolor,SQUARE_SIZE,SQUARE_SIZE,0,"Score: %d",score);
    al_draw_textf(font,txtcolor,SQUARE_SIZE,SQUARE_SIZE*2,0,"Lifes: %d",lifes);
    */
    al_draw_textf(font,txtcolor,SQUARE_SIZE,SQUARE_SIZE*3,0,"Press 'C' to continue");
    al_draw_textf(font,txtcolor,SQUARE_SIZE,SQUARE_SIZE*5,0,"Press 'Q' to quit");
    al_flip_display();      //actualizo display con el backbuffer
    
}

/*******************************************************************************
 * GAME OVER                                     
 ******************************************************************************/
int game_over (void)
{
 
    al_set_target_backbuffer(display);       //hago un backbuffer y lo pongo como target
    al_clear_to_color(al_map_rgb(0,0,0));    //limpio backbuffer

    al_draw_bitmap(pause_menu,0,0,0);
    al_draw_textf(title,txtcolor,SQUARE_SIZE,SQUARE_SIZE*6,0,"GAME OVER");        

    al_flip_display();      //actualizo display con el backbuffer
    
}
