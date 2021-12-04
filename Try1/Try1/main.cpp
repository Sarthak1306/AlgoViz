#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <GLUT/GLUT.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "Point.h"
#ifdef __APPLE__
#else
#endif

#define MAX 10
using namespace std;


int n,i=1,a[25],b[25],cost[25][25],tree[25][25],src,l[2],dist[10];
char s[20],*s1;
void *currentfont;


//BELLMAN VARIABLES
typedef struct {
   int u, v, w;
} Edge;

const int NODES = 5 ; /* the number of nodes */
int EDGES=0;            /* the number of edges */
Edge edges[32];       /* large enough for n <= 2^NODES=32 */
int d[32];            /* d[i] is the minimum distance from source node s to node i */


//prototype of dijkstra
void dijkstra();

//Window Size
GLsizei wh = 1020, ww = 1080;
 
//Used in drawSquare, square length
GLfloat size = 3.0;

float PI = 3.14285714286;

//Radius of circle
int radius = 30;

//No. of nodes in window
int nodeCount = 0;

//Max No. of nodes that can be created
int maxNodes = 26;

//2X2 matrix between the nodes
int adjMatrix[26][26];

//No. of nodes selected to draw a line
int lineNodesCount = 0;

//Source node created
int sourceNodeCreated = 0;

//Destination node created
int destinationNodeCreated = 0;

//Dijkstra begin
int dijkstraRun = 0;

//instruction display
int instruction = 1;


//Create node struct
struct node
{
    int id;
    int xCoordinate;
    int yCoordinate;
}nodes[26], lineNodes[2], sourceNode, destinationNode;


//return distance between two nodes
int computeDistance(struct node a, struct node b)
{
    return (int) sqrt(pow(abs(a.xCoordinate - b.xCoordinate), 2) + pow(abs(a.yCoordinate - b.yCoordinate), 2));
}

//Check if two nodes intersect i.e if to create a new node or select a node
int noRepeat(int x, int y)
{
    //if no nodes on screen, dont bother
    if(!nodeCount)
        return 1;

    //create a temp node for comparison on the clicked coordinate
    struct node temp = {nodeCount, x, y};

    for(int i = 0; i < nodeCount; i++)
    {
        //if the nodes intersect, assume a node is clicked
        if(computeDistance(temp, nodes[i]) < 2*radius)
        {
            //push the clicked node to the stack
            lineNodes[lineNodesCount] = nodes[i];
            return 0;
        }
    }
    //no node was pressed, create a new node at x,y
    return 1;
}

//get perpendicular coordinates to display the text
void getPerpendicularCoordinates(int *x, int *y)
{
    int x1, x2, y1, y2;

    x1 = lineNodes[0].xCoordinate;
    x2 = lineNodes[1].xCoordinate;
    y1 = lineNodes[0].yCoordinate;
    y2 = lineNodes[1].yCoordinate;

    float diff = (x2 - x1)? 0.0001 : x2 - x1;

    float slope = ((float)(y2 - y1)) / (diff);

    float perpendicularSlope = -1*(1 / slope);


    *x = *x + 25;
    *y = *y + perpendicularSlope * (*x) + 7;

}

//write Distance between the nodes to screen
void writeDistance(char *text, int x, int y)
{
    if(instruction)
        glColor3f(1.0, 1.0, 1.0);
    else
        glColor3f(1.0, 1.0, 0.0);
    glRasterPos2i(x,y);
    int len = strlen(text);
    for(int i = 0; i < len; i++)
    {
        glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18 , text[i]);
    }
    glFlush();
}



//write Label to nodes to screen
void writeLabel(char text, int x, int y)
{
    //change the color to yellow
    glColor3f(1.0, 1.0, 0.0);
    glRasterPos2i(x,y);
    // int len = strlen(text);
    // for(int i = 0; i < len; i++)
    // {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24 , text);
    // }
}

//draw Circle at the given coordinates
void drawCircle(float cx, float cy, float r, float num_segments, char colorCharacter)
{
    //theta- angle in radians of the point
    //x- x coordinate of point
    //y- y coordinate of point
    float theta,x,y;

    //Give the center of the circle a label [A-Z]
    if(colorCharacter == 'N')
        writeLabel((char)65 + nodeCount ,cx - 5, cy - 14);

    //change the color to green if colorCharacter = 'N'
    if(colorCharacter == 'N')
        glColor3f(0.0, 1.0, 0.0);

    if(colorCharacter == 'B')
        glColor3f(0.0, 0.0, 1.0);

    if(colorCharacter == 'R')
        glColor3f(1.0, 0.0, 0.0);

    //line loop- connect all the points below
    glBegin(GL_LINE_LOOP);
 
    for(int i = 0; i < num_segments; i++){
        //compute new angle and get the x, y coordinate
        theta = 2.0f * PI * i / num_segments;
         x = r * cosf(theta);
        y = r * sinf(theta);
         
         //draw the point
        glVertex2f(x + cx, y + cy);
    }
    glEnd();

    //flush to output screen
    glFlush();

}

