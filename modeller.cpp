#include <stdio.h>
#include <stdlib.h>
#include "object.h" //object class
#include <math.h> //for sin and cos (for camera movement)
#include <vector>  // used to list objcts
#include <iostream> //following two for files
#include <fstream>
#include <string>
#include "plane.h" //planes for intersection tests

//using std::vector;
using namespace std;

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

// Objects on the screen
vector<Object> objectList;
vector<Object>::iterator draw;

// Lighting specifications
float lightPos1[4] = {-40, 20, 0, 1};
float lightPos2[4] = {40, 20, 0, 1};

float amb1[4] = {0.4, 0.4, 0.4, 1};
float amb2[4] = {0.4, 0.4, 0.4, 1};
float dif1[4] = {0.6, 0.6, 0.6, 1};
float dif2[4] = {0.6, 0.6, 0.6, 1};
float spc1[4] = {0.5, 0.5, 0.5, 1};
float spc2[4] = {0.5, 0.5, 0.5, 1};

int lightcount1 = 0;
int lightcount2 = 0;
bool turnOnLight = false;
bool light1 = false;
bool light2 = false;

// Material Properties (emerald, jade, pearl, gold, cyan plastic)
int matCount = 0;
int mat;

float amb_emerald[4] = {0.0215f, 0.1745f, 0.0215f, 1};
float dif_emerald[4] = {0.07568, 0.61424, 0.07568, 1};
float spc_emerald[4] = {0.633, 0.7278, 0.633, 1};

float amb_jade[4] = {0.135, 0.2225, 0.1575, 1};
float dif_jade[4] = {0.54, 0.89, 0.63, 1};
float spc_jade[4] = {0.316, 0.316, 0.316, 1};

float amb_pearl[4] = {0.25, 0.207, 0.207, 1};
float dif_pearl[4] = {1, 0.829, 0.829, 1};
float spc_pearl[4] = {0.297, 0.297, 0.297, 1};

float amb_gold[4] = {0.247, 0.199, 0.0745};
float dif_gold[4] = {0.752, 0.606, 0.226, 1};
float spc_gold[4] = {0.628, 0.556, 0.366, 1};

float amb_cyan[4] = {0.0, 0.1, 0.06, 1};
float dif_cyan[4] = {0.0, 0.509, 0.509, 1};
float spc_cyan[4] = {0.502, 0.502, 0.502, 1};

// default material is jade
float m_amb[4] = {0.135, 0.2225, 0.1575, 1};
float m_dif[4] = {0.54, 0.89, 0.63, 1};
float m_spc[4] = {0.316, 0.316, 0.316, 1};
float shiny[5] = {0.6*127, 0.1*128, 0.088*128, 0.4*128, 0.25*128} ;

// Camera position and target
float camPos[] = {50, 10, 50};
float camTarget[] = {0, 0, 0};
float horTheta = 44.7f; //value found through trial and error
float verTheta = 1.3f;

double* m_start = new double[3]; /* for mouse-ray picking */
double* m_end = new double[3];

int numObjects = 0;

// bool pick[3] = {false}; /* for intersection testing */

// //light positions
// float pos1[4] = {mapSize, 2*mapSize/3, 0, 0};
// float pos2[4] = {0, 2*mapSize/3, mapSize, 0};

/* TEXTURE stuff from Tutorial 6 */
GLubyte* image0; //floor
GLubyte* image1; //pusheen wallpaper
GLubyte* image2; //stones

int width, height, max0;
int width1, height1, max1;
int width2, height2, max2;

GLuint myTex[4];

int texChoice = 0;


GLubyte* LoadPPM(char* file, int* width, int* height, int* max)
{
    GLubyte* img;
    FILE *fd;
    int n, m;
    int  k, nm;
    char c;
    int i;
    char b[100];
    float s;
    int red, green, blue;
    
    /* first open file and check if it's an ASCII PPM (indicated by P3 at the start) */
    fd = fopen(file, "r");
    fscanf(fd,"%[^\n] ",b);
    if(b[0]!='P'|| b[1] != '3')
    {
        printf("%s is not a PPM file!\n",file);
        exit(0);
    }
    // printf("%s is a PPM file\n", file);
    fscanf(fd, "%c",&c);
    
    /* next, skip past the comments - any line starting with #*/
    while(c == '#')
    {
        fscanf(fd, "%[^\n] ", b);
        printf("%s\n",b);
        fscanf(fd, "%c",&c);
    }
    ungetc(c,fd);
    
    /* now get the dimensions and max colour value from the image */
    fscanf(fd, "%d %d %d", &n, &m, &k);
    
    // printf("%d rows  %d columns  max value= %d\n",n,m,k);
    
    /* calculate number of pixels and allocate storage for this */
    nm = n*m;
    img = (GLubyte*)malloc(3*sizeof(GLuint)*nm);
    s=255.0/k;
    
    /* for every pixel, grab the read green and blue values, storing them in the image data array */
    for(i=0;i<nm;i++)
    {
        fscanf(fd,"%d %d %d",&red, &green, &blue );
        img[3*nm-3*i-3]=red*s;
        img[3*nm-3*i-2]=green*s;
        img[3*nm-3*i-1]=blue*s;
    }
    
    /* finally, set the "return parameters" (width, height, max) and return the image array */
    *width = n;
    *height = m;
    *max = k;
    
    return img;
}

