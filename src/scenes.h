#ifndef SCENES_H
#define SCENES_H

void drawLaunching();
void drawSpace();
void drawStars();
void drawLanding();
void drawInsideRocketScene();

void drawText(const char* text, float x, float y);
void drawStickman(float x, float y, float walkOffset = 0.0f);
#endif