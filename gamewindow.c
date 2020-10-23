#include <time.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>

#include "createmap.c"

#define ScreenWidth 690
#define ScreenHeight 690 
#define maxAliens 30
#define moveSpeed 4;

const float FPS = 30.0;
const float animationFPS = 6.0;
int alienCount = 0;

typedef enum {UP = 3, DOWN = 0, LEFT = 1, RIGHT = 2} Direction;
typedef struct Alien {
    int id;
    int x,y, sourceX;
    Direction dir;
    int isActive;
    int energy;
    int period;
    int isFinished;
} Alien;

Wall* walls;
Alien* aliens;

void drawWalls(ALLEGRO_BITMAP *wallSprite);
void drawFlags(ALLEGRO_BITMAP *startFlagSprite, ALLEGRO_BITMAP *endFlagSprite);
void drawAliens(ALLEGRO_BITMAP *alienSprite);
void animateAliens();
void createAlien(int period, int energy);
void checkCollisions();
bool isCollisioned(int x1, int x2, int y1, int y2);
void getNewDirection(Alien* alien);
void restorePosition(Alien* alien);
void moveAlien();
void updateEnergy();

int main() {
    int executionCounter = FPS;
    srand(time(NULL)); // Inicializacion para numeros random, solo debe llamarse una vez
    walls = createMap();
    aliens = (Alien*) malloc(sizeof(*aliens)*maxAliens);
    createAlien(10, 10);

    // Inicio interfaz grafica
    al_init();
    al_install_keyboard();
    al_init_image_addon();

    ALLEGRO_KEYBOARD_STATE keyState;

    ALLEGRO_BITMAP *wallSprite = al_load_bitmap("sprites/wall3.png");
    ALLEGRO_BITMAP *startFlagSprite = al_load_bitmap("sprites/start-flag.png");
    ALLEGRO_BITMAP *endFlagSprite = al_load_bitmap("sprites/end-flag.png");
    ALLEGRO_BITMAP *alienSprite = al_load_bitmap("sprites/alien.png");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
    ALLEGRO_TIMER* animationTimer = al_create_timer(1.0 / animationFPS);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_DISPLAY* disp = al_create_display(ScreenWidth, ScreenHeight);
    ALLEGRO_FONT* font = al_create_builtin_font();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_timer_event_source(animationTimer));

    bool redraw = true;
    ALLEGRO_EVENT event;

    al_start_timer(timer);
    al_start_timer(animationTimer);
    while(1)
    {
        al_wait_for_event(queue, &event);
        al_get_keyboard_state(&keyState);

        if(event.type == ALLEGRO_EVENT_TIMER){
            if(event.timer.source == timer) {
                moveAlien();
                
                // Disminucion de niveles de energia
                executionCounter -= 1;
                if(executionCounter <= 0) {
                    updateEnergy();
                    executionCounter = FPS;
                }
                redraw = true;
                Alien *alien = &aliens[0];
                if(al_key_down(&keyState, ALLEGRO_KEY_DOWN)){
                    alien->y += moveSpeed;
                    alien->dir = DOWN;
                } else if(al_key_down(&keyState, ALLEGRO_KEY_UP)){
                    alien->y -= moveSpeed;
                    alien->dir = UP;
                } else if(al_key_down(&keyState, ALLEGRO_KEY_RIGHT)){
                    alien->x += moveSpeed;
                    alien->dir = RIGHT;
                } else if(al_key_down(&keyState, ALLEGRO_KEY_LEFT)){
                    alien->x -= moveSpeed;
                    alien->dir = LEFT;
                } else if(al_key_down(&keyState, ALLEGRO_KEY_A)){
                    createAlien(20,20);
                } else if(al_key_down(&keyState, ALLEGRO_KEY_S)){
                    alien->isActive = !alien->isActive;
                }
                checkCollisions();
            } else if(event.timer.source == animationTimer){
                animateAliens();
            }
        }
        else if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            break;

        if(redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));

            char text[100];
            char energy[50];
            sprintf(energy, "%d", aliens[0].energy);
            strcpy(text, "Energia: ");
            strcpy(text, energy);

            al_draw_text(font, al_map_rgb(255, 255, 255), 0, 0, 0, text);
            drawWalls(wallSprite);
            drawFlags(startFlagSprite, endFlagSprite);
            drawAliens(alienSprite);
            al_flip_display();

            redraw = false;
        }
    }

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_timer(animationTimer);
    al_destroy_event_queue(queue);
    al_destroy_bitmap(wallSprite);
    al_destroy_bitmap(startFlagSprite);
    al_destroy_bitmap(endFlagSprite);
    al_destroy_bitmap(alienSprite);

    free(walls);
    return 0;
}

