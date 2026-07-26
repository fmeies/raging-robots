/***************************************************************************
                          player.h  -  description
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

#ifndef PLAYER_H
#define PLAYER_H

class Room;
class GameModel;

enum player_status { PEMPTY = 1, PALIVE = 2 };

class Player {
public: 
	Player(const GameModel* model, const Room* room);
	virtual ~Player();

	virtual void rotateLeft();
	virtual void rotateRight();

	virtual void moveForward();
	virtual void moveBackward();	

	virtual void slideLeft();
	virtual void slideRight();	

    player_status getStatus() const ;
    void setStatus(player_status status);
	
	double getX() const;
	double getDirX() const;
	void setX(double x);
	
	double getZ() const;
	double getDirZ() const;
	void setZ(double z);
	
	double getAngle() const;
    void setAngle(double angle);
	
    double getLegAngle() const;
    void setLegAngle(double angle);
    void changeLegAngle();

    void setRoom(const Room* room);
    const Room* getRoom() const;

private:
    const GameModel* model_;
    const Room* room_;
    player_status status_;
    double posx_;
    double posz_;
    double angle_;
    double dirx_;
    double dirz_;
    double legangle_;
    int legangleinc_;
};

inline player_status Player::getStatus() const { return status_; };
inline void Player::setStatus(player_status status) { status_ = status; };

inline double Player::getX() const { return posx_; };
inline double Player::getZ() const { return posz_; };

inline double Player::getDirX() const { return dirx_; };
inline double Player::getDirZ() const { return dirz_; };

inline void Player::setX(double x) { posx_ = x; };
inline void Player::setZ(double z) { posz_ = z; };

inline double Player::getAngle() const { return angle_; };
inline void Player::setAngle(double angle) { angle_ = angle; };

inline double Player::getLegAngle() const { return legangle_; };
inline void Player::setLegAngle(double angle) { legangle_ = angle; };

inline void Player::setRoom(const Room* room) { room_ = room; };
inline const Room* Player::getRoom() const { return room_; };

#endif
