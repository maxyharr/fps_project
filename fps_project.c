//
//  main.c
//  hw3
//
//  Created by Max Harris on 9/24/14.
//  Copyright (c) 2014 Max Harris. All rights reserved.
//


/*
 *  Key bindings:
 *  m          Toggle between perspective and orthogonal
 *  +/-        Changes field of view for perspective
 *  a          Toggle axes
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int axes=1;       //  Display axes
int mode=1;       //  Projection mode
int fov=50;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=8.0;   //  Size of world
int paused = 0;

int key_state[256] = { 0 };

// Differences from ex13
int move=1;       //  Move light
int light=1;      //  Lighting
// Light values
int one       =   1;  // Unit value
int distance  =   5;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shinyvec[1];    // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   2;  // Elevation of light
int cameraSelected = 1;
int lightSelected = 0;
int isWarpingPointer = 0;

// melee attack
int meleeThrown = 0;
int canThrowMelee = 1;
double meleeTimer = 0;
double MELEE_SECONDS = 1;

// ammo box
double ammoX = -50, ammoZ = -50, ammoXWidth = 0.3, ammoHeight = 0.3, ammoZWidth = 0.3;
int ammoPresent = 1;
double ammoReloadTime;
int totalAmmo = 75;
int MAX_AMMO = 150;
int AMMO_BOX_TYPE = 1;

//autokillbox
double autoKillBoxX = 75, autoKillBoxZ = 75;
int KILL_BOX_TYPE = 2;

int initialized = 0;

// Window Sizing
int windowWidth = 1200;
int windowHeight = 700;
int th = 1200/2;         //  Azimuth of view angle
int ph = 900/2;         //  Elevation of view angle


// Game Constants
int highScore;
int score = 0;
int showScore = 0;
double showScoreTimer = 0;
double SHOW_SCORE_SECONDS = 2;
int update_score;

//CAMERA - angle of rotation - player constants
float lastx, lasty;
double xpos = 0, ypos = 0, zpos = 30, xrot = 100, yrot = 0, angle=0.0;
    //bobble
float walkbias, walkbiasangle;
int isJumping; double jumpAngle = 0;
double jumpHeight = 1.8;
int playerHP = 30;
int enemyTouchingPlayer = 0;
int walkingInNegXDirection, walkingInNegZDirection, walkingInPosXDirection, walkingInPosZDirection;
int allowedToAutoKill = 0;
int showAutoKillBox = 1;
double autoKillTimer = 0;
double AUTO_KILL_SECONDS = 35;

// lasers
int numLasers = 0;
int maxLasers = 25; // Only allow x lasers before reloading // DON'T GO ABOVE 200 W/0 changing array sizes below
int makeLaser = 0;
int laserMoving[20]; //int laserMoving = 0;
int needToReload = 0;
double reloadTimer = 0;
int isReloading = 0;
double RELOAD_SECS = 1.2;
int allowedToShoot = 1;
double shotTimer = 0;
double SHOT_TIMER_SECS = 0.1;

double laserPosX[200], laserPosZ[200], laserPosY[200];
double laserStartX[200], laserStartZ[200], laserStartY[200];
double laserDirectionX[200], laserDirectionY[200];
//double laserPosX, laserPosZ, laserPosY, laserStartX, laserStartZ, laserStartY, laserDirectionX, laserDirectionZ, laserDirectionY;


// enemy constants
double enemyPosX=-75, enemyPosY=0, enemyPosZ=-75;
double enemyZWidth=1, enemyXWidth=1, enemyHeight=2;
double enemyRotation;
int enemyDead = 0;
double ENEMY_MOVEMENT_SECS = 0.2;
int ENEMY_MAX_HP = 20;
int enemyHP = 20;
int numEnemys = 20;

// houses locations
int numHouses = 20;
double houseX[50], houseY[50], houseZ[50];
double houseXWidth = 3, houseHeight = 3.5, houseZWidth = 5;


//textures
unsigned int trunkTexture;
unsigned int leafTexture;
unsigned int houseWallTexture;
unsigned int houseRoofTexture;
unsigned int houseRoofFrontTexture;
unsigned int grassTexture;
unsigned int gravelTexture;
unsigned int doorTexture;
unsigned int hoboTexture;
unsigned int crateTexture;

// camera position - Change with WASD in conjuction with arrow keys to look around
double Ex = 0;
double Ey = 0;
double Ez = 10;

//  Macro for sin & cos in degrees
#define Cos(th) cos(3.1415927/180*(th))
#define Sin(th) sin(3.1415927/180*(th))

// Macro for dot, cross and normalize functions
#define cross(a,b,c) \
(a)[0] = (b)[1] * (c)[2] - (c)[1] * (b)[2]; \
(a)[1] = (b)[2] * (c)[0] - (c)[2] * (b)[0]; \
(a)[2] = (b)[0] * (c)[1] - (c)[0] * (b)[1];

#define dot(v,q) \
((v)[0] * (q)[0] + \
 (v)[1] * (q)[1] + \
 (v)[2] * (q)[2]);

#define normalize(a) \
(a)[0] = (a)[0] / sqrt( ((a)[0]*(a)[0]) + ((a)[1]*(a)[1]) + ((a)[2]*(a)[2]) ); \
(a)[1] = (a)[1] / sqrt( ((a)[0]*(a)[0]) + ((a)[1]*(a)[1]) + ((a)[2]*(a)[2]) ); \
(a)[2] = (a)[2] / sqrt( ((a)[0]*(a)[0]) + ((a)[1]*(a)[1]) + ((a)[2]*(a)[2]) ); \


//  Wall
double WALL_WIDTH = 1;
double WALL_HEIGHT = 1;
double WALL_LENGTH = 1;
//  Door
double DOOR_WIDTH = 0.2;
double DOOR_HEIGHT = 0.4;
//  Light Pole
double POLE_HEIGHT = 1;
double POLE_WIDTH  = 1;
// Tree
double TREE_WIDTH = 0.4;
double TREE_HEIGHT = 2.5;

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  //  Maximum length of text string
void Print(const char* format , ...)
{
    char    buf[LEN];
    char*   ch=buf;
    va_list args;
    //  Turn the parameters into a character string
    va_start(args,format);
    vsnprintf(buf,LEN,format,args);
    va_end(args);
    //  Display the characters one at a time at the current raster position
    while (*ch)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/*
 *  Set projection
 */
static void Project()
{
    //  Tell OpenGL we want to manipulate the projection matrix
    glMatrixMode(GL_PROJECTION);
    //  Undo previous transformations
    glLoadIdentity();
    //  Perspective transformation
    if (mode)
        gluPerspective(fov,asp,0.05,20*dim);
    //  Orthogonal projection
    else
        glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
    //  Switch to manipulating the model matrix
    glMatrixMode(GL_MODELVIEW);
    //  Undo previous transformations
    glLoadIdentity();
}


static void Vertex(double th,double ph)
{
    double x = Sin(th)*Cos(ph);
    double y = Cos(th)*Cos(ph);
    double z =         Sin(ph);
    //  For a sphere at the origin, the position
    //  and normal vectors are the same
    glNormal3d(x,y,z);
    glVertex3d(x,y,z);
}


