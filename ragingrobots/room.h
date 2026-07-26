/***************************************************************************
                          room.h  -  description
                             -------------------
    begin                : Mon Jun 26 2000
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

#ifndef ROOM_H
#define ROOM_H

#include <math.h>

enum direction { NORTH, SOUTH, WEST, EAST };

class Room {

public:
	Room();
	~Room();
	
    void setNeighbor(const Room* r, int d);
    const Room* getNeighbor(int d) const;
    void setCenter(int x, int z);
    int getX() const;
    int getZ() const;
    // checks, if given coords are in this room
    bool checkPosition(double x, double z) const;

private:
    const Room* neighbors_[4];
    int x_;
    int z_;
    bool hasNeighbor_[4];
};

inline void Room::setNeighbor(const Room* r, int d) {
    neighbors_[d] = r;
    hasNeighbor_[d] = true;
}

inline const Room* Room::getNeighbor(int d) const {
    if (hasNeighbor_[d]) {
        return neighbors_[d];
    } else {
        return 0;
    }
}

inline void Room::setCenter(int x, int z) {
    x_ = x;
    z_ = z;
}

inline int Room::getX()const {
    return x_;
}

inline int Room::getZ() const {
    return z_;
}

inline bool Room::checkPosition(double x, double z) const {
    if (fabs(x_ - x) < 4.0 && fabs(z_ - z) < 4.0) {
        return true;
    } else {
        return false;
    }
}

#endif
