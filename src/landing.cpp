#include "scenes.h"
#include <GL/glut.h>
#include <cmath>


extern float rocketY;
extern int frameCount;
extern bool marsLanding;
extern bool onMars;
extern float flameSize;
void drawMarsSurface();
void drawRocket();
void drawFlame();
void drawStickman(float x, float y, float walkOffset = 0.0f, bool animateLegs = true);
void drawFlag(float x, float y);
void drawUFO(float centerX, float centerY, float scale);
void drawText(const char* text, float x, float y);
float easeOutCubic(float t);


void drawStickman(float x, float y, float walkOffset, bool animateLegs) {
    glColor3f(1, 1, 1);
    float r = 0.03f;
    // Head
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x + walkOffset, y);
    for (int i = 0; i <= 360; i += 10) {
        glVertex2f(x + walkOffset + cos(i * 3.14159f / 180) * r, y + sin(i * 3.14159f / 180) * r);
    }
    glEnd();

    glBegin(GL_LINES);

    glVertex2f(x + walkOffset, y - 0.03f);
    glVertex2f(x + walkOffset, y - 0.1f);


    float swing = 0.0f;
    if (animateLegs) {
        float legSwingAmplitude = 0.02f;
        float legSwingSpeed = 0.1f;
        swing = legSwingAmplitude * sin(frameCount * legSwingSpeed);
    }


    glVertex2f(x + walkOffset, y - 0.1f);
    glVertex2f(x + walkOffset - 0.03f + swing, y - 0.15f);

    glVertex2f(x + walkOffset, y - 0.1f);
    glVertex2f(x + walkOffset + 0.03f - swing, y - 0.15f);

    glVertex2f(x + walkOffset, y - 0.06f);
    glVertex2f(x + walkOffset - 0.03f, y - 0.09f);
    glVertex2f(x + walkOffset, y - 0.06f);
    glVertex2f(x + walkOffset + 0.03f, y - 0.09f);
    glEnd();
}