//draw square
void drawSquare(int x, int y)
{
    // y = wh-y;
    // printf("x=%d y=%d \n",x, y );

    glBegin(GL_POLYGON);
        glVertex2f(x+size, y+size);
        glVertex2f(x-size, y+size);
        glVertex2f(x-size, y-size);
        glVertex2f(x+size, y-size);
    glEnd();
    glFlush();
}

//sets the mid point coordinates to the arguments
void getMidPoint(int *midX, int *midY)
{
    *midX = (lineNodes[0].xCoordinate + lineNodes[1].xCoordinate)/2;
    *midY = (lineNodes[0].yCoordinate + lineNodes[1].yCoordinate)/2;
}

//draw lines between two  nodes
void drawLine(char color)
{
    //set color to white
    if(color == 'N' || color == 'R')
        glColor3f(0.3, 0.3, 0.3);

    if(color == 'D')
        glColor3f(0.8, 0.8, 0.8);

    if(color == 'M')
        glColor3f(0.5, 0.0, 0.0);

    //draw lines between selected nodes (stored in lineNodes)
    glBegin(GL_LINES);
        glVertex2f(lineNodes[0].xCoordinate, lineNodes[0].yCoordinate);
        glVertex2f(lineNodes[1].xCoordinate, lineNodes[1].yCoordinate);
    glEnd();
    glFlush();

    //Mid Point of the line segment to display distance
    int midX, midY;
    char distanceString[10];
    //get the mid coordinates
    getMidPoint(&midX, &midY);


    //get nodes id, keys for the adjMatrix
    int id1 = lineNodes[0].id;
    int id2 = lineNodes[1].id;
    //compute distance between the nodes.
    int distance = computeDistance(lineNodes[0], lineNodes[1]);
    //store it in adjMatrix
    adjMatrix[id1][id2] = distance;
    adjMatrix[id2][id1] = distance;

    //Convert distance to string
    sprintf(distanceString, "%d",distance);
    //Display to the window
    getPerpendicularCoordinates(&midX, &midY);
    writeDistance(distanceString, midX, midY);

    glFlush();

    //write both circles' labels again
    writeLabel('A' + lineNodes[0].id, lineNodes[0].xCoordinate - 5, lineNodes[0].yCoordinate - 14);
    writeLabel('A' + lineNodes[1].id, lineNodes[1].xCoordinate - 5, lineNodes[1].yCoordinate - 14);

    glFlush();

    //Display the updated matrix to the console
    if(color == 'N')
    {
        printf("\n");
        for(int i = 0; i < nodeCount; i++)
        {
            for(int j = 0; j < nodeCount; j++)
            {
                printf("%d \t", adjMatrix[i][j]);
            }
            printf("\n");
        }
    }
}


//mouse click handler
void myMouse(int btn, int state, int x, int y)
{
    //Clear Instructions first
    if(instruction)
    {
        instruction = 0;
        glClear(GL_COLOR_BUFFER_BIT);
    }

    //dont listen to mouse events if dijkstra operation is running
    if(dijkstraRun)
    {
        return;
    }

    //fix coordinate system
    y = wh-y;

    //left click: create nodes, join nodes
    if(btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
    {
        // printf("x = %d, y = %d\n",x, y);

        //if not clicked on the circle
        if (noRepeat(x, y))
        {
            //dont draw if nodeCount > 26;
            if(nodeCount >= 26)
            {
                return;
            }

            //draw new circle
            drawCircle(x, y, radius, 200, 'N');
            lineNodesCount = 0;
            //set the values of the new node
            nodes[nodeCount].xCoordinate = x;
            nodes[nodeCount].yCoordinate = y;
            nodes[nodeCount].id = nodeCount;

            //increase the node count
            nodeCount++;
        }
        //draw line
        else
        {
            if(lineNodesCount == 1)
            {
                drawLine('N');
                lineNodesCount = 0;
            }
            else
            {
                lineNodesCount += 1;
            }
        }
    }

    //right click: select source and destination nodes
    if(btn==GLUT_RIGHT_BUTTON && state==GLUT_DOWN && (!sourceNodeCreated || !destinationNodeCreated))
    {
        //if nodes are created
        if(nodeCount)
        {
            //if a node is clicked, lineNodes[0] will have the node clicked
            if(!noRepeat(x, y))
            {
                //if source node is not assigned
                if(!sourceNodeCreated)
                {
                    sourceNode = lineNodes[0];
                    //source node created
                    sourceNodeCreated = 1;
                    //change color of the source node to (R)ed
                    drawCircle(sourceNode.xCoordinate, sourceNode.yCoordinate, radius, 200, 'R');
                }
                else
                {
                    //first, check if source is not selected again
                    if(((sourceNode.xCoordinate - lineNodes[0].xCoordinate) != 0) || ((sourceNode.yCoordinate - lineNodes[0].yCoordinate) != 0 ))
                    {
                        destinationNode = lineNodes[0];
                        //destination node created
                        destinationNodeCreated = 1;
                        //change color of the destination node to (B)lue
                        drawCircle(destinationNode.xCoordinate, destinationNode.yCoordinate, radius, 200, 'B');
                    }
                }

            }
        }
    }

}

//myReshape to Handle change in window size
void myReshape(GLsizei w, GLsizei h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, w, h);
    ww = w;
    wh = h;
}