void save(void) {
    printf("Please enter the name of the file to save to (including .txt): ");
    char fileName[50];
    scanf("%49s", fileName);

    ofstream outfile(fileName); //creates output file

    if(outfile.is_open()) { //if it is open, you can start throwing all the writes here
        for (draw = objectList.begin(); draw != objectList.end(); draw++) {
            outfile << draw->getPos(0) << " " << draw->getPos(1) << " " << draw->getPos(2); // first three is pos
            outfile << " ";
            outfile << draw->getOri(0) << " " << draw->getOri(1) << " " << draw->getOri(2); // next three is ori
            outfile << " ";
            outfile << draw->getType(); // type
            outfile << " ";
            outfile << draw->getScale(0) << " " << draw->getScale(1) << " " << draw->getScale(2); // next three is scale
            outfile << "\n";
        }
        outfile.close();
    } else { //should never hit this
        cout << "Unable to open file.\n";
    }
}

void load(void) {
    printf("Please enter the name of the file to load from (including .txt): ");
    char fileName[50];
    scanf("%49s", fileName);

    string line;
    float pos[3] = {0, 0, 0};
    float ori[3] = {0, 0, 0};
    int type;
    float scale[3];

    int values[10] = {0};

    objectList.clear();

    ifstream infile(fileName);

    if(infile.is_open()) {
        while (getline (infile,line)) {
            size_t found; 
            size_t oldfound = -1;
            for (int i = 0; i < 10; i++) {
                found = line.find(" ", oldfound+1); //find instance of " "
                values[i] = atoi(line.substr(oldfound+1, found).c_str());
                oldfound = found;
            }
            pos[0] = values[0]; pos[1] = values[1]; pos[2] = values[2];
            ori[0] = values[3]; ori[1] = values[4]; ori[2] = values[5];
            type = values[6];
            scale[0] = values[7]; scale[1] = values[8]; scale[2] = values[9];

            objectList.push_back(Object(pos, ori, type, scale));
        }
        infile.close();
    } else {
        cout << "Unable to open file.\n";
    }
}

void keyboard(unsigned char key, int xIn, int yIn)
{
	int mod = glutGetModifiers();
	switch (key)
	{
		case 'q':
		case 27:	//27 is the esc key
			exit(0);
            break;
        case 'm':
            for (draw = objectList.begin(); draw != objectList.end(); draw++) {
                draw->setSelected(false);
            }
            break;
        case 49:
            for (int i = 0; i < 4; i++) {
                m_amb[i] = amb_emerald[i];
                m_dif[i] = dif_emerald[i];
                m_spc[i] = spc_emerald[i];
            }
            break;
        case 50:
            for (int i = 0; i < 4; i++) {
                m_amb[i] = amb_jade[i];
                m_dif[i] = dif_jade[i];
                m_spc[i] = spc_jade[i];
            }
            break;
        case 51:
            for (int i = 0; i < 4; i++) {
                m_amb[i] = amb_pearl[i];
                m_dif[i] = dif_pearl[i];
                m_spc[i] = spc_pearl[i];
            }
            break;
        case 52:
            for (int i = 0; i < 4; i++) {
                m_amb[i] = amb_gold[i];
                m_dif[i] = dif_gold[i];
                m_spc[i] = spc_gold[i];
            }
            break;
        case 53: 
            for (int i = 0; i < 4; i++) {
                m_amb[i] = amb_cyan[i];
                m_dif[i] = dif_cyan[i];
                m_spc[i] = spc_cyan[i];
            }
            break;
        // For rotating objects
        case 'z':
            if (mod = GLUT_ACTIVE_ALT) {
                for (draw = objectList.begin(); draw != objectList.end(); draw++) {
                    if (draw->isSelected() == true) {
                        draw->setOriVec(0,0,1);
                        draw->setOri(2,-1);
                    }
                }
            } else {
                for (draw = objectList.begin(); draw != objectList.end(); draw++) {
                    if (draw->isSelected() == true) {
                        draw->setOriVec(0,0,1);
                        draw->setOri(2,1);
                    }
                }
            }
            break;
        case 'x':
            if (mod = GLUT_ACTIVE_ALT) {
                for (draw = objectList.begin(); draw != objectList.end(); draw++) {
                    if (draw->isSelected() == true) {
                        draw->setOriVec(1,0,0);
                        draw->setOri(0,-1);
                    }
                }
            } else {
                for (draw = objectList.begin(); draw != objectList.end(); draw++) {
                    if (draw->isSelected() == true) {
                        draw->setOriVec(1,0,0);
                        draw->setOri(0,1);
                    }
                }
            }
            break;
        case 'y':
            if (mod = GLUT_ACTIVE_ALT) {
                for (draw = objectList.begin(); draw != objectList.end(); draw++) {
                    if (draw->isSelected() == true) {
                        draw->setOriVec(0,1,0);
                        draw->setOri(1,-1);
                    }
                }
            } else {
                for (draw = objectList.begin(); draw != objectList.end(); draw++) {
                    if (draw->isSelected() == true) {
                        draw->setOriVec(0,1,0);
                        draw->setOri(1,1);
                    }
                }
            }
            break;
        case 'p':
            light2 = true; 
            lightcount2 += 1;           
            if (lightcount2 % 2 == 0) {
                light2 = false;
                lightcount2 = 0;
            }
            break;
        case 'o':
            light1 = true;
            lightcount1 += 1;
            if (lightcount1 % 2 == 0) {
                light1 = false;
                lightcount1 = 0;
            } 
            break;
        case 'r':
            objectList.clear();
            break;
        
        case 's':
            save();
            break;
        
        case 'l':
            load();
            break;

        // To draw a sphere
        case 'b':
            for (draw = objectList.begin(); draw != objectList.end(); draw++) {
                draw->setSelected(false);
            }
            objectList.push_back(Object(0)); 
            break;

        // To draw a cube
        case 'c':
            for (draw = objectList.begin(); draw != objectList.end(); draw++) {
                draw->setSelected(false);
            }
            objectList.push_back(Object(1));
            break;

        // To draw a cone
        case 'n':
            for (draw = objectList.begin(); draw != objectList.end(); draw++) {
                draw->setSelected(false);
            }
            objectList.push_back(Object(2));
            break;
        
        // To draw a donuuttttt :D (torus)
        case 'd':
            for (draw = objectList.begin(); draw != objectList.end(); draw++) {
                draw->setSelected(false);
            }
            objectList.push_back(Object(3));
            break;

        // To draw that teapot
        case 't':   
            for (draw = objectList.begin(); draw != objectList.end(); draw++) {
                draw->setSelected(false);
            }
            objectList.push_back(Object(4));
            break;
        
        // For scaling objects
        case 'i':
            for (draw = objectList.begin(); draw != objectList.end(); draw++) {
                if (draw->isSelected() == true) {
                    draw->setScale(0,1); draw->setScale(1,1); draw->setScale(2,1);
                }
            }
            break;
        case 'u':
            for (draw = objectList.begin(); draw != objectList.end(); draw++) {
                if (draw->isSelected() == true) {
                    draw->setScale(0,-1); draw->setScale(1,-1); draw->setScale(2,-1);
                }
            }
            break;

        // floor texture selection
        case '6' :
            texChoice = 0;
            break;
        case '7' :
            texChoice = 2;
            break;
	}
}

