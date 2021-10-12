#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "SDL2/SDL.h"
#include "algorithms.h"

#define ARRAY_LEN 256
#define THING 256

#define AUTHOR "Adolfo pinto"
#define VERSION "1.0.0"

#define COPYRIGHT \
    "Copyright (C) 2015 Free Software Foundation, Inc.\n" \
    "This is free software; see the source for copying conditions.  There is NO\n" \
    "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"

#define OPT(a,b,c) (!(strcmp(a,b) && strcmp(a,c)))

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int *array, int l, int r)
{
    int pivot = array[l];
    int i = l;
    int j = r + 1;

    for (;;) {
        do {
            ++i;
        } while (array[i] <= pivot && i <= r);

        do {
            --j;
        } while (array[j] > pivot);

        if (i >= j) {
            break;
        }
        swap(&array[i], &array[j]);
    }
    swap(&array[l], &array[j]);
    return j;
}

void quick_sort(SDL_Renderer *renderer, int *array, int l, int r)
{
    if (l < r) {
        int j = partition(array, l, r);
        quick_sort(renderer, array, l, j - 1);
        quick_sort(renderer, array, j + 1, r);
        render(renderer, array);
    }
}

void bubble_sort(SDL_Renderer *renderer, int *array, int n)
{
    int i, j;
    for (i = 1; i < n; i++) {
        for (j = 0; j < n - 1; j++) {
            if (array[j] > array[j + 1]) {
                swap(&array[j], &array[j + 1]);
                render(renderer, array);
            }
        }
    }
}

void insertion_sort(SDL_Renderer *renderer, int *array, int n)
{
    int i, j, temp;

    for (j = 1; j < n; j++) {
        temp = array[j];
        i = j - 1;

        while (i >= 0 && array[i] > temp) {
            array[i + 1] = array[i];
            render(renderer, array);
            i--;
        }

        array[i + 1] = temp;
    }
}

void selection_sort(SDL_Renderer *renderer, int *array, int n)
{
    int i, j, temp;

    for (i = 0; i < (n - 1); i++) {
        temp = i;
        for (j = (i + 1); j < n; j++) {
            if (array[j] < array[temp]) {
                temp = j;
            }
        }

        if (i != temp) {
            swap(&array[i], &array[temp]);
            render(renderer, array);
        }
    }
}

void merge_sort(SDL_Renderer *renderer, int *array, int start_pos, int end_pos)
{
    int i, j, k, half_size;
    int *vetor_temp;

    if (start_pos == end_pos) {
        return;
    }

    /* recursive _sort of two halfs */
    half_size = (start_pos + end_pos) / 2;
    merge_sort(renderer, array, start_pos, half_size);
    merge_sort(renderer, array, half_size + 1, end_pos);


    i = start_pos;
    j = half_size + 1;
    k = 0;
    vetor_temp = malloc(sizeof(int) * (end_pos - start_pos + 1));

    while (i < half_size + 1 || j  < end_pos + 1) {
        if (i == half_size + 1) {
            vetor_temp[k] = array[j];
            j++, k++;
        } else {
            if (j == end_pos + 1) {
                vetor_temp[k] = array[i];
                i++, k++;
            } else {
                if (array[i] < array[j]) {
                    vetor_temp[k] = array[i];
                    i++, k++;
                } else {
                    vetor_temp[k] = array[j];
                    j++, k++;
                }
            }
        }
    }

    for (i = start_pos; i <= end_pos; i++) {
        array[i] = vetor_temp[i - start_pos];
    }

    render(renderer, array);
    free(vetor_temp);
}

void random_vals(int max_val, int arr_len, int *arr)
{
    int i;
    for (i = 0; i < arr_len; i++) {
        arr[i] = (i * max_val) / arr_len;
    }
    for (i = 0; i < arr_len; i++) {
        swap(&arr[rand() % arr_len], &arr[i]);
    }
}

void render(SDL_Renderer *renderer, int *arr)
{
    int i;
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

    for (i = 0; i < ARRAY_LEN; i++) {

        SDL_SetRenderDrawColor(renderer, 0, 127, 255, 255);
        SDL_RenderDrawLine(renderer, i, THING, i, arr[i]);
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(2);
}

int usage(const char *prgm)
{
    printf("usage: %s [OPTIONS]\n"
           "Algorithm visualization in SDL\n"
           "default: 256 random numbers"
           "\n"
           "options:\n"
           "  -h, --help\t\tshow usage text\n"
           "  -v, --version\t\tdisplay version info\n"
           "  -q, --quicksort \truns quicksort algorithm\n"
           "  -b, --bubblesort \truns bubblesort algorithm\n"
           "  -s, --selectionsort \truns selection sort algorithm\n"
           "  -i, --insertionsort \truns insertion sort algorithm\n"
           "  -m, --mergesort \truns merge sort algorithm\n", prgm);
    return EXIT_SUCCESS;
}

int version(const char *prgm)
{
    printf("%s " VERSION "\n" COPYRIGHT, prgm);
    return 0;
}

int main(int argc, char **argv)
{
    char *prgm;
    int i, arr_len;

    int *array;

    SDL_Renderer *renderer;
    SDL_Window *window;

    struct {
        char *algorithm;
        int pos;
    } option;

    srand(time(NULL));


    option.algorithm = NULL;
    option.pos = -1;

    prgm = argv[0];

    for (i = 1; i < argc; i++) {
        if (OPT(argv[i], "-h", "--help")) {
            return usage(prgm);
        } else if (OPT(argv[i], "-v", "--version")) {
            return version(prgm);
        } else if (OPT(argv[i], "-q", "--quicksort")) {
            option.algorithm = "Quick sort algorithm";
            option.pos = 1;
        } else if (OPT(argv[i], "-b", "--bubblesort")) {
            option.algorithm = "Bubble sort algorithm";
            option.pos = 2;
        } else if (OPT(argv[i], "-s", "--selectionsort")) {
            option.algorithm = "Selection sort algorithm";
            option.pos = 3;
        } else if (OPT(argv[i], "-i", "--insertionsort")) {
            option.algorithm = "Insertion sort algorithm";
            option.pos = 4;
        } else if (OPT(argv[i], "-m", "--mergesort")) {
            option.algorithm = "Merge sort algorithm";
            option.pos = 5;
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "%s: invalid option: %s\n", prgm, argv[i]);
            return EXIT_FAILURE;
        } else {
            fprintf(stderr, "%s: no arguments supplied.\n", prgm);
            return EXIT_FAILURE;
        }
    }

    if (option.pos < 0) {
        return usage(prgm);
    }

    SDL_Init(SDL_INIT_VIDEO);

    arr_len = ARRAY_LEN;

    array = malloc(arr_len * sizeof(int));

    SDL_CreateWindowAndRenderer(arr_len, THING, 0, &window, &renderer);
    SDL_SetWindowTitle(window, option.algorithm);

    random_vals(THING, arr_len, array);
    if (option.pos == 1) {
        quick_sort(renderer, array, 0, arr_len);
    } else if (option.pos == 2) {
        bubble_sort(renderer, array, arr_len);
    } else if (option.pos == 3) {
        selection_sort(renderer, array, arr_len);
    } else if (option.pos == 4) {
        insertion_sort(renderer, array, arr_len);
    } else if (option.pos == 5) {
        merge_sort(renderer, array, 0, arr_len - 1);
    }

    SDL_Delay(1000);

    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}
