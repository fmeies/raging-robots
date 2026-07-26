/***************************************************************************
						  gamecontroller.h  -  description
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

#ifndef GameController_H
#define GameController_H

class GameModel;
class GameView;
class Network;

class GameController {
public:
	GameController(GameModel* model, GameView* view, const char* server);
	~GameController();

	// methods for sending data to network
	void reportHit(int playerNo);
	void reportShot() const;
	void reportPosition() const;

	void handleShotPosition(int p, bool valid, double x, double y, double a);
	void handlePlayerPosition(int p, double x, double y, double a, double la);
	void handlePlayerHit(int p);
	void handlePlayerRemove(int p);

	void timerEvent();
	bool playerIdle();
	void keyPressed(unsigned char e);
	void keyReleased(unsigned char e);
	bool keyRepeat();
	void setTitle(int p);

private:
	Network* n_;
	GameModel* m_;
	GameView* v_;
	unsigned char lastKey_;
};

#endif // GameView_H
