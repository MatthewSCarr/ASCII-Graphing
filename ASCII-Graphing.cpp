#define _USE_MATH_DEFINES
#include <cmath>
#include <limits>
#include <string>
#include <iostream>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL.h>

struct Vector3D {
    float x, y, z;
};


int const screenWidth = 110;
int const screenHeight = 55;
int const charW = 12, const charH = 18;
float const stepSizeAng = 0.33f;
float const mvStep = 0.03f;
float const zoomStep = 0.1f;
std::string output = "";

float eqConst = -9.0f;
Vector3D varConst = { 0.0f, 0.0f, 0.0f };
Vector3D varSqrdConst = { 1.0f, -1.0f,1.0f };
Vector3D const zAxis = { 0.0f, 0.0f, 1.0f };

float degToRad(float deg) {
    return deg * (M_PI / 180.0f);
}

inline Vector3D crossProductUnit(const Vector3D& v1, const Vector3D& v2) {
    float cx = v1.y * v2.z - v1.z * v2.y;
    float cy = v1.z * v2.x - v1.x * v2.z;
    float cz = v1.x * v2.y - v1.y * v2.x;
    float magSq = cx * cx + cy * cy + cz * cz;
    if (magSq == 0.0f) return { 0.0f, 0.0f, 0.0f };
    float invMag = 1.0f / std::sqrt(magSq);
    return { cx * invMag, cy * invMag, cz * invMag };
}

static float traceDist(float ox, float oy, float oz, float dx, float dy, float dz) {
    float ax = varSqrdConst.x, ay = varSqrdConst.y, az = varSqrdConst.z;
    float bx = varConst.x, by = varConst.y, bz = varConst.z;
    float c = eqConst;

    float A = ax * dx * dx + ay * dy * dy + az * dz * dz;
    float B = 2 * ax * ox * dx + 2 * ay * oy * dy + 2 * az * oz * dz + bx * dx + by * dy + bz * dz;
    float C = ax * ox * ox + ay * oy * oy + az * oz * oz + bx * ox + by * oy + bz * oz + c;

    float disc = B * B - 4 * A * C;
    if (disc < 0.0f) return std::numeric_limits<float>::infinity();

    float sqrtDisc = std::sqrt(disc);
    float t1 = (-B - sqrtDisc) / (2 * A);
    float t2 = (-B + sqrtDisc) / (2 * A);

    if (t1 > 0.0f && (t2 < 0.0f || t1<=t2)) return t1;
    if (t2 > 0.0f && (t1 < 0.0f || t2<=t1)) return t2;
    return std::numeric_limits<float>::infinity();
}

static void moveOrigin(Vector3D& origin, Vector3D& mvDir, float currMag) {
    origin.x = origin.x + mvDir.x * mvStep * currMag;
    origin.y = origin.y + mvDir.y * mvStep * currMag;
    origin.z = origin.z + mvDir.z * mvStep * currMag;
    float multiplier = abs(currMag) / sqrt(origin.x * origin.x + origin.y * origin.y + origin.z * origin.z);
    origin.x = origin.x * multiplier;
    origin.y = origin.y * multiplier;
    origin.z = origin.z * multiplier;
}

static void changeZoom(Vector3D& origin, float currMag, float incriment) {
    float multiplier = (currMag + incriment) / currMag;
    origin.x = origin.x * multiplier;
    origin.y = origin.y * multiplier;
    origin.z = origin.z * multiplier;
}

int main() {
    std::cout << "WARNING: I WAS TOO LAZY TO ADD PROTECTION FROM MISINPUTS" << std::endl;
    std::cout << "Inputs are in this form: Ax^2+ax+By^2+by+Cz^2+cz+d=0" << std::endl;
    std::cout << "Please enter a value for A: ";
    std::cin >> varSqrdConst.x;
    std::cout << "Please enter a value for a: ";
    std::cin >> varConst.x;
    std::cout << "Please enter a value for B: ";
    std::cin >> varSqrdConst.y;
    std::cout << "Please enter a value for b: ";
    std::cin >> varConst.y;
    std::cout << "Please enter a value for C: ";
    std::cin >> varSqrdConst.z;
    std::cout << "Please enter a value for c: ";
    std::cin >> varConst.z;
    std::cout << "Please enter a value for D: ";
    std::cin >> eqConst;

    Vector3D origin = { 4, 4, 4 };

    SDL_Init(SDL_INIT_VIDEO);

    TTF_Init();

    TTF_Font* font = TTF_OpenFont("C:/Windows/Fonts/consola.ttf", 20);

    int minx, maxx, miny, maxy, advance;
    TTF_GetGlyphMetrics(font, 'A', &minx, &maxx, &miny, &maxy, &advance);

    SDL_Window* window = SDL_CreateWindow("ASCII Renderer", screenWidth * advance, screenHeight * charH, SDL_WINDOW_RESIZABLE);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    bool done = false;
    while (!done) {
        Vector3D horizV = crossProductUnit(origin, zAxis);
        Vector3D vertV = crossProductUnit(origin, horizV);
        float verticalScale = 2.6f;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            float currMag = sqrt(origin.x * origin.x + origin.y * origin.y + origin.z * origin.z);
            if (event.type == SDL_EVENT_QUIT) done = true;
            if (event.type == SDL_EVENT_KEY_DOWN) {
                switch (event.key.key) {
                    case SDLK_A: moveOrigin(origin, horizV, -currMag); break;
                    case SDLK_D: moveOrigin(origin, horizV, currMag); break;
                    case SDLK_W: moveOrigin(origin, vertV, -currMag); break;
                    case SDLK_S: moveOrigin(origin, vertV, currMag); break;
                    case SDLK_UP: changeZoom(origin, currMag, -zoomStep); break;
                    case SDLK_DOWN: changeZoom(origin, currMag, zoomStep); break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        output.clear();

        for (int i = 0; i < screenHeight; i++) {
            for (int j = 0; j < screenWidth; j++) {
                float hOffset = (j - screenWidth / 2.0f) * stepSizeAng;
                float vOffset = (i - screenHeight / 2.0f) * stepSizeAng * verticalScale;
                Vector3D screenPoint = {
                    hOffset * horizV.x + vOffset * vertV.x,
                    hOffset * horizV.y + vOffset * vertV.y,
                    hOffset * horizV.z + vOffset * vertV.z
                };

                Vector3D dir = {
                    screenPoint.x - origin.x,
                    screenPoint.y - origin.y,
                    screenPoint.z - origin.z
                };

                float mag = std::sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
                if (mag > 0.0f) {
                    dir.x /= mag;
                    dir.y /= mag;
                    dir.z /= mag;
                }

                float dist = traceDist(origin.x, origin.y, origin.z, dir.x, dir.y, dir.z);

                if (dist == std::numeric_limits<float>::infinity()) output += " ";
                else if (dist < 0.50f) output += "@";
                else if (dist < 1.25f) output += "%";
                else if (dist < 2.25f) output += "$";
                else if (dist < 3.25f) output += "#";
                else if (dist < 4.25f) output += "=";
                else if (dist < 5.25f) output += "+";
                else if (dist < 6.25f) output += ">";
                else if (dist < 10.0f) output += "-";
                else output += ".";
            }
            output += "\n";
        }

        SDL_Color white = { 255,255,255,255 };
        SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(font, output.c_str(), SDL_strlen(output.c_str()), white, screenWidth * charW);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);
        SDL_FRect dstRect = {
            0.0f, 0.0f,
            static_cast<float>(screenWidth * charW),
            static_cast<float>(screenHeight * charH)
        };
        SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
        SDL_DestroyTexture(texture);

        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}