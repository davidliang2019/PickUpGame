//
//  DiskType.h
//

#ifndef DISK_TYPE_H
#define DISK_TYPE_H

#include <string>

namespace DiskType
{
	const int COUNT = 5;
	const int RED_ROCK = 0;
	const int LEAFY = 1;
	const int ICY = 2;
	const int SANDY = 3;
	const int GREY_ROCK = 4;

	inline const std::string& getModelName (int disk_type)
	{
		static const std::string A_MODEL[COUNT] =
		{
			"Models/DiskA.obj",	//  RED_ROCK
			"Models/DiskB.obj",	//  LEAFY
			"Models/DiskC.obj",	//  ICY
			"Models/DiskD.obj",	//  SANDY
			"Models/DiskE.obj",	//  GREY_ROCK
		};

		return A_MODEL[disk_type];
	}

	inline int getSideLength (int disk_type)
	{
		static const int A_SIDE_LENGTH[COUNT] =
		{
			16,	//  RED_ROCK
			32,	//  LEAFY
			48,	//  ICY
			64,	//  SANDY
			80,	//  GREY_ROCK
		};

		return A_SIDE_LENGTH[disk_type];
	}

	inline int getTexureRepeatCount (int disk_type)
	{
		static const int A_REPEAT_COUNT[COUNT] =
		{
			1,	//  RED_ROCK
			2,	//  LEAFY
			3,	//  ICY
			4,	//  SANDY
			5,	//  GREY_ROCK
		};

		return A_REPEAT_COUNT[disk_type];
	}

	inline const std::string& getTextureName (int disk_type)
	{
		static const std::string A_TEXTURE[COUNT] =
		{
			"Models/DiskA.bmp",	//  RED_ROCK
			"Models/DiskB.bmp",	//  LEAFY
			"Models/DiskC.bmp",	//  ICY
			"Models/DiskD.bmp",	//  SANDY
			"Models/DiskE.bmp",	//  GREY_ROCK
		};

		return A_TEXTURE[disk_type];
	}

	inline float getRingSpeedFactor(int disk_type)
	{
		static const float RING_SPEED_FACTOR[COUNT] =
		{
			1.0f,	//  RED_ROCK
			0.5f,	//  LEAFY
			0.25f,	//  ICY
			0.75f,	//  SANDY
			1.0f,	//  GREY_ROCK
		};

		return RING_SPEED_FACTOR[disk_type];
	}

	inline unsigned int getRodPoints(int disk_type)
	{
		static const int ROD_POINTS[COUNT] =
		{
			1,	//  RED_ROCK
			2,	//  LEAFY
			3,	//  ICY
			4,	//  SANDY
			5,	//  GREY_ROCK
		};

		return ROD_POINTS[disk_type];
	}

	inline float getFrictionFactor(int disk_type)
	{
		static const float A_FRICTION_FACTOR[COUNT] =
		{
			0.005f,	//  RED_ROCK
			0.0001f,//  LEAFY
			0.5f,	//  ICY
			0.2f,	//  SANDY
			0.005f,	//  GREY_ROCK
		};

		return A_FRICTION_FACTOR[disk_type];
	}

	inline float getAccelerationFactor(int disk_type)
	{
		static const float A_ACCELERATION_FACTOR[COUNT] =
		{
			1.0f,	//  RED_ROCK
			0.5f,	//  LEAFY
			0.25f,	//  ICY
			0.25f,	//  SANDY
			1.0f,	//  GREY_ROCK
		};

		return A_ACCELERATION_FACTOR[disk_type];
	}

	inline float getMinSlope(int disk_type)
	{
		static const float MIN_SLOPE[COUNT] =
		{
			0.4f,	//  RED_ROCK
			1.0f,	//  LEAFY
			0.2f,	//  ICY
			0.2f,	//  SANDY
			0.4f,	//  GREY_ROCK
		};

		return MIN_SLOPE[disk_type];
	}

};  // end of namespace DiskType

#endif