static void enemy(double enemyX, double enemyY, double enemyZ, double Xwidth, double height, double Zwidth, double camX, double camZ) {
    double enemyLookAt[3], enemyToCamProj[3], upAux[3];
    double enemyAngleCosine;
    
    glPushMatrix();
    
    // objToCamProj is the vector in world coordinates from the
    // local origin to the camera projected in the XZ plane
    enemyToCamProj[0] = camX - enemyX ;
    enemyToCamProj[1] = 0;
    enemyToCamProj[2] = camZ - enemyZ ;
    
    // This is the original lookAt vector for the object
    // in world coordinates
    enemyLookAt[0] = 0;
    enemyLookAt[1] = 0;
    enemyLookAt[2] = 1;
    
    // normalize both vectors to get the cosine directly afterwards
    normalize(enemyToCamProj);
    
    // easy fix to determine wether the angle is negative or positive
    // for positive angles upAux will be a vector pointing in the
    // positive y direction, otherwise upAux will point downwards
    // effectively reversing the rotation.
    // TODO: WRITE CROSSPRODUCT
    cross(upAux, enemyLookAt,enemyToCamProj);

    
    // compute the angle
    enemyAngleCosine = dot(enemyLookAt,enemyToCamProj);
    
    glTranslated(enemyX, enemyY, enemyZ);
    glScaled(Xwidth, height, Zwidth);
    
    // perform the rotation. The if statement is used for stability reasons
    // if the lookAt and objToCamProj vectors are too close together then
    // |angleCosine| could be bigger than 1 due to lack of precision
    if ((enemyAngleCosine < 0.99990) && (enemyAngleCosine > -0.9999))
        glRotatef(acos(enemyAngleCosine)*180/3.14159,upAux[0], upAux[1], upAux[2]);

    //  Enable textures
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    
    
    glColor4f(1, 1, 1, 1);
    glBindTexture(GL_TEXTURE_2D,hoboTexture);
    
    glBegin(GL_QUADS);
    
    glNormal3f(0,0,1);
    glTexCoord2f(0, 0); glVertex3f(-1,0,0);
    glTexCoord2f(0, 1); glVertex3f(-1,1,0);
    glTexCoord2f(1, 1); glVertex3f(1,1,0);
    glTexCoord2f(1, 0); glVertex3f(1,0,0);
    
    glDisable(GL_TEXTURE_2D);
    glEnd();
    glPopMatrix();
}

/*
 *  Draw a tree
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */



static void ammobox(double ammox, double ammoy, double ammoz, double ammoxwidth, double ammoheight, double ammozwidth, int box_type) {
    glPushMatrix();
    glTranslated(ammox, ammoy, ammoz);
    glScaled(ammoxwidth, ammoheight, ammozwidth);
    
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    
    glBindTexture(GL_TEXTURE_2D, crateTexture);
    
    glBegin(GL_QUADS);
    if(box_type == AMMO_BOX_TYPE)
        glColor3f(1,1,1);
    else if(box_type == KILL_BOX_TYPE)
        glColor3f(1, 0.3,0);
    
    //crate top
    glTexCoord2f(0,0); glVertex3f(-1, 2, -1);
    glTexCoord2f(1,0); glVertex3f(-1, 2, 1);
    glTexCoord2f(1,1); glVertex3f(1, 2, 1);
    glTexCoord2f(0,1); glVertex3f(1, 2, -1);
    
    //crate bottom
    glTexCoord2f(0, 0); glVertex3f(-1, 0, -1);
    glTexCoord2f(0, 1); glVertex3f(-1, 0, 1);
    glTexCoord2f(1, 1);glVertex3f(1, 0, 1);
    glTexCoord2f(1, 0);glVertex3f(1, 0, -1);
    
    // crate left
    glTexCoord2f(0, 0);glVertex3f(-1, 0, -1);
    glTexCoord2f(0, 1);glVertex3f(-1, 2, -1);
    glTexCoord2f(1, 1);glVertex3f(-1, 2, 1);
    glTexCoord2f(1, 0);glVertex3f(-1, 0, 1);
    
    // crate right
    glTexCoord2f(0, 0);glVertex3f(1, 0, -1);
    glTexCoord2f(0, 1);glVertex3f(1, 2, -1);
    glTexCoord2f(1, 1);glVertex3f(1, 2, 1);
    glTexCoord2f(1, 0);glVertex3f(1, 0, 1);

    
    // crate front
    glTexCoord2f(0, 0);glVertex3f(-1, 0, 1);
    glTexCoord2f(0, 1);glVertex3f(-1, 2, 1);
    glTexCoord2f(1, 1);glVertex3f(1, 2, 1);
    glTexCoord2f(1, 0);glVertex3f(1, 0, 1);

    
    // crate back
    glTexCoord2f(0, 0);glVertex3f(-1, 0, -1);
    glTexCoord2f(0,1);glVertex3f(-1, 2, -1);
    glTexCoord2f(1, 1);glVertex3f(1, 2, -1);
    glTexCoord2f(1, 0);glVertex3f(1, 0, -1);

    
    
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    
}

static void tree(double x, double y, double z,
                 double width, double height, double th)
{
    // Save transformation
    glPushMatrix();
    // Offset
    glTranslated(x, y, z);
    glScaled(width, height, width);
    glRotated(th, 0, 1, 0);
    
    //  Enable textures
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    
    glColor3f(1, 1, 1);
    glBindTexture(GL_TEXTURE_2D,trunkTexture);
    
    // Trunk
    glBegin(GL_QUADS);
    glColor3f(0.5, 0.35, 0.05);
    
    // Trunk back
    glNormal3f(0, 0, -1);
    glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
    glTexCoord2f(0, 1); glVertex3f(0, 1, 0);
    glTexCoord2f(1, 1); glVertex3f(1, 1, 0);
    glTexCoord2f(1, 0); glVertex3f(1, 0, 0);
    
    // Trunk front
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0); glVertex3f(0, 0, 1);
    glTexCoord2f(0, 1); glVertex3f(0, 1, 1);
    glTexCoord2f(1, 1); glVertex3f(1, 1, 1);
    glTexCoord2f(1, 0); glVertex3f(1, 0, 1);
    
    
    // Trunk left
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
    glTexCoord2f(0, 1); glVertex3f(0, 1, 0);
    glTexCoord2f(1, 1); glVertex3f(0, 1, 1);
    glTexCoord2f(1, 0); glVertex3f(0, 0, 1);
    
    
    // Trunk right
    glNormal3f(1, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(1, 0, 0);
    glTexCoord2f(0, 1); glVertex3f(1, 1, 0);
    glTexCoord2f(1, 1); glVertex3f(1, 1, 1);
    glTexCoord2f(1, 0); glVertex3f(1, 0, 1);
    glEnd();
    glBindTexture(GL_TEXTURE_2D,leafTexture);
    
    glBegin(GL_QUADS);
    // "Leaves"
    // bottom gets shadow effect
    glColor3f(0, 0.7, 0);
    // bottom
    glNormal3f(0, -1, 0);
    glTexCoord2f(0, 0); glVertex3f(-3, 1, 4);
    glTexCoord2f(1, 0); glVertex3f(4, 1, 4);
    glTexCoord2f(1, 1); glVertex3f(4, 1, -3);
    glTexCoord2f(0, 1); glVertex3f(-3, 1, -3);
    glEnd();
    
    // BODY TODO: ADD NORMAL
    
    glBegin(GL_TRIANGLES);
    // back and front get light effect
    //glColor3f(0, 0.35, 0);
    // body back
    glNormal3f(0, 17.5, -5.6);
    glTexCoord2f(0, 0); glVertex3f(-3, 1, -3);
    glTexCoord2f(1, 0); glVertex3f(4, 1, -3);
    glTexCoord2f(0.5, 1); glVertex3f(0.5, 1.8, 0.5);
    // body front
    glNormal3f(0, 17.5, 5.6);
    glTexCoord2f(0, 0); glVertex3f(4, 1, 4);
    glTexCoord2f(1, 0); glVertex3f(-3, 1, 4);
    glTexCoord2f(0.5,1); glVertex3f(0.5, 1.8, 0.5);

    
    // left and right get shadow effect
    //glColor3f(0, 0.5, 0);
    // body left
    glNormal3f(-5.6, 17.5, 0);
    glTexCoord2f(0, 0); glVertex3f(-3, 1, -3);
    glTexCoord2f(1, 0); glVertex3f(-3, 1, 4);
    glTexCoord2f(0.5, 1); glVertex3f(0.5, 1.8, 0.5);
    // body right
    glNormal3f(5.6, 17.5, 0);
    glTexCoord2f(0, 0); glVertex3f(4, 1, -3);
    glTexCoord2f(1, 0); glVertex3f(4, 1, 4);
    glTexCoord2f(0.5, 1); glVertex3f(0.5, 1.8, 0.5);
    
    glDisable(GL_TEXTURE_2D);
    glEnd();
    glPopMatrix();
}



