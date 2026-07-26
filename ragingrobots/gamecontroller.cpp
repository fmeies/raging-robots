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

#include <iostream>
#include <sstream>
#include <math.h>
#include <GL/glut.h>

// application specific includes
#include "gamecontroller.h"
#include "gamemodel.h"
#include "gameview.h"
#include "network.h"
#include "room.h"
#include "player.h"
#include "shot.h"

// main widget

GameController::GameController(GameModel* m, GameView* v, const char* server)
: m_(m), v_(v), lastKey_(0) {
	// network
	n_ = new Network(this);
	if (!n_->create(server, 7986)) {
		std::cout << "cannot connect to server " << server << "!" << std::endl;
		return;
	}

	// hey, I'm alive!
	Player* me = m_->getMe();
	n_->sendPlayerPosition(me->getX(), me->getZ(), me->getAngle(), me->getLegAngle());
	setTitle(-1);
}

GameController::~GameController() {
	delete n_;
}

void GameController::timerEvent() {
	
	n_->doServer();
	bool update = false;

	// update my shot
	Shot* myShot = m_->getMyShot();
	if (myShot->getStatus() == SVALID) {
		myShot->forward();
		// test, if other players are hit
		const double sx = myShot->getX();
		const double sz = myShot->getZ();
		for (int i = 0; i < MAXPLAYERS; i++) {
			const Player* player = m_->getPlayer(i);
			if (player->getStatus() & PALIVE) {
				const double ox = player->getX();
				const double oz = player->getZ();
				if (fabs(sx - ox) < 0.25 && fabs(sz - oz) < 0.25) {
					// i hit player i
					reportHit(i);
					n_->sendPlayerHit(i);
					myShot->hit();
					break;
				}
			}
		}
		update = true;
	}

	// update other shots
	for (int i = 0; i < MAXPLAYERS; i++) {
		Shot* shot = m_->getShot(i);
		if (shot->getStatus() == SVALID) {
			shot->forward();
			update = true;
		}
	}

	if (update)
		v_->update();
}

bool GameController::playerIdle() {
	Player* p = m_->getMe();
	if (p->getLegAngle() != 0) {
		p->setLegAngle(0);
		reportPosition();
		return true;
	}
	return false;
}

void GameController::keyPressed(unsigned char e) {
	lastKey_ = e;
	
	Player* p = m_->getMe();

	switch (e) { // Look at the key code
		case 'j':
			p->rotateLeft();
			p->setStatus(PALIVE);
			p->changeLegAngle();
			reportPosition();
			break;
		case 'k':
			p->rotateRight();
			p->setStatus(PALIVE);
			p->changeLegAngle();
			reportPosition();
			break;
		case 'i':
			p->moveForward();
			p->setStatus(PALIVE);
			p->changeLegAngle();
			reportPosition();
			break;
		case 'm':
			p->moveBackward();
			p->setStatus(PALIVE);
			p->changeLegAngle();
			reportPosition();
			break;
		case 'y':
			p->slideLeft();
			p->setStatus(PALIVE);
			p->changeLegAngle();
			reportPosition();
			break;
		case 'x':
			p->slideRight();
			p->setStatus(PALIVE);
			p->changeLegAngle();
			reportPosition();
			break;
		case ' ':
		{
			Shot* s = m_->getMyShot();
			if (s->getStatus() != SVALID) {
				s->init(p->getX(), p->getZ(), p->getAngle(), p->getRoom(), SVALID);
				reportShot();
			}
			break;
		}
		default:
			return;
	}

	v_->update();
}

void GameController::keyReleased(unsigned char e) {
	lastKey_ = 0;
	
	switch (e) { // Look at the key code
		case 'j':
		case 'k':
		case 'i':
		case 'm':
		case 'y':
		case 'x':
			playerIdle();
			break;
		default:
			return;
	}

	v_->update();
}

bool GameController::keyRepeat() {
	if(lastKey_ != 0)
	{
		keyPressed(lastKey_);
		return true;
	}
	return false;
}
void GameController::reportHit(int i) {
	// send status of shot to network
	n_->sendShotPosition(0, 0, 0, false);
	m_->increaseHit(i);
	setTitle(i);
};

void GameController::reportShot() const {
	// send position of shot to network
	const Shot* myShot = m_->getMyShot();
	n_->sendShotPosition(
			myShot->getX(),
			myShot->getZ(),
			myShot->getAngle(),
			true
			);
};

void GameController::reportPosition() const {
	// send position of player to network
	const Player* me = m_->getMe();
	n_->sendPlayerPosition(me->getX(), me->getZ(), me->getAngle(), me->getLegAngle());
}

void GameController::handleShotPosition(int p, bool valid, double x, double y, double a) {
	// determine the room the shot is in
	const Room* room = 0;
	for (int i = 0; m_->getRoom(i); i++) {
		if (m_->getRoom(i)->checkPosition(x, y)) {
			room = m_->getRoom(i);
			break;
		}
	}

	// Shot::forward() dereferences the room, so a shot outside every room
	// would crash rather than fly on.
	if (!room) {
		return;
	}

	m_->getShot(p)->init(x, y, a, room, valid ? SVALID : SNOTVALID);
	v_->update();
}

void GameController::handlePlayerPosition(int p, double x, double y, double a, double la) {
	Player* pl = m_->getPlayer(p);
	pl->setX(x);
	pl->setZ(y);
	pl->setAngle(a);
	pl->setLegAngle(la);
	pl->setStatus(PALIVE);
	v_->update();
}

void GameController::handlePlayerHit(int p) {
	m_->increaseHit(p);
	setTitle(p);
}

void GameController::handlePlayerRemove(int p) {
	m_->getPlayer(p)->setStatus(PEMPTY);
	m_->getShot(p)->setStatus(SNOTVALID);
	v_->update();
}

extern void refreshScore(int);

void GameController::setTitle(int p = -1) {
	// update display
	std::ostringstream ss;
	ss << "Score: P0: " << m_->getHits(0) << 
				" P1: " << m_->getHits(1) << 
				" P2: " << m_->getHits(2);
	if (p != -1) {
		ss << " P" << p << " was hit!!!";
		glutTimerFunc(1000, refreshScore, -1);
	}
	v_->setScoreString(ss.str());
	v_->paintScoreGL();
	
}