void special(int key, int xIn, int yIn){
	int mod = glutGetModifiers();
	switch (key){
        case GLUT_KEY_DOWN:
            if (mod == GLUT_ACTIVE_ALT) {
                if (verTheta <= 0.2) break; //limit to how much you can orbit along x axis
                verTheta -= 0.1f;
                camPos[1] = 10*sin(verTheta); //camTargetY + mapSizeY * sin(verTheta);
            } else if (mod == GLUT_ACTIVE_CTRL) {
                for (draw = objectList.begin(); draw != objectList.end(); draw++) {
                    if (draw->isSelected() == true) {
                        draw->setPos(1,-1);
                    }
                }     
            } else if (mod == GLUT_ACTIVE_SHIFT) {
                if (light1 == true && light2 == false) {
                    lightPos1[2] += 0.2f;
                } else if (light2 == true && light1 == false) {
                    lightPos2[2] += 0.2f;
                } else if (light1 == true && light2 == true) {
                    lightPos1[2] += 0.2f;
                    lightPos2[2] += 0.2f;
                }   
            } else {
                for (draw = objectList.begin(); draw != objectList.end(); draw++) {
                    if (draw->isSelected() == true) {
                        draw->setPos(2,1);
                    }
                }
            }
			break;
		case GLUT_KEY_UP:
            if (mod == GLUT_ACTIVE_ALT) {
                if (verTheta >= 1.3) break;
                verTheta += 0.1f;
                camPos[1] = 10*sin(verTheta);
            } else if (mod == GLUT_ACTIVE_CTRL) {
                for (draw = objectList.begin(); draw != objectList.end(); draw++) {
                    if (draw->isSelected() == true) {
                        draw->setPos(1,1);
                    }
                }
            } else if (mod == GLUT_ACTIVE_SHIFT) {
                if (light1 == true && light2 == false) {
                    lightPos1[2] -= 0.2f;
                } else if (light2 == true && light1 == false) {
                    lightPos2[2] -= 0.2f;
                } else if (light1 == true && light2 == true) {
                    lightPos1[2] -= 0.2f;
                    lightPos2[2] -= 0.2f;
                }
            } else {
                for (draw = objectList.begin(); draw != objectList.end(); draw++) {
                    if (draw->isSelected() == true) {
                        draw->setPos(2,-1);
                    }
                }
            }
			break;
        case GLUT_KEY_LEFT:
            if (mod == GLUT_ACTIVE_ALT) {
                horTheta += 0.1f;
                camPos[0] = 185*cos(horTheta); //camTargetX + mapSizeX * cos(horTheta);
                camPos[2] = 185*sin(horTheta); //camTargetZ + mapSizeZ * sin(horTheta);
            } else if (mod == GLUT_ACTIVE_SHIFT) {
                if (light1 == true && light2 == false) {
                    lightPos1[0] -= 0.2f;
                } else if (light2 == true && light1 == false) {
                    lightPos2[0] -= 0.2f;
                } else if (light1 == true && light2 == true) {
                    lightPos1[0] -= 0.2f;
                    lightPos2[0] -= 0.2f;
                }
                camPos[0] = 70*cos(horTheta); //camTargetX + mapSizeX * cos(horTheta);
                camPos[2] = 70*sin(horTheta); //camTargetZ + mapSizeZ * sin(horTheta);
            } else {
                for (draw = objectList.begin(); draw != objectList.end(); draw++) {
                    if (draw->isSelected() == true) {
                        draw->setPos(0,-1);
                    }
                }
            }
            break;
        case GLUT_KEY_RIGHT:
            if (mod == GLUT_ACTIVE_ALT) {
                horTheta -= 0.1f;
                camPos[0] = 185*cos(horTheta); //camTargetX + mapSizeX * cos(horTheta);
                camPos[2] = 185*sin(horTheta); //camTargetZ + mapSizeZ * sin(horTheta);
            } else if (mod == GLUT_ACTIVE_SHIFT) {
                if (light1 == true && light2 == false) {
                    lightPos1[0] += 0.2f;
                } else if (light2 == true && light1 == false) {
                    lightPos2[0] += 0.2f;
                } else if (light1 == true && light2 == true) {
                    lightPos1[0] += 0.2f;
                    lightPos2[0] += 0.2f;
                }
                camPos[0] = 70*cos(horTheta); //camTargetX + mapSizeX * cos(horTheta);
                camPos[2] = 70*sin(horTheta); //camTargetZ + mapSizeZ * sin(horTheta);
            } else {
                for (draw = objectList.begin(); draw != objectList.end(); draw++) {
                    if (draw->isSelected() == true) {
                        draw->setPos(0,1);
                    }
                }
            }
			break;
	}
}

