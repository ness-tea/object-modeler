#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

#include "object.h"
#include <stdio.h> //potentially not needed

Object::Object(int t) {
    pos[0] = 0;
    pos[1] = 0;
    pos[2] = 0;

    ori[0] = 0;
    ori[1] = 0;
    ori[2] = 0;

    oriVec[0] = 0;
    oriVec[1] = 1;
    oriVec[2] = 0;

    type = t;
    selected = true;

    scale[0] = 2;
    scale[1] = 2;
    scale[2] = 2;

    amb[0] = 0.135; 
    amb[1] = 0.2225; 
    amb[2] = 0.1575; 
    amb[3] = 1;
   
    dif[0] = 0.54;
    dif[1] = 0.89;
    dif[2] = 0.63;
    dif[3] = 1.0;
   
    spc[0] = 0.316;
    spc[1] = 0.316; 
    spc[2] = 0.316;
    spc[3] = 1;

    shiny = 0.1*128;

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spc);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
}

Object::Object(float* position, float* orientation, int t, float* sc) {
    pos[0] = position[0];
    pos[1] = position[1];
    pos[2] = position[2];

    ori[0] = orientation[0];
    ori[1] = orientation[1];
    ori[2] = orientation[2];

    oriVec[0] = 0;
    oriVec[1] = 1;
    oriVec[2] = 0;

    type = t;
    selected = true;

    scale[0] = sc[0];
    scale[1] = sc[1];
    scale[2] = sc[2];
}

Object::getPos(int xyz) {
    return pos[xyz];
}

Object::getOri() {
    if (oriVec[0] == 1){
        return ori[0];
    }
    if (oriVec[1] == 1){
        return ori[1];
    }
    if (oriVec[2] == 1){
        return ori[2];
    }
}

Object::getOri(int xyz) {
    return ori[xyz];
}

Object::getOriVec(int xyz) {
    return oriVec[xyz];
}

Object::getType() {
    return type;
}

Object::isSelected() {
    return selected;
}

Object::getScale(int xyz) {
    return scale[xyz];
}

Object::getBounding(int xyzXYZ) {
    return bounding[xyzXYZ];
}

Object::setScale(int xyz, float factor) {
    scale[xyz] += factor;
    setBounding();
}

Object::setPos(int xyz, float xyzNew) {
    pos[xyz] += xyzNew;
    setBounding();
}

Object::setOri(int xyz, int xyzNew) {
    if (xyzNew < 0) {
        ori[xyz] -= xyzNew;
    } else if (xyzNew > 0) {
        ori[xyz] += xyzNew;
    }
}

Object::setOriVec(int x, int y, int z) {
    oriVec[0] = x;
    oriVec[1] = y;
    oriVec[2] = z;
}

Object::setBounding() {
    float xmin = pos[0] - scale[0]/2;
    float xmax = pos[0] + scale[0]/2;
    float ymin = pos[1] - scale[1]/2;
    float ymax = pos[1] + scale[1]/2;
    float zmin = pos[2] - scale[2]/2;
    float zmax = pos[2] + scale[2]/2;
    
    glBegin(GL_LINES);
    // drawing y planes
    glVertex3f(xmin, ymin, zmin);
    glVertex3f(xmin, ymin, zmax);
    glVertex3f(xmax, ymin, zmin);
    glVertex3f(xmax, ymin, zmax);
    glVertex3f(xmin, ymax, zmin);
    glVertex3f(xmin, ymax, zmax);
    glVertex3f(xmax, ymax, zmin);
    glVertex3f(xmax, ymax, zmax);
    //drawing z planes
    glVertex3f(xmin, ymin, zmin);
    glVertex3f(xmin, ymax, zmin);
    glVertex3f(xmin, ymin, zmax);
    glVertex3f(xmin, ymax, zmax);
    glVertex3f(xmax, ymin, zmin);
    glVertex3f(xmax, ymax, zmin);
    glVertex3f(xmax, ymin, zmax);
    glVertex3f(xmax, ymax, zmax);
    //drawing x planes
    glVertex3f(xmin, ymin, zmin);
    glVertex3f(xmax, ymin, zmin);
    glVertex3f(xmin, ymin, zmax);
    glVertex3f(xmax, ymin, zmax);
    glVertex3f(xmin, ymax, zmin);
    glVertex3f(xmax, ymax, zmin);
    glVertex3f(xmin, ymax, zmax);
    glVertex3f(xmax, ymax, zmax);
    glEnd();
    bounding[0] = pos[0] - 0.5*scale[0]; // minX 
    bounding[1] = pos[1] - 0.5*scale[1]; // minY 
    bounding[2] = pos[2] - 0.5*scale[2]; // minZ

    bounding[3] = pos[0] + 0.5*scale[0]; // maxX 
    bounding[4] = pos[1] + 0.5*scale[1]; // maxY
    bounding[5] = pos[2] + 0.5*scale[2]; // maxZ
}

Object::setSelected(bool changed) {
    selected = changed;
}

Object::setMaterial(float* m_amb, float* m_dif, float* m_spc, int mat) {
    for (int i = 0; i < 4; ++i) {
        amb[i] = m_amb[i];
        dif[i] = m_dif[i];
        spc[i] = m_spc[i];
    }
    shiny = mat;
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spc);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
}