//window initilization
void myInit()
{
    glViewport(0,0,ww,wh);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, ww, 0, wh);
    glMatrixMode(GL_MODELVIEW);
    glClearColor(0.0, 0.0, 0.0, 1);
    glColor3f(1.0, 1.0, 1.0);
    // glPointSize(0);
     glLineWidth(5);
}

//Function to display instructions
void display_hello()
{
    writeDistance(" Welcome! To The Simulation of Dijkstra's Algorithn.", 200, 900);
    writeDistance("#  Use Mouse Left Button to Create Nodes.", 250, 800);
    writeDistance("#  Select any two Nodes to form a Path.", 250, 770);
    writeDistance("#  Use Mouse Right Button to select Source and Destrination.", 250, 740);
    writeDistance("#  Press ENTER key to begin Dijkstra's Algorithm.", 250, 710);
}

//display function
void myDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        display_hello();

        glFlush();

}

//fix adjacency matrix: set the distance among the nodes to highest value (99999)
void fixAdjMatrix()
{
    for(int i = 0; i < maxNodes; i++)
    {
        for(int j = 0; j < maxNodes; j++)
        {
            adjMatrix[i][j] = 99999;
        }
    }
}

//keyboard event listener: when enter key is pressed begin dijkstra
void beginDijkstra(unsigned char key, int x, int y)
{
    printf("%d\n",key);

    //if source or destination nodes are not selected dont start
    if(!sourceNodeCreated || !destinationNodeCreated)
    {
        printf("Source node / destination node not selected\n");
        return;
    }
    //begin dijkstra if enter key is pressed
    if((int)key == 13)
    {
        dijkstraRun = 1;
        dijkstra();
    }
}



class Column{
public:
    float width;
    float length;
    float x;
    float y;
    float out;
    sf::Color color = sf::Color::White;
    sf::RectangleShape Draw() {
        sf::RectangleShape col(sf::Vector2f(width, length));
        col.setPosition(sf::Vector2f(x, y));
        col.setOutlineColor(sf::Color::Green);
        col.setFillColor(color);
        return col;
    }
};


//FUNCTION TO SELECT BITMAP FONT
void setFont(void *font)
{
    currentfont=font;
}
//FUNCTION TO DRAW BITMAP string at (x,y)
void drawstring(GLfloat x,GLfloat y,char *string)
{
  char *c;
  glRasterPos2f(x,y);

  for(c=string;*c!='\0';*c++)
  {
                             glutBitmapCharacter(currentfont,*c);
  }
}

//FUNCTION TO DELAY
void delay()
{
    for(int i=0;i<22000;i++)
       for(int j=0;j<22000;j++);
}
//DISPLAY FUNCTION FOR TITLE PAGE
void title()
{
    glLineWidth(3.0);
    glColor3f(1.0,1.0,1.0);
    glBegin(GL_LINE_LOOP);
                          glVertex2f(10,10);
                          glVertex2f(10,490);
                          glVertex2f(490,490);
                          glVertex2f(490,10);
    glEnd();

    setFont(GLUT_BITMAP_HELVETICA_18);
    glColor3f(1.0,1.0,1.0);
    drawstring(100,440,"Bellman Ford Algorithm");
    glColor3f(1.0,1.0,1.0);
    drawstring(100,400,"Submitted by");
    glColor3f(1.0,0.0,0.0);
    drawstring(100,360,"Sarthak & Pranit & Khushboo & !Priyanshu");
    glColor3f(1.0,0.0,0.0);
    drawstring(100,320,"Group 7");
    glColor3f(1.0,0.0,0.0);
    drawstring(100,280,"B.Tech CSE DevOps");
    glColor3f(1.0,1.0,1.0);
    drawstring(100,100,"Right click in My Window for options");
glFlush();
}
 
