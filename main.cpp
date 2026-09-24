#include<GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include<math.h>
#include<bits/stdc++.h>
using namespace std;
const int width = 700;
const int height = 700;

GLfloat eyeX = 28;
GLfloat eyeY =7;
GLfloat eyeZ = -38;

GLfloat centerX = 7;
GLfloat centerY = 6;
GLfloat centerZ = 25;
double angle = 0 ;
bool l_on1 = true;
bool l_on2 = true;
bool l_on3 = true;
bool l_on4 = false;
float rot = -8;
int stop=1;
float door_angle=.5;
float l_height =.5;
float spt_cutoff = 30;
unsigned int ID;
vector<int>v;

int mouseLastX = 0, mouseLastY = 0;
bool mouseLeftDown = false;
bool mouseRightDown = false;
float camYaw = 0.0f;
float camPitch = 0.0f;
float camZoom = 0.0f;

float sceneScale = 1.0f;

int   selectedObject = 0;
float objTransX[4] = {5,0,0,0};
float objTransY[4] = {-1,0,0,0};
float objTransZ[4] = {0,0,0,0};
float objRotY[4]   = {0,0,0,0};
float objScale[4]  = {1,1,1,1};

float globeAngle = 0.0f;

unsigned int woodTexID = 0;
unsigned int brickTexID = 0;

static void getNormal3p(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3)
{
    GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

    Ux = x2-x1;
    Uy = y2-y1;
    Uz = z2-z1;

    Vx = x3-x1;
    Vy = y3-y1;
    Vz = z3-z1;

    Nx = Uy*Vz - Uz*Vy;
    Ny = Uz*Vx - Ux*Vz;
    Nz = Ux*Vy - Uy*Vx;

    glNormal3f(Nx,Ny,Nz);
}

static GLfloat v_cube[8][3] =
{
    {0,0,0},
    {0,0,1},
    {0,1,0},
    {0,1,1},

    {1,0,0},
    {1,0,1},
    {1,1,0},
    {1,1,1}
};

static GLubyte c_ind[6][4] =
{
    {3,5,5,7},
    {6,4,0,2},
    {2,3,7,6},
    {1,0,4,5},
    {7,5,4,6},
    {2,0,1,3}
};

void cube(float R=0.5, float G=0.5, float B=0.5, int type=0, float val=1)
{
    GLfloat m_no[] = {0, 0, 0, 1.0};
    GLfloat m_amb[] = {R,G,B,1};
    GLfloat m_diff[] = {R,G,B,1};
    GLfloat m_spec[] = {0,0,0,0};
    GLfloat m_sh[] = {30};

    GLfloat m_em[] = {0,1,1,0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);
    if(type==1)
    {
        if(l_on1)
            glMaterialfv(GL_FRONT, GL_EMISSION, m_em);
        else
            glMaterialfv(GL_FRONT, GL_EMISSION, m_no);
    }
    else if(type==2)
    {
        if(l_on3)
            glMaterialfv(GL_FRONT, GL_EMISSION, m_em);
        else
            glMaterialfv(GL_FRONT, GL_EMISSION, m_no);
    }
    else
        glMaterialfv(GL_FRONT, GL_EMISSION, m_no);

    glBegin(GL_QUADS);
    for (GLint i = 0; i <6; i++)
    {
        getNormal3p(v_cube[c_ind[i][0]][0], v_cube[c_ind[i][0]][1], v_cube[c_ind[i][0]][2],
                    v_cube[c_ind[i][1]][0], v_cube[c_ind[i][1]][1], v_cube[c_ind[i][1]][2],
                    v_cube[c_ind[i][2]][0], v_cube[c_ind[i][2]][1], v_cube[c_ind[i][2]][2]);
        glTexCoord2f(0,val);
        glVertex3fv(&v_cube[c_ind[i][0]][0]);
        glTexCoord2f(0,0);
        glVertex3fv(&v_cube[c_ind[i][1]][0]);
        glTexCoord2f(val,0);
        glVertex3fv(&v_cube[c_ind[i][2]][0]);
        glTexCoord2f(val,val);
        glVertex3fv(&v_cube[c_ind[i][3]][0]);
    }
    glEnd();
}

class BmpLoader
{
public:
    unsigned char* textureData;
    int iWidth, iHeight;

    BmpLoader(const char*);
    ~BmpLoader();

private:
    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
};

