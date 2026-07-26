/***************************************************************************
						  gameview.cpp  -  description
							 -------------------
	begin                : Sun Mar 19 15:05:04 MET 2000
	copyright            : (C) 2000 by Frank Meies
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <cstdio>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// application specific includes
#include "gameview.h"
#include "ppmreader.h"
#include "gamemodel.h"
#include "room.h"
#include "player.h"
#include "shot.h"

// Data structure for textures
typedef struct _Texture {
	int sizeX;
	int sizeY;
	unsigned char* data;
	GLuint texName;
} Texture;

// DATA_DIR_PREFIX is set at compile time and can be arbitrarily long, so the
// path is built on the heap rather than in a fixed-size buffer.
static std::string dataPath(const char* fileName) {
	return std::string(DATA_DIR_PREFIX) + "/share/ragingrobots/" + fileName;
}

// A missing or damaged texture leaves nothing sensible to draw, so say what is
// wrong and stop rather than handing a null pointer to OpenGL.
static unsigned char* loadTexture(const char* fileName, int* width, int* height) {
	const std::string path = dataPath(fileName);
	unsigned char* data = read_ppm_file(path.c_str(), width, height);
	if (!data) {
		std::cout << "Couldn't load texture " << path << std::endl;
		exit(1);
	}
	return data;
}

static Texture ruin;
static Texture face;
static Texture ground;

// One RGBA colour per player; glMaterialfv reads four components, so the rows
// have to be four wide.
static GLfloat material[][4] = {
	{1.0, 0.0, 0.0, 1.0},
	{0.0, 1.0, 0.0, 1.0},
	{0.0, 0.0, 1.0, 1.0}
};

// display lists
static GLuint walllist;
static GLuint walldoorlist;
static GLuint groundlist;
static GLuint shotlist;
static GLuint playerlist;
static GLuint leftleglist;
static GLuint rightleglist;

// main widget
GameView::GameView(const GameModel* m, const int mainWindow, const int scoreWindow)
: m_(m), mainWindow_(mainWindow), scoreWindow_(scoreWindow) {
}

GameView::~GameView() {
	glDeleteLists(walllist, 1);
	glDeleteLists(walldoorlist, 1);
	glDeleteLists(groundlist, 1);
	glDeleteLists(shotlist, 1);
	glDeleteLists(playerlist, 1);
	glDeleteLists(leftleglist, 1);
	glDeleteLists(rightleglist, 1);
}

void GameView::initializeGL() {
	// lights
	GLfloat ambient[] = {0.6, 0.6, 0.6, 1.0};
	GLfloat diffuse[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat global_ambient[] = {0.4, 0.4, 0.4, 1.0};
	GLfloat position[] = {3.5, 2.2, 4, 1};

	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 4.0);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	glShadeModel(GL_SMOOTH);

	// default color
	glClearColor(0.78, 0.85, 1.0, 1.0);

	// textures
	ruin.sizeX = 256;
	ruin.sizeY = 256;
	ruin.data = loadTexture("ruins.ppm", &ruin.sizeX, &ruin.sizeY);
	glGenTextures(1, &ruin.texName);
	glBindTexture(GL_TEXTURE_2D, ruin.texName);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, ruin.sizeX, ruin.sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, ruin.data);

	face.sizeX = 64;
	face.sizeY = 64;
	face.data = loadTexture("face.ppm", &face.sizeX, &face.sizeY);
	glGenTextures(1, &face.texName);
	glBindTexture(GL_TEXTURE_2D, face.texName);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, face.sizeX, face.sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, face.data);

	ground.sizeX = 128;
	ground.sizeY = 128;
	ground.data = loadTexture("ground.ppm", &ground.sizeX, &ground.sizeY);
	glGenTextures(1, &ground.texName);
	glBindTexture(GL_TEXTURE_2D, ground.texName);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, ground.sizeX, ground.sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, ground.data);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// enable culling
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);

	// generate display lists
	walllist = makeWall();
	walldoorlist = makeWallWithDoor();
	groundlist = makeGround();
	shotlist = makeShot();
	playerlist = makePlayer();
	leftleglist = makeLeftLeg();
	rightleglist = makeRightLeg();

	// fog
	GLfloat fog[] = {0.5, 0.5, 0.5, 1.0};
	glFogi(GL_FOG_MODE, GL_EXP);
	glFogfv(GL_FOG_COLOR, fog);
	glFogf(GL_FOG_DENSITY, 0.5);
	glHint(GL_FOG_HINT, GL_FASTEST);
	glFogf(GL_FOG_START, 0.5);
	glFogf(GL_FOG_END, 5.0);
}

void GameView::resizeGL(int w, int h) const {
	// main window
	glutSetWindow(mainWindow_);
	glViewport(0, 0, (GLint) w, (GLint) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	const float ratio = (float) w / (float) h;
	gluPerspective(45, ratio, 0.001, 35.0);

	// score window
	glutSetWindow(scoreWindow_);
	glutReshapeWindow(w, SCORE_WINDOW_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, w, 0.0, SCORE_WINDOW_HEIGHT);
	glutSetWindow(mainWindow_);
}

void GameView::paintGL() const {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw room
	const Player* me = m_->getMe();

	double x = me->getX();
	double z = me->getZ();
	const double dx = me->getDirX();
	const double dz = me->getDirZ();

	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(x, 1.0, z, x + dx, 1.0, z + dz, 0, 1, 0);

	const int rooms = m_->getNumberOfRooms();
	for (int i = 0; i < rooms; ++i) {
		drawRoom(m_->getRoom(i));
	}

	// draw players
	glBindTexture(GL_TEXTURE_2D, face.texName);
	int i;
	for (i = 0; i < MAXPLAYERS; ++i) {
		const Player* player = m_->getPlayer(i);
		drawPlayer(player, i);
	}

	glDisable(GL_TEXTURE_2D);

	// draw shots
	for (i = 0; i < MAXPLAYERS; i++) {
		Shot* shot = m_->getShot(i);
		if (shot->getStatus() == SVALID) {
			x = shot->getX();
			z = shot->getZ();
			double a = shot->getAngle() * 180.0 / M_PI;
			glPushMatrix();
			glTranslatef(x, 0, z);
			glRotatef(a, 0, 1, 0);
			glCallList(shotlist);
			glPopMatrix();
		}
	}

	// draw my shot
	Shot* myShot = m_->getMyShot();
	if (myShot->getStatus() == SVALID) {
		x = myShot->getX();
		z = myShot->getZ();
		double a = myShot->getAngle() * 180.0 / M_PI;
		glPushMatrix();
		glTranslatef(x, 0, z);
		glRotatef(a, 0, 1, 0);
		glCallList(shotlist);
		glPopMatrix();
	}

	// error handling     
	/*GLenum errCode;
	const GLubyte* errString;
	if ((errCode = glGetError()) != GL_NO_ERROR) {
		errString = gluErrorString(errCode);
		std::cout<<"OpenGL error: "<<errString<<std::endl;
	}*/

	glutSwapBuffers();
}

