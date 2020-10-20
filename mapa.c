#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

const int mapsizex = 21;
const int mapsizey = 21;

#define SCREENWIDTH 630
#define SCREENHEIGHT 630
#define BLOCKSIZE 30

void load_map(char *filename, int** map, int rows, int columns)
{   
    FILE *fptr;
    char *buffer = (char*) malloc(1);
    int compare;
    int lcountx = 0;
    int lcounty = 0;

    if ((fptr =fopen(filename, "r")) == NULL){
        printf("Error al abrir el archivo especificado \n\n");
        exit(1);
    }
    while(1)
    {   
        if(feof(fptr))
        {
            break;
        }
        fread(buffer, sizeof(char), 1, fptr);
        compare = strcmp(buffer, "\n");
        if (compare != 0){
            //printf("Buffer: %s\n", buffer);
            //printf("Countx: %d, County: %d\n", lcountx, lcounty);

            map[lcountx][lcounty] = atoi(buffer);
            lcounty += 1;

            if(lcounty > columns){
                lcounty = 0;
                lcountx += 1;
            }
        }

    }
    free(buffer);
    fclose(fptr);
}

void drawmap(int **map)
{
    for (int i = 0; i < mapsizex; i++)
    {
        for (int j= 0; j < mapsizey; j++)
        {
            if(map[i][j] == 1)
            {
                al_draw_filled_rectangle(i *BLOCKSIZE, j * BLOCKSIZE,
                    i * BLOCKSIZE + BLOCKSIZE, j * BLOCKSIZE + BLOCKSIZE, al_map_rgb(255,255,255));
            }
        }
    }
}

int main()
{   
    char path[256];
    printf("Please enter the path of the file: ");
    scanf("%s", path);

    int rows = mapsizex;
    int columns = mapsizey;
    
    //Crear dinamicamente un arreglo de punteros de tamano rows
    int **map = (int **)malloc(rows * sizeof(int *));
    
    //Asignar memoria dinamicamente de tamano mapsizey para cada fila
    for (int r = 0; r < rows; r++)
        map[r] = (int *)malloc(columns * sizeof(int));

    load_map(path, map, rows, columns);

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

    al_init_primitives_addon();

    while(1)
    {
        drawmap(map);
    }

    al_rest(10.0);
    al_destroy_display(display);

    //Liberar memoria
    for (int i = 0; i < rows; i++)
        free(map[i]);

    free(map);

    return 0;
}