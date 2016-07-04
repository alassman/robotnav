/*
 * Robot Navigation Program
 * www.robotnav.com
 *
 * (C) Copyright 2010 - 2014 Lauro Ojeda
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef APRILTAGS_H
#define APRILTAGS_H

#include "Odometry.h"
#include "Robot.h"
#include "TCPServer.h"
#include "MathFunctions.h"

class Apriltags : public Odometry
{
	Robot *mpSensors;
	TCPServer aptagServer(char* server, int port);
	protected:
		int aID;
		int ax;
		int ay;
		int aroll;
	public:
		Apriltags(Robot *pSensors, char* server, int port);
		virtual void updatePosition();
};

#endif
