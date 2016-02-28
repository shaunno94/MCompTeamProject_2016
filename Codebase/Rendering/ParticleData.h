#pragma once
#include "Math\nclglMath.h"
#include "Rendering\GameObject.h"

class ParticleData : public GameObject
{
public:
	ParticleData(){};
	ParticleData(int maxCount);
	~ParticleData() { }

	ParticleData(const ParticleData &) = delete;
	ParticleData &operator=(const ParticleData &) = delete;

	void generate(int maxSize);
	void kill(int id);
	void wake(int id);
	void swapData(int a, int b);

	int getCount() const { return m_count; }
	int getAliveCount() const { return m_countAlive; }

public:
	Vec4Graphics* m_pos;
	Vec4Graphics* m_col;
	Vec4Graphics* m_startCol;
	Vec4Graphics* m_endCol;
	Vec4Graphics* m_vel;
	Vec4Graphics* m_acc;
	Vec4Graphics* m_time;
	bool*  m_alive;

	int m_count;
	int m_countAlive;
};