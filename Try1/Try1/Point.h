//
//  Point.h
//  Raster Project
//
//  Created by Dennis Bounded on 3/5/16.
//  Copyright © 2016 Dennis Bounded. All rights reserved.
//

#ifndef Point_h
#define Point_h

class Point
{
   
private:
     /* Date Members */
    int x, y;
    
public:
    /* Constructor */
    Point();
    Point(int x, int y);
    
    /* Mutators */
    void set_x(int x);
    void set_y(int y);
    void set(int x, int y);
    
    /* Acessors */
    int get_x();
    int get_y();
    
};


#endif /* Point_h */