void GameView::update() const {
	glutPostWindowRedisplay(mainWindow_);
}

void GameView::paintScoreGL() const {
	glutSetWindow(scoreWindow_);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(4, 4);

	void * font = GLUT_BITMAP_HELVETICA_12;
	for (std::string::const_iterator i = scoreString_.begin(); i != scoreString_.end(); ++i) {
		glutBitmapCharacter(font, *i);
	}

	glutSwapBuffers();
	glutSetWindow(mainWindow_);
}

GLuint GameView::makeWall() {
	GLuint list = glGenLists(1);

	glNewList(list, GL_COMPILE);

	GLfloat mat[] = {1.0, 1.0, 1.0, 1.0};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat);

	glBegin(GL_QUADS);

	// front
	// s follows x and t follows y, one tile per two world units. Getting the
	// two axes the wrong way round turns the texture by 90 degrees, and equal
	// repeat counts on a wall that is 8 wide and 2 high stretch it fourfold.
	glNormal3f(0, 0, -1);
	glTexCoord2f(0, 0);
	glVertex3f(-4.0, 0, 0.0);
	glTexCoord2f(4, 0);
	glVertex3f(4.0, 0, 0.0);
	glTexCoord2f(4, 1);
	glVertex3f(4.0, 2.0, 0.0);
	glTexCoord2f(0, 1);
	glVertex3f(-4.0, 2.0, 0.0);

	glEnd();
	glEndList();

	return (list);
}

