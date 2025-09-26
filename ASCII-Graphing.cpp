#include <vector>
#include <cmath>
#include <limits>
#include <string>
#include <iostream>

int screenWidth = 51;
int screenHeight = 21;
float stepSize = .1;

float eqConst = -9.0;
std::vector<float> varConst = {0.0, 0.0, 0.0};
std::vector<float> varSqrdConst = {1.0, 1.0, 1.0};

float traceDist(float ox, float oy, float oz, float dx, float dy, float dz) {
    float ax = varSqrdConst[0], ay = varSqrdConst[1], az = varSqrdConst[2];
    float bx = varConst[0], by = varConst[1], bz = varConst[2];
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

std::string output = "";

int main(){
    for (int i = 0; i < screenHeight; i++){
        for (int j = 0; j < screenWidth; j++){
            float hDisp = (i - screenHeight/2.0)*stepSize;
            float vDisp = -(j - screenWidth/2.0)*stepSize/2.0;
            float dist = traceDist
            (
                -5.0,-5.0,-5.0,
                3-std::sqrt(2)*hDisp-std::sqrt(2)*vDisp, 3+std::sqrt(2)*hDisp-std::sqrt(2)*vDisp, 3+vDisp
            );
            if (dist == std::numeric_limits<float>::infinity()){
                output += ".";
            } else if (dist < 0.5){
                output += "@";
            } else if (dist < 1.25){
                output += "%";
            } else if (dist < 1.75){
                output += "#";
            } else if (dist < 2.25){
                output += "+";
            } else{
                output += "=";
            }
        }
        output += "\n";
    }
    std::cout << output;
}