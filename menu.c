#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define SCREENWIDTH 690
#define SCREENHEIGHT 820

void draw_menu(ALLEGRO_FONT *title, ALLEGRO_FONT *mode, ALLEGRO_BITMAP *stars);
void draw_algor(ALLEGRO_FONT *title, ALLEGRO_FONT *select, ALLEGRO_BITMAP *stars);
void draw_manual(ALLEGRO_FONT *subtitle, ALLEGRO_FONT *stat, ALLEGRO_FONT *number, int energy_alien, int regen_alien);
void draw_autom(ALLEGRO_FONT *subtitle, ALLEGRO_FONT *stat, ALLEGRO_FONT *number, ALLEGRO_BITMAP *stars, int num_alien, int energy_alien, int regen_alien);

int main()
{   
    if(!al_init())
    {
        printf("Error al inicializar Allegro");
        exit(1);
    }

    al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
    ALLEGRO_DISPLAY *display = al_create_display(SCREENWIDTH, SCREENHEIGHT);
    al_set_window_position(display, 200, 100);
    al_set_window_title(display, "Proyecto 1 PSO");

    if(!display)
    {
        printf("Error al crear pantalla");
        exit(1);
    }

    al_init_font_addon();
    al_init_ttf_addon();
    al_install_keyboard();
    al_init_image_addon();

    ALLEGRO_FONT *title = al_load_ttf_font("Orbitron-Bold.ttf", 36, ALLEGRO_TTF_MONOCHROME);
    ALLEGRO_FONT *mode = al_load_ttf_font("Orbitron-Bold.ttf", 28, ALLEGRO_TTF_MONOCHROME);
    ALLEGRO_BITMAP *stars = al_load_bitmap("stars.jpg");

    ALLEGRO_EVENT_QUEUE *event_menu = al_create_event_queue();
    al_register_event_source(event_menu, al_get_keyboard_event_source());

    int modeop;

    bool menu = false;
    while(!menu)
    {
        
        draw_menu(title, mode, stars);

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
    al_destroy_font(title);
    al_destroy_font(mode);
    al_destroy_event_queue(event_menu);

    //printf("%d\n", modeop);
    if(modeop == 1)
    {   
        ALLEGRO_FONT *title = al_load_ttf_font("Orbitron-Bold.ttf", 36, ALLEGRO_TTF_MONOCHROME);
        ALLEGRO_FONT *select = al_load_ttf_font("Orbitron-Bold.ttf", 28, ALLEGRO_TTF_MONOCHROME);

        ALLEGRO_EVENT_QUEUE *event_plan = al_create_event_queue();
        al_register_event_source(event_plan, al_get_keyboard_event_source());

        int algorithm;

        bool plan = false;
        while(!plan)
        {
            draw_algor(title, select, stars);

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
        al_destroy_font(title);
        al_destroy_font(select);
        al_destroy_event_queue(event_plan);

        ALLEGRO_FONT *subtitle = al_load_ttf_font("Orbitron-Bold.ttf", 22, ALLEGRO_TTF_MONOCHROME);
        ALLEGRO_FONT *stat = al_load_ttf_font("Orbitron-Bold.ttf", 16, ALLEGRO_TTF_MONOCHROME);
        ALLEGRO_FONT *number =  al_load_ttf_font("Orbitron-Bold.ttf", 20, ALLEGRO_TTF_MONOCHROME);

        int energy_alien = 1;
        int regen_alien = 1;

        ALLEGRO_EVENT_QUEUE *event_manual = al_create_event_queue();
        al_register_event_source(event_manual, al_get_keyboard_event_source());

        bool manual = false;
        while(!manual)
        {
            draw_manual(subtitle, stat, number, energy_alien, regen_alien);

            ALLEGRO_EVENT events3;
            al_wait_for_event(event_manual, &events3);

            if (events3.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                switch (events3.keyboard.keycode)
                {
                case ALLEGRO_KEY_X:
                    manual = true;
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
                case ALLEGRO_KEY_ENTER:
                    //funcion de crear marciano
                    break;
                default:
                    break;
                }
            }

            al_clear_to_color(al_map_rgb(0,0,0));
        }
        al_destroy_font(subtitle);
        al_destroy_font(number);
        al_destroy_font(stat);
        al_destroy_event_queue(event_manual);

    }
    else if(modeop == 2)
    {   
        ALLEGRO_FONT *title = al_load_ttf_font("Orbitron-Bold.ttf", 36, ALLEGRO_TTF_MONOCHROME);
        ALLEGRO_FONT *select = al_load_ttf_font("Orbitron-Bold.ttf", 28, ALLEGRO_TTF_MONOCHROME);

        ALLEGRO_EVENT_QUEUE *event_plan = al_create_event_queue();
        al_register_event_source(event_plan, al_get_keyboard_event_source());

        int algorithm;

        bool plan = false;
        while(!plan)
        {
            draw_algor(title, select, stars);

            ALLEGRO_EVENT events4;
            al_wait_for_event(event_plan, &events4);
            if (events4.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                switch (events4.keyboard.keycode)
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
        al_destroy_font(title);
        al_destroy_font(select);
        al_destroy_event_queue(event_plan);

        ALLEGRO_FONT *subtitle = al_load_ttf_font("Orbitron-Bold.ttf", 36, ALLEGRO_TTF_MONOCHROME);
        ALLEGRO_FONT *stat = al_load_ttf_font("Orbitron-Bold.ttf", 16, ALLEGRO_TTF_MONOCHROME);
        ALLEGRO_FONT *number =  al_load_ttf_font("Orbitron-Bold.ttf", 36, ALLEGRO_TTF_MONOCHROME);

        int num_alien = 1;
        int energy_alien = 1;
        int regen_alien = 1;

        ALLEGRO_EVENT_QUEUE *event_autom = al_create_event_queue();
        al_register_event_source(event_autom, al_get_keyboard_event_source());

        bool autom = false;
        while(!autom)
        {   
            draw_autom(subtitle, stat, number, stars, num_alien, energy_alien, regen_alien);

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
                    num_alien += 1;
                    energy_alien = 1;
                    regen_alien = 1;
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
        al_destroy_font(subtitle);
        al_destroy_font(number);
        al_destroy_font(stat);
        al_destroy_bitmap(stars);
        al_destroy_event_queue(event_autom);


        //While del juego
    }


    al_destroy_display(display);

}

void draw_menu(ALLEGRO_FONT *title, ALLEGRO_FONT *mode, ALLEGRO_BITMAP *stars)
{
    al_draw_bitmap(stars, 0, 0, 0);
    al_draw_text(title, al_map_rgb(44, 117, 255), SCREENWIDTH / 2, SCREENHEIGHT / 4, ALLEGRO_ALIGN_CENTRE, "ALIEN");
    al_draw_text(mode, al_map_rgb(0, 255, 0), SCREENWIDTH / 2, SCREENHEIGHT / 2, ALLEGRO_ALIGN_CENTRE, "1) Modo Manual");
    al_draw_text(mode, al_map_rgb(0, 255, 0), SCREENWIDTH / 2, 3 * SCREENHEIGHT / 4, ALLEGRO_ALIGN_CENTRE, "2) Modo Automatico");
    al_flip_display();
}

void draw_algor(ALLEGRO_FONT *title, ALLEGRO_FONT *select, ALLEGRO_BITMAP *stars)
{
    al_draw_bitmap(stars, 0, 0, 0);
    al_draw_text(title, al_map_rgb(44, 117, 255), SCREENWIDTH / 2, SCREENHEIGHT / 4, ALLEGRO_ALIGN_CENTRE, "SELECCIONAR");
    al_draw_text(select, al_map_rgb(0, 255, 0), SCREENWIDTH / 2, SCREENHEIGHT / 2, ALLEGRO_ALIGN_CENTRE, "1) ALGORITMO RM");
    al_draw_text(select, al_map_rgb(0, 255, 0), SCREENWIDTH / 2, 3 * SCREENHEIGHT / 4, ALLEGRO_ALIGN_CENTRE, "2) ALGORITMO EDF");
    al_flip_display();
}

void draw_manual(ALLEGRO_FONT *subtitle, ALLEGRO_FONT *stat, ALLEGRO_FONT *number, int energy_alien, int regen_alien)
{
    al_draw_text(subtitle, al_map_rgb(44, 117, 255), SCREENWIDTH / 2, SCREENHEIGHT - 125, ALLEGRO_ALIGN_CENTRE, "CREAR MARCIANO");

    char alien_energy[10];
    sprintf(alien_energy, "%d", energy_alien);
    al_draw_text(stat, al_map_rgb(0, 255, 0), SCREENWIDTH / 4, SCREENHEIGHT - 90, ALLEGRO_ALIGN_CENTRE, "ENERGIA");
    al_draw_text(stat, al_map_rgb(0, 255, 0), SCREENWIDTH / 4, SCREENHEIGHT - 70, ALLEGRO_ALIGN_CENTRE, "'E': AUMENTAR");
    al_draw_text(stat, al_map_rgb(0, 255, 0), SCREENWIDTH / 4, SCREENHEIGHT - 50, ALLEGRO_ALIGN_CENTRE, "'D': DISMINUIR");
    al_draw_text(number, al_map_rgb(0, 255, 0), SCREENWIDTH / 4, SCREENHEIGHT - 30, ALLEGRO_ALIGN_CENTRE, alien_energy);

    char alien_regen[10];
    sprintf(alien_regen, "%d", regen_alien);
    al_draw_text(stat, al_map_rgb(255, 128, 0), 3 * SCREENWIDTH / 4, SCREENHEIGHT - 90, ALLEGRO_ALIGN_CENTRE, "TIEMPO DE REGENERACION");
    al_draw_text(stat, al_map_rgb(255, 128, 0), 3 * SCREENWIDTH / 4, SCREENHEIGHT - 70, ALLEGRO_ALIGN_CENTRE, "'R': AUMENTAR");
    al_draw_text(stat, al_map_rgb(255, 128, 0), 3 * SCREENWIDTH / 4, SCREENHEIGHT - 50, ALLEGRO_ALIGN_CENTRE, "'F': DISMINUIR");
    al_draw_text(number, al_map_rgb(255, 128, 0), 3 * SCREENWIDTH / 4, SCREENHEIGHT - 30, ALLEGRO_ALIGN_CENTRE, alien_regen);

    al_draw_text(stat, al_map_rgb(44, 117, 255), SCREENWIDTH / 2, SCREENHEIGHT - 20, ALLEGRO_ALIGN_CENTRE, "'ENTER': CREAR");
    al_flip_display();
}

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