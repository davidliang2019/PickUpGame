//
//	Animation.cpp
//

#include <sstream>
#include "ObjLibrary/Vector3.h"
#include "ObjLibrary/DisplayList.h"
#include "ObjLibrary/ObjModel.h"
#include "Animation.h"

using namespace std;
using namespace ObjLibrary;
namespace {
	const string MODEL_NAME = "cbabe";
	//const string MODEL_NAME = "guard";

	const bool IS_LOAD_ANIMATION = true;
	const unsigned int RUN_FRAME_COUNT = 20;
	const unsigned int RUN_COUNTER_MAX = 2;
	const float RUN_FRAMES_FORWARD_PER_SECOND = 20.0f;
	const float RUN_FRAMES_BACKWARD_PER_SECOND = 10.0f;
	const bool IS_CALCULATE_HALF_HEIGHT = false;
	const float PHYSICS_FRAME_LENGTH = 1.0f / 60.0f;

	DisplayList g_stand_list;
	DisplayList g_jump_list;
	DisplayList ga_run_list[RUN_FRAME_COUNT];

	unsigned int m_run_counter = 0;
	float m_run_frame = 0.0f;
}

Animation::Animation()
	: m_run_counter(0)
	, m_run_frame(0.0f)
{}

void Animation::loadModel()
{
	g_stand_list = ObjModel("Models/" + MODEL_NAME + "_stand.obj").getDisplayList();
	g_jump_list = ObjModel("Models/" + MODEL_NAME + "_jump.obj").getDisplayList();
	for (unsigned int i = 0; i < RUN_FRAME_COUNT; i++)
	{
		if (IS_LOAD_ANIMATION || i == 0)
		{
			stringstream ss;
			ss << "Models/" << MODEL_NAME << "_run" << i << ".obj";
			ga_run_list[i] = ObjModel(ss.str()).getDisplayList();
		}
		else
		{
			assert(i > 0);
			assert(ga_run_list[0].isReady());
			ga_run_list[i] = ga_run_list[0];
		}
	}

	if (IS_CALCULATE_HALF_HEIGHT)
	{
		ObjModel stand("Models/" + MODEL_NAME + "_stand.obj");
		double y_min = 1.0e20;
		for (unsigned int v = 0; v < stand.getVertexCount(); v++)
			if (stand.getVertexY(v) < y_min)
				y_min = stand.getVertexY(v);
		cout << "Half height: " << y_min << endl;
	}
}

void Animation::draw(bool jumping)
{
	if (jumping)
		g_jump_list.draw();
	else if (m_run_counter > 0)
	{
		double run_frame_fraction = fmod(m_run_frame, RUN_FRAME_COUNT);
		if (run_frame_fraction < 0.0)
			run_frame_fraction += RUN_FRAME_COUNT;
		assert(run_frame_fraction >= 0.0);
		assert(run_frame_fraction <  RUN_FRAME_COUNT);
		unsigned int run_frame_int = (unsigned int)(run_frame_fraction);
		assert(run_frame_int <  RUN_FRAME_COUNT);
		ga_run_list[run_frame_int].draw();
	}
	else
		g_stand_list.draw();
}

void Animation::resetCount()
{
	m_run_counter = 0;
}

void Animation::decreaseRunCounter()
{
	if (m_run_counter > 0)
		m_run_counter--;
}

void Animation::markRunning(bool is_forward)
{
	m_run_counter = RUN_COUNTER_MAX;

	if (is_forward)
		m_run_frame += RUN_FRAMES_FORWARD_PER_SECOND * PHYSICS_FRAME_LENGTH;
	else
		m_run_frame -= RUN_FRAMES_BACKWARD_PER_SECOND * PHYSICS_FRAME_LENGTH;
}
