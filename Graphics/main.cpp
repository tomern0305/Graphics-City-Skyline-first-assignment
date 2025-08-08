#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "glut.h"

const double PI = 3.14;
const int NUM_STARS = 100;
const int NUM_FIREWORKS = 4;
const int TAIL_LENGTH = 400;
const int NUM_PARTICALS = 1000;
const int LIFE_LENGHT = 100;

/* Reflection */
const double WATERLINE_Y = -0.2; 
const double REFL_SCALE = 0.60;  
const double REFL_DIM = 0.35;

/* wavey offset */
const double WATER_WAVE_AMP = 0.015; 
const double WATER_WAVE_FREQ = 35.0;   

/* helpers for reflection */
static inline double reflect_y(double y) {
    return WATERLINE_Y - REFL_SCALE * (y - WATERLINE_Y);
}
static inline double wave_dx_static(double y_ref, double refl_bot_y, double x) {
    double depth = WATERLINE_Y - y_ref;
    double maxDepth = WATERLINE_Y - refl_bot_y;
    if (maxDepth <= 0) return 0.0;
    double amp = WATER_WAVE_AMP * (1.0 - depth / maxDepth);
    double phase = 6.0 * x; /* per-building phase so each looks a bit different */
    return amp * sin(WATER_WAVE_FREQ * y_ref + phase);
}

typedef struct { double x, y; } POINT2D;

typedef struct { double x, y; int life; } PARTICAL2D;

typedef struct
{
    POINT2D position;
    int size;
    POINT2D tail[TAIL_LENGTH];
    int expload;
    PARTICAL2D explotion[NUM_PARTICALS];
} FIREWORK;

typedef struct { POINT2D position; int size; } STAR;

STAR stars[NUM_STARS];
FIREWORK fireworks[NUM_FIREWORKS];

void init()
{
    int i, j;
    glClearColor(0.0, 0.0, 0.3, 0);
    glOrtho(-1, 1, -1, 1, -1, 1);

    srand((unsigned)time(0));

    for (i = 0; i < NUM_STARS; i++)
    {
        stars[i].position.x = -1 + 2 * ((rand() % 1000) / 1000.0);
        stars[i].position.y = -0.2 + 2 * ((rand() % 1000) / 1000.0);
        stars[i].size = 1 + rand() % 3;
    }

    for (i = 0; i < NUM_FIREWORKS; i++)
    {
        fireworks[i].position.x = -1 + 2 * ((rand() % 1000) / 1000.0);
        fireworks[i].position.y = -0.2;
        fireworks[i].size = 2;
        fireworks[i].expload = 0;

        for (j = 0; j < TAIL_LENGTH; j++)
            fireworks[i].tail[j] = fireworks[i].position;

        for (j = 0; j < NUM_PARTICALS; j++)
            fireworks[i].explotion[j].life = LIFE_LENGHT;
    }
}

void DrawTail(FIREWORK s)
{
    int j;
    double intensity = 1;
    glPointSize(2);
    glBegin(GL_POINTS);
    for (j = 0; j < TAIL_LENGTH; j++)
    {
        glColor3d(intensity, intensity, 0);
        glVertex2d(s.tail[j].x, s.tail[j].y);
        intensity *= 0.998;
    }
    glEnd();
}

void DrawStars()
{
    int i;
    glColor3d(1, 1, 0);
    for (i = 0; i < NUM_STARS; i++)
    {
        glPointSize(stars[i].size);
        glBegin(GL_POINTS);
        glVertex2d(stars[i].position.x, stars[i].position.y);
        glEnd();
    }
}

void DrawSky()
{
    glBegin(GL_POLYGON);
    glColor3d(0.1, 0.0, 0.3);
    glVertex2d(-1, 1);
    glVertex2d(1, 1);
    glColor3d(0.9, 0.7, 0.4);
    glVertex2d(1, -0.2);
    glVertex2d(-1, -0.2);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3d(0.8, 0.5, 0.3);
    glVertex2d(1, -0.2);
    glVertex2d(-1, -0.2);
    glColor3d(0.4, 0.3, 0.5);
    glVertex2d(-1, -1);
    glVertex2d(1, -1);
    glEnd();

    DrawStars();
}

