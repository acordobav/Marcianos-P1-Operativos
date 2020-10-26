#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>

#include "scheduler.c"

#define ScreenWidth 830
#define ScreenHeight 830 

const float FPS = 30.0;
const float animationFPS = 6.0;
long int clk = 0;

void drawWalls(ALLEGRO_BITMAP *wallSprite);
void drawFlags(ALLEGRO_BITMAP *startFlagSprite, ALLEGRO_BITMAP *endFlagSprite);
void drawAliens(ALLEGRO_BITMAP *alienSprite);
void drawAliensInfo(ALLEGRO_FONT* font);
void animateAliens();
void draw_manual(ALLEGRO_FONT *subtitle, ALLEGRO_FONT *stat, ALLEGRO_FONT *number, int energy_alien, int regen_alien);
Alien* gamewindow();
//void updateReport(FILE *fptr);

Alien* gamewindow(int modeop, int algorithm) {
    int executionCounter = FPS;
    srand(time(NULL)); // Inicializacion para numeros random, solo debe llamarse una vez

    int energy_alien = 1;
    int regen_alien = 1;

    //FILE *fptr = fopen("report.txt","w");

    // Inicio interfaz grafica
    al_init();
    al_install_keyboard();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    ALLEGRO_KEYBOARD_STATE keyState;

    ALLEGRO_BITMAP *wallSprite = al_load_bitmap("sprites/wall3.png");
    ALLEGRO_BITMAP *startFlagSprite = al_load_bitmap("sprites/start-flag.png");
    ALLEGRO_BITMAP *endFlagSprite = al_load_bitmap("sprites/end-flag.png");
    ALLEGRO_BITMAP *alienSprite = al_load_bitmap("sprites/alien.png");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
    ALLEGRO_TIMER* animationTimer = al_create_timer(1.0 / animationFPS);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_DISPLAY* disp = al_create_display(ScreenWidth, ScreenHeight);
    al_set_window_title(disp, "Proyecto 1 PSO - Simulacion");
    if(modeop == 2) al_resize_display(disp, ScreenWidth, 690);
    ALLEGRO_FONT* font = al_create_builtin_font();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_timer_event_source(animationTimer));

    ALLEGRO_FONT *subtitle = al_load_ttf_font("Orbitron-Bold.ttf", 22, ALLEGRO_TTF_MONOCHROME);
    ALLEGRO_FONT *stat = al_load_ttf_font("Orbitron-Bold.ttf", 16, ALLEGRO_TTF_MONOCHROME);
    ALLEGRO_FONT *number =  al_load_ttf_font("Orbitron-Bold.ttf", 20, ALLEGRO_TTF_MONOCHROME);

    bool redraw = true;
    ALLEGRO_EVENT event;

    al_start_timer(timer);
    al_start_timer(animationTimer);

    scheduler(algorithm);
    //updateRegenerationTimer();
    int gameLoop = 1;
    while(gameLoop)
    {
        al_wait_for_event(queue, &event);
        al_get_keyboard_state(&keyState);

        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (event.keyboard.keycode) {
                case ALLEGRO_KEY_X:
                    // Detener juego 
                    gameLoop = 0;
                    break;
                case ALLEGRO_KEY_E:
                    // Aumentar energia nuevo Alien
                    energy_alien += 1;
                    break;
                case ALLEGRO_KEY_D:
                    // Disminuir energia nuevo Alien
                    if (energy_alien > 1) energy_alien -= 1;
                    break;
                case ALLEGRO_KEY_R:
                    // Aumentar periodo nuevo Alien
                    regen_alien += 1;
                    break;
                case ALLEGRO_KEY_F:
                    // Disminuir periodo nuevo Alien
                    if(regen_alien > 1) regen_alien -= 1;
                    break;
                case ALLEGRO_KEY_ENTER:
                    // Crear nuevo Alien
                    if (modeop == 1) {
                        if(energy_alien < regen_alien) {
                            createAlien(regen_alien, energy_alien);
                            regen_alien = 1;
                            energy_alien = 1;
                        }
                    }
                    break;
                default:
                    break;
            }
        } else if(event.type == ALLEGRO_EVENT_TIMER){
            if(event.timer.source == timer) {

                pthread_mutex_lock(&clock_mutex);
                frameControl = !frameControl;
                pthread_cond_broadcast(&clock_cond);
                pthread_mutex_unlock(&clock_mutex);
                
                executionCounter -= 1;
                if(executionCounter <= 0) {
                    executionCounter = FPS;
                    // Aumentar contador de tiempo
                    clk++;
                    // Actualizacion de reporte
                    //updateReport(fptr);
                }
                redraw = true;
            } else if(event.timer.source == animationTimer){
                animateAliens();
            }
        }
        else if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            break;

        if(redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            
            int y = Y_OFFSET - 15;
            int x = X_OFFSET + 22 * BLOCK_SIZE;
            char num[5];
            sprintf(num, "%ld", clk);
            al_draw_text(font, al_map_rgb(255, 255, 255), x, y, 0, num);


            if (modeop == 1) draw_manual(subtitle, stat, number, energy_alien, regen_alien);
            drawAliensInfo(font);
            drawWalls(wallSprite);
            drawFlags(startFlagSprite, endFlagSprite);
            drawAliens(alienSprite);
            al_flip_display();

            redraw = false;
        }
    }

    al_destroy_font(font);
    al_destroy_font(subtitle);
    al_destroy_font(stat);
    al_destroy_font(number);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_timer(animationTimer);
    al_destroy_event_queue(queue);
    al_destroy_bitmap(wallSprite);
    al_destroy_bitmap(startFlagSprite);
    al_destroy_bitmap(endFlagSprite);
    al_destroy_bitmap(alienSprite);

    free(walls);
}

