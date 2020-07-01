//
//	Animation.h
//

#ifndef ANIMATION_H
#define ANIMATION_H

class Animation {
public:
	Animation();
	~ Animation() = default;

	void loadModel();
	void draw(bool jumping);
	void resetCount();
	void decreaseRunCounter();
	void markRunning(bool is_forward);
private:
	unsigned int m_run_counter;
	float m_run_frame;
};

#endif
