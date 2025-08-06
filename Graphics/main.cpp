
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
	int i,j;
	glClearColor(0.0,0.0,0.3,0);// color of window background
	glOrtho(-1, 1, -1, 1, -1, 1); // set the coordinates system

	srand(time(0));

	for (i = 0;i < NUM_STARS;i++)
	{
		stars[i].position.x = -1 + 2 * ((rand() % 1000) / 1000.0); // random value in [-1,1)
		stars[i].position.y = -0.2 + 2 * ((rand() % 1000) / 1000.0); // random value in [-0.2,1)
		stars[i].size = 1 + rand() % 3;

	}

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

void DrawBackBuildings()
{
	double x, y;
	double width = 0.13;
	double step = 0.02;

	// for windows
	int store, col;
	int numStores = 25;
	int numWindowsPerStore = 10;
	double dh, dv; // horizontal end vertical spacings

	// near buildings 
	for (x = -1; x <= 1; x += step)
	{
		y = 0.3 * cos(0.3 * PI * x) + 0.15 * sin(x * 17 * PI) + 0.2 * sin(x * 5 * PI);
		//Building Rectengle
		glBegin(GL_POLYGON);
		glColor3d(0.2, 0.2 * x, 0.2);
		glVertex2d(x + width, y);
		glVertex2d(x, y);
		glColor3d(0.3,  fabs(y), 0.3 * y);
		glVertex2d(x, -0.2);
		glVertex2d(x + width, -0.2);
		glEnd();

		//Windows
		dh = width / numWindowsPerStore;
		dv = (y + width + step * 4) / numStores;
		glPointSize(2);
		glColor3d(1, 1, 0);
		for (store = 0; store < numStores; store++)
		{
			for (col = 0; col < numWindowsPerStore; col++)
			{
				glBegin(GL_POINTS);
				glVertex2d(x + col * dh, -0.2 + store * dv);
				glEnd();
			}
		}

		//Bulding Outline
		glBegin(GL_LINE_LOOP);
		glColor3d(0, 0, 0);
		glVertex2d(x, y);
		glVertex2d(x, -0.2);
		glVertex2d(x + width, -0.2);
		glColor3d(1, 1, 1);
		glVertex2d(x + width, y);
		glEnd();

		//Reflections

		

		x += width;
	}

}


void DrawFrontBuildings()
{
	double x, y;
	double width = 0.15;
	double step = 0.05;
	
	// for windows
	int store, col;
	int numStores = 20;
	int numWindowsPerStore = 10;
	double dh, dv; // horizontal end vertical spacings
	
	// near buildings 
	for (x = -1; x <= 1; x += step)
	{

		y = 0.3 * cos(0.3 * PI * x) + 0.1 * sin(x * 50 * PI) + 0.1 * sin(x * 12 * PI);
		//Building Rectengle
		glBegin(GL_POLYGON);
		glColor3d(0.2, 0.2, 0.2);
		glVertex2d(x + width, y);
		glVertex2d(x, y);
		glColor3d(0.8, 0.8 , 0.7);
		glVertex2d(x, -0.2);
		glVertex2d(x + width, -0.2);
		glEnd();

		//Windows
		dh = width / numWindowsPerStore;
		dv = (y + width + step) / numStores;
		glPointSize(2);
		glColor3d(1, 1, 0);
		for (store = 0; store < numStores; store++)
		{
			for (col= 0; col < numWindowsPerStore; col++)
			{
				glBegin(GL_POINTS);
					glVertex2d(x + col * dh, -0.2 + store * dv);
				glEnd();
			}
		}
		
		//Bulding Outline
		glBegin(GL_LINE_LOOP);
		glColor3d(0, 0, 0);
		glVertex2d(x, y);
		glVertex2d(x, -0.2);
		glVertex2d(x + width, -0.2);
		glColor3d(1, 1, 1);
		glVertex2d(x + width, y);
		glEnd();

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

	DrawAxis();
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