/**
 * Funcion para crear un alien nuevo y agregarlo a la lista
 * period: Periodo que debe tener el nuevo alien
 * energy: Energia que debe tener el nuevo alien
**/ 
void createAlien(int period, int energy) {
    Alien alien;
    alien.id = alienCount;// Identificador del alien
    alien.x = flags[0].x; // Posicion en x del punto de partida
    alien.y = flags[0].y; // Posicion en y del punto de partida
    alien.dir = DOWN;     // Direccion de movimiento inicial
    alien.sourceX = BLOCK_SIZE;
    alien.isActive = 1;   // Estado de activiacion inicial
    alien.period = period;// Periodo del alien
    alien.energy = energy;// Energia del alien
    alien.isFinished = 0; // Estado de finalizacion no completado
    aliens[alienCount] = alien; // Se almacena en la lista de aliens
    alienCount++;         // Aumenta contador de aliens
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
    for (int i = 0; i < alienCount; i++) {
        Alien alien = aliens[i];
        if(!alien.isFinished){ // Se verifica que el alien no haya llegado a la meta
            al_draw_bitmap_region(alienSprite, alien.sourceX, alien.dir * BLOCK_SIZE, 
                                BLOCK_SIZE, BLOCK_SIZE, alien.x, alien.y, 0);
        }
    }
}

/**
 * Funcion para realizar la animacion de caminata en los aliens
**/
void animateAliens() {
    for (int i = 0; i < alienCount; i++) {
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
    }
}

/**
 * Funcion para verificar si algun alien ha colisionado con otro objeto
**/
void checkCollisions() {
    for (int i = 0; i < alienCount; i++) {
        Alien *alien = &aliens[i]; // Se obtiene un alien

        // Colisiones con los muros
        for (int j = 0; j < wallCounter; j++) {
            Wall wall = walls[j]; // Se obtiene un muro
            if(isCollisioned(alien->x, wall.x, alien->y, wall.y)){
                // Se restaura la posicion del alien
                restorePosition(alien);
                // Se calcula una nueva direccion para el Alien
                getNewDirection(alien);
            }
        }

        // Colisiones con otros Aliens
        if (alien->isActive) {
            for (int j = 0; j < alienCount; j++) {
                if(i != j) {
                    Alien *alien2 = &aliens[j]; // Se obtiene un alien
                    // Se verifica si los aliens estan colisionando y si tienen la misma direccion
                    if(isCollisioned(alien->x, alien2->x, alien->y, alien2->y) && alien->dir == alien2->dir){
                        // Se restaura la posicion del alien
                        restorePosition(alien);
                        // Se calcula una nueva direccion para el Alien
                        getNewDirection(alien);
                    }
                }
            }
        }

        // Verificacion de llegada a la meta, colision con la bandera de finalizacion
        if(isCollisioned(alien->x, flags[1].x, alien->y, flags[1].y)) {
            alien->isActive = 0;
            alien->isFinished = 1;
            alien->x = 0;
            alien->y = 0;
        }
    }
}

/**
 * Funcion para detectar si existe una colision entre dos objetos
 * x1: posicion en x del objeto 1
 * x2: posicion en x del objeto 2
 * y1: posicion en y del objeto 1
 * y2: posicion en y del objeto 2
 * return: true si hay colision, false en caso contrario
**/ 
bool isCollisioned(int x1, int x2, int y1, int y2) {
    if(x1 < (x2 + BLOCK_SIZE) && (x1 + BLOCK_SIZE) > x2 && y1 < (y2 + BLOCK_SIZE) && (y1 + BLOCK_SIZE) > y2){
        return true;
    }
    return false;
}

/**
 * Funcion para obtener una nueva direccion para un alien
 * alien: puntero al alien al que se le debe calcular una nueva direccion
**/
void getNewDirection(Alien* alien) {
    // Calculo de una nueva direccion random
    int previuosDirection = alien->dir;
    int r = alien->dir;
    while(previuosDirection == r){
        r = rand() % 4;
    }
    alien->dir = r;
}

/**
 * Funcion que restaura la posicion de un Alien a la que tenia
 * antes de colisionar con un objeto
 * alien: puntero del alien al cual restaurarle la posicion
**/
void restorePosition(Alien* alien) {
    // Se verifica si el Alien se ha movido
    if(alien->y % BLOCK_SIZE != 0) {
        if (alien->dir == DOWN){
            alien->y -= moveSpeed;
        } else if (alien->dir == UP) {
            alien->y += moveSpeed;
        }
    }
    // Se verifica si el Alien se ha movido
    if(alien->x % BLOCK_SIZE != 0) {
        if (alien->dir == LEFT) {
            alien->x += moveSpeed;
        } else if (alien->dir == RIGHT) {
            alien->x -= moveSpeed;
        }
    }
}

void moveAlien() {
    for(int i = 0; i < alienCount; i++) {
        Alien *alien = &aliens[i];
        // Se verifica que el Alien se encuentre activo
        if(alien->isActive) {
            switch (alien->dir) {
            case DOWN:
                alien->y += moveSpeed;
                break;
            case UP:
                alien->y -= moveSpeed;
                break; 
            case RIGHT:
                alien->x += moveSpeed;
                break;
            case LEFT:
                alien->x -= moveSpeed;
                break;
            default:
                break;
            }
        }
        // Como solo un Alien deberia moverse a la vez, se 
        // hace un break para descartar los demas casos
        //break;
    }
}

void updateEnergy() {
    for(int i = 0; i < alienCount; i++) {
        Alien *alien = &aliens[i];
        // Se verifica que el Alien se encuentre activo
        if(alien->isActive) {
            alien->energy -= 1;
            // Se verifica el nivel de energia del Alien
            if(alien->energy == 0) {
                alien->isActive = 0;
            }
        }
        // Como solo un Alien deberia moverse a la vez, se 
        // hace un break para descartar los demas casos
        break;
    }
}