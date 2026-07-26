/***************************************************************************
                          gamemodel.cpp  -  description
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

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

// application specific includes
#include "gamemodel.h"
#include "shot.h"
#include "room.h"
#include "player.h"

// main widget
GameModel::GameModel()
{
   myShot_ = new Shot;

   // DATA_DIR_PREFIX is set at compile time and can be arbitrarily long, so the
   // path is built on the heap rather than in a fixed-size buffer.
   const std::string path = std::string(DATA_DIR_PREFIX) + "/share/ragingrobots/rooms.data";
   std::ifstream in(path.c_str());
   if (! in) {
       std::cout<<"Couldn't read data from file"<<std::endl;
        exit(1);
   }
   in>>noRooms_;

   rooms_ = new Room*[noRooms_];
   for (int i = 0; i < noRooms_; i++) {
        rooms_[i] = new Room;
   }

   // set up scene from file
   for (int i = 0; i < noRooms_; i++) {
        int x;
        int z;
        int noNeighbors;
        in>>x>>z>>noNeighbors;
        rooms_[i]->setCenter(x, z);
        for (int j = 0; j < noNeighbors; j++) {
            int neighbor;
            int dir;
            in>>neighbor>>dir;
            rooms_[i]->setNeighbor(rooms_[neighbor], dir);
        }
   }
   in.close();

   for (int i=0; i < MAXPLAYERS; i++) {
        player_[i] = new Player(this, rooms_[0]);
        shot_[i] = new Shot;
        hit_[i] = 0;
   }

   me_ = new Player(this, rooms_[0]);
}

GameModel::~GameModel() {
    for (int i = 0; i < MAXPLAYERS; i++) {
        delete player_[i];
        delete shot_[i];
    }

    for (int i = 0; i < noRooms_; i++) {
        delete rooms_[i];
    }

    delete[] rooms_;
    delete me_;
    delete myShot_;
}