BmpLoader::BmpLoader(const char* filename)
{
    FILE *file=0;
    file=fopen(filename, "rb");
    if(!file)
    {
        cout<<"File not found: "<<filename<<" (using a blank fallback texture)"<<endl;
        iWidth = 2; iHeight = 2;
        textureData = new unsigned char[iWidth*iHeight*3];
        memset(textureData, 200, iWidth*iHeight*3);
        return;
    }
    fread(&bfh, sizeof(BITMAPFILEHEADER),1,file);
    if(bfh.bfType != 0x4D42)
        cout<<"Not a valid bitmap"<<endl;
    fread(&bih, sizeof(BITMAPINFOHEADER),1,file);
    if(bih.biSizeImage==0)
        bih.biSizeImage=bih.biHeight*bih.biWidth*3;
    textureData = new unsigned char[bih.biSizeImage];
    fseek(file, bfh.bfOffBits, SEEK_SET);
    fread(textureData, 1, bih.biSizeImage, file);
    unsigned char temp;
    for(int i=0; i<bih.biSizeImage; i+=3)
    {
        temp = textureData[i];
        textureData[i] = textureData[i+2];
        textureData[i+2] = temp;
    }

    iWidth = bih.biWidth;
    iHeight = bih.biHeight;
    fclose(file);
}

BmpLoader::~BmpLoader()
{
    delete [] textureData;
}

void LoadTexture(const char*filename)
{
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    BmpLoader bl(filename);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, bl.iWidth, bl.iHeight, GL_RGB, GL_UNSIGNED_BYTE, bl.textureData );
}

unsigned int makeProceduralTexture(bool wood)
{
    const int SZ = 64;
    unsigned char img[SZ][SZ][3];

    for (int y = 0; y < SZ; y++)
    {
        for (int x = 0; x < SZ; x++)
        {
            if (wood)
            {
                float stripe = 0.5f + 0.5f * sinf((x + y * 0.35f) * 0.35f);
                unsigned char r = (unsigned char)(90 + 60 * stripe);
                unsigned char g = (unsigned char)(55 + 35 * stripe);
                unsigned char b = (unsigned char)(25 + 15 * stripe);
                int noise = (rand() % 10) - 5;
                img[y][x][0] = (unsigned char)max(0, min(255, r + noise));
                img[y][x][1] = (unsigned char)max(0, min(255, g + noise));
                img[y][x][2] = (unsigned char)max(0, min(255, b + noise));
            }
            else
            {
                int brickH = 8, brickW = 16;
                int row = y / brickH;
                int offset = (row % 2 == 0) ? 0 : brickW / 2;
                int col = (x + offset) % brickW;
                bool mortar = (y % brickH == 0) || (col == 0);
                if (mortar)
                {
                    img[y][x][0] = 190;
                    img[y][x][1] = 190;
                    img[y][x][2] = 185;
                }
                else
                {
                    int noise = (rand() % 20) - 10;
                    img[y][x][0] = (unsigned char)max(0, min(255, 150 + noise));
                    img[y][x][1] = (unsigned char)max(0, min(255, 60 + noise));
                    img[y][x][2] = (unsigned char)max(0, min(255, 45 + noise));
                }
            }
        }
    }

    unsigned int texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, SZ, SZ, GL_RGB, GL_UNSIGNED_BYTE, img);
    return texId;
}

static void res(int width, int height)
{
    glViewport(0,0,width,height);
}

void axes()
{
    float length = 11;
    float width = 0.3;

    glPushMatrix();
    glTranslatef(length/4,0,0);
    glScalef(length,width,width);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(0.8,0.1,0.1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,length/4,0);
    glScalef(width,length,width);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(0.1,0.8,0.1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,0,length/4);
    glScalef(width,width,length);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(0.1,0.1,0.8);
    glPopMatrix();
}