void drawBuilding(double x, double y, double width, double step, int numStores, int numWindowsPerStore)
{
    int store, col, style;
    double dh, dv;

    double wr = 0.75, wg = 0.75, wb = 0.10; // warm, less bright

    /* body (simple gradient) */
    double topR = 0.2, topG = 0.2, topB = 0.2;
    double botR = 0.3 + 0.3 * fabs(sin(x * 5));
    double botG = 0.3 + 0.3 * fabs(sin(x * 3));
    double botB = 0.3 + 0.4 * fabs(cos(x * 4));

    glBegin(GL_POLYGON);
    glColor3d(0.2, 0.2, 0.2);
    glVertex2d(x + width, y);
    glVertex2d(x, y);
    glColor3d(botR, botG, botB);
    glVertex2d(x, -0.2);
    glVertex2d(x + width, -0.2);
    glEnd();

    /* reflection */
    double reflTopY = WATERLINE_Y;
    double reflBotY = WATERLINE_Y - REFL_SCALE * (y - WATERLINE_Y);
    int bands = 16;
    for (int b = 0; b < bands; b++) {
        double t0 = b / (double)bands;
        double t1 = (b + 1) / (double)bands;
        double y0 = reflTopY - (reflTopY - reflBotY) * t0;
        double y1 = reflTopY - (reflTopY - reflBotY) * t1;

        double dx0 = wave_dx_static(y0, reflBotY, x);
        double dx1 = wave_dx_static(y1, reflBotY, x);

        double mix0 = (y0 - reflBotY) / (reflTopY - reflBotY);
        double mix1 = (y1 - reflBotY) / (reflTopY - reflBotY);
        double r0 = (topR * (1 - mix0) + botR * mix0) * REFL_DIM;
        double g0 = (topG * (1 - mix0) + botG * mix0) * REFL_DIM;
        double b0 = (topB * (1 - mix0) + botB * mix0) * REFL_DIM;
        double r1 = (topR * (1 - mix1) + botR * mix1) * REFL_DIM;
        double g1 = (topG * (1 - mix1) + botG * mix1) * REFL_DIM;
        double b1 = (topB * (1 - mix1) + botB * mix1) * REFL_DIM;

        glBegin(GL_POLYGON);
        glColor3d(r0, g0, b0);
        glVertex2d(x + dx0, y0);
        glVertex2d(x + width + dx0, y0);
        glColor3d(r1, g1, b1);
        glVertex2d(x + width + dx1, y1);
        glVertex2d(x + dx1, y1);
        glEnd();
    }

    /* windows */
    double bottom = -0.2;
    double top = y;
    double h = top - bottom;
    double pad = 0.02 * h;
    int rows = numStores;

    dh = width / numWindowsPerStore;
    dv = (h - 2 * pad) / (rows - 1);

    style = ((int)floor((x + 1.0) / (width + step))) % 5;

    switch (style) {
    case 0:
        glPointSize(2);
        glColor3d(wr, wg, wb);
        for (store = 0; store < numStores; store++) {
            double cy = bottom + pad + store * dv;
            for (col = 0; col < numWindowsPerStore; col++) {
                double cx = x + col * dh;
                glBegin(GL_POINTS); glVertex2d(cx, cy); glEnd();

                /* reflected window (dimmer + wavy) */
                double cyR = reflect_y(cy);
                double dxR = wave_dx_static(cyR, reflBotY, x);
                glColor3d(wr * REFL_DIM, wg * REFL_DIM, wb * REFL_DIM);
                glBegin(GL_POINTS); glVertex2d(cx + dxR, cyR); glEnd();
                glColor3d(wr, wg, wb);
            }
        }
        break;

    case 1:
        glPointSize(2);
        glColor3d(wr, wg, wb);
        for (store = 0; store < numStores; store++) {
            double cy = bottom + pad + store * dv;
            double offset = (store % 2) ? 0.5 * dh : 0.0;
            for (col = 0; col < numWindowsPerStore; col++) {
                double cx = x + offset + col * dh;
                glBegin(GL_POINTS); glVertex2d(cx, cy); glEnd();

                double cyR = reflect_y(cy);
                double dxR = wave_dx_static(cyR, reflBotY, x);
                glColor3d(wr * REFL_DIM, wg * REFL_DIM, wb * REFL_DIM);
                glBegin(GL_POINTS); glVertex2d(cx + dxR, cyR); glEnd();
                glColor3d(wr, wg, wb);
            }
        }
        break;

    case 2:
        glPointSize(2);
        glColor3d(wr, wg, wb);
        for (store = 0; store < numStores; store++) {
            double cy = bottom + pad + store * dv;
            for (col = 0; col < numWindowsPerStore; col++) {
                if ((store + col) % 2) continue;
                double cx = x + col * dh;
                glBegin(GL_POINTS); glVertex2d(cx, cy); glEnd();

                double cyR = reflect_y(cy);
                double dxR = wave_dx_static(cyR, reflBotY, x);
                glColor3d(wr * REFL_DIM, wg * REFL_DIM, wb * REFL_DIM);
                glBegin(GL_POINTS); glVertex2d(cx + dxR, cyR); glEnd();
                glColor3d(wr, wg, wb);
            }
        }
        break;

    case 3:
    {
        double hseg = dv * 0.45;
        glColor3d(wr * 0.95, wg * 0.95, wb);
        for (store = 0; store < numStores; store++) {
            double cy = bottom + pad + store * dv;
            for (col = 0; col < numWindowsPerStore; col++) {
                double cx = x + col * dh;
                glBegin(GL_LINES);
                glVertex2d(cx, cy - 0.5 * hseg);
                glVertex2d(cx, cy + 0.5 * hseg);
                glEnd();

                double cyR = reflect_y(cy);
                double dxR = wave_dx_static(cyR, reflBotY, x);
                double hsegR = REFL_SCALE * hseg;
                glColor3d(wr * 0.95 * REFL_DIM, wg * 0.95 * REFL_DIM, wb * REFL_DIM);
                glBegin(GL_LINES);
                glVertex2d(cx + dxR, cyR - 0.5 * hsegR);
                glVertex2d(cx + dxR, cyR + 0.5 * hsegR);
                glEnd();
                glColor3d(wr * 0.95, wg * 0.95, wb);
            }
        }
    }
    break;

    case 4:
    {
        int r2 = numStores / 2;
        int c2 = numWindowsPerStore / 2;
        double dh2 = width / c2;
        double dv2 = (h - 2 * pad) / (r2 - 1);

        glPointSize(3);
        glColor3d(wr * 0.9, wg * 0.9, wb);
        for (store = 0; store < r2; store++) {
            double cy = bottom + pad + store * dv2;
            for (col = 0; col < c2; col++) {
                double cx = x + col * dh2;
                glBegin(GL_POINTS); glVertex2d(cx, cy); glEnd();

                double cyR = reflect_y(cy);
                double dxR = wave_dx_static(cyR, reflBotY, x);
                glColor3d(wr * 0.9 * REFL_DIM, wg * 0.9 * REFL_DIM, wb * REFL_DIM);
                glBegin(GL_POINTS); glVertex2d(cx + dxR, cyR); glEnd();
                glColor3d(wr * 0.9, wg * 0.9, wb);
            }
        }
    }
    break;
    }

    /* outline with that top/right glow */
    glBegin(GL_LINE_LOOP);
    glColor3d(0, 0, 0);
    glVertex2d(x, y);
    glVertex2d(x, -0.2);
    glVertex2d(x + width, -0.2);
    glColor3d(1, 1, 1);
    glVertex2d(x + width, y);
    glEnd();
}

