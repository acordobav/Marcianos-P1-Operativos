#include <stdio.h>
#include "alien.c"

pthread_t schedulerThread;

void *rms();
void updateReport(FILE *fptr);

void scheduler(int algorithm) {
    pthread_cond_init(&updatedCompleteCond, NULL);
    pthread_mutex_init(&updatedCompleteMutex, NULL);
    pthread_create(&schedulerThread, NULL, rms, NULL);
}

void *rms(void* args) {
    FILE *fptr = fopen("report.txt","w");

    while(1) {
        pthread_mutex_lock(&alienCountMutex);
        int aliensCount = alienCount;
        pthread_mutex_unlock(&alienCountMutex);

        // Variables que almacenan el ID del alien con el menor periodo
        int shorterPeriod = __INT_MAX__;
        int idShorterPeriod = 0;

        // Se busca el Alien con menor periodo y disponible para ejecutarse
        for (int i = 0; i < aliensCount; i++) {
            pthread_mutex_lock(&aliens_mutex[i]);
            Alien *alien = &aliens[i];
            alien->isActive = 0;

            // Se busca el proceso con mayor prioridad
            // Se busca Alien no finalizado, regenerado y con menor periodo
            if(!alien->isFinished && alien->isAvailable && alien->period < shorterPeriod) {
                shorterPeriod = alien->period;
                idShorterPeriod = alien->id;
            }
            pthread_mutex_unlock(&aliens_mutex[i]);
        }
        // Se activa el Alien que debe ejecutarse
        if(idShorterPeriod != 0) {
            pthread_mutex_lock(&aliens_mutex[idShorterPeriod-1]);
            Alien *alien = &aliens[idShorterPeriod - 1];
            alien->isActive = 1;
            pthread_mutex_unlock(&aliens_mutex[idShorterPeriod-1]);
        }

        // Se actualiza el reporte
        updateReport(fptr);

        // Se espera a que todos los Aliens se actualicen en el siguiente clk
        pthread_mutex_lock(&updatedCompleteMutex);
        while (updatedCount < aliensCount) {
            pthread_cond_wait(&updatedCompleteCond, &updatedCompleteMutex);
        }
        updatedCount = 0;
        pthread_mutex_unlock(&updatedCompleteMutex);

    }
    fclose(fptr);
}

void updateReport(FILE *fptr) {
    pthread_mutex_lock(&alienCountMutex);
    int aliensCount = alienCount;
    pthread_mutex_unlock(&alienCountMutex);

    int id = 0;
    // Se obtiene el identificador del Alien activo
    for(int i = 0; i < aliensCount; i++) {
        pthread_mutex_lock(&aliens_mutex[i]);
        Alien alien = aliens[i];
        pthread_mutex_unlock(&aliens_mutex[i]);
        if(alien.isActive) {
            id = alien.id;
            break;
        }
    }
    fprintf(fptr, "%d", id); // Escritura del identificador
    fwrite(" ", 1, 1, fptr); // Escritura de un espacio en blanco
}