bool checkIntersect (Object obj, double* R0, double* Rd, float* p0, float* p1, float* p2) {
    Plane planes[6];
    //xy near, xy far, xz near, xz far, yz near, yz far

    double minX = obj.getBounding(0);
    double minY = obj.getBounding(1);
    double minZ = obj.getBounding(2);

    double maxX = obj.getBounding(3);
    double maxY = obj.getBounding(4);
    double maxZ = obj.getBounding(5);

    double minPoints[3] = {minX, minY, minZ};
    double maxPoints[3] = {maxX, maxY, maxZ};

    float p[3];

    //**xy near plane
    double normalXY[3] = {0,0,1};

    planes[0] = Plane();
    planes[0].setFields(normalXY);
    planes[0].updateD(maxPoints);

    double t = planes[0].intersectT(R0, Rd);
    p[0] = R0[0] + t*Rd[0];
    p[1] = R0[1] + t*Rd[1];
    p[2] = R0[2] + t*Rd[2];

    if (minX < p[0] && maxX > p[0] && minY < p[1] && maxY > p[1]) {
        // cout << "XY near intersected\n";
        *p0 = p[0];
        *p1 = p[1];
        *p2 = p[2];
        return true;
    }

    //xy far
    planes[1] = Plane();
    planes[1].setFields(normalXY);
    planes[1].updateD(minPoints);
    
    t = planes[1].intersectT(R0, Rd);
    p[0] = R0[0] + t*Rd[0];
    p[1] = R0[1] + t*Rd[1];
    p[2] = R0[2] + t*Rd[2];

    if (minX < p[0] && maxX > p[0] && minY < p[1] && maxY > p[1]){
        // cout << "XY far intersected\n";
        *p0 = p[0];
        *p1 = p[1];
        *p2 = p[2];
        return true;
    }

    //**xz near plane
    double normalXZ[3] = {0, 1, 0};

    planes[2] = Plane();
    planes[2].setFields(normalXZ);
    planes[2].updateD(maxPoints);

    t = planes[2].intersectT(R0, Rd);
    p[0] = R0[0] + t*Rd[0];
    p[1] = R0[1] + t*Rd[1];
    p[2] = R0[2] + t*Rd[2];

    if (minX < p[0] && maxX > p[0] && minZ < p[2] && maxZ > p[2]){
        // cout << "XZ near intersected\n";
        *p0 = p[0];
        *p1 = p[1];
        *p2 = p[2];
        return true;
    }

    //xz far
    planes[3] = Plane();
    planes[3].setFields(normalXZ);
    planes[3].updateD(minPoints);

    t = planes[3].intersectT(R0, Rd);
    p[0] = R0[0] + t*Rd[0];
    p[1] = R0[1] + t*Rd[1];
    p[2] = R0[2] + t*Rd[2];

    if (minX < p[0] && maxX > p[0] && minZ < p[2] && maxZ > p[2]){
        // cout << "XY far intersected\n";
        *p0 = p[0];
        *p1 = p[1];
        *p2 = p[2];
        return true;
    }

    //**yz near
    double normalYZ[3] = {1, 0, 0};
    
    planes[4] = Plane();
    planes[4].setFields(normalYZ);
    planes[4].updateD(maxPoints);

    t = planes[4].intersectT(R0, Rd);
    p[0] = R0[0] + t*Rd[0];
    p[1] = R0[1] + t*Rd[1];
    p[2] = R0[2] + t*Rd[2];

    if (minY < p[1] && maxY > p[1] && minZ < p[2] && maxZ > p[2]){
        // cout << "YZ near intersected\n";
        *p0 = p[0];
        *p1 = p[1];
        *p2 = p[2];
        return true;
    }

    //yz far
    planes[5] = Plane();
    planes[5].setFields(normalYZ);
    planes[5].updateD(minPoints);
    
    t = planes[5].intersectT(R0, Rd);
    p[0] = R0[0] + t*Rd[0];
    p[1] = R0[1] + t*Rd[1];
    p[2] = R0[2] + t*Rd[2];

    if (minY < p[1] && maxY > p[1] && minZ < p[2] && maxZ > p[2]){
        // cout << "YZ far intersected\n";
        *p0 = p[0];
        *p1 = p[1];
        *p2 = p[2];
        return true;
    }

    return false;
}

