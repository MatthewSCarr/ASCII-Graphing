#define _USE_MATH_DEFINES
#include <cmath>
#include <limits>
#include <string>
#include <iostream>

struct Vector3D {
    float x, y, z;
};

int screenWidth = 50;
int screenHeight = 20;
float stepSizeAng = .2;
std::string output = "";

float eqConst = -1.0;
Vector3D varConst = {0.0, 0.0, 0.0};
Vector3D varSqrdConst = {-1.0, 1.0, 1.0};
Vector3D zAxis = {0.0, 0.0, 1.0};

float degToRad(float deg) {
    return deg * (M_PI/180.0);
}

inline Vector3D crossProductUnit(const Vector3D& v1, const Vector3D& v2) {
    float cx = v1.y * v2.z - v1.z * v2.y;
    float cy = v1.z * v2.x - v1.x * v2.z;
    float cz = v1.x * v2.y - v1.y * v2.x;
    float magSq = cx*cx + cy*cy + cz*cz;

    if (magSq == 0.0f) {
        return {0.0f, 0.0f, 0.0f};
    }
    float invMag = 1.0f / std::sqrt(magSq);
    return { cx * invMag, cy * invMag, cz * invMag };
}
float moveAng(float ox, float oy, float oz, float deg){
    float magSide = sqrt(ox*ox + oy*oy + oz*oz);
    return tan(degToRad(deg))*magSide;
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

int main() {
    Vector3D origin = {1, 2, 3};
    Vector3D horizV = crossProductUnit(origin, zAxis);
    Vector3D vertV  = crossProductUnit(origin, horizV);

    float verticalScale = 2.6f;

    for (int i = 0; i < screenHeight; i++) {
        for (int j = 0; j < screenWidth; j++) {
            float hOffset = (j - screenWidth / 2.0f) * stepSizeAng;
            float vOffset = (i - screenHeight / 2.0f) * stepSizeAng * verticalScale;

            Vector3D screenPoint = {
                0 + hOffset * horizV.x + vOffset * vertV.x,
                0 + hOffset * horizV.y + vOffset * vertV.y,
                0 + hOffset * horizV.z + vOffset * vertV.z
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

            if (dist == std::numeric_limits<float>::infinity()) {
                output += ".";
            } else if (dist < 0.5f) {
                output += "@";
            } else if (dist < 1.25f) {
                output += "%";
            } else if (dist < 2.75f) {
                output += "#";
            } else if (dist < 4.25f) {
                output += "+";
            } else {
                output += "=";
            }
        }
        output += "\n";
    }

    std::cout << output;
}