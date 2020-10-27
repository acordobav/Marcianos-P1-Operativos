#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>

#include "gamewindow.c"

//Dimensiones de pantalla
int SCREENWIDTH = 690;
int SCREENHEIGHT = 690;

//Funciones para dibujar en pantalla
void draw_menu(ALLEGRO_FONT *title, ALLEGRO_FONT *mode, ALLEGRO_BITMAP *stars);
void draw_algor(ALLEGRO_FONT *title, ALLEGRO_FONT *select, ALLEGRO_BITMAP *stars);
void draw_manual(ALLEGRO_FONT *subtitle, ALLEGRO_FONT *stat, ALLEGRO_FONT *number, int energy_alien, int regen_alien);
void draw_autom(ALLEGRO_FONT *subtitle, ALLEGRO_FONT *stat, ALLEGRO_FONT *number, ALLEGRO_BITMAP *stars, int num_alien, int energy_alien, int regen_alien);

void error(ALLEGRO_DISPLAY *display, int alien_id, int alien_energy, int alien_period);


int main()
{   
    srand(time(NULL)); // Inicializacion para numeros random, solo debe llamarse una vez
    walls = createMap();
    aliens = (Alien*) malloc(sizeof(*aliens)*maxAliens);
    pthread_mutex_init(&clock_mutex, NULL);
    pthread_cond_init(&clock_cond, NULL);
    pthread_mutex_init(&alienCountMutex, NULL);

    pthread_mutex_lock(&gameLoopMutex);
    gameLoop = 1;
    pthread_mutex_unlock(&gameLoopMutex);

    //Inicializar Allegro 5
    if(!al_init())
    {
        printf("Error al inicializar Allegro");
        exit(1);
    }

    //Crear el display para el menu
    ALLEGRO_DISPLAY *display = al_create_display(SCREENWIDTH, SCREENHEIGHT);
    al_set_window_position(display, 200, 100);
    al_set_window_title(display, "Proyecto 1 PSO");

    //Prueba
    error(display, 1, 1, 1);

    if(!display)
    {
        printf("Error al crear pantalla");
        exit(1);
    }

    //Inicializar modeos de Allegro 5
    al_init_font_addon();
    al_init_ttf_addon();
    al_install_keyboard();
    al_init_image_addon();

    //Crear las fuentes e imagenes del menu
    ALLEGRO_FONT *title = al_load_ttf_font("Orbitron-Bold.ttf", 36, ALLEGRO_TTF_MONOCHROME);
    ALLEGRO_FONT *select = al_load_ttf_font("Orbitron-Bold.ttf", 28, ALLEGRO_TTF_MONOCHROME);
    ALLEGRO_BITMAP *stars = al_load_bitmap("stars.jpg");

    //Crear cola de eventos del menu
    ALLEGRO_EVENT_QUEUE *event_plan = al_create_event_queue();
    al_register_event_source(event_plan, al_get_keyboard_event_source());

    //Variable del algoritmo seleccionado
    int algorithm;

    //Ventana del menu
    bool plan = false;
    while(!plan)
    {   
        //Dibujar textos e imagenes del menu
        draw_algor(title, select, stars);

        //Eventos del teclado del menu
        ALLEGRO_EVENT events2;
        al_wait_for_event(event_plan, &events2);
        if (events2.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch (events2.keyboard.keycode)
            {
            case ALLEGRO_KEY_X:
                exit(1);
                break;
            case ALLEGRO_KEY_1:
                algorithm = 1;
                plan = true;
                break;
            case ALLEGRO_KEY_2:
                algorithm = 2;
                plan = true;
                break;
            default:
                break;
            }
        }
        al_clear_to_color(al_map_rgb(0,0,0));
    }
    //Liberar memoria de fuentes y eventos
    al_destroy_font(title);
    al_destroy_font(select);
    al_destroy_event_queue(event_plan);

    //Crear las fuentes del modo de operacion
    ALLEGRO_FONT *text = al_load_ttf_font("Orbitron-Bold.ttf", 36, ALLEGRO_TTF_MONOCHROME);
    ALLEGRO_FONT *mode = al_load_ttf_font("Orbitron-Bold.ttf", 28, ALLEGRO_TTF_MONOCHROME);
    
    //Crear cola de eventos del modo de operacion
    ALLEGRO_EVENT_QUEUE *event_menu = al_create_event_queue();
    al_register_event_source(event_menu, al_get_keyboard_event_source());

    //Variable del modo de operacion seleccionado
    int modeop;

    //Ventana del modo de operacion
    bool menu = false;
    while(!menu)
    {   
        //Dibujar textos e imagenes del modo de operacion
        draw_menu(text, mode, stars);

        //Eventos del teclado del modo de operacion
        ALLEGRO_EVENT events1;
        al_wait_for_event(event_menu, &events1);
        if (events1.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch (events1.keyboard.keycode)
            {
            case ALLEGRO_KEY_X:
                exit(1);
                break;
            case ALLEGRO_KEY_1:
                modeop = 1;
                menu = true;
                break;
            case ALLEGRO_KEY_2:
                modeop = 2;
                menu = true;
                break;
            default:
                break;
            }
        }
        al_clear_to_color(al_map_rgb(0,0,0));
    }
    //Liberar memoria de fuentes y eventos
    al_destroy_font(text);
    al_destroy_font(mode);
    al_destroy_event_queue(event_menu);

    //Modo de operacion automatico
    if(modeop == 2)
    {   
        //Crear las fuentes del modo de operacion automatico
        ALLEGRO_FONT *subtitle = al_load_ttf_font("Orbitron-Bold.ttf", 36, ALLEGRO_TTF_MONOCHROME);
        ALLEGRO_FONT *stat = al_load_ttf_font("Orbitron-Bold.ttf", 16, ALLEGRO_TTF_MONOCHROME);
        ALLEGRO_FONT *number =  al_load_ttf_font("Orbitron-Bold.ttf", 36, ALLEGRO_TTF_MONOCHROME);

        //Parametros del alien
        int num_alien = 1;
        int energy_alien = 1;
        int regen_alien = 1;

        //Crear cola de eventos del modo de operacion auto
        ALLEGRO_EVENT_QUEUE *event_autom = al_create_event_queue();
        al_register_event_source(event_autom, al_get_keyboard_event_source());

        //Ventana de modo de operacion automatico
        bool autom = false;
        while(!autom)
        {   
            //Dibujar textos e imagenes del modo de operacion automatico
            draw_autom(subtitle, stat, number, stars, num_alien, energy_alien, regen_alien);

            //Eventos del teclado del modo de operacion automatico
            ALLEGRO_EVENT events3;
            al_wait_for_event(event_autom, &events3);

            if (events3.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                switch (events3.keyboard.keycode)
                {
                case ALLEGRO_KEY_X:
                    exit(1);
                    break;
                case ALLEGRO_KEY_E:
                    energy_alien += 1;
                    break;
                case ALLEGRO_KEY_D:
                    if (energy_alien > 1)
                    {
                        energy_alien -= 1;
                    }
                    break;
                case ALLEGRO_KEY_R:
                    regen_alien += 1;
                    break;
                case ALLEGRO_KEY_F:
                    if(regen_alien > 1)
                    {
                        regen_alien -= 1;
                    }
                    break;
                case ALLEGRO_KEY_SPACE:
                    //Funcion crear marciano
                    if(energy_alien < regen_alien) {
                        createAlien(regen_alien, energy_alien);
                        num_alien += 1;
                        energy_alien = 1;
                        regen_alien = 1;
                    }
                    break;
                case ALLEGRO_KEY_ENTER:
                    autom = true;
                    break;
                default:
                    break;
                }
            }
            al_clear_to_color(al_map_rgb(0,0,0));
        }
        //Liberar memoria de fuentes y imagenes
        al_destroy_font(subtitle);
        al_destroy_font(number);
        al_destroy_font(stat);
        al_destroy_bitmap(stars);
        al_destroy_event_queue(event_autom);

    }
    //Liberar memoria display
    al_destroy_display(display);
    //Iniciar simulacion
    gamewindow(modeop, algorithm);
}