GLuint GameView::makeWallWithDoor() {
	GLuint list = glGenLists(1);

	glNewList(list, GL_COMPILE);

	GLfloat mat[] = {1.0, 1.0, 1.0, 1.0};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat);

	glBegin(GL_QUADS);

	// Same mapping as the plain wall, and continuous across the doorway so the
	// courses on both sides stay in line: s = (x + 4) / 2, t = y / 2.

	// left
	glNormal3f(0, 0, -1);
	glTexCoord2f(0, 0);
	glVertex3f(-4.0, 0, 0.0);
	glTexCoord2f(1.5, 0);
	glVertex3f(-1.0, 0, 0.0);
	glTexCoord2f(1.5, 1);
	glVertex3f(-1.0, 2.0, 0.0);
	glTexCoord2f(0, 1);
	glVertex3f(-4.0, 2.0, 0.0);

	// right
	glNormal3f(0, 0, -1);
	glTexCoord2f(2.5, 0);
	glVertex3f(1.0, 0, 0.0);
	glTexCoord2f(4, 0);
	glVertex3f(4.0, 0.0, 0.0);
	glTexCoord2f(4, 1);
	glVertex3f(4.0, 2.0, 0.0);
	glTexCoord2f(2.5, 1);
	glVertex3f(1.0, 2.0, 0.0);

	glEnd();
	glEndList();

	return (list);
}

GLuint GameView::makeGround() {
	GLuint list = glGenLists(1);

	glNewList(list, GL_COMPILE);

	glBegin(GL_QUADS);

	GLfloat mat[] = {1.0, 1.0, 1.0, 1.0};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat);

	glNormal3f(0, 1, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-4.0, 0, -4.0);
	glTexCoord2f(8, 0);
	glVertex3f(-4.0, 0, 4.0);
	glTexCoord2f(8, 8);
	glVertex3f(4.0, 0, 4.0);
	glTexCoord2f(0, 8);
	glVertex3f(4.0, 0, -4.0);

	glEnd();
	glEndList();

	return (list);
}

GLuint GameView::makeShot() {
	GLuint list;

	list = glGenLists(1);

	glNewList(list, GL_COMPILE);

	GLfloat mat[] = {1.0, 1.0, 0.0, 1.0};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat);

	glBegin(GL_QUADS);

	glNormal3f(0, 1, 0);
	glVertex3f(-0.1, 0.6, 0.3);
	glVertex3f(0.1, 0.6, 0.3);
	glVertex3f(0.1, 0.6, -0.1);
	glVertex3f(-0.1, 0.6, -0.1);

	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex3f(-0.3, 0.6, -0.1);
	glVertex3f(0.3, 0.6, -0.1);
	glVertex3f(0.0, 0.6, -0.3);

	glEnd();

	glEndList();

	return (list);
}