//DISPLAY FUNCTION FOR INITIALIZING (DRAWING) THE  INPUT AND OUTPUT AREAS
void initial()
{
    glClear(GL_COLOR_BUFFER_BIT);

    setFont(GLUT_BITMAP_HELVETICA_18);

    glColor3f(0.0,0.0,0.0);
    drawstring(20,230,"Input Area");
    drawstring(20,470,"Output Area");

    glColor3f(0.0,0.0,0.0);
    glLineWidth(3.0);
    glBegin(GL_LINES);
                          glVertex2f(10,10);
                          glVertex2f(10,490);

                          glVertex2f(10,490);
                          glVertex2f(490,490);

                          glVertex2f(490,490);
                          glVertex2f(490,10);

                          glVertex2f(490,10);
                          glVertex2f(10,10);

                          glVertex2f(10,250);
                          glVertex2f(490,250);
    glEnd();

    glFlush();
}

//BLANK DISPLAY FUNCTION
void display (void)
{

    glFlush();

}

//DRAW A BITMAP NUMBER i at (x,y)
void raster(int x,int y,int i)
{
    char z=i+'0';
    glRasterPos2f(x-5,y-5);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,z);
}

void reverse(char str[], int length)
{
    int start = 0;
    int end = length -1;
    while (start < end)
    {
        swap(*(str+start), *(str+end));
        start++;
        end--;
    }
}

// Implementation of itoa()
char* itoa(int num, char* str, int base)
{
    int i = 0;
    bool isNegative = false;

    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // In standard itoa(), negative numbers are handled only with
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = true;
        num = -num;
    }

    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, i);

    return str;
}

//DRAW THE NODES (SQUARES)
void drawSquare1(int x, int y)
{

    if(i<=n)
    {
            y = 500-y;                              //Convert from screen coordinates
            glPointSize(40);

            if(i==src)
                      glColor3f(0.7f, 0.4f, 0.0f);
            else
                      glColor3f(0.5f, 0.5f, 0.8f);

            glBegin(GL_POINTS);
                               glVertex2f(x , y);
            glEnd();

            a[i]=x;
            b[i]=y;

            glColor3f(0.0f, 1.0f, 0.0f);
            s1=itoa(i,s,10);
            drawstring(x-5,y-5,s1);

            glFlush();
   }
   i=i+1;
}

//READ DATA: |V|,COST MATRIX, SOURCE VERTEX
void read()
{
    printf("Enter the number of vertices\n");
    scanf("%d",&n);
    printf("Enter the cost matrix\n");
    for(int j=1;j<=n;j++)
            for(int k=1;k<=n;k++)
            {
                    scanf("%d",&cost[j][k]);
                    if(cost[j][k]==0 || cost[j][k]==999)
                                     cost[j][k]=999;
                    else
                    {
                        edges[EDGES].u=j;
                        edges[EDGES].v=k;
                        edges[EDGES].w=cost[j][k];
                        EDGES++;
                    }
            }
     printf("\nGO TO MY WINDOW, PLACE THE NODES IN INPUT AREA AND THEN CLICK RIGHT BUTTON FOR NEXT OPTION\n");
     initial(); //Draw the initial screen
}

//DRAW THE EDGES
void drawline()
{
  int j,k,x1,x2,y1,y2;
  for(j=1;j<=n;j++)
  {
   for(k=1;k<=n;k++)
   {
    if(cost[j][k]!=999 && j<k)
    {
     x1=a[j];
     y1=b[j];
     x2=a[k];
     y2=b[k];


     glColor3f(0.0,0.5,0.0);

     glLineWidth(3);
     glBegin(GL_LINES);
                      glVertex2i(x1-7,y1+10);
                      glVertex2i(x2-7,y2+10);
     glEnd();

     s1=itoa(cost[j][k],s,10);
     drawstring((x1+x2-16)/2,(y1+y2+22)/2,s1);
     glFlush();
    }

    if(cost[j][k]!=cost[k][j] && cost[j][k]!=999 && j>k)
    {
     x1=a[j];
     y1=b[j];
     x2=a[k];
     y2=b[k];

     glColor3f(1.0,0.5,0.0);
     glBegin(GL_LINES);
                       glVertex2i(x1+10,y1+18);
                       glVertex2i(x2+10,y2+18);
     glEnd();

     s1=itoa(cost[j][k],s,10);
     drawstring((x1+x2+20)/2,(y1+y2+36)/2,s1);
     glFlush();
    }
   }
  }
}