static void lightTop(double x, double y, double z, double width)
{
    glPushMatrix();
    glTranslated(x, y, z);
    glScaled(width, width, width);
    
    glBegin(GL_QUADS);
    glColor3f(1, 1, 0);
    
    // light bottom
    glNormal3f(0, -1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1);
    glVertex3f(1, 0, 1);
    glVertex3f(1, 0, 0);
    
    // light top
    glNormal3f(0, 1, 0);
    glVertex3f(0, 1, 0);
    glVertex3f(0, 1, 1);
    glVertex3f(1, 1, 1);
    glVertex3f(1, 1, 0);
    
    //light back
    glNormal3f(0, 0, -1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 1, 0);
    glVertex3f(1, 1, 0);
    glVertex3f(1, 0, 0);
    
    //light front
    glNormal3f(0, 0, 1);
    glVertex3f(0, 0, 1);
    glVertex3f(0, 1, 1);
    glVertex3f(1, 1, 1);
    glVertex3f(1, 0, 1);
    
    //light left
    glNormal3f(-1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1);
    glVertex3f(0, 1, 1);
    glVertex3f(0, 1, 0);
    
    //light right
    glNormal3f(1, 0, 0);
    glVertex3f(1, 0, 0);
    glVertex3f(1, 0, 1);
    glVertex3f(1, 1, 1);
    glVertex3f(1, 1, 0);
    
    
    glEnd();
    glPopMatrix();
}



/*
 *  Draw a light pole
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */


static void lightPole(double x, double y, double z,
                      double width, double height, double th)
{
    // Save Transformation
    glPushMatrix();
    //  Offset
    glTranslated(x, y, z);
    glScaled(width, height, width);

    
    //  Pole
    glBegin(GL_QUADS);
    glColor3f(0.1, 0.1, 0.1);
    
    // Pole back
    glNormal3f(0, 0, -1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, POLE_HEIGHT, 0);
    glVertex3f(POLE_WIDTH, POLE_HEIGHT, 0);
    glVertex3f(POLE_WIDTH, 0, 0);
    
    // Pole front
    glNormal3f(0, 0, 1);
    glVertex3f(0, 0, POLE_WIDTH );
    glVertex3f(0, POLE_HEIGHT, POLE_WIDTH);
    glVertex3f(POLE_WIDTH, POLE_HEIGHT, POLE_WIDTH);
    glVertex3f(POLE_WIDTH, 0, POLE_WIDTH);
    
    //  Pole left
    glNormal3f(-1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, POLE_HEIGHT, 0);
    glVertex3f(0, POLE_HEIGHT, POLE_WIDTH);
    glVertex3f(0, 0, POLE_WIDTH);
    
    //  Pole right
    glNormal3f(1, 0, 0);
    glVertex3f(POLE_WIDTH, 0, 0);
    glVertex3f(POLE_WIDTH, POLE_HEIGHT, 0);
    glVertex3f(POLE_WIDTH, POLE_HEIGHT, POLE_WIDTH);
    glVertex3f(POLE_WIDTH, 0, POLE_WIDTH);
    
    
    glEnd();
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(x, y, z);
    glScaled(1, 1, 1);
    lightTop(-0.05, POLE_HEIGHT*height, -0.05, width+0.1);
    glPopMatrix();
}

static void grass(double x, double y, double z,
                  double dx, double dy, double dz) {
    glPushMatrix();
    glTranslated(x, y, z);
    glScaled(dx, dy, dz);
    
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T,
                    GL_REPEAT);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, grassTexture);

    
    glColor3f(0, 0.9, 0);
    glBegin(GL_QUADS);
    
    glNormal3f(0, 1, -0.5);
    
    glTexCoord2f(0,0); glVertex3f(0, 0, 0);
    glTexCoord2f(50,0); glVertex3f(1, 0, 0);
    glTexCoord2f(50,50); glVertex3f(1, 0, 1);
    glTexCoord2f(0,50); glVertex3f(0, 0, 1);
    
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}



/*
 *  Draw a house
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */

