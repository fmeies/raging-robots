/***************************************************************************
                          player.cpp  -  description
                             -------------------
    begin                : Sun Mar 19 2000
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

#include <math.h>
#include "player.h"
#include "gamemodel.h"
#include "room.h"

Player::Player(const GameModel* model, const Room* room)
     : model_(model), room_(room), status_(PEMPTY),
       posx_(0), posz_(0), angle_(0),
       legangle_(0), legangleinc_(15)
{
    dirx_ = -sin(angle_);
    dirz_ = -cos(angle_);
}

Player::~Player(){
}

void Player::rotateLeft() {
    angle_ += 0.1;
    dirx_ = -sin(angle_);
    dirz_ = -cos(angle_);
}
void Player::rotateRight() {
    angle_ -= 0.1;
    dirx_ = -sin(angle_);
    dirz_ = -cos(angle_);
}

void Player::moveForward() {
    double tmpx = posx_ + 0.1 * dirx_;
    double tmpz = posz_ + 0.1 * dirz_;

    // check collision with other players
    for (int i = 0; i < MAXPLAYERS; i++) {
        Player* p = model_->getPlayer(i);
        if ((p->getStatus() & PALIVE) &&
            // new position to close
            fabs(tmpx - p->getX()) < 0.4 &&
            fabs(tmpz - p->getZ()) < 0.4 &&
            // old position not inside
            (fabs(posx_ - p->getX()) > 0.4 ||
             fabs(posz_ - p->getZ()) > 0.4)) {
            return;
        }
    }

    int x = room_->getX();
    int z = room_->getZ();

    const Room* next;

    bool colx = false;
    bool colz = false;

    // check collision with walls
    if (fabs(tmpz - z) > 3.6) {
        // possible collision with front or back wall
        colz = true;
    }
    if (fabs(tmpx - x) > 3.6) {
        colx = true;
    }

    if (colx && colz) return;

    if (! colx && ! colz) {
        posx_ = tmpx;
        posz_ = tmpz;
        return;
    }

    if (colz) {
        if (tmpz - z < 0) {
            // collision with front wall
            if (room_->getNeighbor(NORTH) && fabs(tmpx - x) < 0.7) {
                posz_ = tmpz;
                posx_ = tmpx;
                // check, if I'm already in the next neighbor room
                next = room_->getNeighbor(NORTH);
                if (next->checkPosition(posx_, posz_)) {
                    room_ = next;
                }
            } else if (fabs(posz_ - z) <= 3.6) {
                posx_ = tmpx;
            }
        } else {
            // collision with back wall
            if (room_->getNeighbor(SOUTH) && fabs(tmpx - x) < 0.7) {
                posz_ = tmpz;
                posx_ = tmpx;
                // check, if I'm already in the next neighbor room
                next = room_->getNeighbor(SOUTH);
                if (next->checkPosition(posx_, posz_)) {
                    room_ = next;
                }
            } else if (fabs(posz_ - z) <= 3.6) {
                posx_ = tmpx;
            }
        }
    }

    if (colx) {
        if (tmpx - x < 0) {
            // collision with front wall
            if (room_->getNeighbor(WEST) && fabs(tmpz - z) < 0.7) {
                posz_ = tmpz;
                posx_ = tmpx;
                // check, if I'm already in the next neighbor room
                next = room_->getNeighbor(WEST);
                if (next->checkPosition(posx_, posz_)) {
                    room_ = next;
                }
            } else if (fabs(posx_ - x) <= 3.6) {
                posz_ = tmpz;
            }
        } else {
            // collision with back wall
            if (room_->getNeighbor(EAST) && fabs(tmpz - z) < 0.7) {
                posz_ = tmpz;
                posx_ = tmpx;
                // check, if I'm already in the next neighbor room
                next = room_->getNeighbor(EAST);
                if (next->checkPosition(posx_, posz_)) {
                    room_ = next;
                }
            } else if (fabs(posx_ - x) <= 3.6) {
                posz_ = tmpz;
            }
        }
    }
}

void Player::moveBackward() {
    double tmpx = posx_ - 0.1 * dirx_;
    double tmpz = posz_ - 0.1 * dirz_;

    // check collision with other players
    for (int i = 0; i < MAXPLAYERS; i++) {
        Player* p = model_->getPlayer(i);
        if ((p->getStatus() & PALIVE) &&
            // new position to close
            fabs(tmpx - p->getX()) < 0.4 &&
            fabs(tmpz - p->getZ()) < 0.4 &&
            // old position not inside
            (fabs(posx_ - p->getX()) > 0.4 ||
             fabs(posz_ - p->getZ()) > 0.4)) {
            return;
        }
    }

    int x = room_->getX();
    int z = room_->getZ();

    const Room* next;

    bool colx = false;
    bool colz = false;

    // check collision with walls
    if (fabs(tmpz - z) > 3.6) {
        // possible collision with front or back wall
        colz = true;
    }
    if (fabs(tmpx - x) > 3.6) {
        colx = true;
    }

    if (colx && colz) return;

    if (! colx && ! colz) {
        posx_ = tmpx;
        posz_ = tmpz;
        return;
    }

    if (colz) {
        if (tmpz - z < 0) {
            // collision with front wall
            if (room_->getNeighbor(NORTH) && fabs(tmpx - x) < 0.7) {
                posz_ = tmpz;
                posx_ = tmpx;
                // check, if I'm already in the next neighbor room
                next = room_->getNeighbor(NORTH);
                if (next->checkPosition(posx_, posz_)) {
                    room_ = next;
                }
            } else if (fabs(posz_ - z) <= 3.6) {
                posx_ = tmpx;
            }
        } else {
            // collision with back wall
            if (room_->getNeighbor(SOUTH) && fabs(tmpx - x) < 0.7) {
                posz_ = tmpz;
                posx_ = tmpx;
                // check, if I'm already in the next neighbor room
                next = room_->getNeighbor(SOUTH);
                if (next->checkPosition(posx_, posz_)) {
                    room_ = next;
                }
            } else if (fabs(posz_ - z) <= 3.6) {
                posx_ = tmpx;
            }
        }
    }

    if (colx) {
        if (tmpx - x < 0) {
            // collision with front wall
            if (room_->getNeighbor(WEST) && fabs(tmpz - z) < 0.7) {
                posz_ = tmpz;
                posx_ = tmpx;
                // check, if I'm already in the next neighbor room
                next = room_->getNeighbor(WEST);
                if (next->checkPosition(posx_, posz_)) {
                    room_ = next;
                }
            } else if (fabs(posx_ - x) <= 3.6) {
                posz_ = tmpz;
            }
        } else {
            // collision with back wall
            if (room_->getNeighbor(EAST) && fabs(tmpz - z) < 0.7) {
                posz_ = tmpz;
                posx_ = tmpx;
                // check, if I'm already in the next neighbor room
                next = room_->getNeighbor(EAST);
                if (next->checkPosition(posx_, posz_)) {
                    room_ = next;
                }
            } else if (fabs(posx_ - x) <= 3.6) {
                posz_ = tmpz;
            }
        }
    }
}

void Player::slideLeft() {
    double tmpangle = angle_ + M_PI/2.0;
    double tmpx = posx_ - 0.1 * sin(tmpangle);
    double tmpz = posz_ - 0.1 * cos(tmpangle);

    // check collision with other players
    for (int i = 0; i < MAXPLAYERS; i++) {
        Player* p = model_->getPlayer(i);
        if ((p->getStatus() & PALIVE) &&
            // new position to close
            fabs(tmpx - p->getX()) < 0.4 &&
            fabs(tmpz - p->getZ()) < 0.4 &&
            // old position not inside
            (fabs(posx_ - p->getX()) > 0.4 ||
             fabs(posz_ - p->getZ()) > 0.4)) {
            return;
        }
    }

    int x = room_->getX();
    int z = room_->getZ();

    const Room* next;

    bool colx = false;
    bool colz = false;

    // check collision with walls
    if (fabs(tmpz - z) > 3.6) {
        // possible collision with front or back wall
        colz = true;
    }
    if (fabs(tmpx - x) > 3.6) {
        colx = true;
    }

    if (colx && colz) return;

    if (! colx && ! colz) {
        posx_ = tmpx;
        posz_ = tmpz;
        return;
    }

    if (colz) {
        if (tmpz - z < 0) {
            // collision with front wall
            if (room_->getNeighbor(NORTH) && fabs(tmpx - x) < 0.7) {
                posz_ = tmpz;
                posx_ = tmpx;
                // check, if I'm already in the next neighbor room
                next = room_->getNeighbor(NORTH);
                if (next->checkPosition(posx_, posz_)) {
                    room_ = next;
                }
            } else if (fabs(posz_ - z) <= 3.6) {
                posx_ = tmpx;
            }
        } else {
            // collision with back wall
            if (room_->getNeighbor(SOUTH) && fabs(tmpx - x) < 0.7) {
                posz_ = tmpz;
                posx_ = tmpx;
                // check, if I'm already in the next neighbor room
                next = room_->getNeighbor(SOUTH);
                if (next->checkPosition(posx_, posz_)) {
                    room_ = next;
                }
            } else if (fabs(posz_ - z) <= 3.6) {
                posx_ = tmpx;
            }
        }
    }

    if (colx) {
        if (tmpx - x < 0) {
            // collision with front wall
            if (room_->getNeighbor(WEST) && fabs(tmpz - z) < 0.7) {
                posz_ = tmpz;
                posx_ = tmpx;
                // check, if I'm already in the next neighbor room
                next = room_->getNeighbor(WEST);
                if (next->checkPosition(posx_, posz_)) {
                    room_ = next;
                }
            } else if (fabs(posx_ - x) <= 3.6) {
                posz_ = tmpz;
            }
        } else {
            // collision with back wall
            if (room_->getNeighbor(EAST) && fabs(tmpz - z) < 0.7) {
                posz_ = tmpz;
                posx_ = tmpx;
                // check, if I'm already in the next neighbor room
                next = room_->getNeighbor(EAST);
                if (next->checkPosition(posx_, posz_)) {
                    room_ = next;
                }
            } else if (fabs(posx_ - x) <= 3.6) {
                posz_ = tmpz;
            }
        }
    }
}

void Player::slideRight() {
    double tmpangle = angle_ - M_PI/2.0;
    double tmpx = posx_ - 0.1 * sin(tmpangle);
    double tmpz = posz_ - 0.1 * cos(tmpangle);

    // check collision with other players
    for (int i = 0; i < MAXPLAYERS; i++) {
        Player* p = model_->getPlayer(i);
        if ((p->getStatus() & PALIVE) &&
            // new position to close
            fabs(tmpx - p->getX()) < 0.4 &&
            fabs(tmpz - p->getZ()) < 0.4 &&
            // old position not inside
            (fabs(posx_ - p->getX()) > 0.4 ||
             fabs(posz_ - p->getZ()) > 0.4)) {
            return;
        }
    }

    int x = room_->getX();
    int z = room_->getZ();

    const Room* next;

    bool colx = false;
    bool colz = false;

    // check collision with walls
    if (fabs(tmpz - z) > 3.6) {
        // possible collision with front or back wall
        colz = true;
    }
    if (fabs(tmpx - x) > 3.6) {
        colx = true;
    }

    if (colx && colz) return;

    if (! colx && ! colz) {
        posx_ = tmpx;
        posz_ = tmpz;
        return;
    }

    if (colz) {
        if (tmpz - z < 0) {
            // collision with front wall
            if (room_->getNeighbor(NORTH) && fabs(tmpx - x) < 0.7) {
                posz_ = tmpz;
                posx_ = tmpx;
                // check, if I'm already in the next neighbor room
                next = room_->getNeighbor(NORTH);
                if (next->checkPosition(posx_, posz_)) {
                    room_ = next;
                }
            } else if (fabs(posz_ - z) <= 3.6) {
                posx_ = tmpx;
            }
        } else {
            // collision with back wall
            if (room_->getNeighbor(SOUTH) && fabs(tmpx - x) < 0.7) {
                posz_ = tmpz;
                posx_ = tmpx;
                // check, if I'm already in the next neighbor room
                next = room_->getNeighbor(SOUTH);
                if (next->checkPosition(posx_, posz_)) {
                    room_ = next;
                }
            } else if (fabs(posz_ - z) <= 3.6) {
                posx_ = tmpx;
            }
        }
    }

    if (colx) {
        if (tmpx - x < 0) {
            // collision with front wall
            if (room_->getNeighbor(WEST) && fabs(tmpz - z) < 0.7) {
                posz_ = tmpz;
                posx_ = tmpx;
                // check, if I'm already in the next neighbor room
                next = room_->getNeighbor(WEST);
                if (next->checkPosition(posx_, posz_)) {
                    room_ = next;
                }
            } else if (fabs(posx_ - x) <= 3.6) {
                posz_ = tmpz;
            }
        } else {
            // collision with back wall
            if (room_->getNeighbor(EAST) && fabs(tmpz - z) < 0.7) {
                posz_ = tmpz;
                posx_ = tmpx;
                // check, if I'm already in the next neighbor room
                next = room_->getNeighbor(EAST);
                if (next->checkPosition(posx_, posz_)) {
                    room_ = next;
                }
            } else if (fabs(posx_ - x) <= 3.6) {
                posz_ = tmpz;
            }
        }
    }
}

void Player::changeLegAngle() {
    if (legangle_ >= 45.0) {
        legangleinc_ = -15;
    } else if (legangle_ <= -45.0) {
        legangleinc_ = 15;
    }
    legangle_ += legangleinc_;
}