/**
 * Funcion para dibujar todos los muros del mapa
 * wallSprite: Sprite de los muros
**/
void drawWalls(ALLEGRO_BITMAP *wallSprite){
    for (int i = 0; i < wallCounter; i++) {
        Wall wall1 = walls[i]; // Se obtiene un muro de la lista y se dibuja en la posicion correspondiente
        al_draw_bitmap(wallSprite, wall1.x, wall1.y, 0);
    }
}

/**
 * Funcion para dibujar las banderas de inicio y final
 * startFlagSprite: Sprite de la bandera de inicio
 * endFlagSprite: Sprite de la bandera de finalizacion
**/
void drawFlags(ALLEGRO_BITMAP *startFlagSprite, ALLEGRO_BITMAP *endFlagSprite) {
    al_draw_bitmap(startFlagSprite, flags[0].x, flags[0].y, 0); // Dibujado de la bandera de inicio
    al_draw_bitmap(endFlagSprite, flags[1].x, flags[1].y, 0);   // Dibujado de la bandera del final
}

/**
 * Funcion para dibujar todos los aliens en el mapa
 * alienSprite: Sprite del alien
**/
void drawAliens(ALLEGRO_BITMAP *alienSprite) {
    pthread_mutex_lock(&alienCountMutex);
    int aliensCount = alienCount;
    pthread_mutex_unlock(&alienCountMutex);

    for (int i = 0; i < aliensCount; i++) {
        pthread_mutex_lock(&aliens_mutex[i]);
        Alien alien = aliens[i];
        pthread_mutex_unlock(&aliens_mutex[i]);
        if(!alien.isFinished){ // Se verifica que el alien no haya llegado a la meta
            al_draw_bitmap_region(alienSprite, alien.sourceX, alien.dir * BLOCK_SIZE, 
                                BLOCK_SIZE, BLOCK_SIZE, alien.x, alien.y, 0);
        }
    }
}

/**
 * Funcion encargada de dibujar la informacion de los Aliens
**/
void drawAliensInfo(ALLEGRO_FONT* font) {
    int y = Y_OFFSET;
    int x = X_OFFSET + 22 * BLOCK_SIZE;

    ALLEGRO_COLOR inactiveTextColor = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR activeTextColor = al_map_rgb(255, 0, 0);
    ALLEGRO_COLOR finishedTextColor = al_map_rgb(0, 255, 0);
    ALLEGRO_COLOR availableTextColor = al_map_rgb(255,255,0);

    pthread_mutex_lock(&alienCountMutex);
    int aliensCount = alienCount;
    pthread_mutex_unlock(&alienCountMutex);
    
    for (int i = 0; i < aliensCount; i++) {
        pthread_mutex_lock(&aliens_mutex[i]);
        Alien alien = aliens[i];
        pthread_mutex_unlock(&aliens_mutex[i]);
        ALLEGRO_COLOR textColor = inactiveTextColor;
        if(alien.isAvailable) textColor = availableTextColor;
        if(alien.isActive) textColor = activeTextColor;
        else if (alien.isFinished) textColor = finishedTextColor;

        // Dibujado del identificador
        char id[20] = "";
        char idValue[10];
        sprintf(idValue, "%d", alien.id);
        strcat(id, idValue);
        al_draw_text(font, textColor, x, y, 0, id);

        // Dibujado de la energia
        char energy[20] = "E: ";
        char energyValue[10];
        sprintf(energyValue, "%d", alien.energyCounter);
        strcat(energy, energyValue);
        al_draw_text(font, textColor, x + BLOCK_SIZE, y, 0, energy);
        
        // Dibujado del tiempo de regeneracion
        char regeneration[20] = "R: ";
        char regenerationValue[10];
        sprintf(regenerationValue, "%d", alien.regenerationTimer);
        strcat(regeneration, regenerationValue);
        al_draw_text(font, textColor, x + BLOCK_SIZE * 3, y, 0, regeneration);
        y += BLOCK_SIZE / 2;
    }


}

