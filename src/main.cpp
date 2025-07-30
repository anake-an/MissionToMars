#include <Windows.h>
#include <stdlib.h>
#include <math.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <string>
#include "scenes.h"

#define rgb(v) ((v) / 255.0f)

// Global state variables
float rocketY = -0.8f;
float flameSize = 0.0f;

float starAngle = 0.0f;
bool blink = false;
int frameCount = 0;
bool earthLaunch = false;
bool marsLanding = false;
bool onMars = false;

float cloudAndTowerYOffset = 0.0f;

enum Scene {
    LAUNCHING,
    INSIDE_ROCKET,
    SPACE,
    LANDING
};

Scene currentScene = LAUNCHING;

// Forward declarations for all helper functions used across all scene files:
void drawText(const char* text, float x, float y);
void drawCircle(float cx, float cy, float r, int num_segments);
void drawCloud(float x, float y, float scale = 1.0f);
void drawRocket();
void drawFlame();
void drawBackground();
void drawTower(float baseX, float baseY, float scaleFactor);
void drawStickman(float x, float y);
void drawFlag(float x, float y);
void drawRocketBase();
void drawBase();
void drawMarsSurface();
float easeOutCubic(float t);
void drawUFO(float centerX, float centerY, float scale);
void drawInsideRocketScene();


void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    switch (currentScene) {
    case LAUNCHING:
        drawLaunching();
        break;
    case INSIDE_ROCKET:
        glClear(GL_COLOR_BUFFER_BIT);
        drawInsideRocketScene();
        break;
    case SPACE:
        drawSpace();
        break;
    case LANDING:
        drawLanding();
        break;
    }
    glutSwapBuffers();

}
void update(int val) {
    frameCount++;

    switch (currentScene) {
    case LAUNCHING:
        if (rocketY > 1.25f) {
            currentScene = INSIDE_ROCKET;
            frameCount = 0;
        }
        break;
    case INSIDE_ROCKET:
        if (frameCount >= 600) {
            currentScene = SPACE;
            frameCount = 0;
            starAngle += 1.0f;
            if (starAngle > 360) starAngle -= 360;
            blink = !blink;
        }
        break;
    case SPACE:
        if (frameCount >= 1200) {
            currentScene = LANDING;
            rocketY = 1.25f;
            onMars = false;
            frameCount = 2101;
        }
        break;
    case LANDING:
        break;
    }

    glutPostRedisplay();
    if (frameCount < 7000)
        glutTimerFunc(16, update, 0);
}

void drawText(const char* text, float x, float y) {
    glRasterPos2f(x, y);
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }
}

void drawCircle(float cx, float cy, float r, int num_segments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= num_segments; i++) {
        float theta = 2.0f * 3.14159f * i / num_segments;
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

void drawCloud(float x, float y, float scale) {
    glColor3f(1, 1, 1);
    float radius = 0.12f * scale;
    drawCircle(x, y, radius, 50);
    drawCircle(x + 0.1f * scale, y + 0.05f * scale, radius * 0.8f, 50);
    drawCircle(x + 0.22f * scale, y, radius * 0.75f, 50);
    drawCircle(x + 0.08f * scale, y - 0.06f * scale, radius * 0.85f, 50);
    drawCircle(x - 0.12f * scale, y - 0.02f * scale, radius * 0.75f, 50);
}



void drawRocket() {
    float scale = 1.5f;
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glVertex2f(-0.05f * scale, rocketY);
    glVertex2f(0.05f * scale, rocketY);
    glVertex2f(0.05f * scale, rocketY + 0.4f * scale);
    glVertex2f(-0.05f * scale, rocketY + 0.4f * scale);
    glEnd();

    glColor3f(1, 0, 0);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.05f * scale, rocketY + 0.4f * scale);
    glVertex2f(0.05f * scale, rocketY + 0.4f * scale);
    glVertex2f(0, rocketY + 0.55f * scale);
    glEnd();

    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.05f * scale, rocketY);
    glVertex2f(-0.1f * scale, rocketY - 0.05f * scale);
    glVertex2f(-0.05f * scale, rocketY + 0.1f * scale);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(0.05f * scale, rocketY);
    glVertex2f(0.1f * scale, rocketY - 0.05f * scale);
    glVertex2f(0.05f * scale, rocketY + 0.1f * scale);
    glEnd();

    glColor3f(0, 0, 1);
    const char* label = "UMS";
    float textX = -0.015f * scale;
    float textY = rocketY + 0.3f * scale;
    for (int i = 0; label[i] != '\0'; i++) {
        glRasterPos2f(textX, textY - i * 0.06f * scale);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, label[i]);
    }
}