void cse()
{
    glPushMatrix();
    glScalef(.3,.5,1);
    glTranslatef(65,20,0);
    glPushMatrix();

    glScalef(.4,1.5,2.9);

    glTranslatef(45,3,2);
    float length=3.4;
    float width=.4;

    float a=1,b=0,c=.5;
    glPushMatrix();
    glTranslatef(length/2,1,1);;
    glScalef(3*length,width,width);
    glTranslatef(-.75,-0.5,-0.5);
    cube(a,b,c);
    glPopMatrix();

    glPushMatrix();
    glPushMatrix();
    glTranslatef(1,length/2,1);;
    glScalef(width+3,length+.5,width);
    glTranslatef(0,-0.2,-0.5);
    cube(a,b,c);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(length/2,1,1);;
    glScalef(length*3,width,width);
    glTranslatef(-.75,8.7,-0.5);
    cube(a,b,c);
    glPopMatrix();

    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glScalef(.4,1.5,2.9);
    glTranslatef(15,3,2);

    glPushMatrix();
    glTranslatef(length/2,1,1);;
    glScalef(3*length,width,width);
    glTranslatef(-.75,-0.5,-0.5);
    cube(a,b,c);
    glPopMatrix();

    glPushMatrix();
    glPushMatrix();
    glTranslatef(1,length/2+.1,1);;
    glScalef(width+3,length,width);
    glTranslatef(0,-0.2,-0.5);
    cube(a,b,c);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(length/2,1,1);;
    glScalef(length*3,width,width);
    glTranslatef(-.75,8.7,-0.5);
    cube(a,b,c);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(length,1,1);;
    glScalef(3*length,width,width);
    glTranslatef(-.94,4,-0.5);
    cube(a,b,c);
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();

    glScalef(.4,1.5,2.9);
    glTranslatef(30,3,2);

    glPushMatrix();
    glTranslatef(length/2,1,1);;
    glScalef(3*length,width,width);
    glTranslatef(-.75,-0.5,-0.5);
    cube(a,b,c);
    glPopMatrix();

    glPushMatrix();
    glPushMatrix();
    glTranslatef(1,length/2,1);;
    glScalef(width+3,length/2,width);
    glTranslatef(0,0.7,-0.5);
    cube(a,b,c);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1,length/2,1);;
    glScalef(width+3,length/2+.1,width);
    glTranslatef(-2,-0.5,-0.5);
    cube(a,b,c);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(length/2,1,1);;
    glScalef(length*3,width,width);
    glTranslatef(-.75,8.7,-0.5);
    cube(a,b,c);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(length,1,1);;
    glScalef(3*length,width,width);
    glTranslatef(-.9,4,-0.5);
    cube(a,b,c);
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
}

void flr()
{
    glPushMatrix();
    glTranslatef(-10.0f, -0.999f, -35.0f);
    glScalef(46.0f, 0.5f, 46.0f);
    cube(0.7f, 0.7f, 0.7f);
    glPopMatrix();
}

void wall1()
{
    glPushMatrix();
    glScalef(1,20,55);
    glTranslatef(-19,0,-0.5);
    cube(0.8f, 0.8f, 0.8f);
    glPopMatrix();
}

void wall3()
{
    glPushMatrix();
    glScalef(46,20,1);
    glTranslatef(-.44,0,8);
    cube(0.8f, 0.8f, 0.8f);
    glPopMatrix();
}

void door()
{
    glPushMatrix();
    glScalef(35,20,1);
    glTranslatef(-.542,0,-28);
    cube(0.741, 0.718, 0.420);
    glPopMatrix();
}

void board()
{
    glPushMatrix();
    glScalef(17,10,.8);
    glTranslatef(-.6,.4,9.1);
    cube(1,1,1);
    glPopMatrix();

    glPushMatrix();
    glScalef(17.5,11,.8);
    glTranslatef(-.601,.32,9.4);
    cube(0,0,0);
    glPopMatrix();
}

void window()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, woodTexID);

    glPushMatrix();
    glScalef(.6,.6,12.4);
    glTranslatef(23,13,-1.55);
    cube(.6,.4,.2,0,3);
    glPopMatrix();

    glPushMatrix();
    glScalef(.6,.6,12.4);
    glTranslatef(23,1,-1.55);
    cube(.6,.4,.2,0,3);
    glPopMatrix();

    glPushMatrix();
    glScalef(.6,8,.6);
    glTranslatef(23,.6,-1.6);
    cube(.6,.4,.2,0,2);
    glPopMatrix();

    glPushMatrix();
    glScalef(.6,8,.6);
    glTranslatef(23,.6,9.8);
    cube(.6,.4,.2,0,2);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    glPushMatrix();
    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glColor4f(0.75f, 0.9f, 1.0f, 0.35f);
    glBegin(GL_QUADS);
    glVertex3f(23.4f, 1.0f, -1.0f);
    glVertex3f(23.4f, 1.0f, 10.0f);
    glVertex3f(23.4f, 8.5f, 10.0f);
    glVertex3f(23.4f, 8.5f, -1.0f);
    glEnd();
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void headwall()
{
    glPushMatrix();
    glScalef(56,1,59);
    glTranslatef(-0.5,19,-0.5);
    cube(0.8f, 0.8f, 0.8f);
    glPopMatrix();
}