static void house(double x, double y, double z,
                  double dx, double dy, double dz,
                  double th)
{
    // Save Transformation
    glPushMatrix();
    // Offset
    glTranslated(x, y, z);
    glRotated(th, 0, 1, 0);
    glScaled(dx, dy, dz);
    
    //  Enable textures
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glColor3f(1,1,1);
    glBindTexture(GL_TEXTURE_2D, houseWallTexture);

    
    glBegin(GL_QUADS);
    
    //  back wall
    //glColor3f(0.5, 0.5, 0.5);
    glNormal3f(0, 0, -1);
    glTexCoord2f(0, 0); glVertex3f(-1, 0, -1);
    glTexCoord2f(0, 1); glVertex3f(-1, WALL_HEIGHT, -1);
    glTexCoord2f(1, 1); glVertex3f(WALL_WIDTH, WALL_HEIGHT, -1);
    glTexCoord2f(1, 0); glVertex3f(WALL_WIDTH, 0, -1);
    
    // left wall
    glNormal3f(-1, 0, 0);
//    glColor3f(0.3, 0.3, 0.3);
    glTexCoord2f(0, 0); glVertex3f(-1, 0, -1);
    glTexCoord2f(0, 1); glVertex3f(-1, WALL_HEIGHT, -1);
    glTexCoord2f(1, 1); glVertex3f(-1, WALL_HEIGHT, WALL_LENGTH);
    glTexCoord2f(1, 0); glVertex3f(-1, 0, WALL_LENGTH);
    
    // right wall
    glNormal3f(1, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(WALL_WIDTH, 0, -1);
    glTexCoord2f(0, 1); glVertex3f(WALL_WIDTH, WALL_HEIGHT, -1);
    glTexCoord2f(1, 1); glVertex3f(WALL_WIDTH, WALL_HEIGHT, WALL_LENGTH);
    glTexCoord2f(1, 0); glVertex3f(WALL_WIDTH, 0, WALL_LENGTH);
    
    // front
    glNormal3f(0, 0, 1);
    //glColor3f(0.5, 0.5, 0.5);
    glTexCoord2f(0, 0); glVertex3f(-1, 0, WALL_LENGTH);
    glTexCoord2f(1, 0); glVertex3f(WALL_WIDTH, 0, WALL_LENGTH);
    glTexCoord2f(1, 1); glVertex3f(WALL_WIDTH, WALL_HEIGHT, WALL_LENGTH);
    glTexCoord2f(0, 1); glVertex3f(-1, WALL_HEIGHT, WALL_LENGTH);
    glEnd();
    
    glColor3f(1,1,1);
    glBindTexture(GL_TEXTURE_2D,houseRoofTexture);
    glBegin(GL_QUADS);

    // roof (left)
    glNormal3f(-1.08746046,1.2,0);
    glTexCoord2f(0, 0); glVertex3f(-1, WALL_HEIGHT, -1);
    glTexCoord2f(1, 0); glVertex3f(-1, WALL_HEIGHT, WALL_LENGTH);
    glTexCoord2f(1, 1); glVertex3f(0, WALL_WIDTH*tan(45), WALL_LENGTH);  //tan (45 rads)
    glTexCoord2f(0, 1); glVertex3f(0, WALL_WIDTH*tan(45), -1);            //tan (45 rads)
    
    // roof (right)
    glNormal3f(1.08746046,1.2,0);
    glTexCoord2f(0, 0); glVertex3f(WALL_WIDTH, WALL_HEIGHT, -1);
    glTexCoord2f(1, 0); glVertex3f(WALL_WIDTH, WALL_HEIGHT, WALL_LENGTH);
    glTexCoord2f(1, 1); glVertex3f(0, WALL_WIDTH*tan(45), WALL_LENGTH);  //tan (45 rads)
    glTexCoord2f(0, 1); glVertex3f(0, WALL_WIDTH*tan(45), -1);            //tan (45 rads)
    
    glEnd();
    
    glBegin(GL_QUADS);
    //  floor
    glNormal3f(0, -1, 0);
    glColor3f(0, .6, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, WALL_LENGTH);
    glVertex3f(WALL_WIDTH, 0, WALL_LENGTH);
    glVertex3f(WALL_WIDTH, 0, 0);
    glEnd();

    

    // door on house
    glBindTexture(GL_TEXTURE_2D,doorTexture);
    glBegin(GL_QUADS);
    glColor3f(1, 1, 0);
    glNormal3f(0, 0, 1);
    glTexCoord2f(0,0); glVertex3f(-DOOR_WIDTH/2, 0, WALL_LENGTH+0.0001);
    glTexCoord2f(1,0); glVertex3f(DOOR_WIDTH/2, 0, WALL_LENGTH+0.0001);
    glTexCoord2f(1,1); glVertex3f(DOOR_WIDTH/2, DOOR_HEIGHT, WALL_LENGTH+0.01);
    glTexCoord2f(0,1); glVertex3f(-DOOR_WIDTH/2, DOOR_HEIGHT, WALL_LENGTH+0.01);
    glEnd();
    
    glBindTexture(GL_TEXTURE_2D, houseRoofFrontTexture);
    glBegin(GL_TRIANGLES);
    glColor3f(0.5, 0.35, 0.05);
    
    // roof front
    glNormal3f(0, 0, 1);
    glTexCoord2f(0,0); glVertex3f(-1, WALL_HEIGHT, WALL_LENGTH);
    glTexCoord2f(0.5,1); glVertex3f(0, WALL_WIDTH*tan(45), WALL_LENGTH);
    glTexCoord2f(0, 1); glVertex3f(WALL_WIDTH, WALL_HEIGHT, WALL_LENGTH);
    
    // roof back
    glNormal3f(0, 0, -1);
    glTexCoord2f(0, 0); glVertex3f(-1, WALL_HEIGHT, -1);
    glTexCoord2f(0.5, 1); glVertex3f(0, WALL_WIDTH*tan(45), -1);
    glTexCoord2f(0, 1); glVertex3f(WALL_WIDTH, WALL_HEIGHT, -1);
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

}

// To draw a ball where the light should be
static void ball(double x,double y,double z,double r)
{
    int th,ph;
    float yellow[] = {1.0,1.0,0.0,1.0};
    float Emission[]  = {0.0,0.0,0.01*emission,1.0};
    //  Save transformation
    glPushMatrix();
    //  Offset, scale and rotate
    glTranslated(x,y,z);
    glRotated(90, 0, 1, 0);
    glScaled(r,r,r);
    //  White ball
    glColor3f(1,1,1);
    glMaterialfv(GL_FRONT,GL_SHININESS,shinyvec);
    glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
    glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
    //  Bands of latitude
    for (ph=-90;ph<90;ph+=inc)
    {
        glBegin(GL_QUAD_STRIP);
        for (th=0;th<=360;th+=2*inc)
        {
            Vertex(th,ph);
            Vertex(th,ph+inc);
        }
        glEnd();
    }
    //  Undo transofrmations
    glPopMatrix();
}


/*
 *  GLUT calls this routine when the mouse is moved without buttons pushed
 */

void mouseMovementNoButtons(int x, int y) {
    
    if (!isWarpingPointer) {
        double diffx=x-lastx; //check the difference between the current x and the last x position
        double diffy=y-lasty; //check the difference between the current y and the last y position
        lastx=x; //set lastx to the current x position
        lasty=y; //set lasty to the current y position
        xrot += diffy; //set the xrot to xrot with the addition of the difference in the y position
        yrot += diffx;// set the xrot to yrot with the addition of the difference in the x position
        if (x >= windowWidth-10 || x <= 10 || y>= windowHeight-10 ||y <= 10) {
            isWarpingPointer = 1;
            glutWarpPointer(windowWidth/2, windowHeight/2);
        }
    } else {
        lastx = x;
        lasty = y;
        isWarpingPointer = 0;
    }
}

void mouseMovementWithButtons(int x, int y) {
    
    if (!isWarpingPointer) {
        int diffx=x-lastx; //check the difference between the current x and the last x position
        int diffy=y-lasty; //check the difference between the current y and the last y position
        lastx=x; //set lastx to the current x position
        lasty=y; //set lasty to the current y position
        xrot += (float) diffy; //set the xrot to xrot with the addition of the difference in the y position
        yrot += (float) diffx;// set the xrot to yrot with the addition of the difference in the x position
        if (x >= windowWidth-10 || x <= 10 || y>= windowHeight-10 ||y <= 10) {
            isWarpingPointer = 1;
            glutWarpPointer(windowWidth/2, windowHeight/2);
        }
    } else {
        lastx = x;
        lasty = y;
        isWarpingPointer = 0;
    }
}

static void laser(double x, double y, double z,
                  double dx, double dy, double dz) {
    glPushMatrix();
    glTranslated(x, y, z);
    glScaled(dx, dy, dz);
    
    glColor3f(1,0,0);
    glBegin(GL_QUADS);
    
    //left
    glVertex3f(0,0,0);
    glVertex3f(0,1,0);
    glVertex3f(0,1,1);
    glVertex3f(0,0,1);
    
    //right
    glVertex3f(1,0,0);
    glVertex3f(1,1,0);
    glVertex3f(1,1,1);
    glVertex3f(1,0,1);
    
    //front
    glVertex3f(0,0,1);
    glVertex3f(1,0,1);
    glVertex3f(1,1,1);
    glVertex3f(0,1,1);
    
    //back
    glVertex3f(0,0,0);
    glVertex3f(1,0,0);
    glVertex3f(1,1,0);
    glVertex3f(0,1,0);
    
    //top
    glVertex3f(0,1,0);
    glVertex3f(0,1,1);
    glVertex3f(1,1,1);
    glVertex3f(1,1,0);
    
    //bottom
    glVertex3f(0,0,0);
    glVertex3f(0,0,1);
    glVertex3f(1,0,1);
    glVertex3f(1,0,0);

    
    glEnd();
    glPopMatrix();
}


void mousePressed(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        makeLaser = 1;
        glutPostRedisplay();
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        makeLaser = 0;
        glutPostRedisplay();
    }
}


