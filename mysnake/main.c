/*******************************************************************************
 * Liberías utilizadas
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h> 
#include <allegro5/allegro_audio.h> 
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "schlange.h"
#include "common_operations.h"
/*******************************************************************************
 * Definiciones
 ******************************************************************************/
#define FPS			5.0
#define CUADRADITO_SIZE         32
#define SCREEN_W		CUADRADITO_SIZE*27//864
#define SCREEN_H		CUADRADITO_SIZE*18//576
#define CUADRADITO_MIDDLE       CUADRADITO_SIZE/2
#define MAX_SONGS               4
#define S_COLOR1 al_color_name("green") //mascara color serpiente
#define S_COLOR2 al_color_name("slategray") //mascara color serpiente
#define S_COLOR3 al_color_name("maroon") //mascara color serpiente
#define A_COLOR al_color_name("red") //mascara color manzana
#define OFFCOLOR al_color_name("black") //Mascara color apagado
#define FLOOR_1 al_color_name("lawngreen") //mascara color
#define FLOOR_2 al_color_name("silver") //mascara color
#define FLOOR_3 al_color_name("darkorange") //mascara color
#define WHITE al_color_name("white")
#define YELLOW al_color_name("gold")

enum MYKEYS {
	KEY_A, KEY_W, KEY_S, KEY_D, KEY_Q, KEY_H, KEY_C, KEY_E, KEY_P//arrow keys
};

enum MYSTATE {
    MENU, PAUSA, PLAY, DIF, NAME
};
/*
 * 
 */
static body_t* snake;  //Puntero a la serpiente
static body_t* apple;  //Puntero a la manzana
static int* score; //Puntero al puntaje
static char dificultad;
static char name[5]={'A','A','A','A','\0'};
static char ind_let;
static ALLEGRO_DISPLAY *display = NULL;
static ALLEGRO_EVENT_QUEUE *event_queue = NULL;
static ALLEGRO_TIMER *timer = NULL;
static ALLEGRO_FONT *little_font = NULL;
static ALLEGRO_FONT *big_font = NULL;
static ALLEGRO_SAMPLE *samples[MAX_SONGS]={NULL,NULL,NULL,NULL};
static ALLEGRO_BITMAP  *main_menu   = NULL;
static ALLEGRO_BITMAP *pause_menu   = NULL;

static void print_menu(void);
static void change_sample(void);
void print_difs(char select);
void print_name(char select);
void* print_file(char*name);

