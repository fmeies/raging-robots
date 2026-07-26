/***************************************************************************
                          shot.h  -  description
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

#ifndef SHOT_H
#define SHOT_H

class Room;

enum shot_status { SVALID, SNOTVALID };

class Shot {
public: 
	Shot();
	virtual ~Shot();
	
	virtual void forward();
	
	shot_status getStatus() const;
    void setStatus(shot_status status);

	void hit();
	void init(double posx, double posz, double angle, const Room* room, shot_status status);
	
	double getX() const;
	void setX(double x);
	
	double getZ() const;
	void setZ(double z);
	
	double getAngle() const;
	void setAngle(double angle);

    void setRoom(const Room* room);
    const Room* getRoom();

private:
    shot_status status_;
    const Room* room_;
    double posx_;
    double posz_;
    double angle_;
    double dirx_;
    double dirz_;
};

inline shot_status Shot::getStatus() const { return status_; };
inline void Shot::setStatus(shot_status status) { status_ = status; };

inline void Shot::hit() { status_ = SNOTVALID; };

inline double Shot::getX() const { return posx_; };
inline void Shot::setX(double x) { posx_ = x; };

inline double Shot::getZ() const { return posz_; };
inline void Shot::setZ(double z) { posz_ = z; };

inline double Shot::getAngle() const { return angle_; };

inline void Shot::setRoom(const Room* room) { room_ = room; };
inline const Room* Shot::getRoom() { return room_; };

#endif