/**
 * Funcion para realizar la animacion de caminata en los aliens
**/
void animateAliens() {
    pthread_mutex_lock(&alienCountMutex);
    int aliensCount = alienCount;
    pthread_mutex_unlock(&alienCountMutex);

    for (int i = 0; i < aliensCount; i++) {
        pthread_mutex_lock(&aliens_mutex[i]);
        Alien *alien = &aliens[i];
        if(!alien->isFinished){ // Se verifica que el alien no haya llegado a la meta
            if(alien->isActive) { // Se verifica que el alien se encuentre en movimiento
                alien->sourceX += BLOCK_SIZE;
                if(alien->sourceX >= BLOCK_SIZE*3) {
                    alien->sourceX = 0;
                }
            } else {
                alien->sourceX = BLOCK_SIZE;
            }
        }
        pthread_mutex_unlock(&aliens_mutex[i]);
    }
}

void draw_manual(ALLEGRO_FONT *subtitle, ALLEGRO_FONT *stat, ALLEGRO_FONT *number, int energy_alien, int regen_alien)
{
    al_draw_text(subtitle, al_map_rgb(44, 117, 255), ScreenWidth / 2, ScreenHeight - 125 - BLOCK_SIZE, ALLEGRO_ALIGN_CENTRE, "CREAR MARCIANO");

    char alien_energy[10];
    sprintf(alien_energy, "%d", energy_alien);
    al_draw_text(stat, al_map_rgb(0, 255, 0), ScreenWidth / 4, ScreenHeight - 90 - BLOCK_SIZE, ALLEGRO_ALIGN_CENTRE, "ENERGIA");
    al_draw_text(stat, al_map_rgb(0, 255, 0), ScreenWidth / 4, ScreenHeight - 70 - BLOCK_SIZE, ALLEGRO_ALIGN_CENTRE, "'E': AUMENTAR");
    al_draw_text(stat, al_map_rgb(0, 255, 0), ScreenWidth / 4, ScreenHeight - 50 - BLOCK_SIZE, ALLEGRO_ALIGN_CENTRE, "'D': DISMINUIR");
    al_draw_text(number, al_map_rgb(0, 255, 0), ScreenWidth / 4, ScreenHeight - 30 - BLOCK_SIZE, ALLEGRO_ALIGN_CENTRE, alien_energy);

    char alien_regen[10];
    sprintf(alien_regen, "%d", regen_alien);
    al_draw_text(stat, al_map_rgb(255, 128, 0), 3 * ScreenWidth / 4, ScreenHeight - 90 - BLOCK_SIZE, ALLEGRO_ALIGN_CENTRE, "TIEMPO DE REGENERACION");
    al_draw_text(stat, al_map_rgb(255, 128, 0), 3 * ScreenWidth / 4, ScreenHeight - 70 - BLOCK_SIZE, ALLEGRO_ALIGN_CENTRE, "'R': AUMENTAR");
    al_draw_text(stat, al_map_rgb(255, 128, 0), 3 * ScreenWidth / 4, ScreenHeight - 50 - BLOCK_SIZE, ALLEGRO_ALIGN_CENTRE, "'F': DISMINUIR");
    al_draw_text(number, al_map_rgb(255, 128, 0), 3 * ScreenWidth / 4, ScreenHeight - 30 - BLOCK_SIZE, ALLEGRO_ALIGN_CENTRE, alien_regen);

    al_draw_text(stat, al_map_rgb(44, 117, 255), ScreenWidth / 2, ScreenHeight - 20 - BLOCK_SIZE, ALLEGRO_ALIGN_CENTRE, "'ENTER': CREAR");
}