int main(int argc, char** argv) 
{
 
    
    bool key_pressed[9] = { false, false, false, false, false, false, false, false, false};//Estado de teclas, true cuando esta apretada
    bool game_state[5] = { false, false, false, false, false }; //Estado del juego
    bool redraw = false;
    bool do_exit = false;
    bool oneclick = false;  //flag para evitar muchas lecturas apretando 1 vez
 
    if(!al_init()) {
        fprintf(stderr, "failed to initialize allegro!\n");
        return -1;
    }
    
    al_init_font_addon(); // initialize the font addon
    al_init_ttf_addon();// initialize the ttf (True Type Font) addon
    
    if(!al_install_keyboard()) {
        fprintf(stderr, "failed to initialize the keyboard!\n");
        return -1;
    }
    
    if(!al_install_audio()){
        fprintf(stderr, "failed to initialize audio!\n");
        return -1;
    }
    
    little_font = al_load_ttf_font("phil_lyrics.TTF",CUADRADITO_SIZE,0);
    if (!little_font){
        fprintf(stderr, "Could not load 'phil_lyrics.TTF'.\n");
        return -1;
    }
    
    big_font = al_load_ttf_font("phil_lyrics.TTF",CUADRADITO_SIZE*1.6,0);
    if (!big_font){
        fprintf(stderr, "Could not load 'phil_lyrics.TTF'.\n");
        return -1;
    }
    
    if(!al_init_acodec_addon()){
        fprintf(stderr, "failed to initialize audio codecs!\n");
        return -1;
    }
    
    if (!al_reserve_samples(2)){
        fprintf(stderr, "failed to reserve samples!\n");
        return -1;
    }
    
    samples[0] = al_load_sample( "SaintSeiya.wav" );
    if (!samples[0]){
        printf( "Audio clip sample not loaded!\n" ); 
        return -1;
    }
    samples[1] = al_load_sample( "Megaman 2.wav" );
    if (!samples[1]){
        printf( "Audio clip sample not loaded!\n" ); 
        return -1;
    }
    samples[2] = al_load_sample( "Tetris.wav");
    if (!samples[2]){
        printf( "Audio clip sample not loaded!\n" ); 
        return -1;
    }
    samples[3] = al_load_sample( "Zelda.wav" );
    if (!samples[1]){
        printf( "Audio clip sample not loaded!\n" ); 
        return -1;
    }
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
        fprintf(stderr, "failed to create timer!\n");
        return -1;
    }
    
    else if( !al_init_image_addon())
    {
	fprintf(stderr,"Unable to start image addon \n"); 
	al_uninstall_system();
	return -1;
    }
    
    main_menu = al_load_bitmap("Menu.png");
    if(!main_menu) 
    {
	al_destroy_timer(timer);
        al_shutdown_image_addon();
        fprintf(stderr, "failed to load image !\n");
        return 0;
    }
    pause_menu = al_load_bitmap("Pause_menu.png");
    if(!pause_menu) 
    {
	al_destroy_timer(timer);
        al_shutdown_image_addon();
        fprintf(stderr, "failed to load image !\n");
        return 0;
    }
    
    if(!al_init_primitives_addon()){
        fprintf(stderr, "failed to initialize primitives!\n");
        al_uninstall_audio();
        al_destroy_timer(timer);
        al_shutdown_image_addon();
        return -1;
   }
    display = al_create_display(SCREEN_W,SCREEN_H);
    if(!display) {
        fprintf(stderr, "failed to create display!\n");
        al_destroy_timer(timer);
        al_shutdown_primitives_addon();
        al_shutdown_image_addon();
        return -1;
    }
    
    
    event_queue = al_create_event_queue();
    if(!event_queue) {
	fprintf(stderr, "failed to create event_queue!\n");
	al_shutdown_primitives_addon();
	al_destroy_display(display);
	al_destroy_timer(timer);
	al_shutdown_image_addon();
        return -1;
    }
    
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source()); //REGISTRAMOS EL TECLADO
    
    al_play_sample(samples[0], 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);
    al_clear_to_color(WHITE);
    al_flip_display();
    al_start_timer(timer);
    game_state[MENU]=true;//Empezamos en el menu
    
    while(!do_exit)
    {
        ALLEGRO_EVENT ev;
        if(al_get_next_event(event_queue, &ev)) //Toma un evento de la cola, VER RETURN EN DOCUMENT.
        {
            if(ev.type == ALLEGRO_EVENT_TIMER)  //entra cada 1/FPS segundos
            {
                if(key_pressed[KEY_A] && (!oneclick)) 
                {
                    oneclick=true;
                    if(game_state[PLAY]==true)
                    {   
                        new_direction(KEY_LEFT);
                    }
                    else if(game_state[NAME]==true)
                    {
                        --ind_let;
                    }
                }
                else if(key_pressed[KEY_W] && (!oneclick)) 
                {
                    oneclick=true;
                    if(game_state[PLAY]== true)
                    {
                        new_direction(KEY_UP);
                    }
                    else if(game_state[DIF]== true)
                    {
                        ++dificultad;
                    }
                    else if(game_state[NAME]== true)
                    {
                        ++name[ind_let-1];
                    }
                }
                else if(key_pressed[KEY_S] && (!oneclick)) 
                {
                    oneclick=true;
                    if(game_state[PLAY]== true)
                    {
                        new_direction(KEY_DOWN);
                    }
                    else if(game_state[DIF]== true)
                    {
                        --dificultad;
                    }
                    else if(game_state[NAME]== true)
                    {
                        --name[ind_let-1];
                    }
                }
                else if(key_pressed[KEY_D] && (!oneclick)) 
                {
                    oneclick=true;
                    if(game_state[PLAY]==true)
                    {   
                        new_direction(KEY_RIGHT);
                    }
                    else if(game_state[NAME]== true)
                    {
                        ++ind_let;
                    }
                }
                else if(key_pressed[KEY_Q] && (!oneclick)) 
                {
                    oneclick=true;
                    if(game_state[PLAY]== true)//Se quiere ir al menu
                    {
                        game_state[PLAY] = false;
                        game_state[MENU] = true;
                        al_set_timer_speed(timer,1/FPS);
                    }
                    else if(game_state[DIF]== true)
                    {
                        game_state[DIF]=false;
                        game_state[MENU]=true;
                    }
                    else if(game_state[MENU]== true)//Se quiere salir
                    {
                        do_exit = true;
                    }
                }
                else if(key_pressed[KEY_H] && (!oneclick)) 
                {
                    oneclick=true;
                    if(game_state[MENU]== true)//Se quiere ver los HIGHSCORES
                    {
                        print_file("score.txt");
                        al_rest(2.5);
                    }
                    
                }
                else if(key_pressed[KEY_C] && (!oneclick) && game_state[MENU]== true) 
                {
                    oneclick=true;
                    change_sample();
                    
                }
                else if(key_pressed[KEY_E] && (!oneclick))
                {
                    oneclick=true;
                    if(game_state[MENU]== true)
                    {
                        game_state[MENU]= false;
                        game_state[DIF]= true;
                        dificultad=DIF1;//Seteo la dificultad
                    }
                    else if(game_state[DIF]== true)
                    {
                        game_state[DIF]= false;
                        game_state[PLAY]= true;
                        score=initsnake(dificultad);
                        al_set_timer_speed(timer,1/(FPS*dificultad));
                    }
                    else if(game_state[NAME]==true)
                    {
                        game_state[NAME]=false;
                        game_state[MENU]=true;
                        writescore("score.txt",name,*score);
                        print_file("score.txt");
                        al_rest(2.5);
                    }
                }
                else if(key_pressed[KEY_P] && (!oneclick))
                {
                    oneclick=true;
                    if(game_state[PLAY]== true)
                    {
                        game_state[PAUSA]=true;
                        game_state[PLAY]=false;
                    }
                    else if(game_state[PAUSA]== true)
                    {
                        game_state[PAUSA]=false;
                        game_state[PLAY]=true;
                    }
                }
                redraw = true;
            }
            else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) 
				do_exit = true;

            else if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                switch(ev.keyboard.keycode)     //activa indicador de tecla correspondiente
                {
                        case ALLEGRO_KEY_A:
                                key_pressed[KEY_A] = true;
                                break;
                        case ALLEGRO_KEY_W:
                                key_pressed[KEY_W] = true;
                                break;
                        case ALLEGRO_KEY_S: 
                                key_pressed[KEY_S] = true;
                                break;
                        case ALLEGRO_KEY_D:
                                key_pressed[KEY_D] = true;
                                break;
                        case ALLEGRO_KEY_Q:
                                key_pressed[KEY_Q] = true;
                                break;
                        case ALLEGRO_KEY_H:
                                key_pressed[KEY_H] = true;
                                break;
                        case ALLEGRO_KEY_C:
                                key_pressed[KEY_C] = true;
                                break;
                        case ALLEGRO_KEY_E:
                                key_pressed[KEY_E] = true;
                                break;
                        case ALLEGRO_KEY_P:
                                key_pressed[KEY_P] = true;
                                break;
                }
            }
            else if(ev.type == ALLEGRO_EVENT_KEY_UP)    //soltar tecla
            {
                switch(ev.keyboard.keycode)   //desactiva indicador
                {
                        case ALLEGRO_KEY_A:
                                key_pressed[KEY_A] = false;
                                break;
                        case ALLEGRO_KEY_W:
                                key_pressed[KEY_W] = false;
                                break;
                        case ALLEGRO_KEY_S: 
                                key_pressed[KEY_S] = false;
                                break;
                        case ALLEGRO_KEY_D:
                                key_pressed[KEY_D] = false;
                                break;
                        case ALLEGRO_KEY_Q:
                                key_pressed[KEY_Q] = false;
                                break;
                        case ALLEGRO_KEY_H:
                                key_pressed[KEY_H] = false;
                                break;
                        case ALLEGRO_KEY_C:
                                key_pressed[KEY_C] = false;
                                break;
                        case ALLEGRO_KEY_E:
                                key_pressed[KEY_E] = false;
                                break;
                        case ALLEGRO_KEY_P:
                                key_pressed[KEY_P] = false;
                                break;
                }
                oneclick=false; //habilita nuevos imputs
            }
            
            if(redraw && al_is_event_queue_empty(event_queue)) //actualizar
            {   
                redraw = false; //resetea flag
                
                if(game_state[PLAY]== true)
                {
                    body_t* applechk;
                    ALLEGRO_COLOR colors[3]={S_COLOR1,S_COLOR2,S_COLOR3};
                    ALLEGRO_COLOR floors[3]={FLOOR_1,FLOOR_2,FLOOR_3};
                    ALLEGRO_COLOR treats[3]={A_COLOR,S_COLOR1,YELLOW};
                    al_clear_to_color(floors[dificultad-1]);
                    al_draw_rectangle(CUADRADITO_MIDDLE,CUADRADITO_MIDDLE,CUADRADITO_SIZE*17+CUADRADITO_MIDDLE,SCREEN_H-CUADRADITO_MIDDLE,OFFCOLOR,CUADRADITO_SIZE);
                    al_draw_textf(little_font,OFFCOLOR,CUADRADITO_SIZE*18+CUADRADITO_MIDDLE,0,0,"SCORE:%d",*score);
                    al_draw_textf(little_font,OFFCOLOR,CUADRADITO_SIZE*18+CUADRADITO_MIDDLE,CUADRADITO_SIZE*6,0,"W->UP");
                    al_draw_textf(little_font,OFFCOLOR,CUADRADITO_SIZE*18+CUADRADITO_MIDDLE,CUADRADITO_SIZE*8,0,"A->LEFT");
                    al_draw_textf(little_font,OFFCOLOR,CUADRADITO_SIZE*18+CUADRADITO_MIDDLE,CUADRADITO_SIZE*10,0,"S->DOWN");
                    al_draw_textf(little_font,OFFCOLOR,CUADRADITO_SIZE*18+CUADRADITO_MIDDLE,CUADRADITO_SIZE*12,0,"D->LEFT");
                    al_draw_textf(little_font,OFFCOLOR,CUADRADITO_SIZE*18+CUADRADITO_MIDDLE,CUADRADITO_SIZE*14,0,"P->PAUSE");
                    al_draw_textf(little_font,OFFCOLOR,CUADRADITO_SIZE*18+CUADRADITO_MIDDLE,CUADRADITO_SIZE*16,0,"Q->EXIT");
                    
                    applechk=treat();
                    if(applechk)
                    {
                        apple=applechk;
                    }
                    al_draw_filled_circle(CUADRADITO_MIDDLE + CUADRADITO_SIZE*(get_position(apple,'x')+1),CUADRADITO_MIDDLE + CUADRADITO_SIZE*(get_position(apple,'y')+1), CUADRADITO_MIDDLE, treats[dificultad-1]);
                    snake=moove();
                    if(snake)
                    {
                        while(snake->cola!=NULL)
                        {//Imprimimos el CUERPO
                            al_draw_filled_circle(CUADRADITO_MIDDLE + CUADRADITO_SIZE*(get_position(snake,'x')+1),CUADRADITO_MIDDLE + CUADRADITO_SIZE*(get_position(snake,'y')+1), CUADRADITO_MIDDLE,colors[dificultad-1]);
                            snake=snake->cola;
                        }//Imprimimos la COLA
                        al_draw_filled_circle(CUADRADITO_MIDDLE + CUADRADITO_SIZE*(get_position(snake,'x')+1),CUADRADITO_MIDDLE + CUADRADITO_SIZE*(get_position(snake,'y')+1), CUADRADITO_MIDDLE, colors[dificultad-1]);
                        al_flip_display();
                    }
                    else
                    {
                        char position;
                        al_clear_to_color(WHITE);
                        al_draw_textf(big_font,OFFCOLOR,CUADRADITO_SIZE*8,CUADRADITO_SIZE*7,0,"SCORE %d",*score);
                        al_flip_display();
                        al_rest(2.5);
                        position=chkscore("score.txt",*score);
                        ind_let=1;//Por si acaso
                        game_state[PLAY]=false;
                        al_set_timer_speed(timer,1/FPS);//Volvemos a la normalidad
                        
                        if(position&&position!=LOOSER)
                            game_state[NAME]=true;
                        else
                            game_state[MENU]=true;
                    }
                }
                else if(game_state[MENU]==true)
                {
                    print_menu();
                }
                else if(game_state[PAUSA]==true)
                {
                    al_draw_textf(big_font,OFFCOLOR,CUADRADITO_SIZE*5,CUADRADITO_SIZE*5,0,"PAUSE");
                    al_flip_display();
                }
                else if(game_state[DIF]== true)
                {
                    if(dificultad>DIF3)
                        dificultad=DIF3;
                    if(dificultad<DIF1)
                        dificultad=DIF1;
                    print_difs(dificultad);
                }
                else if(game_state[NAME]==true)
                {
                    if(ind_let>4)
                        ind_let=4;
                    else if(ind_let<1)
                        ind_let=1;
                    
                    if(name[ind_let-1]>'Z')
                        name[ind_let-1]='Z';
                    else if(name[ind_let-1]<'A')
                        name[ind_let-1]='A';
                    
                    print_name(ind_let);
                }
            }
        }
    }
    al_shutdown_primitives_addon();
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_uninstall_audio();
    al_destroy_sample(samples[0]);
    al_destroy_sample(samples[1]);
    al_shutdown_image_addon();
    
    return (EXIT_SUCCESS);
}
void change_sample(void)
{//Reproduce toda la musica y tiene la opcion de muteo
    static char ind_music=0;
    ++ind_music;
    al_stop_samples();
    if(ind_music>MAX_SONGS)
        ind_music=0;
    if(ind_music<MAX_SONGS)
        al_play_sample(samples[ind_music], 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);
}
void print_menu(void)
{
    al_clear_to_color(WHITE);   //limpio backbuffer
    
    //Dibujo las letras del menu
    al_draw_bitmap(main_menu,0,0,0);
    al_draw_textf(big_font,OFFCOLOR,CUADRADITO_SIZE*10,CUADRADITO_SIZE,0,"SNAKE");        
    al_draw_textf(little_font,OFFCOLOR,CUADRADITO_SIZE,CUADRADITO_SIZE*7,0,"Press 'E' to start");
    al_draw_textf(little_font,OFFCOLOR,CUADRADITO_SIZE,CUADRADITO_SIZE*9,0,"Press 'H' for highscores");
    al_draw_textf(little_font,OFFCOLOR,CUADRADITO_SIZE,CUADRADITO_SIZE*11,0,"Press 'C' to change the music");
    al_draw_textf(little_font,OFFCOLOR,CUADRADITO_SIZE,CUADRADITO_SIZE*13,0,"Press 'Q' to quit");
    al_flip_display();  
}
void print_difs(char select)
{
    ALLEGRO_COLOR colors[3];
    colors[0]=OFFCOLOR;
    colors[1]=OFFCOLOR;
    colors[2]=OFFCOLOR;
    colors[select-1]=S_COLOR1;
    al_draw_bitmap(pause_menu,0,0,0);
    
    //Dibujo las distintas dificultades
    al_draw_textf(big_font,colors[2],CUADRADITO_SIZE,CUADRADITO_SIZE*2,0,"Hard");
    al_draw_textf(big_font,colors[1],CUADRADITO_SIZE,CUADRADITO_SIZE*8,0,"Normal");
    al_draw_textf(big_font,colors[0],CUADRADITO_SIZE,CUADRADITO_SIZE*14,0,"Easy");
    al_flip_display();
}
void print_name(char select)
{
    ALLEGRO_COLOR colors[4];
    ALLEGRO_COLOR floors[3]={FLOOR_1,FLOOR_2,FLOOR_3};
    colors[0]=OFFCOLOR;
    colors[1]=OFFCOLOR;
    colors[2]=OFFCOLOR;
    colors[3]=OFFCOLOR;
    colors[select-1]=WHITE;
    al_clear_to_color(floors[dificultad-1]);   //limpio backbuffer
    
    //Dibujo el nombre
    al_draw_textf(big_font,OFFCOLOR,CUADRADITO_SIZE*2,CUADRADITO_SIZE*4,0,"Choose your name:");
    al_draw_textf(big_font,colors[0],CUADRADITO_SIZE*4,CUADRADITO_SIZE*8,0,"%c",name[0]);
    al_draw_textf(big_font,colors[1],CUADRADITO_SIZE*8,CUADRADITO_SIZE*8,0,"%c",name[1]);
    al_draw_textf(big_font,colors[2],CUADRADITO_SIZE*12,CUADRADITO_SIZE*8,0,"%c",name[2]);
    al_draw_textf(big_font,colors[3],CUADRADITO_SIZE*15,CUADRADITO_SIZE*8,0,"%c",name[3]);
    al_flip_display();
    
}
void* print_file(char*name)
{
    al_draw_bitmap(pause_menu,0,0,0);
    float x=CUADRADITO_SIZE*4;
    char line[256];
    FILE* puntero=fopen(name, "r");
    if(puntero==NULL)
    {
          printf("No se pudo abrir el archivo.\n");

    }
    else
    {
          while(fgets(line,sizeof(line),puntero) != NULL)
          {
              char c;
              for(c=0;line[c]!='\n';++c);
              line[c]=' ';//Saco el enter
              al_draw_textf(big_font,OFFCOLOR,CUADRADITO_SIZE*2,x,0,"%s",line);
              x+=CUADRADITO_SIZE*1.6*2;   
          }
    }
    al_flip_display();
    fclose(puntero);
    return(puntero);
}