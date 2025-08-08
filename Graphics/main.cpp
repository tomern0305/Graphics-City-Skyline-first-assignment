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

typedef struct
{
	double x, y;
} POINT2D;

typedef struct
{
	double x, y;
	int life;
} PARTICAL2D;

typedef struct
{
	POINT2D position;
	int size;
	POINT2D tail[TAIL_LENGTH];
	int expload;
	PARTICAL2D explotion[NUM_PARTICALS];
}FIREWORK;

typedef struct
{
	POINT2D position;
	int size;
} STAR;

STAR stars[NUM_STARS];
FIREWORK fireworks[NUM_FIREWORKS];

void init()
{
	int i, j;
	glClearColor(0.0, 0.0, 0.3, 0);// color of window background
	glOrtho(-1, 1, -1, 1, -1, 1); // set the coordinates system

	srand(time(0));
	//init stars in the sky
	for (i = 0;i < NUM_STARS;i++)
	{
		stars[i].position.x = -1 + 2 * ((rand() % 1000) / 1000.0); // random value in [-1,1)
		stars[i].position.y = -0.2 + 2 * ((rand() % 1000) / 1000.0); // random value in [-0.2,1)
		stars[i].size = 1 + rand() % 3;

	}
	//init fireworks
	for (i = 0; i < NUM_FIREWORKS; i++)
	{
		fireworks[i].position.x = -1 + 2 * ((rand() % 1000) / 1000.0); // (-1,1)
		fireworks[i].position.y = -0.2;
		fireworks[i].size = 2; //TO BE Changed maybe
		fireworks[i].expload = 0;

		for (j = 0; j < TAIL_LENGTH; j++)
			fireworks[i].tail[j] = fireworks[i].position;

		for (j = 0; j < NUM_PARTICALS; j++)
		{
			fireworks[i].explotion[j].life = LIFE_LENGHT;
		}
	}

}

void DrawTail(FIREWORK s)
{
	int j;
	double intensity = 1;
	glPointSize(2);
	glBegin(GL_POINTS);
	for (j = 0;j < TAIL_LENGTH;j++)
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

	glColor3d(1, 1, 0); // yellow

	for (i = 0;i < NUM_STARS;i++)
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
	glVertex2d(-1, 1); // left-top
	glVertex2d(1, 1); // right-top
	glColor3d(0.9, 0.7, 0.4);
	glVertex2d(1, -0.2); // right-water level
	glVertex2d(-1, -0.2); // left-water level
	glEnd();

	glBegin(GL_POLYGON);
	glColor3d(0.8, 0.5, 0.3);
	glVertex2d(1, -0.2); // right-water level
	glVertex2d(-1, -0.2); // left-water level
	glColor3d(0.4, 0.3, 0.5);
	glVertex2d(-1, -1); // left-top
	glVertex2d(1, -1); // right-top
	glEnd();

	DrawStars();
}