double distance(float pointAx, float pointAy, float pointAz, double* pointB) {
    double dist = sqrt(pow(pointAx-pointB[0],2) + pow(pointAy-pointB[1],2) + pow(pointAz-pointB[2],2));
    return dist;
}

/* Much of mouse function was taken from LectureCode7
*/
void mouse(int btn, int state, int x, int y){
    if (btn == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN){
            /*
            * Mouse-ray casting from LectureCode7
            */
            // printf("time for un projection!!!!\n");
            
			// printf("(%f,%f,%f)----(%f,%f,%f)\n", m_start[0], m_start[1], m_start[2], m_end[0], m_end[1], m_end[2]);

			double matModelView[16], matProjection[16]; 
			int viewport[4]; 

			// get matrix and viewport:
			glGetDoublev( GL_MODELVIEW_MATRIX, matModelView ); 
			glGetDoublev( GL_PROJECTION_MATRIX, matProjection ); 
			glGetIntegerv( GL_VIEWPORT, viewport ); 

			// window pos of mouse, Y is inverted on Windows
			double winX = (double)x; 
			double winY = viewport[3] - (double)y; 

			// get point on the 'near' plane (third param is set to 0.0)
			gluUnProject(winX, winY, 0.0, matModelView, matProjection, 
					viewport, &m_start[0], &m_start[1], &m_start[2]); 

			// get point on the 'far' plane (third param is set to 1.0)
			gluUnProject(winX, winY, 1.0, matModelView, matProjection, 
					viewport, &m_end[0], &m_end[1], &m_end[2]); 

			// now you can create a ray from m_start to m_end
			// printf("(%f,%f,%f)----(%f,%f,%f)\n\n", m_start[0], m_start[1], m_start[2], m_end[0], m_end[1], m_end[2]);
        
            /*
                Start of intersection test stuff
            */            
            double* R0 = new double[3];
			double* Rd = new double[3];

			double xDiff = m_end[0] - m_start[0];
			double yDiff = m_end[1] - m_start[1];
			double zDiff = m_end[2] - m_start[2];

			double mag = sqrt(xDiff*xDiff + yDiff*yDiff + zDiff*zDiff);
			R0[0] = m_start[0]; //ray origin
			R0[1] = m_start[1];
            R0[2] = m_start[2];

            Rd[0] = xDiff / mag; //ray direction
			Rd[1] = yDiff / mag;
            Rd[2] = zDiff / mag;

            bool intersected[objectList.size()];
            double poi[objectList.size()][3]; //point of intersection, if any
            float p0, p1, p2;

            if (objectList.size() != 0) {
                for (int i = 0; i < objectList.size(); i++) {
                    intersected[i] = checkIntersect(objectList[i], R0, Rd, &p0, &p1, &p2);
                    poi[i][0] = p0;
                    poi[i][1] = p1;
                    poi[i][2] = p2;
                }
                
                double minDist = 500; //arbitrary value
                int selected;
                for (int i = 0; i < objectList.size(); i++) { //find minimum distance and corresponding object
                    if (intersected[i]) {
                        p0 = poi[i][0];
                        p1 = poi[i][1];
                        p2 = poi[i][2];
                        double dist = distance(p0, p1, p2, R0);
                        if (dist < minDist) {
                            minDist = dist;
                            selected = i;
                        }
                    }
                }

                for (draw = objectList.begin(); draw != objectList.end(); draw++) { //set everything to false
                    draw->setSelected(false);
                }
                if (selected < objectList.size()) //to avoid random crashes
                    objectList[selected].setSelected(true); //except for the one that should be selected
            }
		}
    }
    if (btn == GLUT_RIGHT_BUTTON) { 
        if (state == GLUT_DOWN) {
			double matModelView[16], matProjection[16]; 
			int viewport[4]; 

			// get matrix and viewport:
			glGetDoublev( GL_MODELVIEW_MATRIX, matModelView ); 
			glGetDoublev( GL_PROJECTION_MATRIX, matProjection ); 
			glGetIntegerv( GL_VIEWPORT, viewport ); 

			// window pos of mouse, Y is inverted on Windows
			double winX = (double)x; 
			double winY = viewport[3] - (double)y; 

			// get point on the 'near' plane (third param is set to 0.0)
			gluUnProject(winX, winY, 0.0, matModelView, matProjection, 
					viewport, &m_start[0], &m_start[1], &m_start[2]); 

			// get point on the 'far' plane (third param is set to 1.0)
			gluUnProject(winX, winY, 1.0, matModelView, matProjection, 
					viewport, &m_end[0], &m_end[1], &m_end[2]); 
       
            double* R0 = new double[3];
			double* Rd = new double[3];

			double xDiff = m_end[0] - m_start[0];
			double yDiff = m_end[1] - m_start[1];
			double zDiff = m_end[2] - m_start[2];

			double mag = sqrt(xDiff*xDiff + yDiff*yDiff + zDiff*zDiff);
			R0[0] = m_start[0]; //ray origin
			R0[1] = m_start[1];
            R0[2] = m_start[2];

            Rd[0] = xDiff / mag; //ray direction
			Rd[1] = yDiff / mag;
            Rd[2] = zDiff / mag;

            bool intersected[objectList.size()];
            double poi[objectList.size()][3]; //point of intersection, if any
            float p0, p1, p2;

            if (objectList.size() != 0) {
                for (int i = 0; i < objectList.size(); i++) {
                    intersected[i] = checkIntersect(objectList[i], R0, Rd, &p0, &p1, &p2);
                    poi[i][0] = p0;
                    poi[i][1] = p1;
                    poi[i][2] = p2;
                }
                
                double minDist = 500; //arbitrary value
                int selected;
                for (int i = 0; i < objectList.size(); i++) { //find minimum distance and corresponding object
                    if (intersected[i]) {
                        p0 = poi[i][0];
                        p1 = poi[i][1];
                        p2 = poi[i][2];
                        double dist = distance(p0, p1, p2, R0);
                        if (dist < minDist) {
                            minDist = dist;
                            selected = i;
                        }
                    }
                }

                objectList.erase(objectList.begin() + selected);
            }
        }
    }
}