float k=0;
float angle1=0;
bool markdoor=0;
double window_val=0;
void spin()
{
    globeAngle += 0.6f;
    if (globeAngle >= 360.0f) globeAngle -= 360.0f;

    angle = (angle+k)*stop;
    k+=.001;
    if (angle >= 360)
        angle = 0;
    angle1+=.2;
    if (angle1 >= 360)
        angle1 = 0;
    bool ok=0;
    bool ok1=0;
    if(markdoor&&door_angle>-.8)
    {
        door_angle-=.001,ok=1;
        if(door_angle<.4)
            window_val+=.00085;
        window_val=min( window_val,1.00);
        l_on4=1;
    }
    if(!markdoor&&door_angle<.5)
    {
        door_angle+=.001,ok=1;
        window_val-=.001;
        window_val=max(window_val,0.00);
    }
    if(!ok)
        l_on4=markdoor;
}

double a=2;
double b=1;
double c= 10;
void fan()
{
    float length = 10;
    float width = 0.3;
    glPushMatrix();
    glTranslatef(1,length/2,-4);
    glScalef(width,length,width);
    glTranslatef(-20,1,20);
    cube(0.4,0.4,1);
    glPopMatrix();

    glPushMatrix();
    glScalef(1,.1,1);
    glTranslatef(-5,150,2);
    cube(1, 1, 1);
    glRotatef(angle, 0, 0, 1);
    glRecti(-a, -a, a, a);
    cube(.5, 0, .5);
    glRecti(-b, a, b, c);
    glRecti(-c, -b, -a, b);
    glRecti(-b, -c, b, -a);
    glRecti(a, -b, c, b);
    glPopMatrix();
}