void DrawBackBuildings()
{
    double x, y;
    double width = 0.13;
    double step = 0.02;

    int numStores = 30;
    int numWindowsPerStore = 10;

    for (x = -1; x <= 1; x += step)
    {
        y = 0.3 * cos(0.3 * PI * x) + 0.15 * sin(x * 17 * PI) + 0.2 * sin(x * 5 * PI);
        drawBuilding(x, y, width, step, numStores, numWindowsPerStore);
        x += width;
    }
}

void DrawFrontBuildings()
{
    double x, y;
    double width = 0.15;
    double step = 0.05;

    int numStores = 20;
    int numWindowsPerStore = 10;

    for (x = -1; x <= 1; x += step)
    {
        y = 0.3 * cos(0.3 * PI * x) + 0.1 * sin(x * 50 * PI) + 0.1 * sin(x * 12 * PI);
        drawBuilding(x, y, width, step, numStores, numWindowsPerStore);
        x += width;
    }
}

void DrawAxis()
{
    glColor3d(1, 1, 1);
    glBegin(GL_LINES);
    glVertex2d(-1, 0);
    glVertex2d(1, 0);
    glVertex2d(0, 1);
    glVertex2d(0, -1);
    glEnd();
}

void DrawFireworks()
{
    int i, j;
    glColor3d(1, 1, 1);

    for (i = 0; i < NUM_FIREWORKS; i++)
    {
        glPointSize(fireworks[i].size);
        glBegin(GL_POINTS);
        glVertex2d(fireworks[i].position.x, fireworks[i].position.y);
        glEnd();

        DrawTail(fireworks[i]);

        if (fireworks[i].expload)
        {
            for (j = 0; j < NUM_PARTICALS; j++)
            {
                if (fireworks[i].explotion[j].life > 0)
                {
                    double intensity = fireworks[i].explotion[j].life / (double)LIFE_LENGHT;
                    glColor3d(intensity, intensity * (i / (double)NUM_FIREWORKS), i / (double)NUM_FIREWORKS);
                    glPointSize(2);
                    glBegin(GL_POINTS);
                    glVertex2d(fireworks[i].explotion[j].x, fireworks[i].explotion[j].y);
                    glEnd();
                }
            }
        }
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    DrawSky();
    DrawFireworks();

    DrawBackBuildings();
    DrawFrontBuildings();

    glutSwapBuffers();
}

void idle()
{
    int index;
    int i, j, allDead;

    if (rand() % 10 == 0)
    {
        index = rand() % NUM_STARS;
        stars[index].size = 1 + rand() % 3;
    }

    for (i = 0; i < NUM_FIREWORKS; i++)
    {
        for (j = TAIL_LENGTH - 1; j > 0; j--)
            fireworks[i].tail[j] = fireworks[i].tail[j - 1];
        fireworks[i].tail[0] = fireworks[i].position;

        fireworks[i].position.y += 0.0004;

        if (fireworks[i].position.y >= 0.8 && fireworks[i].expload == 0)
        {
            fireworks[i].expload = 1;

            for (j = 0; j < NUM_PARTICALS; j++)
            {
                fireworks[i].explotion[j].x = fireworks[i].position.x;
                fireworks[i].explotion[j].y = fireworks[i].position.y;
                fireworks[i].explotion[j].life = LIFE_LENGHT;
            }
        }

        if (fireworks[i].expload)
        {
            for (j = 0; j < NUM_PARTICALS; j++)
            {
                if (fireworks[i].explotion[j].life > 0)
                {
                    fireworks[i].explotion[j].x += ((rand() % 200) - 100) / 10000.0;
                    fireworks[i].explotion[j].y += ((rand() % 200) - 100) / 10000.0;

                    fireworks[i].explotion[j].life--;
                }
            }
        }

        allDead = 1;
        for (j = 0; j < NUM_PARTICALS; j++)
        {
            if (fireworks[i].explotion[j].life > 0)
            {
                allDead = 0;
                break;
            }
        }

        if (fireworks[i].expload && allDead)
        {
            fireworks[i].position.x = -1 + 2 * ((rand() % 1000) / 1000.0);
            fireworks[i].position.y = -0.2;
            fireworks[i].expload = 0;

            for (j = 0; j < TAIL_LENGTH; j++)
                fireworks[i].tail[j] = fireworks[i].position;
        }
    }
    glutPostRedisplay();
}

void main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(400, 100);
    glutCreateWindow("City skyline");
    glutDisplayFunc(display);
    glutIdleFunc(idle);

    init();

    glutMainLoop();
}
