/***************************************************************************
						  gameview.h  -  description
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

#ifndef GameView_H
#define GameView_H

#include <string>

#define INITIAL_WINDOW_WIDTH  400
#define INITIAL_WINDOW_HEIGHT 300
#define SCORE_WINDOW_HEIGHT    20

class GameModel;
class Room;
class Player;

class GameView {
public:
	GameView(const GameModel* model, const int mainWindow, const int scoreWindow);
	~GameView();

	void initializeGL();
	void resizeGL(int w, int h) const;
	void paintGL() const;
	void paintScoreGL() const;

	// Requests a redraw of the 3D window by name. glutPostRedisplay() would
	// target whichever window happens to be current, which is not defined
	// inside a timer callback.
	void update() const;

	void setScoreString(const std::string& scoreString);

private:
	const GameModel* m_;
	const int mainWindow_;
	const int scoreWindow_;
	std::string scoreString_;

	GLuint makeWall();
	GLuint makeWallWithDoor();
	GLuint makeGround();
	GLuint makeShot();
	GLuint makePlayer();
	GLuint makeLeftLeg();
	GLuint makeRightLeg();

	void drawRoom(const Room* room) const;
	void drawPlayer(const Player* player, int i) const;
};

#endif // GameView_H
