#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

#include <stdio.h> //potentially not needed
#include "plane.h"

Plane::Plane() {
    a = 0;
    b = 0;
    c = 0;
    d = 0;
}

Plane::setFields(double* normal) {
    a = normal[0];
    b = normal[1];
    c = normal[2];
}

double Plane::intersectT(double* rayStart, double* rayDir) {
    double denom = a*rayDir[0] + b*rayDir[1] + c*rayDir[2];
    if (denom == 0) return 0;
    double t = -(a*rayStart[0] + b*rayStart[1] + c*rayStart[2] + d) / denom;
    
    // printf("Plane A:%f B:%f C:%f D: %f t: %f\n", a, b, c, d, t);
    
    return t;
}

Plane::updateD(double* point) {
    d = -point[0]*a - point[1]*b - point[2]*c;
}