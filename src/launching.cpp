#include "scenes.h"
#include <GL/glut.h>
#include <string>
#include <cmath>
using namespace std;

extern float rocketY;
extern int frameCount;
extern bool earthLaunch;
extern float flameSize;
extern float cloudAndTowerYOffset;

void drawCloud(float x, float y, float scale = 1.0f);
void drawTower(float baseX, float baseY, float scaleFactor);
void drawStickman(float x, float y, float walkOffset);
void drawRocketBase();
void drawBase();
void drawRocket();
void drawFlame();
void drawText(const char* text, float x, float y);
void drawBackground();
void drawLadder(float baseX, float baseY, float height);

void drawLadder(float baseX, float baseY, float height) {
    glColor3f(0.7f, 0.7f, 0.7f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(baseX, baseY);
    glVertex2f(baseX, baseY + height);
    glVertex2f(baseX + 0.05f, baseY);
    glVertex2f(baseX + 0.05f, baseY + height);
    for (float y = baseY; y <= baseY + height; y += 0.05f) {
        glVertex2f(baseX, y);
        glVertex2f(baseX + 0.05f, y);
    }
    glEnd();
}

void drawStickman(float x, float y, float walkOffset) {
    glColor3f(1, 1, 1);
    float r = 0.03f;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 360; i += 10) {
        glVertex2f(x + cos(i * 3.14159f / 180) * r, y + sin(i * 3.14159f / 180) * r);
    }
    glEnd();

    float legSwingAmplitude = 0.02f;
    float legSwingSpeed = 0.1f;
    float swing = legSwingAmplitude * sin(frameCount * legSwingSpeed + walkOffset);

    glBegin(GL_LINES);
    glVertex2f(x, y - 0.03f);
    glVertex2f(x, y - 0.1f);
    glVertex2f(x, y - 0.1f);
    glVertex2f(x - 0.03f + swing, y - 0.15f);
    glVertex2f(x, y - 0.1f);
    glVertex2f(x + 0.03f - swing, y - 0.15f);
    glVertex2f(x, y - 0.06f);
    glVertex2f(x - 0.03f, y - 0.09f);
    glVertex2f(x, y - 0.06f);
    glVertex2f(x + 0.03f, y - 0.09f);
    glEnd();
}