void loadTextures(void) {
    /* TEXTURES from Tutorial 6 */
    glEnable(GL_TEXTURE_2D);
    glGenTextures(3, myTex);
    
    /* Set the image parameters*/
    glBindTexture(GL_TEXTURE_2D, myTex[0]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT ); 
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    
    /*Get and save image*/
    image0 = LoadPPM("floor_a.ppm", &width, &height, &max0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image0);

    /* Set the image parameters*/
    glBindTexture(GL_TEXTURE_2D, myTex[1]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT ); 
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    
    /*Get and save image*/
    image1 = LoadPPM("pusheen_a.ppm", &width1, &height1, &max1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, image1);


    /* Set the image parameters*/
    glBindTexture(GL_TEXTURE_2D, myTex[2]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT ); 
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    
    /*Get and save image*/
    image2 = LoadPPM("stones_a.ppm", &width2, &height2, &max2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);

    // glMatrixMode(GL_TEXTURE);
    // glScalef(1,-1,-1);
}

void init(void) {
    glClearColor(0, 0, 0, 0);
    glColor3f(1, 1, 1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1, 1, 300);

    glShadeModel(GL_SMOOTH);
    
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);

    glEnable(GL_DEPTH_TEST);

    loadTextures();
}

void drawRoom(void) {
    glBindTexture(GL_TEXTURE_2D, myTex[texChoice]);
    glPushMatrix(); //floor
        glBegin(GL_POLYGON); 
            glNormal3f(0,1,0);
            // glColor3f(0.5, 0.5, 0.5);         

            glTexCoord2f(0.0, 0.0);
            glVertex3f(-50,-5.5,-50);
            // glNormal3f(-50,-5.5,-50);

            glTexCoord2f(0.0, 1.0);
            glVertex3f(-50,-5.5,50);
            // glNormal3f(-50,-5.5,50);            
 
            glTexCoord2f(1.0, 1.0);
            glVertex3f(50,-5.5,50);
            // glNormal3f(50,-5.5,50);
            
            glTexCoord2f(1.0, 0.0);
            glVertex3f(50,-5.5,-50);
            // glNormal3f(50,-5.5,-50);
        glEnd();
    glPopMatrix();
    
    glBindTexture(GL_TEXTURE_2D, myTex[1]);
    glPushMatrix();
        glBegin(GL_POLYGON);    
            glNormal3f(1,0,0); 
            glTexCoord2f(1.0, 0.0);
            glVertex3f(-50, -5.5, -50);

            glTexCoord2f(1.0, 1.0);
            glVertex3f(-50, 44.5, -50);

            glTexCoord2f(0.0, 1.0);
            glVertex3f(-50, 44.5, 50);

            glTexCoord2f(0.0, 0.0);
            glVertex3f(-50, -5.5, 50);
        glEnd();
    glPopMatrix();

    glPushMatrix();
        glBegin(GL_POLYGON);   
            glNormal3f(0, 0, 1);  
            glTexCoord2f(0.0, 0.0);
            glVertex3f(-50, -5.5, -50);

            glTexCoord2f(1.0, 0.0);
            glVertex3f(50, -5.5, -50);

            glTexCoord2f(1.0, 1.0);
            glVertex3f(50, 44.5, -50);

            glTexCoord2f(0.0, 1.0);
            glVertex3f(-50, 44.5, -50);

        glEnd();
    glPopMatrix();

    glPushMatrix();
        glBegin(GL_POLYGON);   
            glNormal3f(-1, 0, 0);  
            glTexCoord2f(0.0, 0.0);
            glVertex3f(50, -5.5, -50);

            glTexCoord2f(1.0, 0.0);
            glVertex3f(50, -5.5, 50);

            glTexCoord2f(1.0, 1.0);
            glVertex3f(50, 44.5, 50);

            glTexCoord2f(0.0, 1.0);
            glVertex3f(50, 44.5, -50);
        glEnd();
    glPopMatrix();

    glPushMatrix();
        glBegin(GL_POLYGON);  
            glNormal3f(0,0,-1);   
            glTexCoord2f(0.0, 0.0);
            glVertex3f(50, -5.5, 50);

            glTexCoord2f(1.0, 0.0);
            glVertex3f(-50, -5.5, 50);

            glTexCoord2f(1.0, 1.0);
            glVertex3f(-50, 44.5, 50);

            glTexCoord2f(0.0, 1.0);
            glVertex3f(50, 44.5, 50);
        glEnd();
    glPopMatrix();
}