void shortestpath(int src)
{

    //START OF BELLMAN FORD
    int j,p,q,x1,y1,x2,y2,x,y;
    int d[MAX],parent[MAX];

   int it,flag=0,child[MAX];

   //INITIALIZE DATA OBJECTS
   for (it = 1; it <= n; ++it)
   {
       d[it] = INFINITY;
       parent[it]=src;

   }
   d[src] = 0;

   //RELAXATION METHOD
   for(int m=0;m<n;m++)//REPEAT N TIMES
   {
           //RELAX ALL EDGES
           for (it = 1; it <=n; ++it) {
               for (j = 1; j <=n; ++j) {

                                           if(d[it]+cost[it][j]<d[j])
                                           {

                                                                     d[j]=d[it]+cost[it][j];
                                                                     parent[j]=it;
                                           }
               }
           }
   }

   //CHECK FOR NEGATIVE LOOPS
for (it = 1; it <=n; ++it) {
       for (j = 1; j <=n; ++j) {
           if(cost[it][j]==INFINITY) continue;
           if(d[it]+cost[it][j]<d[j])
           {
                                    printf("\n\nGraph contains a negative-weight cycle\n");


       return;
           }
       }
   }
    printf("From source %d\n",src);
    for(i=1;i<=n;i++)
    if(i!=src)
    printf("The shortest distance to %d is %d\n",i,d[i]);
    printf("\n");
   //INITIALIZE SPANNING TREE EDGES
   int l=0;
   for (int it = 1; it <= n; ++it) {
   if(parent[it]==it) continue;
tree[l][1]=parent[it];
tree[l++][2]=it;
   }

   //DRAW THE SPANNING TREE
   for(int r=1;r<=n;r++)
   {

             x=a[r];
             y=b[r];

             glPointSize(25);
             if(r==src)
                       glColor3f(0.7f, 0.4f, 0.0f);
             else
                       glColor3f(0.5f, 0.5f, 0.8f);

             glBegin(GL_POINTS);
                                glVertex2f(x,y+250);
             glEnd();

             glColor3f(0.0,1.0,0.0);

             s1=itoa(r,s,10);
             drawstring(x,y+250,s1);

             glFlush();

             }

             for(int x=0;x<l;x++)
             {
                     p=tree[x][1];
                     q=tree[x][2];
                     if(p==0||q==0) continue;

                     x1=a[p];
                     y1=b[p];
                     x2=a[q];
                     y2=b[q];

                     if(p<q)
                     {
                            glColor3f(0.0,0.5,0.0);
                            glBegin(GL_LINES);
                                              glVertex2i(x1,y1+250);
                                              glVertex2i(x2,y2+250);
                            glEnd();

                            s1=itoa(cost[p][q],s,10);
                            drawstring((x1+x2)/2,(y1+y2+500)/2,s1);
                     }

                     else
                     {
                            glColor3f(1.0,0.5,0.0);
                            glBegin(GL_LINES);
                                              glVertex2i(x1,y1+250);
                                              glVertex2i(x2,y2+250);
                            glEnd();

                            s1=itoa(cost[p][q],s,10);
                            drawstring((x1+x2)/2,(y1+y2+500)/2,s1);
                     }
                     }
             glFlush();

}


void mouse(int bin, int state , int x , int y)
{
    if(bin==GLUT_LEFT_BUTTON&&state==GLUT_DOWN)
                                               drawSquare1(x,y);

}

void top_menu(int option)
{
int x,y;
    switch(option)
    {
                  case 1:
                       read();
                       glutPostRedisplay();
                       break;
                  case 2:
                       drawline();
                       glutPostRedisplay();
                       break;
                  case 3:
                       for(int i=1; i<=n; i++)
{
glClear(GL_COLOR_BUFFER_BIT);
initial();
for(int r=1;r<=n;r++)
{
x=a[r];
y=b[r];
glPointSize(40);
if(r==src)
glColor3f(0.7f, 0.4f, 0.0f);
else
glColor3f(0.5f, 0.5f, 0.8f);
glBegin(GL_POINTS);
glVertex2f(x,y);
glEnd();
glColor3f(0.0,1.0,0.0);
s1=itoa(r,s,10);
drawstring(x-5,y-5,s1);
setFont(GLUT_BITMAP_HELVETICA_18);
glColor3f(0.0,0.0,0.0);
drawstring(130,470,"For source");
glFlush();
}
drawline();
s1=itoa(i,s,10);
setFont(GLUT_BITMAP_HELVETICA_18);
glColor3f(0.0,0.0,0.0);
drawstring(225,470,s1);
glutPostRedisplay();
shortestpath(i);
delay();
}
break;
                  case 4:
                       exit(0);
    }
}


