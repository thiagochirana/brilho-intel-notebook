#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 400
#define HEIGHT 200

const char* BRIGHTNESS_FILE = "/sys/class/backlight/intel_backlight/brightness";
const char* MAX_BRIGHTNESS_FILE = "/sys/class/backlight/intel_backlight/max_brightness";

int read_int_from_file(const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) return -1;
    int value;
    fscanf(f, "%d", &value);
    fclose(f);
    return value;
}

void write_int_to_file(const char* path, int value) {
    FILE* f = fopen(path, "w");
    if (!f) return;
    fprintf(f, "%d", value);
    fclose(f);
}

void render_text(SDL_Renderer* ren, TTF_Font* font, const char* text, int x, int y) {
    SDL_Color color = {255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, surface);
    SDL_Rect dst = {x, y, surface->w, surface->h};
    SDL_RenderCopy(ren, texture, NULL, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* win = SDL_CreateWindow("Controle de Brilho", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_BORDERLESS);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSans.ttf", 16);
    if (!font) {
        fprintf(stderr, "Erro ao carregar fonte: %s\n", TTF_GetError());
        return 1;
    }

    int max = read_int_from_file(MAX_BRIGHTNESS_FILE);
    int original = read_int_from_file(BRIGHTNESS_FILE);
    int current = original;

    SDL_Event e;
    int quit = 0;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                quit = 1;
            else if (e.type == SDL_MOUSEBUTTONDOWN || (e.type == SDL_MOUSEMOTION && (e.motion.state & SDL_BUTTON_LMASK))) {
                int mx = e.motion.x;
                int my = e.motion.y;

                // slider area
                if (my > 70 && my < 100) {
                    if (mx > 20 && mx < WIDTH - 20) {
                        current = max * (mx - 20) / (WIDTH - 40);
                    }
                }

                // save button
                if (mx > 80 && mx < 180 && my > 140 && my < 170) {
                    write_int_to_file(BRIGHTNESS_FILE, current);
                    quit = 1;
                }

                // cancel button
                if (mx > 220 && mx < 320 && my > 140 && my < 170) {
                    quit = 1;
                }
            }
        }

        SDL_SetRenderDrawColor(ren, 25, 25, 25, 255);
        SDL_RenderClear(ren);

        // título
        render_text(ren, font, "Controle de Brilho", 20, 20);

        // slider fundo
        SDL_SetRenderDrawColor(ren, 70, 70, 70, 255);
        SDL_Rect slider_bg = {20, 80, WIDTH - 40, 20};
        SDL_RenderFillRect(ren, &slider_bg);

        // slider preenchido
        SDL_SetRenderDrawColor(ren, 180, 180, 255, 255);
        SDL_Rect slider_fill = {20, 80, (WIDTH - 40) * current / max, 20};
        SDL_RenderFillRect(ren, &slider_fill);

        // percentual
        char percent[16];
        snprintf(percent, sizeof(percent), "%d%%", current * 100 / max);
        render_text(ren, font, percent, WIDTH - 60, 50);

        // botões
        SDL_Rect save_btn = {80, 140, 100, 30};
        SDL_Rect cancel_btn = {220, 140, 100, 30};

        SDL_SetRenderDrawColor(ren, 0, 120, 0, 255);
        SDL_RenderFillRect(ren, &save_btn);
        render_text(ren, font, "Salvar", 105, 147);

        SDL_SetRenderDrawColor(ren, 120, 0, 0, 255);
        SDL_RenderFillRect(ren, &cancel_btn);
        render_text(ren, font, "Cancelar", 235, 147);

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
