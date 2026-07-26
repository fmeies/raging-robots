/***************************************************************************
						  main.cpp  -  description
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

#include <GL/glut.h>
#include <iostream>

#include "gameview.h"
#include "gamemodel.h"
#include "gamecontroller.h"

GameView* view;
GameController* controller;

void display(void) {
	view->paintGL();
}

void displayScore(void) {
	view->paintScoreGL();
}

void reshape(int x, int y) {
	view->resizeGL(x, y);
}

#define TIMER_MS 50

// Each key press starts its own repeat chain. The generation tag lets a new
// chain retire the previous one, otherwise they all keep ticking side by side
// and the player moves once per surviving chain.
static int keyRepeatGeneration = 0;

void keyRepeat(int generation) {
	if (generation != keyRepeatGeneration) {
		return;
	}
	const bool repeat = controller->keyRepeat();
	if(repeat){
		glutTimerFunc(TIMER_MS, keyRepeat, generation);
	}
}

void keyPressed(unsigned char key, int, int) {
	controller->keyPressed(key);
	glutTimerFunc(TIMER_MS * 2, keyRepeat, ++keyRepeatGeneration);
}

void keyReleased(unsigned char key, int, int) {
	controller->keyReleased(key);
}

void timer(int value) {
	controller->timerEvent();
	glutTimerFunc(TIMER_MS, timer, value);
}

void refreshScore(int p){
	controller->setTitle(p);
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	const int mainWindow = glutCreateWindow("Raging Robots");
	glutReshapeWindow(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(TIMER_MS, timer, 0);
	glutKeyboardFunc(keyPressed);
	glutKeyboardUpFunc(keyReleased);
	glutIgnoreKeyRepeat(true);

	const int scoreWindow = glutCreateSubWindow(mainWindow, 0, 0, INITIAL_WINDOW_WIDTH, SCORE_WINDOW_HEIGHT);
	glutDisplayFunc(displayScore);
	glutSetWindow(mainWindow);

	const char* server = "localhost";
	if (argc == 2) {
		server = argv[1];
	} else {
		std::cout << "No server specified on command line, choosing localhost.\n";
	}

	GameModel* model = new GameModel();
	view = new GameView(model, mainWindow, scoreWindow);
	controller = new GameController(model, view, server);

	view->initializeGL();

	glutMainLoop();
	return 0;
}