void drawFlame() {
    if (!(earthLaunch || marsLanding)) return;
    glColor3f(1.0f, 0.5f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.04f, rocketY);
    glVertex2f(0.04f, rocketY);
    glVertex2f(0.0f, rocketY - flameSize);
    glEnd();
}



void drawBackground() {
    float t = (rocketY + 0.8f) / 2.0f;
    float skyR = (0.4f - t);
    float skyG = (0.7f - t) * 0.7f;
    float skyB = (1.0f - t) * 0.4f;
    glBegin(GL_QUADS);
    glColor3f(skyR, skyG, skyB);
    glVertex2f(-1, -1);
    glVertex2f(1, -1);
    glColor3f(0, 0, 0);
    glVertex2f(1, 1);
    glVertex2f(-1, 1);
    glEnd();
}

void drawTower(float baseX, float baseY, float scaleFactor) {
    auto scaleX = [=](float px) { return (px / 1000.0f) * 2.0f * scaleFactor; };
    auto scaleY = [=](float py) { return (py / 1000.0f) * 2.0f * scaleFactor; };

    glColor3f(rgb(92), rgb(141), rgb(148));
    glBegin(GL_QUADS);
    glVertex2f(baseX, baseY);
    glVertex2f(baseX + scaleX(30), baseY);
    glVertex2f(baseX + scaleX(30), baseY + scaleY(140));
    glVertex2f(baseX, baseY + scaleY(140));
    glEnd();

    glColor3f(rgb(101), rgb(203), rgb(224));
    glBegin(GL_QUADS);
    glVertex2f(baseX - scaleX(10), baseY + scaleY(140));
    glVertex2f(baseX + scaleX(40), baseY + scaleY(140));
    glVertex2f(baseX + scaleX(40), baseY + scaleY(150));
    glVertex2f(baseX - scaleX(10), baseY + scaleY(150));
    glEnd();

    glColor3f(rgb(173), rgb(184), rgb(186));
    glBegin(GL_QUADS);
    glVertex2f(baseX, baseY + scaleY(150));
    glVertex2f(baseX + scaleX(30), baseY + scaleY(150));
    glVertex2f(baseX + scaleX(50), baseY + scaleY(190));
    glVertex2f(baseX - scaleX(20), baseY + scaleY(190));
    glEnd();

    glColor3f(rgb(241), rgb(161), rgb(36));
    glBegin(GL_QUADS);
    glVertex2f(baseX - scaleX(20), baseY + scaleY(190));
    glVertex2f(baseX + scaleX(50), baseY + scaleY(190));
    glVertex2f(baseX + scaleX(50), baseY + scaleY(210));
    glVertex2f(baseX - scaleX(20), baseY + scaleY(210));
    glEnd();

    glLineWidth(3);
    glColor3f(rgb(43), rgb(43), rgb(48));
    glBegin(GL_LINES);
    glVertex2f(baseX - scaleX(10), baseY + scaleY(200));
    glVertex2f(baseX + scaleX(40), baseY + scaleY(200));
    glEnd();

    glColor3f(rgb(78), rgb(187), rgb(221));
    glBegin(GL_QUADS);
    glVertex2f(baseX - scaleX(20), baseY + scaleY(210));
    glVertex2f(baseX + scaleX(50), baseY + scaleY(210));
    glVertex2f(baseX + scaleX(30), baseY + scaleY(220));
    glVertex2f(baseX, baseY + scaleY(220));
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(baseX, baseY + scaleY(220));
    glVertex2f(baseX + scaleX(30), baseY + scaleY(220));
    glVertex2f(baseX + scaleX(30), baseY + scaleY(230));
    glVertex2f(baseX, baseY + scaleY(230));
    glEnd();
}

