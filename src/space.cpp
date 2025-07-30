#include "scenes.h"
#include <GL/glut.h>
#include <cstdlib>
#include <cmath>

// Declare external global variables from main.cpp
extern float rocketY;
extern int frameCount;
extern bool earthLaunch;
extern float flameSize;

// Declare external helper functions implemented in main.cpp
void drawStars();
void drawRocket();
void drawFlame();
void drawBackground();

// Meteor position arrays and metadata
static float* startX = nullptr;
static float* startY = nullptr;
static int storedMeteorCount = 0;
static bool initialized = false;

void freeMeteorPositions() {
    if (startX) {
        delete[] startX;
        startX = nullptr;
    }
    if (startY) {
        delete[] startY;
        startY = nullptr;
    }
    initialized = false;
    storedMeteorCount = 0;
}

void drawCrater(float cx, float cy, float r) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    int num_segments = 10;
    for (int j = 0; j <= num_segments; j++) {
        float theta = 2.0f * 3.1415926f * float(j) / float(num_segments);
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

void drawStars() {
    glColor3f(1, 1, 1);
    glBegin(GL_POINTS);
    for (int i = 0; i < 30; i++) {
        float x = ((rand() % 200) - 100) / 100.0f;
        float y = ((rand() % 200) - 100) / 100.0f;
        glVertex2f(x, y);
    }
    glEnd();
}

void drawMeteors(int meteorCount) {
    float prog = (frameCount - 300) * 0.0015f;
    if (prog < 0) return;

    if (!initialized || meteorCount != storedMeteorCount) {
        freeMeteorPositions();

        startX = new float[meteorCount];
        startY = new float[meteorCount];
        storedMeteorCount = meteorCount;

        const float minDist = 0.35f;
        const float minX = 1.0f;
        const float maxX = 2.0f;
        const float minY = 0.0f;
        const float maxY = 2.0f;

        for (int i = 0; i < meteorCount; i++) {
            bool validPos = false;
            int attempts = 0;
            float candidateX, candidateY;

            while (!validPos && attempts < 500) {
                candidateX = minX + static_cast<float>(rand()) / RAND_MAX * (maxX - minX);
                candidateY = minY + static_cast<float>(rand()) / RAND_MAX * (maxY - minY);

                validPos = true;
                for (int j = 0; j < i; j++) {
                    float dx = candidateX - startX[j];
                    float dy = candidateY - startY[j];
                    float dist = sqrtf(dx * dx + dy * dy);
                    if (dist < minDist) {
                        validPos = false;
                        break;
                    }
                }
                attempts++;
            }

            if (!validPos) {
                startX[i] = 10.0f;
                startY[i] = 10.0f;
            }
            else {
                startX[i] = candidateX;
                startY[i] = candidateY;
            }
        }
        initialized = true;
    }

    const float dirX = -2.0f;
    const float dirY = -1.5f;

    float meteorRadius = 0.03f;

    for (int i = 0; i < meteorCount; i++) {
        float mx = startX[i] + dirX * prog;
        float my = startY[i] + dirY * prog;

        if (mx < -1.2f || my < -1.2f) continue;


        int flameCount = 30;
        float flameBaseRadius = meteorRadius + 0.010f;

        glColor3f(1.0f, 0.4f, 0.0f);
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= flameCount; j++) {
            float angle = j * (2.0f * 3.1415926f / flameCount);
            float flicker = 0.01f * sinf(frameCount * 0.3f + j);
            float r1 = flameBaseRadius;
            float r2 = flameBaseRadius + 0.02f + flicker;

            glVertex2f(mx + r1 * cosf(angle), my + r1 * sinf(angle)); // Inner point
            glVertex2f(mx + r2 * cosf(angle), my + r2 * sinf(angle)); // Outer flame point
        }
        glEnd();

        float tailLength = 0.10f;
        float tailWidth = 0.03f;

        float tailX = mx - dirX * 0.02f;
        float tailY = my - dirY * 0.02f;

        float unitDirX = dirX / sqrtf(dirX * dirX + dirY * dirY);
        float unitDirY = dirY / sqrtf(dirX * dirX + dirY * dirY);

        float perpX = -unitDirY;
        float perpY = unitDirX;

        int flameLayers = 3;
        for (int f = 0; f < flameLayers; f++) {
            float alpha = 1.0f - f * 0.3f;
            float length = tailLength - f * 0.05f;
            float width = tailWidth - f * 0.008f;

            glColor4f(1.0f, 0.4f + 0.2f * f, 0.0f, alpha);
            glBegin(GL_TRIANGLES);
            glVertex2f(tailX + perpX * width, tailY + perpY * width);
            glVertex2f(tailX - perpX * width, tailY - perpY * width);
            glVertex2f(tailX - unitDirX * length, tailY - unitDirY * length);
            glEnd();
        }


        glColor3f(0.5f, 0.5f, 0.5f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(mx, my);
        int num_segments = 20;
        for (int j = 0; j <= num_segments; j++) {
            float theta = 2.0f * 3.1415926f * float(j) / float(num_segments);
            float x = meteorRadius * cosf(theta);
            float y = meteorRadius * sinf(theta);
            glVertex2f(mx + x, my + y);
        }
        glEnd();
    }
}


void drawSpace() {
    drawBackground();
    drawStars();
    drawMeteors(10); // Pass the number of meteors you want here
    rocketY = -0.2f;
    earthLaunch = true;
    flameSize = 0.08f + 0.02f * sin(frameCount * 0.1f);
    drawRocket();
    drawFlame();
}