void Chair12()
{
    float height=2.5;
    float width=4;
    float length=2;

    float base_height=.5;
    float leg_height=height-base_height;
    float leg_width=.4;

    glPushMatrix();
    glTranslatef(0,leg_height,0);

    glPushMatrix();
    glScalef(width,base_height,length);
    glTranslatef(-0.5,0,-0.5);
    cube(.4f, .5f, .3f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef((width/2-leg_width/2),0,(length/2-leg_width/2));
    glScalef(leg_width,leg_height,leg_width);
    glTranslatef(-0.5,-1,-0.5);
    cube(0.4f, 0.302f, 0.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef((width/2-leg_width/2),0,-(length/2-leg_width/2));
    glScalef(leg_width,leg_height,leg_width);
    glTranslatef(-0.5,-1,-0.5);
    cube(0.4f, 0.302f, 0.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-(width/2-leg_width/2),0,(length/2-leg_width/2));
    glScalef(leg_width,leg_height,leg_width);
    glTranslatef(-0.5,-1,-0.5);
    cube(0.4f, 0.302f, 0.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-(width/2-leg_width/2),0,-(length/2-leg_width/2));
    glScalef(leg_width,leg_height,leg_width);
    glTranslatef(-0.5,-1,-0.5);
    cube(0.4f, 0.302f, 0.0f);
    glPopMatrix();

    glPushMatrix();
    glScalef(3,2,.5);
    glTranslatef(-0.5,.5,-1);
    cube(.7f, .5f, .3f);
    glPopMatrix();

    glPushMatrix();
    glScalef(.2,1,.2);
    glTranslatef(3,0,-2);
    cube(0,0,0);
    glPopMatrix();

    glPushMatrix();
    glScalef(.2,1,.2);
    glTranslatef(-4,0,-2);
    cube(0,0,0);
    glPopMatrix();

    glPopMatrix();
}

void Table12(float height=2.4, float width=4,float length=2,float base_height=.5,float leg_width=.2)
{
    float leg_height=height-base_height;

    glPushMatrix();
    glTranslatef(0,leg_height,0);

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, woodTexID);
    glScalef(width,base_height,length);
    glTranslatef(-0.5,0,-0.5);
    cube(.6,.3,.3,0,4);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    glPushMatrix();
    glTranslatef((width/2-leg_width/2),0,(length/2-leg_width/2));
    glScalef(leg_width,leg_height,leg_width);
    glTranslatef(-0.5,-1,-0.5);
    cube(.8f, .6f, 0.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef((width/2-leg_width/2),0,-(length/2-leg_width/2));
    glScalef(leg_width,leg_height,leg_width);
    glTranslatef(-0.5,-1,-0.5);
    cube(.8f, .6f, 0.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-(width/2-leg_width/2),0,(length/2-leg_width/2));
    glScalef(leg_width,leg_height,leg_width);
    glTranslatef(-0.5,-1,-0.5);
    cube(.8f, .6f, 0.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-(width/2-leg_width/2),0,-(length/2-leg_width/2));
    glScalef(leg_width,leg_height,leg_width);
    glTranslatef(-0.5,-1,-0.5);
    cube(.8f, .6f, 0.0f);
    glPopMatrix();
    glPopMatrix();
}

void test()
{
    glPushMatrix();
    glTranslatef(10.0, 1, -14+9 * 2.0 + 2.2f);
    glScalef(0.8f, 0.8f, 0.8f);
    glRotatef(180.0, 0.0, 1.0, 0.0);
    Chair12();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(10.0, 1, -16+9 * 2.0 + 2.2f);
    glScalef(1.7f, 1.3f, 1.0f);
    Table12(2,4,2,.6,.3);
    glPopMatrix();

    for (int i = -6; i <= 8; i+=2)
    {
        for (int j = -4; j <= 4; j+=2)
        {
            glPushMatrix();
            glTranslatef(i*2.0, 0.6f, -16+j * 2.0 + 2.2f);
            glScalef(0.5f, 0.5f, 0.5f);
            glRotatef(0.0, 0.0, 1.0, 0.0);
            Chair12();
            glPopMatrix();
        }
    }

    for (int i = -6; i <= 8; i+=2)
    {
        for (int j = -4; j <= 4; j+=2)
        {
            glPushMatrix();
            glTranslatef(i*2.0 + 0.3f, 1.2f,-14.2+ j * 2.0 + 1.2f);
            glScalef(0.75f, 0.7f, 0.6f);
            Table12();
            glPopMatrix();
        }
    }
}

void table()
{
    float height=.5;
    float width=42.5;
    float length=7;

    float base_height=.5;
    float leg_height=height-base_height;
    float leg_width=1;

    glPushMatrix();
    glTranslatef(2.7,0,5);
    glPushMatrix();
    glTranslatef(0,leg_height,0);

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, woodTexID);
    glScalef(width,base_height,length);
    glTranslatef(-0.5,0,-0.5);
    cube(0.4,0.6,0.6,0,6);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    glPushMatrix();
    glTranslatef((width/2-leg_width/2),0,(length/2-leg_width/2));
    glScalef(leg_width,leg_height,leg_width);
    glTranslatef(-0.5,-1,-0.5);
    cube(0.53,0.39,0.28);
    glPopMatrix();

    glPushMatrix();
    glTranslatef((width/2-leg_width/2),0,-(length/2-leg_width/2));
    glScalef(leg_width,leg_height,leg_width);
    glTranslatef(-0.5,-1,-0.5);
    cube(0.53,0.39,0.28);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-(width/2-leg_width/2),0,(length/2-leg_width/2));
    glScalef(leg_width,leg_height,leg_width);
    glTranslatef(-0.5,-1,-0.5);
    cube(0.53,0.39,0.28);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-(width/2-leg_width/2),0,-(length/2-leg_width/2));
    glScalef(leg_width,leg_height,leg_width);
    glTranslatef(-0.5,-1,-0.5);
    cube(0.53,0.39,0.28);
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();
}

