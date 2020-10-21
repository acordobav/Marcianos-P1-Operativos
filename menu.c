#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define SCREENWIDTH 630
#define SCREENHEIGHT 680

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

    ALLEGRO_EVENT_QUEUE *event_menu = al_create_event_queue();
    al_register_event_source(event_menu, al_get_keyboard_event_source());

    ALLEGRO_FONT *title = al_load_ttf_font("Orbitron-Bold.ttf", 36, ALLEGRO_TTF_MONOCHROME);
    ALLEGRO_FONT *mode = al_load_ttf_font("Orbitron-Bold.ttf", 28, ALLEGRO_TTF_MONOCHROME);
    ALLEGRO_BITMAP *stars = al_load_bitmap("stars.jpg");

    int modeop;

    bool menu = false;
    while(!menu)
    {
        al_draw_bitmap(stars, 0, 0, 0);
        al_draw_text(title, al_map_rgb(44, 117, 255), SCREENWIDTH / 2, SCREENHEIGHT / 4, ALLEGRO_ALIGN_CENTRE, "ALIEN");
        al_draw_text(mode, al_map_rgb(0, 255, 0), SCREENWIDTH / 2, SCREENHEIGHT / 2, ALLEGRO_ALIGN_CENTRE, "1) Modo Manual");
        al_draw_text(mode, al_map_rgb(0, 255, 0), SCREENWIDTH / 2, 3 * SCREENHEIGHT / 4, ALLEGRO_ALIGN_CENTRE, "2) Modo Automatico");
        al_flip_display();

        ALLEGRO_EVENT events1;
        al_wait_for_event(event_menu, &events1);

        if (events1.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch (events1.keyboard.keycode)
            {
            case ALLEGRO_KEY_X:
                menu = true;
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
        bool manual = false;

    }
    else if(modeop == 2)
    {
        ALLEGRO_FONT *number =  al_load_ttf_font("Orbitron-Bold.ttf", 36, ALLEGRO_TTF_MONOCHROME);
        ALLEGRO_FONT *subtitle = al_load_ttf_font("Orbitron-Bold.ttf", 36, ALLEGRO_TTF_MONOCHROME);
        int num_alien = 1;
        char alien_num[10];

        ALLEGRO_FONT *stat = al_load_ttf_font("Orbitron-Bold.ttf", 16, ALLEGRO_TTF_MONOCHROME);
        int energy_alien = 1;
        char alien_energy[10];
        int regen_alien = 1;
        char alien_regen[10];

        ALLEGRO_EVENT_QUEUE *event_autom = al_create_event_queue();
        al_register_event_source(event_autom, al_get_keyboard_event_source());

        bool autom = false;
        while(!autom)
        {   
            al_draw_bitmap(stars, 0, 0, 0);

            char alien[256] = "CREAR MARCIANO #";
            sprintf(alien_num, "%d", num_alien);
            strcat(alien, alien_num);
            al_draw_text(subtitle, al_map_rgb(44, 117, 255), SCREENWIDTH / 2, 10, ALLEGRO_ALIGN_CENTRE, alien);

            char energy[256] = "PRESIONE 'E' PARA AUMENTAR LA ENERGIA";
            char denergy[256] = "PRESIONE 'D' PARA DISMINUIR LA ENERGIA";
            sprintf(alien_energy, "%d", energy_alien);
            al_draw_text(stat, al_map_rgb(0, 255, 0), SCREENWIDTH / 2, SCREENHEIGHT / 4, ALLEGRO_ALIGN_CENTRE, energy);
            al_draw_text(stat, al_map_rgb(0, 255, 0), SCREENWIDTH / 2, (SCREENHEIGHT / 4) + 20, ALLEGRO_ALIGN_CENTRE, denergy);
            al_draw_text(number, al_map_rgb(0, 255, 0), SCREENWIDTH / 2, (SCREENHEIGHT / 4) + 60, ALLEGRO_ALIGN_CENTRE, alien_energy);


            char regen[256] = "PRESIONE 'R' PARA AUMENTAR TIEMPO DE REGENERACION";
            char dregen[256] = "PRESIONE 'F' PARA DISMINUIR EL TIEMPO DE REGENERACION";
            sprintf(alien_regen, "%d", regen_alien);
            al_draw_text(stat, al_map_rgb(255, 128, 0), SCREENWIDTH / 2, SCREENHEIGHT / 2, ALLEGRO_ALIGN_CENTRE, regen);
            al_draw_text(stat, al_map_rgb(255, 128, 0), SCREENWIDTH / 2, (SCREENHEIGHT / 2) + 20, ALLEGRO_ALIGN_CENTRE, dregen);
            al_draw_text(number, al_map_rgb(255, 128, 0), SCREENWIDTH / 2, (SCREENHEIGHT / 2) + 60, ALLEGRO_ALIGN_CENTRE, alien_regen);

            char next[256] = "SIGUIENTE MARCIANO: 'ESPACIO'";
            char start[256] = "INICIAR SIMULACION: 'ENTER'";
            al_draw_text(stat, al_map_rgb(44, 117, 255), SCREENWIDTH / 2, (3 * SCREENHEIGHT / 4) + 20, ALLEGRO_ALIGN_CENTRE, next);
            al_draw_text(stat, al_map_rgb(44, 117, 255), SCREENWIDTH / 2, (3 * SCREENHEIGHT / 4) + 40, ALLEGRO_ALIGN_CENTRE, start);
            al_flip_display();

            ALLEGRO_EVENT events2;
            al_wait_for_event(event_autom, &events2);

            if (events2.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                switch (events2.keyboard.keycode)
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
    }


    al_destroy_display(display);

}