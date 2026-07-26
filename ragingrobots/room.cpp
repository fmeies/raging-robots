/***************************************************************************
                          room.cpp  -  description
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

#include "room.h"

Room::Room() {
    for (int i = 0; i < 4; i++) {
        hasNeighbor_[i] = false;
    }
}

Room::~Room(){
}