// Draws object onto screen based on objectType int passed
void drawObj(int objectType) {
    if (draw->isSelected()) {
        draw->setBounding();
    }

    if (objectType == 0){
        glTranslatef(draw->getPos(0), draw->getPos(1), draw->getPos(2));
        glRotatef(draw->getOri(), draw->getOriVec(0), draw->getOriVec(1), draw->getOriVec(2));
        glScalef(draw->getScale(0), draw->getScale(1), draw->getScale(2));

        // glColor3f(1,0,0);
        draw->setMaterial(m_amb, m_dif, m_spc, mat);
        glutSolidSphere(1, 20, 20);
    }

    if (objectType == 1) {
        glTranslatef(draw->getPos(0), draw->getPos(1), draw->getPos(2));
        glRotatef(draw->getOri(), draw->getOriVec(0), draw->getOriVec(1), draw->getOriVec(2));
        glScalef(draw->getScale(0), draw->getScale(1), draw->getScale(2));
            // printf("%i", draw->getPos(0));
        // glColor3f(0,0,1);
        draw->setMaterial(m_amb, m_dif, m_spc, mat);
        glutSolidCube(1);
    }

    if (objectType == 2) {
        glTranslatef(draw->getPos(0), draw->getPos(1), draw->getPos(2));
        glRotatef(draw->getOri(), draw->getOriVec(0), draw->getOriVec(1), draw->getOriVec(2));
        glRotatef(-90,1,0,0);
        glScalef(draw->getScale(0), draw->getScale(1), draw->getScale(2));

        // glColor3f(0,1,0);
        draw->setMaterial(m_amb, m_dif, m_spc, mat);
        glutSolidCone(1,2,20,20);
    }

    if (objectType == 3) {
        glTranslatef(draw->getPos(0), draw->getPos(1), draw->getPos(2));
        glRotatef(draw->getOri(), draw->getOriVec(0), draw->getOriVec(1), draw->getOriVec(2));        
        glScalef(draw->getScale(0), draw->getScale(1), draw->getScale(2));

        // glColor3f(0.8,0.4,0);
        draw->setMaterial(m_amb, m_dif, m_spc, mat);
        glutSolidTorus(0.4,1,20,20);
    }

    if (objectType == 4) {
        glTranslatef(draw->getPos(0), draw->getPos(1), draw->getPos(2));
        glRotatef(draw->getOri(), draw->getOriVec(0), draw->getOriVec(1), draw->getOriVec(2));
        glScalef(draw->getScale(0), draw->getScale(1), draw->getScale(2));

        // glColor3f(1,0,1);
        draw->setMaterial(m_amb, m_dif, m_spc, mat);
        glutSolidTeapot(1);
    }
}

void display(void) {    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(camPos[0], camPos[1], camPos[2], camTarget[0], camTarget[1], camTarget[2], 0, 1, 0);    

    // glBegin(GL_LINES); //projection lines! comment out when finished with testing
    //     glColor3f(1,1,1);
    //     glVertex3f(m_start[0], m_start[1], m_start[2]);
    //     glVertex3f(m_end[0], m_end[1], m_end[2]);
    // glEnd();


   // Lighting Functions
    if (light1 == true) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glLightfv(GL_LIGHT0, GL_POSITION, lightPos1);
        glLightfv(GL_LIGHT0, GL_AMBIENT, amb1);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, dif1);
        glLightfv(GL_LIGHT0, GL_SPECULAR, spc1);

        glPushMatrix();
            glTranslatef(lightPos1[0], lightPos1[1], lightPos1[2]);
            glColor3f(1,1,1);
            glutSolidSphere(1, 20, 20);
        glPopMatrix();
    } else {
        glDisable(GL_LIGHT0);
    }

    if (light2 == true) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT1);

        glLightfv(GL_LIGHT1, GL_POSITION, lightPos2);
        glLightfv(GL_LIGHT1, GL_AMBIENT, amb2);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, dif2);
        glLightfv(GL_LIGHT1, GL_SPECULAR, spc2);

        glPushMatrix();
            glTranslatef(lightPos2[0], lightPos2[1], lightPos2[2]);
            glColor3f(1,1,1);
            glutSolidSphere(1, 20, 20);
           glPopMatrix();
    } else {
        glDisable(GL_LIGHT1);
    }

    if (light1 == false && light2 == false) {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
    }

    
    drawRoom();

    glBindTexture(GL_TEXTURE_2D, myTex[3]); //blank texture
    for (draw = objectList.begin(); draw != objectList.end(); draw++) {
        glPushMatrix();
            //glEnable(GL_COLOR_MATERIAL);
            drawObj(draw->getType());
           // glDisable(GL_COLOR_MATERIAL);
        glPopMatrix();
    }

    
 
    glutSwapBuffers();
    glutPostRedisplay();
}