void camera(void) {
    glRotatef(xrot,1.0,0.0,0.0);  //rotate our camera on the x-axis (left and right)
    glRotatef(yrot,0.0,1.0,0.0);  //rotate our camera on the y-axis (up and down)
    glTranslated(-xpos,-ypos-0.4-walkbias,-zpos); //translate the screen to the position of our camera
}



/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
    
    const double len=1.5;  //  Length of axes
    // Make the Background Sky Blue
    glClearColor(0, 0.8, 1, 0);
    //  Erase the window and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    //  Enable Z-buffering in OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    //  Undo previous transformations
    glLoadIdentity();
    //  Perspective - set eye position
    if (mode)
    {
        //    Position                 Direction                    Top of Head
        //gluLookAt(Ex,0.4,Ez,   Ex+Sin(th),0.4+Sin(ph),Ez+Cos(th),     0,1,0);
        camera();
    }
    //  Orthogonal - set world orientation
    else
    {
        glRotatef(ph,1,0,0);
        glRotatef(th,0,1,0);
    }
    
    // ####################################
    // ##           Light Switch         ##
    // ####################################
    
    if (light)
    {
        //  Translate intensity to color vectors
        float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
        float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
        float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
        //  Light position
        float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
        //  Draw light position as ball (still no lighting here)
        glColor3f(1,1,1);
        ball(Position[0],Position[1],Position[2] , 0.1);
        //  OpenGL should normalize normal vectors
        glEnable(GL_NORMALIZE);
        //  Enable lighting
        glEnable(GL_LIGHTING);
        //  Location of viewer for specular calculations
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
        //  glColor sets ambient and diffuse color materials
        glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
        //  Enable light 0
        glEnable(GL_LIGHT0);
        //  Set ambient, diffuse, specular components and position of light 0
        glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
        glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
        glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
        glLightfv(GL_LIGHT0,GL_POSITION,Position);
    }
    else
        glDisable(GL_LIGHTING);
    
    
    
    // ####################################
    // ######       Draw Scene      #######
    // ####################################
    
    for (int i=0; i<numLasers; i++){
        if (laserMoving[i]) {
            laser(laserPosX[i], 0.4+laserPosY[i], laserPosZ[i], 0.05, 0.05, 0.05);
        }
    }
    

    
    //  Flat or smooth shading
    glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);
    
//    // Draw a couple houses on the left
//    for (int i=-3; i< 9; i=i+3){
//        house(-5, 0, i, 1, 1, 2, 85);
//    }
    
//    // Draw a couple of lights in the scene
//    for (int i=-3; i<9; i=i+3){
//        lightPole(-2, 0, i+0.3, 0.1, 1, 0);
//    }
    
//    // Draw a couple different houses on the right
//    for (int i=-4; i< 9; i=i+3){
//        house(5, 0, i, 1.2, 1, 2, -85);
//    }
    
//    // Draw a couple of lights on the other side
//    for (int i=-4; i<9; i=i+3){
//        lightPole(2.5, 0, i, 0.1, 1, 0);
//    }
    
//    // Draw trees everywhere
//    for (int i=-9; i<10; i=i+3) {
//        tree(-9.5, 0, i, TREE_WIDTH, TREE_HEIGHT, -20);
//        tree(9.5, 0, i, TREE_WIDTH, TREE_HEIGHT, 40);
//        if (i < 8 && i > -8)
//            tree(i, 0, -9.5, TREE_WIDTH, TREE_HEIGHT, 30);
//    }
    
    // Draw a giant church-like house in the back
    houseX[0] = houseY[0] = houseZ[0] = 0;
    house(houseX[0], houseY[0], houseZ[0], houseXWidth, houseHeight, houseZWidth, 0);
    lightPole(-1.8, 0, 3.5, 0.3, 3, 0);
    
    if (ammoPresent)
        ammobox(ammoX, 0, ammoZ, ammoXWidth, ammoHeight, ammoZWidth, AMMO_BOX_TYPE);
    
    if (showAutoKillBox)
        ammobox(autoKillBoxX, 0, autoKillBoxZ, ammoXWidth, ammoHeight, ammoZWidth, KILL_BOX_TYPE);
    
    // Draw a plot of land
    grass(-100, 0, -100, 200, 0, 200);
    
    
    // draw an enemy
    if (!enemyDead){
        enemy(enemyPosX, enemyPosY, enemyPosZ, enemyXWidth, enemyHeight, enemyZWidth, xpos, zpos);
    }
    
    
    
    // Draw a road
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, gravelTexture);
    
    glBegin(GL_QUADS);
    glColor3f(0.45, 0.45, 0.45);
    glNormal3f(-25, 0.1, 100);
    glTexCoord2f(0,0); glVertex3f(-1.2, 0.001, -3);
    glTexCoord2f(100,0); glVertex3f(1.5, 0.001, -3);
    glTexCoord2f(100,100); glVertex3f(1.5, 0.001, 100);
    glTexCoord2f(0,100); glVertex3f(-1.2, 0.001, 100);
    
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    
    //  Draw axes
    glColor3f(1,1,1);
    if (axes)
    {
        glBegin(GL_LINES);
        glVertex3d(0.0,0.0,0.0);
        glVertex3d(len,0.0,0.0);
        glVertex3d(0.0,0.0,0.0);
        glVertex3d(0.0,len,0.0);
        glVertex3d(0.0,0.0,0.0);
        glVertex3d(0.0,0.0,len);
        glEnd();
        //  Label axes
        glRasterPos3d(len,0.0,0.0);
        Print("X");
        glRasterPos3d(0.0,len,0.0);
        Print("Y");
        glRasterPos3d(0.0,0.0,len);
        Print("Z");
    }
    //  Display parameters
    glWindowPos2i(10,10);
    Print("HP: %.*s", playerHP, "| | | | | | | | | | | | | | | | | ");
    glWindowPos2i(10, 30);
    Print("Ammo: %d / %d", maxLasers - numLasers, totalAmmo);
    glWindowPos2i(10, windowHeight - 50);
    Print("Score: %d", score);
    glWindowPos2i(10, windowHeight - 30);
    Print("HIGH SCORE: %d", highScore);
    glWindowPos2i(windowWidth/2 - 50, windowHeight-140);
    Print("Hobo HP: %d", enemyHP);
    if (allowedToAutoKill) {
        glWindowPos2i(windowWidth/2 - 50, windowHeight-100);
        Print("Press G to AutoKill!");
    }
    if (showScore) {
        glWindowPos2i(windowWidth/2 - 50, windowHeight- 120);
        Print("+ %d", update_score);
    }
//    Print("numLasers: %d", numLasers);
//    glWindowPos2i(10, 90);
//    Print("maxLasers: %d", maxLasers);
//    glWindowPos2i(10, 110);
//    Print("totalAmmo: %d", totalAmmo);
//    glWindowPos2i(xpos, zpos);
//    Print("xpos: %d   zpos: %d", xpos, zpos);
//   glWindowPos2i((int)yrot%360, 70);
//  Print("yrot: %d", yrot);
//    glWindowPos2i(10, 70);
//    Print("+Z: %i, -Z: %i, +X: %i, -X: %i", walkingInPosZDirection, walkingInNegZDirection, walkingInPosXDirection, walkingInNegXDirection);
    //  Render the scene and make it visible
    glWindowPos2i(windowWidth/2, windowHeight/2);
    Print("O");
    glFlush();
    glutSwapBuffers();
}



