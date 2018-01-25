#ifndef __OBJECT_H__	//guard against cyclic dependancy
#define __OBJECT_H__ //written with help of LectureCode1 from avenue

class Object{
	public:
		Object(int); //position and orientation
		Object(float*, float*, int, float*);
		// setScale(float, float, float);
		// setMat(float*, float*, float*, float);
		// setType(int);
		
		// Getters for object position, orientation, and type
		getPos(int);
		getOri();
		getOri(int);
		getOriVec(int);
		getType();
		isSelected();
		getScale(int);
		getBounding(int);

		// Setters for object
		setPos(int, float);
		setOri(int, int);
		setOriVec(int, int, int);
		setSelected(bool);
		setBounding();
		setMaterial(float[4], float[4], float[4], int);
		setScale(int, float);

		float pos[3];
		float posi[3];
		float ori[3]; //orientiation
		float oriVec[3];
		int type;

		bool selected;

		float scale[3];
		
		double bounding[6];

		//Material
		float amb[4];
		float dif[4];
		float spc[4];
		float shiny;
};


#endif
