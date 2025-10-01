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

int screenWidth = 50;
int screenHeight = 20;
float stepSizeAng = 0.2f;
std::string output = "";

float eqConst = -1.0f;
Vector3D varConst = {0.0f, 0.0f, 0.0f};
Vector3D varSqrdConst = {-1.0f, 1.0f, 1.0f};
Vector3D zAxis = {0.0f, 0.0f, 1.0f};

float degToRad(float deg) {
    return deg * (M_PI / 180.0f);
}

inline Vector3D crossProductUnit(const Vector3D& v1, const Vector3D& v2) {
    float cx = v1.y * v2.z - v1.z * v2.y;
    float cy = v1.z * v2.x - v1.x * v2.z;
    float cz = v1.x * v2.y - v1.y * v2.x;
    float magSq = cx*cx + cy*cy + cz*cz;
    if (magSq == 0.0f) return {0.0f, 0.0f, 0.0f};
    float invMag = 1.0f / std::sqrt(magSq);
    return {cx * invMag, cy * invMag, cz * invMag};
}

float traceDist(float ox, float oy, float oz, float dx, float dy, float dz) {
    float ax = varSqrdConst.x, ay = varSqrdConst.y, az = varSqrdConst.z;
    float bx = varConst.x, by = varConst.y, bz = varConst.z;
    float c  = eqConst;

    float A = ax*dx*dx + ay*dy*dy + az*dz*dz;
    float B = 2*ax*ox*dx + 2*ay*oy*dy + 2*az*oz*dz + bx*dx + by*dy + bz*dz;
    float C = ax*ox*ox + ay*oy*oy + az*oz*oz + bx*ox + by*oy + bz*oz + c;

    float disc = B*B - 4*A*C;
    if (disc < 0.0f) return std::numeric_limits<float>::infinity();

    float sqrtDisc = std::sqrt(disc);
    float t1 = (-B - sqrtDisc) / (2*A);
    float t2 = (-B + sqrtDisc) / (2*A);

    if (t1 > 0.0f) return t1;
    if (t2 > 0.0f) return t2;
    return std::numeric_limits<float>::infinity();
}

int main(int argc, char* argv[]) {
     if (SDL_Init(SDL_INIT_VIDEO) != true) {
         std::cerr << "SDL_Init failed" << "\n";
         return 1;
     }

     if (TTF_Init() == -1) {
         std::cerr << "TTF_Init failed" << "\n";
         return 1;
     }

     SDL_Window* window = SDL_CreateWindow("ASCII Renderer", 800, 600, SDL_WINDOW_RESIZABLE);
     if (!window) {
         std::cerr << "Could not create window" << "\n";
         return 1;
     }

     SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
     if (!renderer) {
         std::cerr << "Could not create renderer" << "\n";
         return 1;
     }

     TTF_Font* font = TTF_OpenFont("C:/Windows/Fonts/consola.ttf", 20);
     if (!font) {
         std::cerr << "Failed to load font" << "\n";
         return 1;
     }

    bool done = false;
    while (!done) {
         SDL_Event event;
         while (SDL_PollEvent(&event)) {
             if (event.type == SDL_EVENT_QUIT) done = true;
         }

         SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
         SDL_RenderClear(renderer);
         output.clear();

        Vector3D origin = {1, 2, 1};
        Vector3D horizV = crossProductUnit(origin, zAxis);
        Vector3D vertV  = crossProductUnit(origin, horizV);
        float verticalScale = 2.6f;

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

                float mag = std::sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
                if (mag > 0.0f) {
                    dir.x /= mag;
                    dir.y /= mag;
                    dir.z /= mag;
                }

                float dist = traceDist(origin.x, origin.y, origin.z, dir.x, dir.y, dir.z);

                if (dist == std::numeric_limits<float>::infinity()) output += ".";
                else if (dist < 0.75f) output += "@";
                else if (dist < 1.25f) output += "%";
                else if (dist < 2.75f) output += "#";
                else if (dist < 4.25f) output += "+";
                else output += "=";
            }
            output += "\n";
        }

         //Render ASCII characters
         int charW = 24, charH = 36;
         for (int i = 0; i < screenHeight; i++) {
             for (int j = 0; j < screenWidth; j++) {
                 char c = output[i * (screenWidth + 1) + j];
                 if (c == '\n') continue;

                 std::string s(1, c);

                 SDL_Color white = { 255,255,255,255 };

                 SDL_Surface* surface = TTF_RenderText_Blended(font, s.c_str(), SDL_strlen(s.c_str()), white);
                 if (!surface) {
                     SDL_LogError(SDL_LOG_CATEGORY_ERROR, "TTF_RenderText failed: %s", SDL_GetError());
                     continue;
                 }

                 SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                 SDL_DestroySurface(surface);
                 if (!texture) {
                     SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
                     continue;
                 }

                 SDL_FRect dstRect = {
                    static_cast<float>(j * charW),
                    static_cast<float>(i * charH),
                    static_cast<float>(charW),
                    static_cast<float>(charH)
                 };
                 if (SDL_RenderTexture(renderer, texture, nullptr, &dstRect) != true) {
                     SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_RenderCopy failed: %s", SDL_GetError());
                 }
                 SDL_DestroyTexture(texture);
             }
         }

         SDL_RenderPresent(renderer);
    }

     TTF_CloseFont(font);
     SDL_DestroyRenderer(renderer);
     SDL_DestroyWindow(window);
     TTF_Quit();
     SDL_Quit();
    return 0;
}