void drawBuilding(double x, double y, double width, double step, int numStores, int numWindowsPerStore)
{
	int store, col, style;
	double dh, dv; // horizontal end vertical spacings
	
	double wr = 0.75, wg = 0.75, wb = 0.10; // warm, less bright

	//rectengle
	glBegin(GL_POLYGON);
	glColor3d(0.2, 0.2, 0.2);
	glVertex2d(x + width, y);
	glVertex2d(x, y);
	glColor3d(	0.3 + 0.3 * fabs(sin(x * 5)),
				0.3 + 0.3 * fabs(sin(x * 3)),
				0.3 + 0.4 * fabs(cos(x * 4)));
	glVertex2d(x, -0.2);
	glVertex2d(x + width, -0.2);
	glEnd();

	//windows
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
			for (col = 0; col < numWindowsPerStore; col++) {
				glBegin(GL_POINTS);
				glVertex2d(x + col * dh, -0.2 + store * dv);
				glEnd();
			}
		}
		break;

	case 1: 
		glPointSize(2);
		glColor3d(wr, wg, wb);
		for (store = 0; store < numStores; store++) {
			double offset = (store % 2) ? 0.5 * dh : 0.0;
			for (col = 0; col < numWindowsPerStore; col++) {
				glBegin(GL_POINTS);
				glVertex2d(x + offset + col * dh, -0.2 + store * dv);
				glEnd();
			}
		}
		break;

	case 2:
		glPointSize(2);
		glColor3d(wr, wg, wb);
		for (store = 0; store < numStores; store++) {
			for (col = 0; col < numWindowsPerStore; col++) {
				if ((store + col) % 2) continue;
				glBegin(GL_POINTS);
				glVertex2d(x + col * dh, -0.2 + store * dv);
				glEnd();
			}
		}
		break;

	case 3:
	{
		double hseg = dv * 0.45; // window height
		glColor3d(wr * 0.95, wg * 0.95, wb); // slightly darker
		for (store = 0; store < numStores; store++) {
			for (col = 0; col < numWindowsPerStore; col++) {
				double cx = x + col * dh;
				double cy = -0.2 + store * dv;
				glBegin(GL_LINES);
				glVertex2d(cx, cy - hseg * 0.5);
				glVertex2d(cx, cy + hseg * 0.5);
				glEnd();
			}
		}
	}
	break;

	case 4:
	{
		int r2 = numStores / 2;
		int c2 = numWindowsPerStore / 2;
		double dh2 = width / c2;
		double dv2 = (y + width + step) / r2;

		glPointSize(3); // bigger dots
		glColor3d(wr * 0.9, wg * 0.9, wb);
		for (store = 0; store < r2; store++) {
			for (col = 0; col < c2; col++) {
				glBegin(GL_POINTS);
				glVertex2d(x + col * dh2, -0.2 + store * dv2);
				glEnd();
			}
		}
	}
	break;
	}
	/*glPointSize(2);
	glColor3d(1, 1, 0);
	for (store = 0; store < numStores; store++)
	{
		for (col = 0; col < numWindowsPerStore; col++)
		{
			glBegin(GL_POINTS);
			glVertex2d(x + col * dh, -0.2 + store * dv);
			glEnd();
		}
	}*/

	//Bulding Outline
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

	// for windows
	int store, col;
	int numStores = 30;
	int numWindowsPerStore = 10;
	double dh, dv; // horizontal end vertical spacings

	// near buildings 
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
	int store, col;
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

	for (i = 0;i < NUM_FIREWORKS;i++)
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
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	DrawSky();
	DrawFireworks();

	DrawBackBuildings();
	DrawFrontBuildings();

	//DrawAxis();
	glutSwapBuffers(); // show all
}

void idle()
{
	int index;
	int i, j, allDead;

	// sparking	for the sky
	if (rand() % 10 == 0)
	{
		index = rand() % NUM_STARS;
		stars[index].size = 1 + rand() % 3;
	}

	//firework going up
	for (i = 0; i < NUM_FIREWORKS; i++)
	{
		// update the tail if the star is still within the window
		for (j = TAIL_LENGTH - 1; j > 0; j--)
		{
			fireworks[i].tail[j] = fireworks[i].tail[j - 1];
		}
		fireworks[i].tail[0] = fireworks[i].position;

		fireworks[i].position.y += 0.0004;

		if (fireworks[i].position.y >= 0.8 && fireworks[i].expload == 0)
		{
			// setting the ground for the explotion
			fireworks[i].expload = 1;

			for (j = 0; j < NUM_PARTICALS; j++)
			{
				fireworks[i].explotion[j].x = fireworks[i].position.x; //settings the location of the explotion
				fireworks[i].explotion[j].y = fireworks[i].position.y;
				fireworks[i].explotion[j].life = LIFE_LENGHT;
			}
		}

		//the explotion
		if (fireworks[i].expload)
		{
			for (j = 0; j < NUM_PARTICALS; j++)
			{
				if (fireworks[i].explotion[j].life > 0)
				{
					fireworks[i].explotion[j].x += ((rand() % 200) - 100) / 10000.0; //range: [-0.02,0.02]
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
	glutPostRedisplay(); // indirect call to display
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE); // defines visual memory
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(400, 100);
	glutCreateWindow("City skyline");
	glutDisplayFunc(display); // this is a pointer to refresh window function
	glutIdleFunc(idle); // this is a pointer to idle function that runs in background all the time

	init();

	glutMainLoop();
}