void drawLaunching() {
    drawBackground();

    struct Cloud {
        float x, y, scale;
    };
    static Cloud clouds[] = {
        {-0.8f, 0.75f, 0.6f}, {-0.5f, 0.8f, 0.5f}, {-0.2f, 0.72f, 0.7f},
        {0.18f, 0.77f, 0.6f},  {0.5f, 0.82f, 0.55f}, {0.79f, 0.73f, 0.65f} };

    float cloudTopY = 0.83f;
    if (rocketY > cloudTopY) {
        cloudAndTowerYOffset -= 0.0007f;
        if (cloudAndTowerYOffset < -1.3f) cloudAndTowerYOffset = -1.3f;
    }

    static float launchDescendOffset = 0.0f;
    const float descendSpeed = 0.0005f;
    if (earthLaunch) {
        launchDescendOffset += descendSpeed;
    }

    float towerY = -0.9f + cloudAndTowerYOffset - launchDescendOffset;

    // Only draw clouds, towers, and ladder if tower visible
    if (towerY > -1.1f) {
        for (const auto& c : clouds) {
            float cy = c.y + cloudAndTowerYOffset - launchDescendOffset;
            if (cy > -1.0f) drawCloud(c.x, cy, c.scale);
        }

        drawTower(-0.75f, towerY, 1.2f);
        drawTower(0.6f, towerY, 1.5f);
    }

    // Draw rocket base moved down with rest
    glPushMatrix();
    glTranslatef(0.0f, cloudAndTowerYOffset - launchDescendOffset, 0.0f);
    drawRocketBase();
    glPopMatrix();

    float ladderBaseX = 0.1f;
    float ladderHeight = 0.45f;
    float ladderBaseY = towerY - launchDescendOffset;
    float ladderCenterX = ladderBaseX + 0.025f;
    float ladderTopY = ladderBaseY + ladderHeight;

    static const int stickmanCount = 5;
    static float groundY = -0.77f + cloudAndTowerYOffset - launchDescendOffset;
    static float startXPositions[stickmanCount] = { -1.1f, -1.25f, -1.4f, -1.55f , -1.7 };
    static float stickmanX[stickmanCount] = { startXPositions[0], startXPositions[1], startXPositions[2], startXPositions[3] , startXPositions[4] };
    static float stickmanY[stickmanCount] = { groundY, groundY, groundY, groundY, groundY };

    enum Phase { WALKING, CLIMBING, BOARDING, DONE };
    static Phase phases[stickmanCount] = { WALKING, WALKING, WALKING, WALKING,WALKING };
    static float boardingX[stickmanCount] = { ladderCenterX, ladderCenterX, ladderCenterX, ladderCenterX, ladderCenterX };
    static int boardingIndex = 0;

    // Draw ladder after rocket base so ladder appears in front, only if still boarding
    if (towerY > -1.1f && boardingIndex < stickmanCount) {
        drawLadder(ladderBaseX, ladderBaseY, ladderHeight);
    }

    float walkSpeed = 0.002f;
    float climbSpeed = 0.0025f;
    float boardSpeed = 0.002f;
    float legSwingAmp = 0.02f;
    float legSwingSpeed = 0.15f;

    int frame = frameCount;
    bool allDone = true;

    for (int i = 0; i < stickmanCount; i++) {
        float legOffset = legSwingAmp * sin(frame * legSwingSpeed + i * 3.14159f / 2);

        switch (phases[i]) {
        case WALKING:
            if (stickmanX[i] + walkSpeed < ladderCenterX) {
                stickmanX[i] += walkSpeed;
                allDone = false;
            }
            else {
                stickmanX[i] = ladderCenterX;
                phases[i] = CLIMBING;
                allDone = false;
            }
            stickmanY[i] = groundY;
            drawStickman(stickmanX[i], stickmanY[i], legOffset);
            break;

        case CLIMBING:
            if (stickmanY[i] + climbSpeed < ladderTopY) {
                stickmanY[i] += climbSpeed;
                allDone = false;
            }
            else {
                stickmanY[i] = ladderTopY;
                phases[i] = BOARDING;
                allDone = false;
            }
            drawStickman(ladderCenterX, stickmanY[i], legOffset);
            break;

        case BOARDING:
            if (i == boardingIndex) {
                if (boardingX[i] - boardSpeed > ladderCenterX - 0.1f) {
                    boardingX[i] -= boardSpeed;
                    drawStickman(boardingX[i], stickmanY[i], legOffset);
                    allDone = false;
                }
                else {
                    phases[i] = DONE;
                    boardingIndex++;
                    allDone = false;
                }
            }
            else {
                drawStickman(ladderCenterX, stickmanY[i], legOffset);
                allDone = false;
            }
            break;

        case DONE:
            // not drawn
            break;
        }
    }

    if (allDone && boardingIndex >= stickmanCount) {
        static int launchStartFrame = 0;
        if (launchStartFrame == 0) launchStartFrame = frame + 600;

        int framesLeft = launchStartFrame - frame;
        if (framesLeft > 0) {
            glColor3f(1, 1, 1);
            string text = "Time to Launch: " + to_string(framesLeft / 60 + 1);
            drawText(text.c_str(), -0.15f, 0.4f);
            drawBase();
            drawRocket();
        }
        else {
            // Reset matrix to draw text in fixed coords
            glPushMatrix();
            glLoadIdentity();
            glColor3f(1, 1, 1);
            drawText("Launching from Earth...", -0.2f, 0.5f);
            glPopMatrix();

            earthLaunch = true;
            if (rocketY < 1.25f) rocketY += 0.003f;
            flameSize = 0.08f + 0.02f * sin(frame * 0.1f);
            drawRocket();
            drawFlame();
        }
    }
    else {
        glColor3f(1, 1, 1);
        drawText("Boarding astronauts...", -0.3f, 0.4f);
        drawBase();
        drawRocket();
    }
}