void FPSTimer(int value){ //60fps
	glutTimerFunc(17, FPSTimer, 0);
    glutPostRedisplay();
}

int manual()
{
    printf("\n\n\n --- Welcome to the 3D MODELLER --- \n");
    printf("\n How to navigate the program:\n\n");
    printf("   Objects: \n");
    printf("    *** 'b' - Places a sphere \n");
    printf("    *** 'c' - Places a cube \n");
    printf("    *** 'n' - Places a cone \n");
    printf("    *** 'd' - Places a donut \n");
    printf("    *** 't' - Places a teapot \n");
    printf("    ------------------------------- \n");
    printf("   Object Movement: Click on an object with your mouse\n");
    printf("    *** 'UP' arrow key - Move object along -z \n");
    printf("    *** 'DOWN' arrow key - Move object along +z \n");
    printf("    *** 'LEFT' arrow key - Move object along -x \n");
    printf("    *** 'RIGHT' arrow key - Move object along +x \n");
    printf("    *** CTRL + 'UP' arrow key - Move object along +y \n");
    printf("    *** CTRL + 'DOWN' arrow key - Move object along -y \n");
    printf("    ------------------------------- \n");
    printf("   Camera Control: \n");
    printf("    *** ALT + 'UP' arrow key - Move camera up around y-axis \n");
    printf("    *** ALT + 'DOWN' arrow key - Move camera down around y-axis \n");
    printf("    *** ALT + 'LEFT' arrow key - Orbit camera to the left around origin \n");
    printf("    *** ALT + 'RIGHT' arrow key - Orbit camera to the right around origin \n");
    printf("    ------------------------------- \n");
    printf("   Object Manipulation: \n");
    printf("    *** 'i' - Increase size \n");
    printf("    *** 'u' - Decrease size \n");
    printf("    *** 'z' - Rotate +z axis \n");
    printf("    *** 'y' - Rotate +y axis \n");
    printf("    *** 'x' - Rotate +x axis \n");
    printf("    *** ALT + 'z' - Rotate -z axis \n");
    printf("    *** ALT + 'y' - Rotate -y axis \n");
    printf("    *** ALT + 'x' - Rotate -x axis \n");
    printf("    ------------------------------- \n");
    printf("   Scene file: \n");
    printf("    *** 's' - Prompts to save scene \n");
    printf("    *** 'l' - Prompts to load scene \n");
    printf("    *** 'r' - Resets the scene \n");
    printf("    ------------------------------- \n");
    printf("   Lighting: \n");
    printf("    *** 'o' - Sets first light \n");
    printf("    *** 'p' - Sets second light \n");
    printf("    *** SHIFT + Arrow Key - Moves lights \n");
    printf("   Material select (with Lighting on): \n");
    printf("    *** '1' - Sets material to EMERALD \n");
    printf("    *** '2' - Sets material to JADE \n");
    printf("    *** '3' - Sets material to PEARL \n");
    printf("    *** '4' - Sets material to GOLD \n");
    printf("    *** '5' - Sets material to CYAN PLASTIC \n");
    printf("    ------------------------------- \n");
    printf("   Texture select: \n");
    printf("    *** '6' - Sets floor to floorboards texture \n");
    printf("    *** '7' - Sets floor to stones texture \n");
    printf("    ------------------------------- \n");
    printf("   Exit: \n");
    printf("    *** ESC or 'q' - Terminates Application \n");
    printf("\n\n\n");
    printf("    Harrison Lau (400020821) & Vanessa Truong (400023632) \n");

    return 0;
}

/* main function - program entry point */
int main(int argc, char** argv)
{
    manual(); // prints out the program commands

	glutInit(&argc, argv);		//starts up GLUT
	// glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600,600);
	glutInitWindowPosition(50,50);
	glutCreateWindow("3GC3 Assignment 3: Modeller");	//creates the window
    
    glutDisplayFunc(display);	//registers "display" as the display callback function

    //keyboard callback
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);

    //mouse callback
	glutMouseFunc(mouse);
    
    //fps timer callback
    glutTimerFunc(17, FPSTimer, 0);

    init();
    
    glutMainLoop();				//starts the event loop
    return(0);					//return may not be necessary on all compilers
}
