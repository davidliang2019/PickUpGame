//
//	DeltaTime.h
//

#ifndef DELTA_TIME_H
#define DELTA_TIME_H

namespace DeltaTime {
	static unsigned int start_time = 0;
	static unsigned int previous_time = 0;
	static unsigned int current_time = 0;
	static unsigned int elapsed_time = 0;
	static unsigned int  physics_update = 0;
	static unsigned int total_frames = 0;
	static float update_lag = 0.0f;
	const float FIXED_DELTA_TIME = 1.0f / 60.0f;
	const unsigned int FRAME_PER_SECOND = 60;
}

#endif