//Funcion para dibujar texto e imagenes en el menu
void draw_menu(ALLEGRO_FONT *title, ALLEGRO_FONT *mode, ALLEGRO_BITMAP *stars)
{
    al_draw_bitmap(stars, 0, 0, 0);
    al_draw_text(title, al_map_rgb(44, 117, 255), SCREENWIDTH / 2, SCREENHEIGHT / 4, ALLEGRO_ALIGN_CENTRE, "MODO DE OPERACION");
    al_draw_text(mode, al_map_rgb(0, 255, 0), SCREENWIDTH / 2, SCREENHEIGHT / 2, ALLEGRO_ALIGN_CENTRE, "1) Modo Manual");
    al_draw_text(mode, al_map_rgb(0, 255, 0), SCREENWIDTH / 2, 3 * SCREENHEIGHT / 4, ALLEGRO_ALIGN_CENTRE, "2) Modo Automatico");
    al_flip_display();
}

//Funcion para dibujar texto e imagenes del modo de operacion
void draw_algor(ALLEGRO_FONT *title, ALLEGRO_FONT *select, ALLEGRO_BITMAP *stars)
{
    al_draw_bitmap(stars, 0, 0, 0);
    al_draw_text(title, al_map_rgb(44, 117, 255), SCREENWIDTH / 2, SCREENHEIGHT / 4, ALLEGRO_ALIGN_CENTRE, "SIMULACION DE PROCESOS");
    al_draw_text(title, al_map_rgb(44, 117, 255), SCREENWIDTH / 2, SCREENHEIGHT / 4 + 35, ALLEGRO_ALIGN_CENTRE, "EN TIEMPO REAL");
    al_draw_text(select, al_map_rgb(0, 255, 0), SCREENWIDTH / 2, SCREENHEIGHT / 2, ALLEGRO_ALIGN_CENTRE, "1) ALGORITMO RM");
    al_draw_text(select, al_map_rgb(0, 255, 0), SCREENWIDTH / 2, 3 * SCREENHEIGHT / 4, ALLEGRO_ALIGN_CENTRE, "2) ALGORITMO EDF");
    al_flip_display();
}