GLuint GameView::makePlayer() {
	GLuint list;

	list = glGenLists(1);

	glNewList(list, GL_COMPILE);

	glBegin(GL_QUADS);

	// body
	// front
	glNormal3f(0, 0, 1);
	glVertex3f(-0.2, 0.35, 0.1);
	glVertex3f(0.2, 0.35, 0.1);
	glVertex3f(0.2, 0.7, 0.1);
	glVertex3f(-0.2, 0.7, 0.1);
	// back
	glNormal3f(0, 0, -1);
	glVertex3f(-0.2, 0.35, -0.1);
	glVertex3f(-0.2, 0.7, -0.1);
	glVertex3f(0.2, 0.7, -0.1);
	glVertex3f(0.2, 0.35, -0.1);
	// left
	glNormal3f(-1, 0, 0);
	glVertex3f(-0.2, 0.35, 0.1);
	glVertex3f(-0.2, 0.7, 0.1);
	glVertex3f(-0.2, 0.7, -0.1);
	glVertex3f(-0.2, 0.35, -0.1);
	// right
	glNormal3f(1, 0, 0);
	glVertex3f(0.2, 0.35, 0.1);
	glVertex3f(0.2, 0.35, -0.1);
	glVertex3f(0.2, 0.7, -0.1);
	glVertex3f(0.2, 0.7, 0.1);
	// top
	glNormal3f(0, 1, 0);
	glVertex3f(-0.2, 0.7, 0.1);
	glVertex3f(0.2, 0.7, 0.1);
	glVertex3f(0.2, 0.7, -0.1);
	glVertex3f(-0.2, 0.7, -0.1);
	// bottom

	// neck
	// front
	glNormal3f(0, 0, 1);
	glVertex3f(-0.05, 0.7, 0.05);
	glVertex3f(0.05, 0.7, 0.05);
	glVertex3f(0.05, 0.75, 0.05);
	glVertex3f(-0.05, 0.75, 0.05);
	// back
	glNormal3f(0, 0, -1);
	glVertex3f(-0.05, 0.7, -0.05);
	glVertex3f(-0.05, 0.75, -0.05);
	glVertex3f(0.05, 0.75, -0.05);
	glVertex3f(0.05, 0.7, -0.05);
	// left
	glNormal3f(-1, 0, 0);
	glVertex3f(-0.05, 0.7, 0.05);
	glVertex3f(-0.05, 0.75, 0.05);
	glVertex3f(-0.05, 0.75, -0.05);
	glVertex3f(-0.05, 0.7, -0.05);
	// right
	glNormal3f(1, 0, 0);
	glVertex3f(0.05, 0.7, 0.05);
	glVertex3f(0.05, 0.7, -0.05);
	glVertex3f(0.05, 0.75, -0.05);
	glVertex3f(0.05, 0.75, 0.05);

	// head
	// front
	glNormal3f(0, 0, 1);
	glTexCoord2f(1, 1);
	glVertex3f(-0.12, 0.75, 0.12);
	glTexCoord2f(0, 1);
	glVertex3f(0.12, 0.75, 0.12);
	glTexCoord2f(0, 0);
	glVertex3f(0.12, 1.0, 0.12);
	glTexCoord2f(1, 0);
	glVertex3f(-0.12, 1.0, 0.12);
	// back
	glNormal3f(0, 0, -1);
	glVertex3f(-0.12, 0.75, -0.12);
	glVertex3f(-0.12, 1.0, -0.12);
	glVertex3f(0.12, 1.0, -0.12);
	glVertex3f(0.12, 0.75, -0.12);
	// left
	glNormal3f(-1, 0, 0);
	glVertex3f(-0.12, 0.75, 0.12);
	glVertex3f(-0.12, 1.0, 0.12);
	glVertex3f(-0.12, 1.0, -0.12);
	glVertex3f(-0.12, 0.75, -0.12);
	// right
	glNormal3f(1, 0, 0);
	glVertex3f(0.12, 0.75, 0.12);
	glVertex3f(0.12, 0.75, -0.12);
	glVertex3f(0.12, 1.0, -0.12);
	glVertex3f(0.12, 1.0, 0.12);

	// arm1
	// front
	glNormal3f(0, 0, 1);
	glVertex3f(-0.3, 0.6, 0.28);
	glVertex3f(-0.2, 0.6, 0.28);
	glVertex3f(-0.2, 0.7, 0.28);
	glVertex3f(-0.3, 0.7, 0.28);
	// back
	glNormal3f(0, 0, -1);
	glVertex3f(-0.3, 0.6, -0.05);
	glVertex3f(-0.3, 0.7, -0.05);
	glVertex3f(-0.2, 0.7, -0.05);
	glVertex3f(-0.2, 0.6, -0.05);
	// left
	glNormal3f(-1, 0, 0);
	glVertex3f(-0.3, 0.6, 0.28);
	glVertex3f(-0.3, 0.7, 0.28);
	glVertex3f(-0.3, 0.7, -0.05);
	glVertex3f(-0.3, 0.6, -0.05);
	// right
	glNormal3f(1, 0, 0);
	glVertex3f(-0.2, 0.6, 0.28);
	glVertex3f(-0.2, 0.6, -0.05);
	glVertex3f(-0.2, 0.7, -0.05);
	glVertex3f(-0.2, 0.7, 0.28);
	// top
	glNormal3f(0, 1, 0);
	glVertex3f(-0.3, 0.7, 0.28);
	glVertex3f(-0.2, 0.7, 0.28);
	glVertex3f(-0.2, 0.7, -0.05);
	glVertex3f(-0.3, 0.7, -0.05);
	// bottom
	glNormal3f(0, -1, 0);
	glVertex3f(-0.3, 0.6, 0.28);
	glVertex3f(-0.3, 0.6, -0.05);
	glVertex3f(-0.2, 0.6, -0.05);
	glVertex3f(-0.2, 0.6, 0.28);

	// arm2
	// front
	glNormal3f(0, 0, 1);
	glVertex3f(0.3, 0.6, 0.28);
	glVertex3f(0.3, 0.7, 0.28);
	glVertex3f(0.2, 0.7, 0.28);
	glVertex3f(0.2, 0.6, 0.28);
	// back
	glNormal3f(0, 0, -1);
	glVertex3f(0.3, 0.6, -0.05);
	glVertex3f(0.2, 0.6, -0.05);
	glVertex3f(0.2, 0.7, -0.05);
	glVertex3f(0.3, 0.7, -0.05);
	// right
	glNormal3f(1, 0, 0);
	glVertex3f(0.3, 0.6, 0.28);
	glVertex3f(0.3, 0.6, -0.05);
	glVertex3f(0.3, 0.7, -0.05);
	glVertex3f(0.3, 0.7, 0.28);
	// left
	glNormal3f(-1, 0, 0);
	glVertex3f(0.2, 0.6, 0.28);
	glVertex3f(0.2, 0.7, 0.28);
	glVertex3f(0.2, 0.7, -0.05);
	glVertex3f(0.2, 0.6, -0.05);
	// top
	glNormal3f(0, 1, 0);
	glVertex3f(0.3, 0.7, 0.28);
	glVertex3f(0.3, 0.7, -0.05);
	glVertex3f(0.2, 0.7, -0.05);
	glVertex3f(0.2, 0.7, 0.28);
	// bottom
	glNormal3f(0, 1, 0);
	glVertex3f(0.3, 0.6, 0.28);
	glVertex3f(0.2, 0.6, 0.28);
	glVertex3f(0.2, 0.6, -0.05);
	glVertex3f(0.3, 0.6, -0.05);

	glEnd();

	glEndList();

	return (list);
}