void init (void)
{
    glClearColor (1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport( 0,0, 500, 500 );
    glMatrixMode( GL_PROJECTION );
    glOrtho( 0.0, 500.0, 0.0, 500.0, 1.0, -1.0 );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glFlush();
}
void myInit1()
{
  glClearColor(0.0,0.0,0.0,0.0);
  glColor3f(0.0f,0.0f,0.0f);
  glPointSize(5.0);
  gluOrtho2D(0.0,500.0,0.0,500.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  setFont(GLUT_BITMAP_HELVETICA_18);
}


void display1(void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  title();

}

//BRESSENHAM


static const int

PIXEL_SIZE = 10,

GAP_SIZE = 1,

WIN_SIZE = 500,

RDIM = WIN_SIZE / (PIXEL_SIZE + GAP_SIZE);

Point rasterBress[RDIM][RDIM];




/* Global Variables */

static bool firstPixelChosen = false;

static int

raster_x,

raster_y,

start_x,

start_y = 0;




// Turns pixel on

void setPixelOn(int x, int y)

{

    glColor3ub(255, 0, 0);

    glVertex2i(rasterBress[x][y].get_x(), rasterBress[x][y].get_y());

}




// Translates pixel coordinate to raster coordinate

// y is in screen coordinate

void pixelToRaster(int x, int y)

{

    raster_x = (x - (2 * GAP_SIZE)) / (PIXEL_SIZE + GAP_SIZE); // x - 2 * GAP_SIZE to "round down"

    raster_y = (WIN_SIZE - y) / (PIXEL_SIZE + GAP_SIZE);

}







// Draw a raster RDIM by RDIM size

void drawRaster()

{

    

    int i,j,k,l = 0;

    

    

    /* Clear Window */

    glClear(GL_COLOR_BUFFER_BIT);

    

    glPointSize(PIXEL_SIZE);  // Sets Pixel Size

    

    glColor3ub(255, 255, 255);

    

    glBegin(GL_POINTS);

    

        /* Draw Raster */

        for (i = PIXEL_SIZE - (2 * GAP_SIZE), k = 0; i < WIN_SIZE; i += (PIXEL_SIZE + GAP_SIZE), k++) {

            for (j = PIXEL_SIZE- (2 * GAP_SIZE), l = 0; j < WIN_SIZE; j += (PIXEL_SIZE + GAP_SIZE), l++){

                glVertex2i(i, j);

                // Save Pixel Coordinate as Raster Coordinate

                rasterBress[k][l].set(i,j);

            }

        }

    

    glEnd();

    

}




// Draws a line between two pixels using

// Bresenham's Algorithm

void lineBresenham(int x0, int y0, int x1, int y1)

{

    int dx, dy;

    int stepx, stepy;

    

    dx = x1 - x0;

    dy = y1 - y0;

    

    if (dy<0) {dy=-dy; stepy=-1;} else {stepy=1;}

    if (dx<0) {dx=-dx; stepx=-1;} else {stepx=1;}

    dy <<= 1; /* dy is now 2*dy */

    dx <<= 1; /* dx is now 2*dx */

    

    if ((0 <= x0) && (x0 < RDIM) && (0 <= y0) && (y0 < RDIM))

        setPixelOn(x0, y0);

    

    if (dx > dy) {

        int fraction = dy - (dx >> 1);

        while (x0 != x1) {

            x0 += stepx;

            if (fraction >= 0) {

                y0 += stepy;

                fraction -= dx;

            }

            fraction += dy;

            if ((0 <= x0) && (x0 < RDIM) && (0 <= y0) && (y0 < RDIM))

                setPixelOn(x0, y0);

            

        }

        

    } else {

        int fraction = dx - (dy >> 1);

        while (y0 != y1) {

            if (fraction >= 0) {

                x0 += stepx;

                fraction -= dy;

            }

            y0 += stepy;

            fraction += dx;

            if ((0 <= x0) && (x0 < RDIM) && (0 <= y0) && (y0 < RDIM))

                setPixelOn(x0, y0);

        }

    }

    

    

}







void displayBress()

{

    

    drawRaster();

    

    /* Draw lines using Bresenham's Algorithm */

    /*

    lineBresenham(6, 33,  3, 13);

    lineBresenham(9, 33, 12, 13);

    lineBresenham(4,  4, 23,  3);

    lineBresenham(10,  38, 33, 42);

    lineBresenham(28, 12, 14, 30);

    lineBresenham(8, 5, 21, 24);

    */

    

    std::cout << "PICK TWO PIXELS WITH MOUSE\n";

    

    /* Flush GL buffers */

    glFlush();

}




void mouseBress(int button, int state, int x, int y)

{

    // Set raster coordinate from corresponding pixel coordinate

    pixelToRaster(x,y);

    

    if (!firstPixelChosen && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)

    {

        std::cout << "START PIXEL: (" << raster_x << "," << raster_y << ")\n";

        glBegin(GL_POINTS);

            setPixelOn(raster_x,raster_y);

        glEnd();

        

        // Save first raster coordinate

        start_x = raster_x;

        start_y = raster_y;

        firstPixelChosen = true;

        

        glFlush();

       

    }

    

    else if(firstPixelChosen && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)

    {

        std::cout << "END PIXEL: (" << raster_x << "," << raster_y << ")\n";

        glBegin(GL_POINTS);

            lineBresenham(start_x, start_y, raster_x, raster_y);

        glEnd();

        firstPixelChosen = false;

        

        glFlush();

    }

    /* */

}




void initBress()

{

    /* Initializations */

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutInitWindowSize(WIN_SIZE, WIN_SIZE);

    glutInitWindowPosition(0, 0);

    glutCreateWindow("Raster Bresenham Line");

    glClearColor(0.0, 0.0, 0.0, 0.0);

    glColor3f(1.0, 1.0, 1.0);

    

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    gluOrtho2D(0.0, WIN_SIZE, 0.0, WIN_SIZE);

    

    glutDisplayFunc(displayBress);

    glutMouseFunc(mouseBress);




    

}

int main(int argc, char* argv[] ) {
    
    int opt;
  //  char choice;
   // do{
    cout<<"Press 1 for Insertion Sort"<<endl;
    cout<<"Press 2 for Djikstra"<<endl;
    cout<<"Press 3 for Wieghted Graph "<<endl;
    cout<<"Press 4 for Bress"<<endl;
    cin>>opt;
    
    
   
    switch(opt){
        case 1:
        {
            sf::RenderWindow window(sf::VideoMode(800, 600), "Insertion Sort Algorithm");
            int numbers[] = { 66, 83, 343, 111, 500, 182, 46, 370, 480, 527, 266, 167, 163, 551, 462, 101, 143, 549, 82, 100, 69, 482, 491, 388, 588, 424, 138, 564, 519, 504, 52, 97, 419, 171, 275, 48, 247, 23, 441, 98, 240, 420, 203, 582, 365, 415, 267, 301, 512, 210, 16, 278, 463, 139, 392, 581, 338, 74, 282, 211, 296, 518, 468, 54, 204, 363, 375, 33, 130, 99, 31, 59, 575, 37, 361, 39, 312, 434, 200, 106, 389, 221, 220, 255, 196, 68, 474, 292, 231, 513, 369, 283, 201, 445, 449, 194, 251, 294, 206, 274 };
//            cout<<"Enter the Array"<<endl;
//            for(int num = 0; num < 100; num++)
//                cin>>numbers[num];
            int len = std::end(numbers) - std::begin(numbers);
            int height = 1;
            float unit = 2000 / len;
            sf::Time time = sf::milliseconds(1000);
            bool done = false;
            window.clear(sf::Color::Black);
            sf::sleep(sf::seconds(5));
            
            for (int k = 0;k < len;k++) {
                Column col;
                col.width = unit - 1;
                col.length = -numbers[k] * height;
                col.x = k * unit;
                col.y = 600;
                window.draw(col.Draw());
                window.display();
            }
            while (window.isOpen()) {
                sf::Event event;
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed) {
                        window.close();
                    }
                }
                    
                window.clear(sf::Color::Black);
                
                if (!done) {
                    for (int i = 0;i < len;i++) {

                        int j = i;
                        
                        while (j > 0 && numbers[j] < numbers[j - 1]) {
                            
                            sf::sleep(time);
                            
                            for (int k = 0;k < len;k++) {
                                if (k != j) {
                                    Column col;
                                    col.width = unit - 1;
                                    col.length = -numbers[k] * height;
                                    col.x = k * unit;
                                    col.y = 600;
                                    window.draw(col.Draw());
                                    //window.display();
                                }
                            }
                            Column col;
                            col.width = unit - 1;
                            col.length = -numbers[j] * height;
                            col.x = j * unit;
                            col.y = 600;
                            col.color = sf::Color::Red;
                            window.draw(col.Draw());
                            window.display();
                            
                            window.clear(sf::Color::Black);
                            int a = numbers[j];
                            numbers[j] = numbers[j - 1];
                            numbers[j - 1] = a;
                            j--;

                        }
                        
                        if (i == len - 1) {
                            done = true;
                        }
                    }
                }
                if (done) {
                    for (int k = 0;k < len;k++) {
                        Column col;
                        col.width = unit - 1;
                        col.length = -numbers[k] * height;
                        col.x = k * unit;
                        col.y = 600;
                        col.color = sf::Color::Green;
                        window.draw(col.Draw());
                    }
                }

                window.display();
            }
            break;
    }
    
        case 2:
        {
            fixAdjMatrix();
            printf("Performing Djikstra's Algorithm\n\n");
            glutInit(&argc, argv);
            glutInitWindowSize(ww, wh);
            glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
            glutCreateWindow("Dijksta's Algorithm");
            myInit();
            glutReshapeFunc(myReshape);
            glutMouseFunc(myMouse);
            glutKeyboardFunc(beginDijkstra);
            // glutMotionFunc(drawSquare);
            glutDisplayFunc(myDisplay);
            glutMainLoop();
    
            break;
        }
        
        case 3:
        {
            glutInit(&argc,argv);

       glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
       glutInitWindowPosition(520,0);
       glutInitWindowSize(500,500);
       glutCreateWindow("Front Sheet");
       glutDisplayFunc(display1);
       myInit1();

       glutInitDisplayMode( GLUT_SINGLE|GLUT_RGB );
       glutInitWindowSize(500,500);
       glutInitWindowPosition(0,0);
       glutCreateWindow("My Window");
       glutDisplayFunc(display);
       glutMouseFunc(mouse);
       glutCreateMenu(top_menu);
       glutAddMenuEntry("Read Cost Matrix",1);
       glutAddMenuEntry("Display Weighted Graph",2);
       glutAddMenuEntry("Display Shortest Path",3);
       glutAddMenuEntry("Exit",4);
       glutAttachMenu(GLUT_RIGHT_BUTTON);
       printf("\nGO TO MY WINDOW AND CLICK RIGHT BUTTON FOR NEXT OPTION\n");
       init();

       glutMainLoop();
       break;
        }
        case 4:
        {
            glutInit(&argc , argv);

            initBress();

            glutMainLoop();
        }
       
        
    }
        /*cout<<"Run again? (Y/N)";
        cin>>choice;
    }while(choice ==  'Y');*/
    
    return 0;
}