void drop()
{
    glPushMatrix();
    cube(113/255.0,74/255.0,44/255.0);
    glBegin(GL_POLYGON);
    glVertex3f (460, 660, 0);
    glVertex3f (470, 670, 0);
    glVertex3f (330, 670,0);
    glVertex3f (340, 660, 0);
    glVertex3f (340, 320, 0);
    glVertex3f (460, 320, 0);
    glEnd();

    glPopMatrix();

    glPushMatrix();

    glTranslatef(0,0,0);
    glScalef(105,95,10);

    glPushMatrix();
    glTranslatef(3.32,4.6,-.49);
    cube(62/255.0,43/255.0,45/255.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(3.32,5.7,-.49);
    cube(62/255.0,43/255.0,45/255.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(3.32,3.5,-.49);
    cube(62/255.0,43/255.0,45/255.0);
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1,0,0);
    glScalef(20,10,10);
    glPushMatrix();
    glTranslatef(19.5,38,-.5);
    cube(1,1,1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(19.5,58,-.5);
    cube(1,1,1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(19.5,48,-.5);
    cube(1,1,1);
    glPopMatrix();

    glPopMatrix();

    cube(62/255.0,43/255.0,35/255.0);
    glColor3ub (62,43,35);
    glBegin(GL_POLYGON);
    glVertex3f (470, 670, 0);
    glVertex3f (500, 667, 0);
    glVertex3f (492, 657, 0);
    glVertex3f (492, 330, 0);
    glVertex3f (460, 320, 0);
    glVertex3f (460, 660, 0);
    glEnd();

    glPopMatrix();
}

void weardrop()
{
    glPushMatrix();
    glTranslatef(-8,-7,7.5);
    glScalef(-.02,.021,.4);
    drop();
    glPopMatrix();
}

void fan1()
{
    glPushMatrix();
    glTranslatef(6,-.85,-10);
    cube(1,1,1);
    fan();
    glPopMatrix();
}

void movableProp()
{
    glPushMatrix();
    glTranslatef(objTransX[0], objTransY[0], objTransZ[0]);
    glRotatef(objRotY[0], 0, 1, 0);
    glScalef(objScale[0], objScale[0], objScale[0]);

    glTranslatef(-0.5f, 0.0f, -0.5f);
    cube(0.15f, 0.55f, 0.2f);
    glPushMatrix();
    glTranslatef(0.5f, 1.0f, 0.5f);
    GLfloat m_amb[]  = {0.05f, 0.3f, 0.05f, 1};
    GLfloat m_diff[] = {0.1f, 0.55f, 0.1f, 1};
    GLfloat m_spec[] = {0.2f, 0.2f, 0.2f, 1};
    GLfloat m_sh[]   = {16};
    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);
    glutSolidSphere(0.6, 16, 16);
    glPopMatrix();

    glPopMatrix();
}

void spinningGlobe()
{
    glPushMatrix();
    glTranslatef(11.5f, 2.6f, -14.5f);
    glRotatef(globeAngle, 0, 1, 0);
    glRotatef(23.4f, 0, 0, 1);

    GLfloat m_amb[]  = {0.05f, 0.15f, 0.35f, 1};
    GLfloat m_diff[] = {0.15f, 0.35f, 0.75f, 1};
    GLfloat m_spec[] = {0.6f, 0.6f, 0.6f, 1};
    GLfloat m_sh[]   = {64};
    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);
    glutSolidSphere(0.4, 24, 24);

    glPopMatrix();
    glPushMatrix();
    glTranslatef(11.5f, 2.2f, -14.5f);
    glScalef(0.08f, 0.4f, 0.08f);
    glTranslatef(-0.5f, -1.0f, -0.5f);
    cube(0.2f, 0.2f, 0.2f);
    glPopMatrix();
}

float al1,dl1,sl1;
float al2,dl2,sl2;
float al3,dl3,sl3;

void light1(float a,float b,float c)
{
    glEnable(GL_LIGHT0);

    GLfloat l_no[] = {.9, 0, .9, 1.0};
    GLfloat l_amb[] = {0.4+al1, 0.4+al1, 0.4+al1, 1.0};
    GLfloat l_dif[] = {.4+dl1,.4+dl1,.4+dl1,1};
    GLfloat l_spec[] = {.4+sl1,.4+sl1,.4+sl1,1};
    GLfloat l_pos[] = {a,b,c,1.0};

    if(l_on1)
        glLightfv(GL_LIGHT0, GL_AMBIENT, l_amb);
    else
        glLightfv(GL_LIGHT0, GL_AMBIENT, l_no);
    if(l_on1)
        glLightfv(GL_LIGHT0, GL_DIFFUSE, l_dif);
    else
        glLightfv(GL_LIGHT0, GL_DIFFUSE, l_no);
    if(l_on1)
        glLightfv(GL_LIGHT0, GL_SPECULAR, l_spec);
    else
        glLightfv(GL_LIGHT0, GL_SPECULAR, l_no);

    glLightfv(GL_LIGHT0, GL_POSITION, l_pos);
}

void light2(float a,float b,float c)
{
    glEnable(GL_LIGHT2);

    GLfloat l_no[] = {0, 0, 0, 1.0};
    GLfloat l_amb[] = {0.3+al2, 0.3+al2, 0.3+al2, 1.0};
    GLfloat l_dif[] = {.6+dl2,.6+dl2,.6+dl2,1};
    GLfloat l_spec[] = {5+sl2,5+sl2,5+sl2,1};
    GLfloat l_pos[] = {a,b,c,1.0};

    if(l_on3)
        glLightfv(GL_LIGHT2, GL_AMBIENT, l_amb);
    else
        glLightfv(GL_LIGHT2, GL_AMBIENT, l_no);
    if(l_on3)
        glLightfv(GL_LIGHT2, GL_DIFFUSE, l_dif);
    else
        glLightfv(GL_LIGHT2, GL_DIFFUSE, l_no);
    if(l_on3)
        glLightfv(GL_LIGHT2, GL_SPECULAR, l_spec);
    else
        glLightfv(GL_LIGHT2, GL_SPECULAR, l_no);

    glLightfv(GL_LIGHT2, GL_POSITION, l_pos);
}

void window_light(float a,float b,float c)
{
    glEnable(GL_LIGHT3);

    GLfloat l_no[] = {0, 0, 0, 1.0};
    GLfloat l_amb[] = {.5* window_val, .5* window_val, .5* window_val, 1.0};
    GLfloat l_dif[] = {1* window_val,1* window_val,1* window_val,1};
    GLfloat l_spec[] = {1* window_val,1* window_val,1* window_val,1};
    GLfloat l_pos[] = {a,b,c,1.0};

    if(l_on4)
        glLightfv(GL_LIGHT3, GL_AMBIENT, l_amb);
    else
        glLightfv(GL_LIGHT3, GL_AMBIENT, l_no);
    if(l_on4)
        glLightfv(GL_LIGHT3, GL_DIFFUSE, l_dif);
    else
        glLightfv(GL_LIGHT3, GL_DIFFUSE, l_no);
    if(l_on4)
        glLightfv(GL_LIGHT3, GL_SPECULAR, l_spec);
    else
        glLightfv(GL_LIGHT3, GL_SPECULAR, l_no);

    glLightfv(GL_LIGHT3, GL_POSITION, l_pos);
    GLfloat l_spt[] = {0,-1,0,1};
    GLfloat spt_ct[] = {84};
    glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, l_spt);
    glLightfv(GL_LIGHT3, GL_SPOT_CUTOFF, spt_ct);
}