GLuint GameView::makeLeftLeg() {
	GLuint list;

	list = glGenLists(1);

	glNewList(list, GL_COMPILE);

	glBegin(GL_QUADS);

	// leg1
	// front
	glNormal3f(0, 0, 1);
	glVertex3f(-0.15, 0.1, 0.05);
	glVertex3f(-0.05, 0.1, 0.05);
	glVertex3f(-0.05, 0.35, 0.05);
	glVertex3f(-0.15, 0.35, 0.05);
	// back
	glNormal3f(0, 0, -1);
	glVertex3f(-0.15, 0.1, -0.05);
	glVertex3f(-0.15, 0.35, -0.05);
	glVertex3f(-0.05, 0.35, -0.05);
	glVertex3f(-0.05, 0.1, -0.05);
	// left
	glNormal3f(-1, 0, 0);
	glVertex3f(-0.15, 0.1, 0.05);
	glVertex3f(-0.15, 0.35, 0.05);
	glVertex3f(-0.15, 0.35, -0.05);
	glVertex3f(-0.15, 0.1, -0.05);
	// right
	glNormal3f(1, 0, 0);
	glVertex3f(-0.05, 0.1, 0.05);
	glVertex3f(-0.05, 0.1, -0.05);
	glVertex3f(-0.05, 0.35, -0.05);
	glVertex3f(-0.05, 0.35, 0.05);
	// top
	glNormal3f(1, 0, 0);
	glVertex3f(-0.15, 0.35, 0.05);
	glVertex3f(-0.05, 0.35, 0.05);
	glVertex3f(-0.05, 0.35, -0.05);
	glVertex3f(-0.15, 0.35, -0.05);

	// foot1
	// front
	glNormal3f(0, 0, 1);
	glVertex3f(-0.15, 0.05, 0.17);
	glVertex3f(-0.05, 0.05, 0.17);
	glVertex3f(-0.05, 0.1, 0.17);
	glVertex3f(-0.15, 0.1, 0.17);
	// back
	glNormal3f(0, 0, -1);
	glVertex3f(-0.15, 0.05, -0.05);
	glVertex3f(-0.15, 0.1, -0.05);
	glVertex3f(-0.05, 0.1, -0.05);
	glVertex3f(-0.05, 0.05, -0.05);
	// left
	glNormal3f(-1, 0, 0);
	glVertex3f(-0.15, 0.05, 0.17);
	glVertex3f(-0.15, 0.1, 0.17);
	glVertex3f(-0.15, 0.1, -0.05);
	glVertex3f(-0.15, 0.05, -0.05);
	// right
	glNormal3f(1, 0, 0);
	glVertex3f(-0.05, 0.05, 0.17);
	glVertex3f(-0.05, 0.05, -0.05);
	glVertex3f(-0.05, 0.1, -0.05);
	glVertex3f(-0.05, 0.1, 0.17);
	// top
	glNormal3f(0, 1, 0);
	glVertex3f(-0.15, 0.1, 0.17);
	glVertex3f(-0.05, 0.1, 0.17);
	glVertex3f(-0.05, 0.1, -0.05);
	glVertex3f(-0.15, 0.1, -0.05);
	// bottom
	glNormal3f(0, 1, 0);
	glVertex3f(-0.15, 0.05, 0.17);
	glVertex3f(-0.15, 0.05, -0.05);
	glVertex3f(-0.05, 0.05, -0.05);
	glVertex3f(-0.05, 0.05, 0.17);

	glEnd();

	glEndList();

	return (list);
}

