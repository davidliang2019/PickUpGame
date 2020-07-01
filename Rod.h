//
//	Rod.h
//

#ifndef ROD_H
#define ROD_H

#include "ObjLibrary/Vector3.h"

class Rod {
public:
	Rod();
	~Rod() = default;
	Rod(ObjLibrary::Vector3 position, int disk_type);
	
	void draw();
	void pickingUp(ObjLibrary::Vector3 position, float radius, float half_height);
	bool isPickedup();
	unsigned int getPoint();
private:
	bool isModelsLoaded();
	void loadModel();
	ObjLibrary::Vector3 r_position;
	bool pickup;
	unsigned int point;
};

#endif