//Funcion para dibujar texto e imagenes del modo de operacion automatico
void draw_autom(ALLEGRO_FONT *subtitle, ALLEGRO_FONT *stat, ALLEGRO_FONT *number, ALLEGRO_BITMAP *stars, int num_alien, int energy_alien, int regen_alien)
{
    al_draw_bitmap(stars, 0, 0, 0);

    char alien_num[10];
    char alien[256] = "CREAR MARCIANO #";
    sprintf(alien_num, "%d", num_alien);
    strcat(alien, alien_num);
    al_draw_text(subtitle, al_map_rgb(44, 117, 255), SCREENWIDTH / 2, 10, ALLEGRO_ALIGN_CENTRE, alien);

    char alien_energy[10];
    sprintf(alien_energy, "%d", energy_alien);
    al_draw_text(stat, al_map_rgb(0, 255, 0), SCREENWIDTH / 2, SCREENHEIGHT / 4, ALLEGRO_ALIGN_CENTRE, "PRESIONE 'E' PARA AUMENTAR LA ENERGIA");
    al_draw_text(stat, al_map_rgb(0, 255, 0), SCREENWIDTH / 2, (SCREENHEIGHT / 4) + 20, ALLEGRO_ALIGN_CENTRE, "PRESIONE 'D' PARA DISMINUIR LA ENERGIA");
    al_draw_text(number, al_map_rgb(0, 255, 0), SCREENWIDTH / 2, (SCREENHEIGHT / 4) + 60, ALLEGRO_ALIGN_CENTRE, alien_energy);

    char alien_regen[10];
    sprintf(alien_regen, "%d", regen_alien);
    al_draw_text(stat, al_map_rgb(255, 128, 0), SCREENWIDTH / 2, SCREENHEIGHT / 2, ALLEGRO_ALIGN_CENTRE, "PRESIONE 'R' PARA AUMENTAR TIEMPO DE REGENERACION");
    al_draw_text(stat, al_map_rgb(255, 128, 0), SCREENWIDTH / 2, (SCREENHEIGHT / 2) + 20, ALLEGRO_ALIGN_CENTRE, "PRESIONE 'F' PARA DISMINUIR EL TIEMPO DE REGENERACION");
    al_draw_text(number, al_map_rgb(255, 128, 0), SCREENWIDTH / 2, (SCREENHEIGHT / 2) + 60, ALLEGRO_ALIGN_CENTRE, alien_regen);

    al_draw_text(stat, al_map_rgb(44, 117, 255), SCREENWIDTH / 2, (3 * SCREENHEIGHT / 4) + 20, ALLEGRO_ALIGN_CENTRE, "SIGUIENTE MARCIANO: 'ESPACIO'");
    al_draw_text(stat, al_map_rgb(44, 117, 255), SCREENWIDTH / 2, (3 * SCREENHEIGHT / 4) + 40, ALLEGRO_ALIGN_CENTRE, "INICIAR SIMULACION: 'ENTER'");
    al_flip_display();
}

//Funcion que abre la pantalla de error de calendarizacion
void error(ALLEGRO_DISPLAY *display, int alien_id, int alien_energy, int alien_period)
{   
    char id[10];
    char energy[10];
    char period[10];
    sprintf(id, "%d", alien_id);
    sprintf(energy, "%d", alien_energy);
    sprintf(period, "%d", alien_period);

    char error[100] = "Error de Calendarizacion";
    char text[100] = "Error Marciano ";
    strcat(text, id);
    strcat(text, "\n\nEnergia = ");
    strcat(text, energy);
    strcat(text, "\nTiempo de Regeneracion = ");
    strcat(text, period);
    char question[100] = "\n\nDesear observar el reporte del programa?";
    strcat(text, question);


    int respond = al_show_native_message_box(display, "Error", error, text, NULL, ALLEGRO_MESSAGEBOX_ERROR | ALLEGRO_MESSAGEBOX_YES_NO);
    if (respond == 1)
    {
        //Terminar el while del juego
    }
    else
    {
        exit(1);
    }
}