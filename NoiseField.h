//
//	NoiseField.h
//

#ifndef NOISEFIELD_H
#define NOISEFIELD_H

#include <ctime>
#include <cmath>
#include "GetGlut.h"
#include "ObjLibrary/Vector2.h"

#define PI 3.14159265f

class NoiseField {
public:
	/* constructors and destructor */
	NoiseField(float g, float a);
	NoiseField() : grid_size(0.0f), amplitude(0.0f) {}
	~NoiseField() = default;

	/* member functions */
	float valueNoise(float x, float y);
	float perlinNoise(float x, float y);
private:
	unsigned int pseudorandom(int x, int y);
	float fade(float n);
	ObjLibrary::Vector2 lattice(int x, int y);
	/* member variables */
	unsigned int seed[7];
	float grid_size;
	float amplitude;
};

#endif