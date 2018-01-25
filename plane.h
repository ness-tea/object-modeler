#ifndef __PLANE_H__	//guard against cyclic dependancy
#define __PLANE_H__ //written with help of LectureCode1 from avenue

class Plane{
    public:
        Plane();
        setFields(double*);
        double intersectT(double*, double*);
        updateD(double*);

        double a, b, c, d;
};


#endif