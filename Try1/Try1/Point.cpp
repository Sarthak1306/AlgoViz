//
//  Point.cpp
//  Raster Project
//
//  Created by Dennis Bounded on 3/5/16.
//  Copyright © 2016 Dennis Bounded. All rights reserved.
//

#include "Point.h"



/* Constructors */
Point::Point()
{
    this->x = 0;
    this->y =0;
}
Point::Point(int x, int y)
{
    this->x = x;
    this->y = y;
}

/* Mutators */
void Point:: set_x(int x)
{
    this->x = x;
}

void Point:: set_y(int y)
{
    this->y = y;
}

void Point:: set(int x, int y)
{
    this->x = x;
    this->y = y;
}

/* Accessors */
int Point:: get_x()
{
    return this->x;
}

int Point:: get_y()
{
    return this->y;
}