GLuint GameView::makeRightLeg() {
	GLuint list;

	list = glGenLists(1);

	glNewList(list, GL_COMPILE);

	glBegin(GL_QUADS);

	// leg2
	// front
	glNormal3f(0, 0, 1);
	glVertex3f(0.15, 0.1, 0.05);
	glVertex3f(0.15, 0.35, 0.05);
	glVertex3f(0.05, 0.35, 0.05);
	glVertex3f(0.05, 0.1, 0.05);
	// back
	glNormal3f(0, 0, -1);
	glVertex3f(0.15, 0.1, -0.05);
	glVertex3f(0.05, 0.1, -0.05);
	glVertex3f(0.05, 0.35, -0.05);
	glVertex3f(0.15, 0.35, -0.05);
	// right
	glNormal3f(1, 0, 0);
	glVertex3f(0.15, 0.1, 0.05);
	glVertex3f(0.15, 0.1, -0.05);
	glVertex3f(0.15, 0.35, -0.05);
	glVertex3f(0.15, 0.35, 0.05);
	// left
	glNormal3f(-1, 0, 0);
	glVertex3f(0.05, 0.1, 0.05);
	glVertex3f(0.05, 0.35, 0.05);
	glVertex3f(0.05, 0.35, -0.05);
	glVertex3f(0.05, 0.1, -0.05);
	// top
	glNormal3f(1, 0, 0);
	glVertex3f(0.05, 0.35, 0.05);
	glVertex3f(0.15, 0.35, 0.05);
	glVertex3f(0.15, 0.35, -0.05);
	glVertex3f(0.05, 0.35, -0.05);

	// foot2
	// front
	glNormal3f(0, 0, 1);
	glVertex3f(0.15, 0.05, 0.17);
	glVertex3f(0.15, 0.1, 0.17);
	glVertex3f(0.05, 0.1, 0.17);
	glVertex3f(0.05, 0.05, 0.17);
	// back
	glNormal3f(0, 0, -1);
	glVertex3f(0.15, 0.05, -0.05);
	glVertex3f(0.05, 0.05, -0.05);
	glVertex3f(0.05, 0.1, -0.05);
	glVertex3f(0.15, 0.1, -0.05);
	// right
	glNormal3f(1, 0, 0);
	glVertex3f(0.15, 0.05, 0.17);
	glVertex3f(0.15, 0.05, -0.05);
	glVertex3f(0.15, 0.1, -0.05);
	glVertex3f(0.15, 0.1, 0.17);
	// left
	glNormal3f(-1, 0, 0);
	glVertex3f(0.05, 0.05, 0.17);
	glVertex3f(0.05, 0.1, 0.17);
	glVertex3f(0.05, 0.1, -0.05);
	glVertex3f(0.05, 0.05, -0.05);
	// top
	glNormal3f(0, 1, 0);
	glVertex3f(0.15, 0.1, 0.17);
	glVertex3f(0.15, 0.1, -0.05);
	glVertex3f(0.05, 0.1, -0.05);
	glVertex3f(0.05, 0.1, 0.17);
	// bottom
	glNormal3f(0, 1, 0);
	glVertex3f(0.15, 0.05, 0.17);
	glVertex3f(0.05, 0.05, 0.17);
	glVertex3f(0.05, 0.05, -0.05);
	glVertex3f(0.15, 0.05, -0.05);

	glEnd();

	glEndList();

	return (list);
}

