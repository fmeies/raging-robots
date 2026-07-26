/***************************************************************************
                          shot.cpp  -  description
                             -------------------
    begin                : Tue Mar 21 2000
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
#include "shot.h"
#include "room.h"

Shot::Shot() : status_(SNOTVALID), room_(0), posx_(0), posz_(0), angle_(0),
               dirx_(0), dirz_(0) {
}

Shot::~Shot(){
}

void Shot::init(double posx, double posz, double angle, const Room* room, shot_status status) {
    posx_ = posx;
    posz_ = posz;
    angle_ = angle;
    room_ = room;
    dirx_ = -sin(angle);
    dirz_ = -cos(angle);
    status_ = status;
}

void Shot::forward() {
    double tmpx = posx_ + 0.5 * dirx_;
    double tmpz = posz_ + 0.5 * dirz_;

    int x = room_->getX();
    int z = room_->getZ();

    bool coll = false;

    // check collision with walls
    if (fabs(z - tmpz) > 4.0) {
        // possible collision with front or back wall
        if (tmpz - z < 0) {
            // possible collision with front wall
            if (room_->getNeighbor(NORTH) && fabs(posx_ - x) < 0.8) {
                // now I'm in the northern neighbor room
                room_ = room_->getNeighbor(NORTH);
            } else {
                // collision with front wall
                coll = true;
            }
        } else {
            // possible collision with back wall
            if (room_->getNeighbor(SOUTH) && fabs(posx_ - x) < 0.8) {
                // now I'm in the southern neighbor room
                room_ = room_->getNeighbor(SOUTH);
            } else {
                // collision with back wall
                coll = true;
            }
        }
    }

    if (fabs(x - tmpx) > 3.6) {
        // possible collision with front or back wall
        if (tmpx - x < 0) {
            // possible collision with left wall
            if (room_->getNeighbor(WEST) && fabs(posz_ - z) < 0.8) {
                // now I'm in the western neighbor room
                room_ = room_->getNeighbor(WEST);
            } else {
                // collision with left
                coll = true;
            }
        } else {
            // possible collision with right wall
            if (room_->getNeighbor(EAST) && fabs(posz_ - z) < 0.8) {
                posx_ = tmpx;
                // now I'm in the eastern neighbor room
                room_ = room_->getNeighbor(EAST);
            } else {
                // collision with right wall
                coll = true;
            }
        }
    }

    if (! coll) {
        posx_ = tmpx;
        posz_ = tmpz;
    } else {
        status_ = SNOTVALID;
    }
}

void Shot::setAngle(double angle) {
    angle_ = angle;
    dirx_ = -sin(angle);
    dirz_ = -cos(angle);
};

