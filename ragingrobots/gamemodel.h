/***************************************************************************
                          gamemodel.h  -  description
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

#ifndef GameModel_H
#define GameModel_H

#define MAXPLAYERS 3

class Player;
class Shot;
class Room;

class GameModel {

public:
  	GameModel();
  	~GameModel();

    Player* getPlayer(int noPlayer) const;
    Player* getMe() const;

    Shot* getShot(int playerNo) const;
    Shot* getMyShot() const;

    Room* getRoom(int i) const;
	int getNumberOfRooms() const;
	
	void increaseHit(int i);
	int getHits(int i) const;

private:
    int noRooms_;
    Player* player_[MAXPLAYERS];
    Player* me_;
    Shot* shot_[MAXPLAYERS];
    Shot* myShot_;
    int hit_[MAXPLAYERS];
    Room** rooms_;
};

inline Player* GameModel::getPlayer(int playerNo) const { return player_[playerNo]; };
inline Player* GameModel::getMe() const { return me_; };
inline Shot* GameModel::getShot(int playerNo) const { return shot_[playerNo]; };
inline Shot* GameModel::getMyShot() const { return myShot_; };
inline Room* GameModel::getRoom(int i) const {
    if (i < noRooms_) {
        return rooms_[i];
    } else {
        return 0;
    }
}
inline void GameModel::increaseHit(int i) { hit_[i]++; };
inline int GameModel::getHits(int i) const { return hit_[i]; };
inline int GameModel::getNumberOfRooms() const { return noRooms_; };

#endif // GameModel_H
