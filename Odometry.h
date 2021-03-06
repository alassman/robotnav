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

#ifndef ODOMETRY_H
#define ODOMETRY_H

#include "Robot.h"
#include "MathFunctions.h"

class Odometry
{
	Robot *mpSensors;
	protected:
		float mDisplacement;
		float mRotation;
		float mPeriod;
		virtual void getDisplacements();
	public:
		float mX;
		float mY;
		float mHeading;
		float mSpeed;
		float mRate;
		Odometry(Robot *pSensors);
		virtual void updatePosition();
		inline float getPeriod(){return mPeriod;};
		void reset();
};

#endif
