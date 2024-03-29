#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define X_OFFSET 30
#define Y_OFFSET 30
#define BLOCK_SIZE 30


struct Flag{
    int x, y;
};
typedef struct Wall{
    int x, y;  // Posicion en los ejes "x" y "y"
} Wall;

Wall* walls; // Lista con los muros

char* readFile(char* filename);
int countWalls(int** map, int rows, int columns);
Wall* createMap();
int wallCounter = 0;
struct Flag *flags;
/**
 * Funcion para crear una lista con todos los muros del laberinto
**/
Wall* createMap() {
    // Lectura del archivo para obtener el mapa
    char* filename = "map.txt";
    char* filecontent = readFile(filename);

    // Se copia el valor para poder obtener las dimensiones del mapa
    char* mapString = (char*) malloc(strlen(filecontent) + 1);
    strcpy(mapString, filecontent);

    // Obtener del numero de filas y columnas
    char* token = strtok(filecontent, "\n");
    int rows = atoi(strtok(token, " "));
    int columns = atoi(strtok(NULL, " "));

    // Construccion del mapa
    token = strtok(mapString, "\n");    // Se separa la primera fila del archivo
    char value[2];                      
    value[1] = '\0'; 
    int map[rows][columns];
    int totalWalls = 0; // Numero total de muros en el laberinto

    flags = malloc(sizeof(*flags)*2);
    struct Flag startFlag;
    struct Flag endFlag;
    // Construccion de una matriz donde un 1 es un muro y un 0 es un camino
    for (int i = 0; i < rows; i++) {
        token = strtok(NULL, "\n");    // Se obtiene una fila del archivo
        for (int j = 0; j < columns; j++) {    
            value[0] = token[j];
            int num = atoi(value);

            switch (num) {
            case 1: // Caso en el que se trata de un muro
                map[i][j] = num;
                totalWalls += num;
                break;
            case 2: // Caso en el que se trata de la bandera inicial
                startFlag.x = X_OFFSET + j * BLOCK_SIZE;  // Calculo de la posicion en X
                startFlag.y = Y_OFFSET + i * BLOCK_SIZE;  // Calculo de la posicion en Y
                map[i][j] = 0;
                break;
            case 3: // Caso en el que se trata de la bandera final
                endFlag.x = X_OFFSET + j * BLOCK_SIZE + BLOCK_SIZE;  // Calculo de la posicion en X
                endFlag.y = Y_OFFSET + i * BLOCK_SIZE;  // Calculo de la posicion en Y
                map[i][j] = 0;
                break;
            default: // Caso en el que se trata de un camino
                map[i][j] = 0;
            }
        }
    }
    flags[0] = startFlag;
    flags[1] = endFlag;
    
    // Se construye una lista con todos los muros del laberinto
    walls = malloc(sizeof(*walls)*totalWalls);
    wallCounter = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {    
            if(map[i][j]) { // Verifica si en la casilla se debe colocar un muro
                struct Wall wallEntity;  // Se crea un nuevo muro
                wallEntity.x = X_OFFSET + j * BLOCK_SIZE;  // Calculo de la posicion en X
                wallEntity.y = Y_OFFSET + i * BLOCK_SIZE;  // Calculo de la posicion en Y
                walls[wallCounter] = wallEntity;  // Alcenamiento en una lista de muros
                wallCounter++;  // Aumentar contador de muros
            }
        }
    }
    free(filecontent);
    free(mapString);
    return walls;
}

/**
 * Funcion para leer todo el contenido de un archivo de texto
**/ 
char* readFile(char* filename) {
    FILE *pFile;
    if ((pFile = fopen(filename, "r")) == NULL){ // Apertura del archivo en modo lectura
        printf("Error al abrir el archivo especificado \n\n");
        exit(1);
    }
    fseek(pFile, 0, SEEK_END);          // Se mueve puntero a final del archivos
    long fsize = ftell(pFile);          // Obtener largo del archivo
    fseek(pFile, 0, SEEK_SET);          // Mover puntero al inicio del archivo
    char *content = malloc(fsize + 1);  
    fread(content, 1, fsize, pFile);    // Lectura de todo el contenido del archivo
    fclose(pFile);

    return content;
}