void GameView::drawRoom(const Room* room) const {
	int x = room->getX();
	int z = room->getZ();

	glBindTexture(GL_TEXTURE_2D, ruin.texName);

	// draw north
	glPushMatrix();
	glTranslatef(x, 0, z - 4);
	if (room->getNeighbor(NORTH)) {
		glCallList(walldoorlist);
	} else {
		glCallList(walllist);
	}
	glPopMatrix();

	// draw west
	glPushMatrix();
	glTranslatef(x - 4, 0, z);
	glRotatef(90, 0, 1, 0);
	if (room->getNeighbor(WEST)) {
		glCallList(walldoorlist);
	} else {
		glCallList(walllist);
	}
	glPopMatrix();

	// draw east
	glPushMatrix();
	glTranslatef(x + 4, 0, z);
	glRotatef(-90, 0, 1, 0);
	if (room->getNeighbor(EAST)) {
		glCallList(walldoorlist);
	} else {
		glCallList(walllist);
	}
	glPopMatrix();

	// draw south
	glPushMatrix();
	glTranslatef(x, 0, z + 4);
	glRotatef(180, 0, 1, 0);
	if (room->getNeighbor(SOUTH)) {
		glCallList(walldoorlist);
	} else {
		glCallList(walllist);
	}
	glPopMatrix();

	// draw ground
	glBindTexture(GL_TEXTURE_2D, ground.texName);
	glPushMatrix();
	glTranslatef(x, 0, z);
	glCallList(groundlist);
	glPopMatrix();
}

void GameView::drawPlayer(const Player* player, int i) const {
	if (player->getStatus() != PALIVE)
		return;

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, material[i % 3]);
	const double x = player->getX();
	const double z = player->getZ();
	const double a = player->getAngle() * 180.0 / M_PI;
	glPushMatrix();
	glTranslatef(x, 0, z);
	glRotatef(180.0 + a, 0, 1, 0);
	glCallList(playerlist);
	glPushMatrix();
	glTranslatef(0, 0.35, 0);
	glRotatef(player->getLegAngle(), 1, 0, 0);
	glTranslatef(0, -0.35, 0);
	glCallList(leftleglist);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0, 0.35, 0);
	glRotatef(-player->getLegAngle(), 1, 0, 0);
	glTranslatef(0, -0.35, 0);
	glCallList(rightleglist);
	glPopMatrix();
	glPopMatrix();
}

void GameView::setScoreString(const std::string& scoreString) {
	scoreString_ = scoreString;
}