void light()
{
    glPushMatrix();
    glPushMatrix();

    glRotatef(200, 0,1,0);
    float a=-5,b=16,c=9;
    light1(a,b,c);
    glPopMatrix();
    glTranslatef(a,b+1,c);
    glScalef(15,1,1);
    glTranslatef(-0.08,-0.85,-8);
    cube(1,1,1,1);
    glPopMatrix();

    glPushMatrix();
    glPushMatrix();

    glRotatef(200, 0,1,0);
    a=15,b=17,c=-15;
    light2(a,b,c);
    glPopMatrix();
    glTranslatef(4,b+1,c);
    glScalef(15,1,1);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(1,1,1,1);
    glPopMatrix();
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-3, 3, -2, 3, 2.0, 100.0);
    gluPerspective(0,0,0,0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyeX,eyeY,eyeZ, centerX,centerY,centerZ, 0,1,0);

    glTranslatef(0, 0, camZoom);
    glRotatef(camPitch, 1, 0, 0);
    glRotatef(camYaw,   0, 1, 0);

    glScalef(sceneScale, sceneScale, sceneScale);

    glRotatef(rot, .2,.5,.5);

    table();
    flr();
    wall1();
    wall3();
    door();
    board();
    headwall();

    test();
    window();

    weardrop();
    fan1();
    spinningGlobe();
    movableProp();

    light();

    glutSwapBuffers();
}

