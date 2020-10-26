#include <stdio.h>
#include <pthread.h>
#include "createmap.c"

#define maxAliens 30
#define moveSpeed 4
#define gameFPS 30

typedef enum {UP = 3, DOWN = 0, LEFT = 1, RIGHT = 2} Direction;
typedef struct Alien {
    int id;
    int x,y, sourceX;
    Direction dir;
    int isActive;
    int energy;
    int period;
    int isFinished;
    int regenerationTimer;
    int energyCounter;
    int isAvailable;
} Alien;

int alienCount = 0;
pthread_mutex_t alienCountMutex;

Alien* aliens;
pthread_mutex_t aliens_mutex[maxAliens];
pthread_t threads_id[maxAliens];

pthread_cond_t clock_cond;
pthread_mutex_t clock_mutex;

pthread_cond_t updatedCompleteCond;
pthread_mutex_t updatedCompleteMutex;
int updatedCount = 0;

void *alienLoop(void* params);
void updateEnergy(Alien* alien);
void updateRegenerationTimer(Alien* alien);
void moveAlien(Alien* alien);
bool isCollisioned(int x1, int x2, int y1, int y2);
void checkCollisions(Alien* alien);
void restorePosition(Alien* alien);
void restorePosition(Alien* alien);
void getNewDirection(Alien* alien);

int frameControl = 0;

/**
 * Funcion para crear un alien nuevo y agregarlo a la lista
 * period: Periodo que debe tener el nuevo alien
 * energy: Energia que debe tener el nuevo alien
**/ 
int createAlien(int period, int energy) {
    Alien alien;
    alien.id = alienCount+1;// Identificador del alien
    alien.x = flags[0].x;            // Posicion en x del punto de partida
    alien.y = flags[0].y;            // Posicion en y del punto de partida
    alien.dir = DOWN;       // Direccion de movimiento inicial
    alien.sourceX = BLOCK_SIZE;
    alien.isActive = 0;     // Estado de activiacion inicial
    alien.period = period;  // Periodo del alien
    alien.energy = energy;  // Energia del alien
    alien.isFinished = 0;   // Estado de finalizacion no completado
    alien.regenerationTimer = period; // Contador de tiempo de regeneracion
    alien.energyCounter = energy; // Contador de energia
    alien.isAvailable = 1;
    aliens[alienCount] = alien;
    
    // Creacion de un mutex para el Alien
    pthread_mutex_t alienmutex;
    pthread_mutex_init(&alienmutex, NULL);
    aliens_mutex[alienCount] = alienmutex;
    
    // Creacion del hilo para el Alien
    pthread_create(&threads_id[alienCount], NULL, alienLoop, &aliens[alienCount]);
    
    // Aumentar contador de Aliens
    pthread_mutex_lock(&alienCountMutex);
    alienCount++;
    pthread_mutex_unlock(&alienCountMutex);
    return alien.id;
}

void *alienLoop(void* params){
    Alien *alien = (Alien*) params;
    
    pthread_mutex_lock(&clock_mutex);
    int internalFrameControl = frameControl;
    pthread_mutex_unlock(&clock_mutex);

    int executionCounter = gameFPS;
    
    while(1) {
        // Se espera a recibir un cambio del clock
        pthread_mutex_lock(&clock_mutex);
        while (internalFrameControl == frameControl) {
            pthread_cond_wait(&clock_cond, &clock_mutex);
        }
        internalFrameControl = frameControl;
        pthread_mutex_unlock(&clock_mutex);
        
        // Se mueve el Alien si se encuentra activo
        moveAlien(alien);
        checkCollisions(alien);

        // Se actualizan niveles de energia cada segundo
        executionCounter--;
        if(executionCounter <= 0) {
            // Actualizacion de niveles de energia
            updateEnergy(alien);
            updateRegenerationTimer(alien);
            // Se actualiza el contador de Aliens actualizados
            pthread_mutex_lock(&updatedCompleteMutex);
            updatedCount++;
            pthread_mutex_unlock(&updatedCompleteMutex);
            pthread_cond_broadcast(&updatedCompleteCond);

            // Restablecimiento del contador
            executionCounter = gameFPS;
        }



    }
}

/**
 * Funcion para actualizar el valor de energia de los Aliens
**/
void updateEnergy(Alien* alien) {
    pthread_mutex_lock(&aliens_mutex[alien->id-1]);
    // Se verifica que el Alien se encuentre activo
    if(alien->isActive) {
        // Se verifica el nivel de energia del Alien
        if(alien->energyCounter > 0) alien->energyCounter -= 1;
        if(alien->energyCounter == 0) {
            alien->isActive = 0;
            alien->isAvailable = 0;
        }
    }
    pthread_mutex_unlock(&aliens_mutex[alien->id-1]);
}

/**
 * Funcion para actualizar el contador de regeneracion de los Aliens
**/
void updateRegenerationTimer(Alien* alien) {
    pthread_mutex_lock(&aliens_mutex[alien->id-1]);
    // Se resta un segundo al contador de regeneracion     
    alien->regenerationTimer--;

    // Se verifica si el Alien se ha regenerado
    if(alien->regenerationTimer == 0) {
        alien->isAvailable = 1;
        //alien->isActive = 1;
        alien->energyCounter = alien->energy;
        alien->regenerationTimer = alien->period;
    }

    pthread_mutex_unlock(&aliens_mutex[alien->id-1]);
}

/**
 * Funcion para mover automaaticamente a los Aliens
**/
void moveAlien(Alien* alien) {
    pthread_mutex_lock(&aliens_mutex[alien->id-1]);
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
    pthread_mutex_unlock(&aliens_mutex[alien->id-1]);
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
 * Funcion para verificar si un alien ha colisionado con un objeto
**/
void checkCollisions(Alien* alien) {
    pthread_mutex_lock(&aliens_mutex[alien->id-1]);
    if (alien->isActive) {
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

        pthread_mutex_lock(&alienCountMutex);
        int aliensCount = alienCount;
        pthread_mutex_unlock(&alienCountMutex);
        
        // Colisiones con otros Aliens
        for (int j = 0; j < aliensCount; j++) {
            if(alien->id != j+1) {
                pthread_mutex_lock(&aliens_mutex[j]);
                Alien alien2 = aliens[j]; // Se obtiene un alien
                pthread_mutex_unlock(&aliens_mutex[j]);

                // Se verifica si los aliens estan colisionando y si tienen la misma direccion
                if(isCollisioned(alien->x, alien2.x, alien->y, alien2.y) && alien->dir == alien2.dir){
                    // Se restaura la posicion del alien
                    restorePosition(alien);
                    // Se calcula una nueva direccion para el Alien
                    getNewDirection(alien);
                }
            }
        }
        // Verificacion de llegada a la meta, colision con la bandera de finalizacion
        if(isCollisioned(alien->x, flags[1].x, alien->y, flags[1].y)) {
            alien->isActive = 0;
            alien->isFinished = 1;
            alien->isAvailable = 0;
            alien->x = 0;
            alien->y = 0;
        }
    }
    pthread_mutex_unlock(&aliens_mutex[alien->id-1]);
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