void drawStickman(float x, float y) {
    glColor3f(1, 1, 1);
    float r = 0.03f;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 360; i += 10) {
        glVertex2f(x + cos(i * 3.14159 / 180) * r, y + sin(i * 3.14159 / 180) * r);
    }
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(x, y - 0.03f);
    glVertex2f(x, y - 0.1f);
    glVertex2f(x, y - 0.1f);
    glVertex2f(x - 0.03f, y - 0.15f);
    glVertex2f(x, y - 0.1f);
    glVertex2f(x + 0.03f, y - 0.15f);
    glVertex2f(x, y - 0.06f);
    glVertex2f(x - 0.03f, y - 0.09f);
    glVertex2f(x, y - 0.06f);
    glVertex2f(x + 0.03f, y - 0.09f);
    glEnd();
}

void drawFlag(float x, float y) {
    glColor3f(1, 1, 1);
    glBegin(GL_LINES);
    glVertex2f(x, y);
    glVertex2f(x, y - 0.15f);
    glEnd();

    glColor3f(1, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + 0.05f, y + 0.01f);
    glVertex2f(x + 0.05f, y - 0.03f);
    glVertex2f(x, y - 0.03f);
    glEnd();
}

void drawRocketBase() {
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(-0.15f, -0.8f);
    glVertex2f(0.15f, -0.8f);
    glVertex2f(0.2f, -0.9f);
    glVertex2f(-0.2f, -0.9f);
    glEnd();
}

void drawBase() {
    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(-1, -1);
    glVertex2f(1, -1);
    glVertex2f(1, -0.9f);
    glVertex2f(-1, -0.9f);
    glEnd();
}

void drawMarsSurface() {
    glColor3f(0.8f, 0.3f, 0.1f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0, -1.0f);
    for (int i = 0; i <= 180; i++) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(cos(angle), -1.0f + sin(angle));
    }
    glEnd();
}

float easeOutCubic(float t) {
    t--;
    return t * t * t + 1.0f;
}

void drawUFO(float centerX, float centerY, float scale) {
    auto scaleX = [=](float px) { return (px / 1000.0f) * 2.0f * scale; };
    auto scaleY = [=](float py) { return (py / 1000.0f) * 2.0f * scale; };

    glColor3f(rgb(192), rgb(192), rgb(192));
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 180; i++) {
        float angle = i * 3.14159f / 180.0f;
        float x = scaleX(60) * cos(angle);
        float y = scaleY(20) * sin(angle);
        glVertex2f(centerX + x, centerY + y);
    }
    glEnd();

    glColor3f(rgb(135), rgb(206), rgb(250));
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 180; i++) {
        float angle = i * 3.14159f / 180.0f;
        float x = scaleX(30) * cos(angle);
        float y = scaleY(30) * sin(angle);
        glVertex2f(centerX + x, centerY + scaleY(20) + y);
    }
    glEnd();

    glColor3f(0.2f, 0.2f, 0.4f);
    for (int i = -2; i <= 2; i++) {
        float windowX = centerX + scaleX(i * 20);
        float windowY = centerY;
        glBegin(GL_POLYGON);
        for (int j = 0; j < 360; j++) {
            float theta = j * 3.14159f / 180.0f;
            float x = scaleX(5) * cos(theta);
            float y = scaleY(5) * sin(theta);
            glVertex2f(windowX + x, windowY + y);
        }
        glEnd();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Mars Journey");
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gluOrtho2D(-1, 1, -1, 1);
    glClearColor(0, 0, 0, 1);
    glutDisplayFunc(display);
    glutTimerFunc(0, update, 0);
    glutMainLoop();
    return 0;
}