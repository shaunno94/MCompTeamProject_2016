#pragma once

#define PI_DEF 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899
#define RAD_DEF 57.295779513082320876798154814105

/// <summary>
/// Value of pi(ish).
/// </summary>
static const float PI_F = static_cast<float>(PI_DEF);
/// <summary>
/// Value of pi(ish).
/// </summary>
static const double PI_D = static_cast<double>(PI_DEF);

/// <summary>
/// Value of 1.0f over pi(ish).
/// </summary>
static const float PI_F_R = 1.0f / PI_F;

/// <summary>
/// Value of 1.0f over pi(ish).
/// </summary>
static const double PI_D_R = 1.0f / PI_D;

#define PI   PI_F
#define PI_R PI_F_R



/// <summary>
/// Pi divided by 360.0f.
/// </summary>
static const float PI_OVER_360 = PI_F / 360.0f;

static const float RAD_F = static_cast<float>(RAD_DEF);
static const double RAD_D = static_cast<double>(RAD_DEF);

static const float RAD_F_R = 1.0f / static_cast<float>(RAD_DEF);
static const double RAD_D_R = 1.0 / static_cast<double>(RAD_DEF);

#define RAD RAD_F
#define RAD_R RAD_F_R


/// <summary>
/// Conversion from radians to degrees.
/// </summary>
/// <param name="rad">Angle in radians.</param>
/// <returns>Angle in degrees.</returns>
static inline float RadToDeg(float rad)	{ return rad * 180.0f * PI_F_R; };

/// <summary>
/// Conversion from radians to degrees.
/// </summary>
/// <param name="rad">Angle in radians.</param>
/// <returns>Angle in degrees.</returns>
static inline double RadToDeg(double rad)	{ return rad * 180.0 * PI_D_R; };

/// <summary>
/// Conversion from degrees to radians.
/// </summary>
/// <param name="deg">Angle in degrees.</param>
/// <returns>Angle in radians.</returns>
static inline float DegToRad(float deg)	{ return deg * PI * 0.00555555556f; };

/// <summary>
/// Conversion from degrees to radians.
/// </summary>
/// <param name="deg">Angle in degrees.</param>
/// <returns>Angle in radians.</returns>
static inline double DegToRad(double deg) { return deg * PI * 0.0055555555555555556; };
