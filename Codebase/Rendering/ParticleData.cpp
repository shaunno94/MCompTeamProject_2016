#include "ParticleData.h"

ParticleData::ParticleData(int maxCount)
:m_count(0),
m_countAlive(0)
{
	generate(maxCount);
}

void ParticleData::generate(int maxSize)
{
	m_count = maxSize;
	m_countAlive = 0;

	m_pos[maxSize];
	m_col[maxSize];
	m_startCol[maxSize];
	m_endCol[maxSize];
	m_vel[maxSize];
	m_acc[maxSize];
	m_time[maxSize];
	m_alive[maxSize];
}

void ParticleData::kill(int id)
{
	if (m_countAlive > 0)
	{
		m_alive[id] = false;
		swapData(id, m_countAlive - 1);
		m_countAlive--;
	}
}

void ParticleData::wake(int id)
{
	if (m_countAlive < m_count)
	{
		m_alive[id] = true;
		swapData(id, m_countAlive);
		m_countAlive++;
	}
}

void ParticleData::swapData(int a, int b)
{
	std::swap(m_pos[a], m_pos[b]);
	std::swap(m_col[a], m_col[b]);
	std::swap(m_startCol[a], m_startCol[b]);
	std::swap(m_endCol[a], m_endCol[b]);
	std::swap(m_vel[a], m_vel[b]);
	std::swap(m_acc[a], m_acc[b]);
	std::swap(m_time[a], m_time[b]);
	std::swap(m_alive[a], m_alive[b]);
}