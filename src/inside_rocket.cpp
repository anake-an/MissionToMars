#include <GL/glut.h>
#include <string>
#include <cmath>
#include <cstdlib>  // for rand()

extern float rocketY;
extern int frameCount;
extern void drawStickman(float x, float y, float walkOffset);
extern void drawText(const char* text, float x, float y);
extern float starAngle;
extern bool blink;

void drawStars(int count, float minX, float maxX, float minY, float maxY) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < count; i++) {
        float x = minX + static_cast<float>(rand()) / RAND_MAX * (maxX - minX);
        float y = minY + static_cast<float>(rand()) / RAND_MAX * (maxY - minY);
        glVertex2f(x, y);
    }
    glEnd();
}

void drawInsideRocketScene() {
    // Draw the walls of the rocket
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();

    // Draw the ceiling
    glColor3f(0.1f, 0.1f, 0.1f); // Even darker gray for ceiling
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, 1.0f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(1.0f, 1.5f);
    glVertex2f(-1.0f, 1.5f);
    glEnd();

    // Draw window with dark blue background
    glColor3f(0.0f, 0.2f, 0.4f);
    const float centerX = 0.0f;
    const float centerY = 0.5f;
    const float radius = 0.3f;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 10) {
        float rad = i * 3.14159f / 180.0f;
        glVertex2f(centerX + radius * cos(rad), centerY + radius * sin(rad));
    }
    glEnd();

    // Draw static stars inside the window
    glPushMatrix();
    glTranslatef(centerX, centerY, 0.0f);
    drawStars(15, -radius, radius, -radius, radius);
    glPopMatrix();

    // Draw blinking star inside the window only when blink is true
    if (blink) {
        glPushMatrix();
        glTranslatef(centerX, centerY, 0.0f);
        glRotatef(starAngle, 0.0f, 0.0f, 1.0f);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_POINTS);
        glVertex2f(0.1f, 0.1f);
        glVertex2f(-0.1f, -0.1f);
        glVertex2f(0.1f, -0.1f);
        glVertex2f(-0.1f, 0.1f);
        glEnd();
        glPopMatrix();
    }

    // --- Draw control panel background ---
    glColor3f(0.2f, 0.2f, 0.2f); // Slightly darker grey
    glBegin(GL_QUADS);
    glVertex2f(-0.6f, -0.2f);
    glVertex2f(0.6f, -0.2f);
    glVertex2f(0.6f, -0.7f);
    glVertex2f(-0.6f, -0.7f);
    glEnd();

    // --- Draw monitor frame (bezel) ---
    glColor3f(0.1f, 0.1f, 0.1f); // Black bezel
    glBegin(GL_QUADS);
    glVertex2f(-0.32f, -0.08f);
    glVertex2f(0.32f, -0.08f);
    glVertex2f(0.32f, -0.32f);
    glVertex2f(-0.32f, -0.32f);
    glEnd();

    // --- Draw monitor screen (blinks) ---
    if (blink) {
        glColor3f(1.0f, 0.0f, 0.0f);  // Bright red
    }
    else {
        glColor3f(0.4f, 0.0f, 0.0f);  // Dark red
    }
    glBegin(GL_QUADS);
    glVertex2f(-0.3f, -0.1f);
    glVertex2f(0.3f, -0.1f);
    glVertex2f(0.3f, -0.3f);
    glVertex2f(-0.3f, -0.3f);
    glEnd();

    // --- Optional screen shine/reflection (glass effect) ---
    glColor4f(1.0f, 1.0f, 1.0f, 0.1f); // Semi-transparent white
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.3f, -0.1f);
    glVertex2f(0.3f, -0.1f);
    glVertex2f(0.3f, -0.3f);
    glEnd();

    // --- Draw monitor stand ---
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(-0.05f, -0.32f);
    glVertex2f(0.05f, -0.32f);
    glVertex2f(0.05f, -0.45f);
    glVertex2f(-0.05f, -0.45f);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(-0.15f, -0.45f);
    glVertex2f(0.15f, -0.45f);
    glVertex2f(0.15f, -0.48f);
    glVertex2f(-0.15f, -0.48f);
    glEnd();

    // --- Red blinking alarm light (synchronized with monitor) ---
    glColor3f(blink ? 1.0f : 0.4f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-0.05f, -0.55f);
    glVertex2f(0.05f, -0.55f);
    glVertex2f(0.05f, -0.65f);
    glVertex2f(-0.05f, -0.65f);
    glEnd();

    // --- Red siren light on the left of the window ---
    glColor3f(blink ? 1.0f : 0.4f, 0.0f, 0.0f); // Blinking red
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 10) {
        float rad = i * 3.14159f / 180.0f;
        glVertex2f(-0.45f + 0.05f * cos(rad), 0.5f + 0.05f * sin(rad)); // Left of window
    }
    glEnd();

    // --- Red siren light on the right of the window ---
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 10) {
        float rad = i * 3.14159f / 180.0f;
        glVertex2f(0.45f + 0.05f * cos(rad), 0.5f + 0.05f * sin(rad)); // Right of window
    }
    glEnd();


    glColor3f(0.15f, 0.15f, 0.15f); // Dark grey floor
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -0.8f);
    glVertex2f(1.0f, -0.8f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(-1.0f, -1.0f);
    glEnd();

    // Draw 2 stickmen, raised higher and static pose (walkOffset=0)
    float baseX = -0.2f;
    float stepX = 0.4f;
    float stickmanY = -0.6f;
    for (int i = 0; i < 2; i++) {
        float x = baseX + i * stepX;
        glPushMatrix();
        glTranslatef(x, stickmanY, 0.0f);     // Move to stickman position
        glScalef(4.0f, 4.0f, 1.0f);           // Scale stickman size (x and y)
        drawStickman(0.0f, 0.0f, 0.0f);      // Draw at origin, since already translated
        glPopMatrix();
    }

    // Draw alert text inside the monitor area, permanent display after entering the scene
    glColor3f(1.0f, 1.0f, 1.0f);  // White text for contrast
    drawText("ALERT: Meteor is coming!", -0.25f, -0.22f);
}