//return unselected node with minimum distance from the node
int getMinimumDistanceNode(int *distanceFromSource, int *selected)
{
    int minimumNode = -1;
    int minimumDistance = 99999;

    //loop through the nodes
    for(int i = 0; i < nodeCount; i++)
    {
        //if the node is not selected and has low distance choose it for now.
        if(selected[i] != 1 && distanceFromSource[i] < minimumDistance)
        {
            minimumNode = i;
            minimumDistance = distanceFromSource[i];
        }
    }

    //return the node with minimum distance
    return minimumNode;
}

//route source to destination
void routeNodes(int n1, int n2)
{
    lineNodes[0] = nodes[n1];
    lineNodes[1] = nodes[n2];

    drawLine('M');
}

void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;
 
    // Stroing start time
    clock_t start_time = clock();
 
    // looping till required time is not acheived
    while (clock() < start_time + milli_seconds);
}


//Dijkstra's algorithm
void dijkstra()
{

    //distance of all nodes from the source node
    int distanceFromSource[26];

    //node connecting to the current node
    int prev[26];

    //if node is visited
    int selected[26];

    //the current node
    int current;

    //initilize nodes distance with 99999(ie not visited yet), selected to -1(not visited yet), prev to -1(no connection yet)
    for(int i = 0; i < nodeCount; i++)
    {
        distanceFromSource[i] = 99999;
        prev[i] = -1;
        selected[i] = -1;
    }

    //distance of source node to 0 to select it first
    distanceFromSource[sourceNode.id] = 0;

    for(int i = 0; i < nodeCount; i++)
    {
        //get node with minimum distance not selected
        current = getMinimumDistanceNode(distanceFromSource, selected);
        //mark it selected

        if(current == -1)
            break;

        selected[current] = 1;
        printf("\n%c\n", current + 65);

        //loop through all the nodes finding its neighbours
        for(int j = 0; j < nodeCount; j++)
        {
            //if it's already visited dont bother
            if(selected[j] != 1)
            {
                //if the distance from the current node is less, select it as it's previous node
                if((distanceFromSource[current] + adjMatrix[current][j]) < distanceFromSource[j])
                {
                    distanceFromSource[j] = distanceFromSource[current] + adjMatrix[current][j];

                    if(prev[j] != -1)
                    {
                        lineNodes[0] = nodes[prev[j]];
                        lineNodes[1] = nodes[j];
                        delay(700);
                        drawLine('R');

                        delay(700);
                        lineNodes[0] = nodes[current];
                        drawLine('D');
                    }
                    else
                    {
                        delay(700);
                        lineNodes[1] = nodes[j];
                        lineNodes[0] = nodes[current];
                        drawLine('D');
                    }

                    prev[j] = current;
                }
            }
        }

    }

    while(1)
    {
        if(prev[destinationNode.id] == -1)
            break;
        delay(500);
        routeNodes(destinationNode.id, prev[destinationNode.id]);
        destinationNode = nodes[prev[destinationNode.id]];
    }

    //display distance from array and previous array to console
    printf("node(i)\t\tdistance\tprevious\tselected\n");
    for(int i = 0; i < nodeCount; i++)
    {
        printf("%c\t\t%d\t\t%c\t\t%d\n",65+i, distanceFromSource[i], ((prev[i] != -1)? 65+prev[i] : ' ') , selected[i]);
    }

}
