#pragma once

struct Particle 
{
	Particle() : m_Position(Vec3Graphics(0, 0, 0)),
	m_OldPos(Vec3Graphics(0, 0, 0)),
	m_Velocity(Vec3Graphics(0, 0, 0)),
	m_Colour(Vec4Graphics(0, 0, 0, 0)),
	m_Rotate(0),
	m_Life(0),
	m_Size(0),
	m_Age(0)
	{}

	Vec3Graphics m_Position;
	Vec3Graphics m_OldPos;
	Vec3Graphics m_Velocity;
	Vec4Graphics m_Colour;

	float m_Rotate;
	float m_Life;
	float m_Size;
	float m_Age;
};
