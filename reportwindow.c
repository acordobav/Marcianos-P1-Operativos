#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scheduler.c"

void reportWindow();
void createColors();
void drawReport(int x_offset, int y_offset, ALLEGRO_FONT* font);
int drawProcessInTime(int x_offset, int y_offset, ALLEGRO_FONT* font);
void moveCamera(Direction direction);
void drawLines(int x_off, int y_off);

typedef struct Color {
    int r, g, b;
} Color;

Color colors[maxAliens];
const int y_blocksize = 40;
const int x_blocksize = 20;
int x_offset = x_blocksize;
int y_offset = x_blocksize;

void reportWindow() {
    createColors();

    int screenwidth = 1400;
    int screenheight = 800;

    al_init();
    al_install_keyboard();
    al_init_primitives_addon();
    al_init_font_addon();

    ALLEGRO_KEYBOARD_STATE keyState;

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_DISPLAY* disp = al_create_display(screenwidth, screenheight);
    al_set_window_title(disp, "Reporte");
    ALLEGRO_FONT* font = al_create_builtin_font();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool redraw = true;
    ALLEGRO_EVENT event;

    //loadReport();
    al_start_timer(timer);
    while(1) {
        al_wait_for_event(queue, &event);
        al_get_keyboard_state(&keyState);

        if(event.type == ALLEGRO_EVENT_TIMER){
            redraw = true;
            // Movimiento de la camara
            if(al_key_down(&keyState, ALLEGRO_KEY_LEFT))
                moveCamera(LEFT);
            else if(al_key_down(&keyState, ALLEGRO_KEY_RIGHT)) 
                moveCamera(RIGHT);
            else if(al_key_down(&keyState, ALLEGRO_KEY_UP)) 
                moveCamera(UP);
            else if(al_key_down(&keyState, ALLEGRO_KEY_DOWN)) 
                moveCamera(DOWN);
        } else if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            break;
        /*else if(event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if(al_key_down(&keyState, ALLEGRO_KEY_X)) break;
        }*/
        // Renderizado de los objetos
        if(redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(255, 255, 255));
            
            // Dibujado de las lineas y rectangulos
            int y = y_offset + y_blocksize * 2 * alienCount;
            drawLines(x_offset, y);
            drawProcessInTime(x_offset, y_offset, font);
            drawReport(x_offset, y, font);

            al_flip_display();

            redraw = false;
        }
    }
    // Limpieza de memoria
    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
}

/**
 * Funcion para generar colores random para identificar cada proceso
**/
void createColors() {
    // Creacion de maxAliens colores aleatorios para representar los procesos
    for (int i = 0; i < maxAliens; i++) {
        Color color;
        color.r = rand() % 255; // Canal rojo
        color.g = rand() % 255; // Canal verde
        color.b = rand() % 255; // Canal azul
        colors[i] = color;
    }
}

/**
 * Funcion para dibujar los elementos del reporte
**/
void drawReport(int x_offset, int y_offset, ALLEGRO_FONT* font) {
    int counter = 0;
    for(int i = 0; i < reportCount; i++) {
        int id = report[i];
        
        // Calculo de las coordenadas del rectangulo
        int x1 = x_offset + i * x_blocksize;
        int y1 = y_offset;
        int x2 = x1 + x_blocksize;
        int y2 = y_offset + y_blocksize;
        
        if(id != 0) {
            Color color = colors[id-1];
            ALLEGRO_COLOR al_color = al_map_rgb(color.r, color.g, color.b);


            // Dibujado del fondo del rectangulo
            al_draw_filled_rectangle(x1, y1, x2, y2, al_color);

            // Dibujado del borde negro del rectangulo
            al_draw_rectangle(x1, y1, x2, y2, al_map_rgb(0, 0, 0), 1.0);

            // Dibujado del identificador del proceso
            char num[5];
            sprintf(num, "%d", id);
            al_draw_text(font, al_map_rgb(255, 255, 255), x1, y1, 0, num);
        }

        // Renderizado de los valores del eje x
        if(counter % 5 == 0) {
            int x = x2 - x_blocksize - 6; 
            int y = y2 + 2;
            char count[5];
            sprintf(count, "%d", counter);
            al_draw_text(font, al_map_rgb(0, 0, 0), x, y, 0, count);
        }
        counter++;
    }
}

/**
 * Funcion para dibujar la aparicion de los procesos segun su periodo
**/
int drawProcessInTime(int x_offset, int y_offset, ALLEGRO_FONT* font) {
    for(int i = 0; i < alienCount; i++) {
        Alien alien = aliens[i];
        Color color = colors[alien.id-1];
        ALLEGRO_COLOR al_color = al_map_rgb(color.r, color.g, color.b);
        int counter = alien.energy;
        int periodCounter = 0;

        for(int j = 0; j < reportCount; j++) {
            if (alien.startTime <= j){
                periodCounter++;
                if(counter > 0) {
                    // Calculo de las coordenadas del rectangulo
                    int x1 = x_offset + j * x_blocksize;
                    int y1 = y_offset;
                    int x2 = x1 + x_blocksize;
                    int y2 = y_offset + y_blocksize;

                    // Dibujado del fondo del rectangulo
                    al_draw_filled_rectangle(x1, y1, x2, y2, al_color);

                    // Dibujado del borde negro del rectangulo
                    al_draw_rectangle(x1, y1, x2, y2, al_map_rgb(0, 0, 0), 1.0);
                
                    char num[5];
                    sprintf(num, "%d", alien.id);
                    al_draw_text(font, al_map_rgb(255, 255, 255), x1, y1, 0, num);
                    counter--;
                } else if(periodCounter == alien.period) {
                    counter = alien.energy;
                    periodCounter = 0;
                }
            }
        }
        y_offset += y_blocksize * 2;
    }
    return y_offset;
}

/**
 * Funcion para dibujar las lineas del fondo del reporte
**/
void drawLines(int x_off, int y_off) {
    int y1 = y_off + y_blocksize;

    // Lineas superior e inferior
    int x = x_offset + reportCount * x_blocksize;
    al_draw_line(x_offset, y_offset, x, y_offset, al_map_rgb(105, 105, 105), 1.0); //Superior
    al_draw_line(x_offset, y1, x, y1, al_map_rgb(0, 0, 0), 1.0); // Inferior

    // Lineas horizontales
    int counter = 5;
    for(int i = 0; i < reportCount; i++) {
        if(counter % 5 == 0) {
            int x1 = x_off + i * x_blocksize;
            al_draw_line(x1, y1, x1, y_offset, al_map_rgb(105, 105, 105), 1.0);
            counter = 0;
        }
        counter++;
    }
}

/**
 * Funcion para mover la camara
**/
void moveCamera(Direction direction) {
    switch (direction)
    {
    case LEFT:
        x_offset += x_blocksize;
        break;
    
    case RIGHT:
        x_offset -= x_blocksize;
        break;
    
    case UP:
        y_offset -= 10;
        break;
    
    case DOWN:
        y_offset += 10;
        break;

    default:
        break;
    }
    
}