/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
    //  Right arrow key - increase angle by 2 degrees
    if (key == GLUT_KEY_RIGHT) {
        if (cameraSelected)
            th -= 2;
    }
    //  Left arrow key - decrease angle by 2 degrees
    else if (key == GLUT_KEY_LEFT) {
        if (cameraSelected)
            th += 2;
    }
    
    //  Up arrow key - increase elevation by 2 degrees
    else if (key == GLUT_KEY_UP && ph <= 50 && cameraSelected)
        ph += 2;
    //  Down arrow key - decrease elevation by 2 degrees
    else if (key == GLUT_KEY_DOWN && ph >= -26 && cameraSelected)
        ph -= 2;
    //  PageUp key - increase dim
    else if (key == GLUT_KEY_PAGE_UP)
        dim += 0.1;
    //  PageDown key - decrease dim
    else if (key == GLUT_KEY_PAGE_DOWN && dim>1)
        dim -= 0.1;
    //  Keep angles to +/-360 degrees
    th %= 360;
    ph %= 360;
    //  Update projection
    Project();
    //  Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
}

static void updateHighScore() {
    if (score > highScore) {
        FILE *fp;
        fp=fopen("highscore.txt", "w+");
        if (fp == NULL) {
            perror("Error reading High Score");
        } else {
            fprintf(fp, "%d", score);
            fclose(fp);
        }
    }
    
}

static void readInHighScore() {
    FILE *fp;
    fp=fopen("highscore.txt", "r");
    if (fp == NULL) {
        perror("Error reading High Score");
    } else {
        fscanf(fp, "%d", &highScore);
        fclose(fp);
    }
}

void key_down(unsigned char ch, int x, int y)
{
    key_state[ch] = 1;
    
    // change elevation of light
    if (ch == 'e')
        ylight+=1;
    else if (ch == 'E')
        ylight-=1;
    
    //space bar to jump
    else if (ch == ' ')
        isJumping = 1;
    else if (ch == 'r' && totalAmmo > 0) {
        isReloading = 1;
    }
    else if (ch == '1') {
        enemyDead = 0;
        enemyHP = ENEMY_MAX_HP;
    }
    else if (ch == 'p')
        paused = 1-paused;
    
    else if (ch == 'g' && allowedToAutoKill) {
        update_score = 300;
        score += update_score;
        showScore = 1;
        
        enemyDead = 1;
        autoKillTimer = 0;
        allowedToAutoKill = 0;
    }
    
    else if (ch == 'c' && canThrowMelee) {
        meleeThrown = 1;
    }
    
    
}