static void key(unsigned char key, int x, int y)
{
    switch (key)
    {

    case 27:
        exit(0);
        break;
    case '+':
        eyeZ++;
        break;
    case '-':
        eyeZ--;
        break;

    case 'a':
        eyeX++;
        break;
    case 's':
        eyeX--;
        break;

    case 'g':
        eyeY++;
        break;
    case 'h':
        eyeY--;
        break;

    case 'w':
        centerY++;
        break;
    case 'e':
        centerY--;
        break;

    case 'o':
        centerX++;
        break;
    case 'p':
        centerX--;
        break;

    case 'k':
        centerZ++;
        break;
    case 'l':
        centerZ--;
        break;

    case ',':
        rot++;
        break;
    case '.':
        rot--;
        break;

    case '*':
        stop=0;
        break;

    case ';':
        angle=0,stop=1,k=0;
        break;

    case '1':
        l_height+=.1;
        break;
    case '2':
        l_height-=.1;
        break;

    case '3':
        spt_cutoff++;
        break;
    case '4':
        spt_cutoff--;
        break;
    case 't':
        l_on1=1-l_on1;
        break;

    case 'u':
        l_on2=1-l_on2;
        break;

    case 'y':
        l_on3=1-l_on3;
        break;
    case 'd':
        markdoor=1-markdoor;
        break;

    case '[':
        sceneScale -= 0.05f;
        if (sceneScale < 0.3f) sceneScale = 0.3f;
        break;
    case ']':
        sceneScale += 0.05f;
        if (sceneScale > 2.5f) sceneScale = 2.5f;
        break;

    case 'z':
        objRotY[selectedObject] -= 5.0f;
        break;
    case 'x':
        objRotY[selectedObject] += 5.0f;
        break;
    case 'c':
        objScale[selectedObject] -= 0.1f;
        if (objScale[selectedObject] < 0.2f) objScale[selectedObject] = 0.2f;
        break;
    case 'v':
        objScale[selectedObject] += 0.1f;
        if (objScale[selectedObject] > 3.0f) objScale[selectedObject] = 3.0f;
        break;

    case 'r':
        camYaw = 0; camPitch = 0; camZoom = 0; sceneScale = 1.0f;
        break;

    }
    glutPostRedisplay();
}

static void specialKey(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        objTransX[selectedObject] -= 0.3f;
        break;
    case GLUT_KEY_RIGHT:
        objTransX[selectedObject] += 0.3f;
        break;
    case GLUT_KEY_UP:
        objTransZ[selectedObject] -= 0.3f;
        break;
    case GLUT_KEY_DOWN:
        objTransZ[selectedObject] += 0.3f;
        break;
    case GLUT_KEY_PAGE_UP:
        objTransY[selectedObject] += 0.3f;
        break;
    case GLUT_KEY_PAGE_DOWN:
        objTransY[selectedObject] -= 0.3f;
        break;
    }
    glutPostRedisplay();
}

static void mouseButton(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
        mouseLeftDown = (state == GLUT_DOWN);
    else if (button == GLUT_RIGHT_BUTTON)
        mouseRightDown = (state == GLUT_DOWN);

    mouseLastX = x;
    mouseLastY = y;
}

static void mouseMotion(int x, int y)
{
    int dx = x - mouseLastX;
    int dy = y - mouseLastY;

    if (mouseLeftDown)
    {
        camYaw   += dx * 0.4f;
        camPitch += dy * 0.4f;
    }
    if (mouseRightDown)
    {
        camZoom += dy * 0.1f;
    }

    mouseLastX = x;
    mouseLastY = y;
    glutPostRedisplay();
}

static void idle(void)
{
    glutPostRedisplay();
    spin();
}

void texture_image()
{
    LoadTexture("C:\\Users\\User\\OneDrive\\Desktop\\New folder\\111.bmp");
    v.push_back(ID);
    LoadTexture("C:\\Users\\User\\OneDrive\\Desktop\\New folder\\333.bmp");
    v.push_back(ID);
    LoadTexture("C:\\Users\\User\\OneDrive\\Desktop\\New folder\\333.bmp");
    v.push_back(ID);
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(width,height);
    glutInitWindowPosition(50,50);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Tonmoy_1361");
    glutReshapeFunc(res);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(specialKey);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutIdleFunc(idle);

    glEnable(GL_DEPTH_TEST);
    glShadeModel( GL_SMOOTH );
    glEnable(GL_NORMALIZE);
    glEnable(GL_BLEND);

    glEnable(GL_LIGHTING);
    GLfloat globalAmbient[] = {0.0,0.0,0.0,1.0};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,globalAmbient);
    texture_image();
    woodTexID  = makeProceduralTexture(true);
    brickTexID = makeProceduralTexture(false);
    int t=1;

    printf("%d. Press ESC key for exit the main programme\n",t++);
    printf("%d. Mouse: LEFT-drag orbits the camera, RIGHT-drag zooms (viewing transform)\n",t++);
    printf("%d. [ / ] : scale the whole scene down/up  |  'r' resets camera+scale\n",t++);
    printf("%d. Arrow keys / Page Up / Page Down : move the plant prop (object transform)\n",t++);
    printf("%d. 'z' / 'x' : rotate the plant prop   |  'c' / 'v' : scale the plant prop\n",t++);
    printf("%d. 'd' opens/closes the door (reveals window light) | 't','u','y' toggle lights\n",t++);

    glutMainLoop();

    return EXIT_SUCCESS;
}
