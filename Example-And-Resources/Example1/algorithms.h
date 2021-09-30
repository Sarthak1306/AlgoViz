#define _ALGORITHMS_H

#include "SDL2/SDL.h"

void render(SDL_Renderer *, int *);
void swap(int *, int *);
int partition(int *, int, int);
void quickSort(SDL_Renderer *, int *, int, int);
void stoogeSort(SDL_Renderer *, int *, int , int);

/* #endif  _ALGORITHMS_H */