void key_up(unsigned char ch, int x, int y)
{
    key_state[ch] = 0;
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void update_func()
{
    
    if (playerHP == 0) {
        updateHighScore();
        exit(0);
    }

    
    if ((int)yrot%360 > 270 && (int)yrot%360 < 360) {
        if (key_state['w']) {
            walkingInNegZDirection = 1;
            walkingInPosZDirection = 0;
            walkingInNegXDirection = 1;
            walkingInPosXDirection = 0;
        }
        else if (key_state['d']) {
            walkingInNegZDirection = 1;
            walkingInPosXDirection = 1;
            walkingInPosZDirection = 0;
            walkingInNegXDirection = 0;
        }
        else if (key_state['a']) {
            walkingInPosZDirection = 1;
            walkingInNegXDirection = 1;
            walkingInNegZDirection = 0;
            walkingInPosXDirection = 0;
        }
        else if (key_state['s']) {
            walkingInPosZDirection = 1;
            walkingInPosXDirection = 1;
            walkingInNegZDirection = 0;
            walkingInNegXDirection = 0;
        }
    }
    

    else if ((int)yrot%360 > 0 && (int)yrot%360 < 90) {
        if (key_state['w']) {
            walkingInNegZDirection = 1;
            walkingInPosXDirection = 1;
            walkingInPosZDirection = 0;
            walkingInNegXDirection = 0;
        }
        else if (key_state['d']) {
            walkingInPosZDirection = 1;
            walkingInPosXDirection = 1;
            walkingInNegZDirection = 0;
            walkingInNegXDirection = 0;
        }
        else if (key_state['a']) {
            walkingInNegZDirection = 1;
            walkingInNegXDirection = 1;
            walkingInPosZDirection = 0;
            walkingInPosXDirection = 0;
        }
        else if (key_state['s']) {
            walkingInPosZDirection = 1;
            walkingInNegXDirection = 1;
            walkingInNegZDirection = 0;
            walkingInPosXDirection = 0;
        }
    }
    
    else if ((int)yrot%360 > 90 && (int)yrot%360 < 180) {
        if (key_state['w']) {
            walkingInPosZDirection = 1;
            walkingInPosXDirection = 1;
            walkingInNegZDirection = 0;
            walkingInNegXDirection = 0;
        }
        else if (key_state['d']) {
            walkingInPosZDirection = 1;
            walkingInNegXDirection = 1;
            walkingInNegZDirection = 0;
            walkingInPosXDirection = 0;
        }
        else if (key_state['a']) {
            walkingInNegZDirection = 1;
            walkingInPosXDirection = 1;
            walkingInPosZDirection = 0;
            walkingInNegXDirection = 0;
        }
        else if (key_state['s']) {
            walkingInNegZDirection = 1;
            walkingInNegXDirection = 1;
            walkingInPosZDirection = 0;
            walkingInPosXDirection = 0;
        }
    }
    
    else if ((int)yrot%360 > 180 && (int)yrot%360 < 270) {
        if (key_state['w']) {
            walkingInPosZDirection = 1;
            walkingInNegXDirection = 1;
            walkingInNegZDirection = 0;
            walkingInPosXDirection = 0;
        }
        else if (key_state['d']) {
            walkingInNegZDirection = 1;
            walkingInNegXDirection = 1;
            walkingInPosZDirection = 0;
            walkingInPosXDirection = 0;
            
        }
        else if (key_state['a']) {
            walkingInPosZDirection = 1;
            walkingInPosXDirection = 1;
            walkingInNegZDirection = 0;
            walkingInNegXDirection = 0;
        }
        else if (key_state['s']) {
            walkingInNegZDirection = 1;
            walkingInPosXDirection = 1;
            walkingInPosZDirection = 0;
            walkingInNegXDirection = 0;
        }
    }
    
    else if ((int)yrot%360 == 0) {
        if (key_state['w'])
            walkingInNegZDirection = 1;
        else if (key_state['d'])
            walkingInPosXDirection = 1;
        else if (key_state['a'])
            walkingInNegXDirection = 1;
        else if (key_state['s'])
            walkingInPosZDirection = 1;
    }

    else if ((int)yrot%360 == 180) {
        if (key_state['w'])
            walkingInPosZDirection = 1;
        else if (key_state['d'])
            walkingInNegXDirection = 1;
        else if (key_state['a'])
            walkingInPosXDirection = 1;
        else if (key_state['s'])
            walkingInNegZDirection = 1;
    }

    else if ((int)yrot%360 == 90) {
        if (key_state['w'])
            walkingInPosXDirection = 1;
        else if (key_state['d'])
            walkingInPosZDirection = 1;
        else if (key_state['a'])
            walkingInNegZDirection = 1;
        else if (key_state['s'])
            walkingInNegXDirection = 1;
    }
    
    else if ((int)yrot%360 == 270) {
        if (key_state['w'])
            walkingInNegXDirection = 1;
        else if (key_state['d'])
            walkingInNegZDirection = 1;
        else if (key_state['a'])
            walkingInPosZDirection = 1;
        else if (key_state['s'])
            walkingInPosXDirection = 1;
    }

    for (int i = 0; i<numHouses; i++) {
        if (zpos < houseZ[i] + houseZWidth + 0.2 && zpos > houseZ[i] - houseZWidth - 0.2 &&
            xpos < houseX[i] + houseXWidth + 0.2 && xpos > houseX[i] - houseXWidth - 0.2) {
            
            if (walkingInNegZDirection && walkingInNegXDirection) {
                zpos += 0.1;
                xpos += 0.1;
            }
            
            else if (walkingInPosZDirection && walkingInNegXDirection) {
                zpos -= 0.1;
                xpos += 0.1;
            }
            
            else if (walkingInNegZDirection && walkingInPosXDirection) {
                zpos += 0.1;
                xpos -= 0.1;
            }
            
            else if (walkingInPosZDirection && walkingInPosXDirection) {
                zpos -= 0.1;
                xpos -= 0.1;
            }
        }
    }
    
    
    // set the mouse to the middle on load
    if (!initialized) {
        playerHP = 30;
        readInHighScore();
        glutWarpPointer(windowWidth/2+117, windowHeight/2-90);
        initialized = 1;
    }

    if (isJumping && jumpAngle < 3.1415) {
        jumpAngle += 0.1/jumpHeight;
        ypos = jumpHeight*sin(jumpAngle);
    } else {
        jumpAngle = 0;
        isJumping =0;
        ypos =0;
    }
    
    if (enemyDead) {
        update_score = 100;
        score += update_score;
        showScore = 1;
        
        enemyHP = ENEMY_MAX_HP;
        enemyDead = 0;
        srand(time(0));
        enemyPosX = rand()%100;
        enemyPosZ = rand()%100;
        if (rand()%2 == 1)
            enemyPosX = -enemyPosX;
        if (rand()%2 == 1)
            enemyPosZ = -enemyPosZ;
    }
    
    if (canThrowMelee && meleeThrown) {
        canThrowMelee = 0;
        meleeTimer = 0;
        
        if (fabs(xpos-enemyPosX) <= 6 && fabs(zpos - enemyPosZ) <= 6) {
            double posVec[3];
            posVec[0] = xpos - enemyPosX;
            posVec[1] = 0;
            posVec[2] = zpos - enemyPosZ;
            normalize(posVec);
            enemyPosX -= posVec[0]*12;
            enemyPosZ -= posVec[2]*12;
            enemyHP -= 10;
        }
        meleeThrown = 0;
    }
    
    if (!canThrowMelee) {
        meleeTimer += 1;
        if (meleeTimer > 60*MELEE_SECONDS) {
            canThrowMelee = 1;
        }
    }
    
    if (enemyTouchingPlayer) {
        double posVec[3];
        posVec[0] = xpos - enemyPosX;
        posVec[1] = 0;
        posVec[2] = zpos - enemyPosZ;
        normalize(posVec);
        enemyPosX -= posVec[0]*3;
        enemyPosZ -= posVec[2]*3;
        enemyTouchingPlayer = 0;
    }
    
    // update position of enemy
    if (!enemyDead) {
        double posVec[3];
        posVec[0] = xpos - enemyPosX;
        posVec[1] = 0;
        posVec[2] = zpos - enemyPosZ;
        normalize(posVec);
        enemyPosX += posVec[0]*0.15;
        enemyPosZ += posVec[2]*0.15;
    }
    
    // need to borrow lasers from ammo storage
    if (!allowedToShoot){
        shotTimer += 1;
        if (shotTimer > 60*SHOT_TIMER_SECS){
            shotTimer = 0;
            allowedToShoot = 1;
        }
    }
    
    if (!showAutoKillBox && !allowedToAutoKill) {
        autoKillTimer += 1;
        if (autoKillTimer > 60*AUTO_KILL_SECONDS) {
            showAutoKillBox = 1;
        }
    }
    
    // camera-autokillbox collision
    if (fabs(xpos-autoKillBoxX) < 2 && fabs(zpos-autoKillBoxZ) < 2) {
        update_score = 1000;
        score += update_score;
        showScore = 1;
        
        showAutoKillBox = 0;
        allowedToAutoKill = 1;
        
        autoKillBoxX = rand()%100;
        autoKillBoxZ = rand()%100;
        if (rand()%2 == 1)
            autoKillBoxX = -autoKillBoxX;
        if (rand()%2 == 1)
            autoKillBoxZ = -autoKillBoxZ;
    }
    
    
    if (isReloading) {
        reloadTimer += 1;
        
        // reload complete == figure out new values
        if (reloadTimer > 60*RELOAD_SECS) {
            if (totalAmmo > 25) {
                totalAmmo -= numLasers;
                numLasers = 0;
            }
            else {
                if ((maxLasers-numLasers+totalAmmo)<25) {
                    numLasers = maxLasers - (maxLasers - numLasers+totalAmmo);
                    totalAmmo = 0;
                }
                
                else {
                    totalAmmo -= numLasers;
                    numLasers = 0;
                }
            }
            
            needToReload = 0;
            reloadTimer = 0;
            isReloading = 0;
        }
    }
    
    if (numLasers == maxLasers){
        needToReload = 1;
    }
    
    //enemy-laser collision detection
    for (int i=0; i<numLasers; i++){
//        if (laserPosZ[i] >= enemyPosZ - enemyZWidth/2 && laserPosZ[i] <= enemyPosZ+enemyZWidth/2
//            && laserPosY[i]+0.4 >= enemyPosY && laserPosY[i]+0.4 <= enemyPosY+enemyHeight
//            && laserPosX[i] >= enemyPosX - enemyXWidth/2 && laserPosX[i] <= enemyPosX+enemyXWidth/2)
//        {
        if (fabs(laserPosZ[i] - enemyPosZ) < enemyZWidth &&
            fabs(laserPosX[i] - enemyPosX) < enemyXWidth &&
            fabs(laserPosY[i] - enemyPosY) < enemyHeight)
        {
            enemyHP -= 1;
        }
    }
    
    if (enemyHP <= 0) {
        enemyDead = 1;
    }
    
    
    // camera-ammo collision detection
    if (fabs(ammoX - xpos) < 2 && fabs(ammoZ - zpos) < 2) {
        update_score = 400;
        score += update_score;
        showScore = 1;
        
        if (totalAmmo + 50 < MAX_AMMO)
            totalAmmo += 25;
        else
            totalAmmo = MAX_AMMO;
        
        //calculate new spot for ammo
        ammoX = rand()%100;
        ammoZ = rand()%100;
//        ammoPresent = 0;
        if (rand()%2 == 1)
            ammoX = -ammoX;
        if (rand()%2 == 1)
            ammoZ = -ammoZ;
    }
    
    if (showScore) {
        showScoreTimer += 1;
        if (showScoreTimer > 60*SHOW_SCORE_SECONDS) {
            showScore = 0;
            showScoreTimer = 0;
        }
    }
    
    //enemy-camera collision detection
    //TODO: FIX ypos
    if (fabs(enemyPosX - xpos) < 4 && fabs(enemyPosZ - zpos) < 4){
        enemyTouchingPlayer = 1;
        playerHP -= 3;
    }
    
    // user created a new laser
    if (makeLaser && !needToReload && !isReloading && allowedToShoot ) {
        allowedToShoot = 0;
        laserMoving[numLasers] = 1;
        
        // where the laser starts
        laserStartX[numLasers] = xpos;
        laserStartZ[numLasers] = zpos;
        laserStartY[numLasers] = ypos;
        
        // where the lasers position is at start
        laserPosX[numLasers] = laserStartX[numLasers];
        laserPosZ[numLasers] = laserStartZ[numLasers];
        laserPosY[numLasers] = laserStartY[numLasers];
        
        // laser direction start
        laserDirectionX[numLasers] = xrot;
        laserDirectionY[numLasers] = yrot;
    
        numLasers++;
    }
    
    // update position of laser on screen if there is one
    for (int j=0; j<numLasers; j++){
        if (laserMoving[j]){
            float xrotrad, yrotrad;
            yrotrad = (laserDirectionY[j] / 180 * 3.141592654f);
            xrotrad = (laserDirectionX[j] / 180 * 3.141592654f);
            laserPosX[j] += (float)(sin(yrotrad)*1);
            laserPosZ[j] -= (float)(cos(yrotrad)*1);
            laserPosY[j] -= (float)(sin(xrotrad)*1);
        }
    }
    
    //  Elapsed time in seconds
    double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
    zh = fmod(90*t,360.0);
    
    //  Exit on ESC
    if (key_state[27]) {
        updateHighScore();
        exit(0);
    }
    
    //  Reset view angle
    else if (key_state['0'])
    {
        ph = 0;
        th = 0;
    }
    
    // ##################################
    // #####  FROM CAMERA TUTORIAL  #####
    // ##################################
    
    
    // Move forward
    else if (key_state['w'] || key_state['W'])
    {

        float xrotrad, yrotrad;
        yrotrad = (yrot / 180 * 3.141592654f);
        xrotrad = (xrot / 180 * 3.141592654f);
        xpos += (float)(sin(yrotrad)*0.1);
        zpos -= (float)(cos(yrotrad)*0.1);
        //ypos -= (float)(sin(xrotrad)*0.1); // uncomment for character flying ability
        
        
        if (walkbiasangle >= 359.0f)                 // Is walkbiasangle>=359?
            walkbiasangle = 0.0f;                   // Make walkbiasangle Equal 0
        else                                // Otherwise
            walkbiasangle+= 10;                    // If walkbiasangle < 359 Increase It By 10

        walkbias = (float)sin(walkbiasangle * 3.141592654f/180)/20.0f;     // Causes The Player To Bounce
    }
    
    // Move backward
    else if (key_state['s'] || key_state['S'])
    {
        float xrotrad, yrotrad;
        yrotrad = (yrot / 180 * 3.141592654f);
        xrotrad = (xrot / 180 * 3.141592654f);
        xpos -= (float)(sin(yrotrad)*0.1);
        zpos += (float)(cos(yrotrad)*0.1);
        //ypos += (float)(sin(xrotrad)*0.1); // uncomment for character flying ability

        if (walkbiasangle <= 1.0f)                   // Is walkbiasangle<=1?
            walkbiasangle = 359.0f;                 // Make walkbiasangle Equal 359
        else                                // Otherwise
            walkbiasangle-= 10;                 // If walkbiasangle > 1 Decrease It By 10
        walkbias = (float)sin(walkbiasangle * 3.141592654f/180)/20.0f;     // Causes The Player To Bounce
        
    }
    
    // Strafe right
    else if (key_state['d'] || key_state['D'])
    {
        float yrotrad;
        yrotrad = (yrot / 180 * 3.141592654f);
        xpos += (float)(cos(yrotrad)) * 0.1;
        zpos += (float)(sin(yrotrad)) * 0.1;
        
        if (walkbiasangle >= 359.0f)                 // Is walkbiasangle>=359?
            walkbiasangle = 0.0f;                   // Make walkbiasangle Equal 0
        else                                // Otherwise
            walkbiasangle+= 10;                    // If walkbiasangle < 359 Increase It By 10
        
        walkbias = (float)sin(walkbiasangle * 3.141592654f/180)/20.0f;     // Causes The Player To Bounce
        
    }
    
    //  Strafe left
    else if (key_state['a'] || key_state['A'])
    {
        float yrotrad;
        yrotrad = (yrot / 180 * 3.141592654f);
        xpos -= (float)(cos(yrotrad)) * 0.1;
        zpos -= (float)(sin(yrotrad)) * 0.1;

        if (walkbiasangle <= 1.0f)                   // Is walkbiasangle<=1?
            walkbiasangle = 359.0f;                 // Make walkbiasangle Equal 359
        else                                // Otherwise
            walkbiasangle-= 10;                 // If walkbiasangle > 1 Decrease It By 10
        walkbias = (float)sin(walkbiasangle * 3.141592654f/180)/20.0f;     // Causes The Player To Bounce
    }
    
    else if (key_state['g'])
    {
        distance += 1;
    }
    
    else if (key_state['b'])
    {
        distance -= 1;
    }
    
    
    // Change the size of the world
    else if (key_state['z'] || key_state['Z'])
        dim += 0.1;
    
    else if ((key_state['x'] || key_state['X']) && dim>1)
        dim -= 0.1;

    else if (key_state['l'] || key_state['L'])
        light = !light;
    
    else if (key_state['1']) {
        cameraSelected = !cameraSelected;
        lightSelected = !lightSelected;
    }

    //  Reproject
    Project();
    //  Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
}



/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
    //  Ratio of the width to the height of the window
    asp = (height>0) ? (double)width/height : 1;
    //  Set the viewport to the entire window
    glViewport(0,0, width,height);
    //  Set projection
    Project();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
    //  Initialize GLUT
    glutInit(&argc,argv);
    //  Request double buffered, true color window with Z buffering at 600x600
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(windowWidth,windowHeight);
    glutCreateWindow("Projections");
    //  Set callbacks
    glutDisplayFunc(display);
    
    glutReshapeFunc(reshape);
    glutSpecialFunc(special);
    
    glutKeyboardFunc(key_down);
    glutKeyboardUpFunc(key_up);
    
    glutSetKeyRepeat(GLUT_KEY_REPEAT_ON);
    glutPassiveMotionFunc(mouseMovementNoButtons);
    glutMotionFunc(mouseMovementWithButtons);
    glutMouseFunc(mousePressed);

    glutIdleFunc(update_func);
    
    glutSetCursor(GLUT_CURSOR_NONE);
    
    trunkTexture = LoadTexBMP("trunkTexture.bmp");
    leafTexture = LoadTexBMP("leafTexture.bmp");
    houseWallTexture = LoadTexBMP("houseWallTexture.bmp");
    houseRoofTexture = LoadTexBMP("houseRoofTexture.bmp");
    doorTexture = LoadTexBMP("doorTexture.bmp");
    houseRoofFrontTexture = LoadTexBMP("houseRoofFrontTexture.bmp");
    grassTexture = LoadTexBMP("grassTexture.bmp");
    gravelTexture = LoadTexBMP("gravelTexture.bmp");
    hoboTexture = LoadTexBMP("hoboTexture.bmp");
    crateTexture =LoadTexBMP("crateTexture.bmp");
    
    
    //  Pass control to GLUT so it can interact with the user
    if (!paused) {
        glutMainLoop();
        return 0;
    }
    
}