void drawLanding() {
    drawMarsSurface();

    // Rocket Landing Logic
    if (rocketY > -0.4f) {
        rocketY -= 0.002f;
        marsLanding = true;
        flameSize = 0.08f + 0.02f * sin(frameCount * 0.1f);
    }
    else {
        rocketY = -0.4f;
        marsLanding = false;
        flameSize = 0.0f;
        onMars = true;
    }

    drawRocket();
    drawFlame();

    static int stickmanCount = 0;
    static int lastStickmanFrame = 0;
    static float walkOffsets[5] = { 0, 0, 0, 0, 0 };
    static bool walkingCompleted[5] = { false, false, false, false, false };
    static bool animateLegs[5] = { true, true, true, true, true };
    static float walkDownOffsets[5] = { 0, 0, 0, 0, 0 };

    static int astronautTextTimer = 0;
    static bool showAstronautText = false;
    static bool ufoComingTextVisible = false;

    static bool ufoArrived = false;
    static int stolenStickmanIndex = -1;
    static float stealProgress = 0.0f;
    static bool stealAnimInProgress = false;
    static int stealAnimStartFrame = 0;

    static bool ufoLeaving = false;
    static int ufoLeaveStartFrame = 0;

    if (onMars) {
        int landedFrames = frameCount - 2100;

        // Spawn stickmen gradually
        if (landedFrames - lastStickmanFrame >= 40 && stickmanCount < 5 && !stealAnimInProgress && !ufoLeaving) {
            walkingCompleted[stickmanCount] = false;
            stickmanCount++;
            lastStickmanFrame = landedFrames;
        }

        float finalPosX[5] = { -0.25f, -0.15f, 0.15f, 0.25f, 0.35f };
        float walkSpeed = 0.0015f;
        float walkDownSpeed = 0.0008f;

        // Horizontal walking
        for (int i = 0; i < stickmanCount; i++) {
            if (!walkingCompleted[i]) {
                if (i < 2) {
                    if (walkOffsets[i] > finalPosX[i]) {
                        walkOffsets[i] -= walkSpeed;
                        if (walkOffsets[i] <= finalPosX[i]) {
                            walkOffsets[i] = finalPosX[i];
                            walkingCompleted[i] = true;
                            animateLegs[i] = true;
                        }
                    }
                }
                else {
                    if (walkOffsets[i] < finalPosX[i]) {
                        walkOffsets[i] += walkSpeed;
                        if (walkOffsets[i] >= finalPosX[i]) {
                            walkOffsets[i] = finalPosX[i];
                            walkingCompleted[i] = true;
                            animateLegs[i] = true;
                        }
                    }
                }
            }
        }

        // Check all horizontal walking done
        bool allWalkingDone = true;
        for (int i = 0; i < stickmanCount; i++) {
            if (!walkingCompleted[i]) {
                allWalkingDone = false;
                break;
            }
        }

        // Vertical walking down
        if (stickmanCount >= 5 && allWalkingDone) {
            for (int i = 0; i < stickmanCount; i++) {
                if (walkDownOffsets[i] < 0.15f) {
                    walkDownOffsets[i] += walkDownSpeed;
                    if (walkDownOffsets[i] >= 0.15f) {
                        walkDownOffsets[i] = 0.15f;
                        animateLegs[i] = false;
                    }
                    else {
                        animateLegs[i] = true;
                    }
                }
            }
        }

        // Draw stickmen and animate stolen stickman during stealing animation
        for (int i = 0; i < stickmanCount; i++) {
            float drawX = walkOffsets[i];
            float drawY = -0.4f - walkDownOffsets[i];
            bool legsAnimate = animateLegs[i];

            if (stealAnimInProgress && i == stolenStickmanIndex) {
                float elapsed = float(frameCount - stealAnimStartFrame);
                float duration = 300.0f;
                float progress = elapsed / duration;
                if (progress > 1.0f) progress = 1.0f;

                // UFO position calculation
                float ufoStartX = 14.3f;
                float ufoEndX = 0.6f;
                float ufoStartY = 0.9f;
                float scale = 1.5f;
                float ufoEndY = rocketY + 0.4f * scale;
                float landedFramesLocal = landedFrames;
                float ufoprog = (landedFramesLocal - 200) / 2200.0f;
                if (ufoprog > 1.0f) ufoprog = 1.0f;
                float easeProg = easeOutCubic(ufoprog);
                float ufoX = ufoStartX + (ufoEndX - ufoStartX) * easeProg;
                float ufoY = ufoStartY + (ufoEndY - ufoStartY) * easeProg;

                // Animate stickman moving towards UFO vertically only
                drawY = drawY + (ufoY - drawY) * progress;
                legsAnimate = false;

                // End animation and remove stickman after stealing completes
                if (progress >= 1.0f) {
                    stealAnimInProgress = false;
                    ufoLeaving = true;
                    ufoLeaveStartFrame = frameCount;

                    // Remove the stolen stickman by shifting remaining stickmen
                    for (int j = stolenStickmanIndex; j < stickmanCount - 1; ++j) {
                        walkOffsets[j] = walkOffsets[j + 1];
                        walkingCompleted[j] = walkingCompleted[j + 1];
                        animateLegs[j] = animateLegs[j + 1];
                        walkDownOffsets[j] = walkDownOffsets[j + 1];
                    }
                    stickmanCount--;
                    stolenStickmanIndex = -1;
                }
            }

            drawStickman(drawX, drawY, drawX, legsAnimate);
        }

        // Show "Astronauts exploring..." text temporarily
        if (stickmanCount >= 5 && allWalkingDone && walkDownOffsets[0] >= 0.15f && !stealAnimInProgress && !ufoLeaving) {
            if (!showAstronautText) {
                showAstronautText = true;
                astronautTextTimer = frameCount;
            }
            if (frameCount - astronautTextTimer < 500) {
                glColor3f(1, 1, 1);
                drawText("Astronauts exploring...", -0.4f, 0.7f);
            }
            else {
                showAstronautText = false;
                ufoComingTextVisible = true;
            }
        }

        // Draw flag on Mars surface
        if (stickmanCount >= 5 && allWalkingDone && walkDownOffsets[0] >= 0.15f) {
            drawFlag(0.25f, -0.55f);
        }

        float ufoStartX = 14.3f;
        float ufoEndX = 0.74f;
        float ufoStartY = 0.9f;
        float scale = 1.5f;
        float ufoEndY = rocketY + 0.4f * scale;
        float ufoprog = float(landedFrames - 200) / 2200.0f;
        if (ufoprog > 1.0f) ufoprog = 1.0f;
        float easeProg = easeOutCubic(ufoprog);
        float ufoX = ufoStartX + (ufoEndX - ufoStartX) * easeProg;
        float ufoY = ufoStartY + (ufoEndY - ufoStartY) * easeProg;


        if (ufoLeaving) {
            float elapsed = frameCount - ufoLeaveStartFrame;
            float leaveDuration = 600.0f;
            float leaveProgress = elapsed / leaveDuration;
            if (leaveProgress > 1.0f) leaveProgress = 1.0f;

            ufoY += leaveProgress * 3.0f;

            if (leaveProgress >= 1.0f) {
                ufoLeaving = false;
                ufoComingTextVisible = false;
            }
        }

        drawUFO(ufoX, ufoY, 0.9f);


        if (ufoArrived && !ufoLeaving) {
            float beamBaseRadius = 0.05f;
            int beamSegments = 20;
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            for (int i = 0; i <= beamSegments; ++i) {
                float t = float(i) / beamSegments;
                float alpha = 0.6f * (1.0f - t);
                float radius = beamBaseRadius * (1.0f + 4.0f * t);

                glColor4f(1.0f, 1.0f, 0.6f, alpha);
                glBegin(GL_TRIANGLE_FAN);
                glVertex2f(ufoX, ufoY - t * (ufoY + 0.5f));
                int circlePoints = 20;
                for (int j = 0; j <= circlePoints; ++j) {
                    float angle = 2.0f * 3.1415926f * j / circlePoints;
                    float x = ufoX + cosf(angle) * radius;
                    float y = (ufoY - t * (ufoY + 0.5f)) + sinf(angle) * radius * 0.3f;
                    glVertex2f(x, y);
                }
                glEnd();
            }
            glDisable(GL_BLEND);
        }


        if (ufoComingTextVisible) {
            glColor3f(1, 1, 1);
            drawText("UFO is coming...", -0.5f, 0.6f);
        }


        if (!ufoArrived) {
            glColor3f(1, 1, 1);
            drawText("Landed on Mars", -0.3f, 0.8f);
        }

        if (!ufoArrived && ufoprog >= 1.0f) {
            ufoArrived = true;
            stolenStickmanIndex = (stickmanCount > 0) ? stickmanCount - 1 : -1;
            if (stolenStickmanIndex >= 0) {
                stealAnimInProgress = true;
                stealAnimStartFrame = frameCount;
                ufoComingTextVisible = true;
            }
        }
    }
}