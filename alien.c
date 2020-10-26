#include <stdio.h>
#include <pthread.h>

#define maxAliens 30
#define moveSpeed 4;

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

Alien* aliens;
pthread_mutex_t aliens_mutex[maxAliens];
pthread_t threads_id[maxAliens];

pthread_cond_t clock_cond;
pthread_mutex_t clock_mutex;
long int clk = 0;

void *alienLoop(void* params);
void updateEnergy(Alien* alien);
void updateRegenerationTimer(Alien* alien);

/**
 * Funcion para crear un alien nuevo y agregarlo a la lista
 * period: Periodo que debe tener el nuevo alien
 * energy: Energia que debe tener el nuevo alien
**/ 
/**
 * Funcion para crear un alien nuevo y agregarlo a la lista
 * period: Periodo que debe tener el nuevo alien
 * energy: Energia que debe tener el nuevo alien
**/ 
int createAlien(int period, int energy, int x, int y, int sourceX) {
    Alien alien;
    alien.id = alienCount+1;// Identificador del alien
    alien.x = x;            // Posicion en x del punto de partida
    alien.y = y;            // Posicion en y del punto de partida
    alien.dir = DOWN;       // Direccion de movimiento inicial
    alien.sourceX = sourceX;
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
    alienCount++;
    return alien.id;
}

void *alienLoop(void* params){
    Alien *alien = (Alien*) params;
    
    pthread_mutex_lock(&clock_mutex);
    long int clkCounter = clk+1;
    pthread_mutex_unlock(&clock_mutex);
    
    while(1) {
        // Se espera a recibir un cambio del clock
        pthread_mutex_lock(&clock_mutex);
        while (clkCounter != clk) {
            pthread_cond_wait(&clock_cond, &clock_mutex);
            //printf("counter: %ld - clk: %ld\n", clkCounter, clk);
        }
        pthread_mutex_unlock(&clock_mutex);

        clkCounter++; // Aumentar contador de tiempo
        //printf("%d\n", alien->id);
        updateEnergy(alien);
        updateRegenerationTimer(alien);
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
        if(alien->energyCounter == 0) alien->isActive = 0;
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