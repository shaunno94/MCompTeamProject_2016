
#include "../Helpers/MeasuringTimer.h"
#include "../Helpers/RNG.h"
#include "../Math/nclglMath.h"


const unsigned dataSize = 61000;
QuaternionIntrinsics test1data[dataSize];
QuaternionSimple test2data[dataSize];

void setupTestData()
{
	for (int i = 0; i < dataSize; ++i) {
		for (int j = 0; j < 4; ++j) {
			test1data[i].mmvalue.m128_f32[j] = RNG32::Rand(0.0f, 1000.0f);
		}
		test2data[i].x = RNG32::Rand(0.0f, 1000.0f);
		test2data[i].y = RNG32::Rand(0.0f, 1000.0f);
		test2data[i].z = RNG32::Rand(0.0f, 1000.0f);
		test2data[i].w = RNG32::Rand(0.0f, 1000.0f);
	}
}

float test1(long long testLoops)
{
	__m128 test = _mm_set_ps(4, 3, 2, 1);
	float result = 0.0;
	for (long long j = 0; j < testLoops; ++j) {
		test1data[j] = test1data[j + 1] + test1data[j + 2];
	}
	return result;
}

float test2(long long testLoops)
{
	__m128 test = _mm_set_ps(4, 3, 2, 1);
	float result = 0.0;
	for (long long j = 0; j < testLoops; ++j) {
		test2data[j] = test2data[j + 1] + test2data[j + 2];
	}
	return result;
}

void test3(unsigned int testLoops)
{
	for (unsigned j = 0; j < testLoops; ++j) {

	}
}


int main(int argc, char** argv)
{
	setupTestData();

	unsigned TEST_TAKES = 10;
	long long TEST_LOOPS = 60000;
	static const char* const startText = "TakeStart";
	static const char* const endText = "TakeEnd";

	MeasuringTimer testTracker(10000);
	float result = 0;

	testTracker.LogStart();
	for (unsigned i = 0; i < TEST_TAKES; ++i) {
		testTracker.LogStart(startText);
result +=		test1(TEST_LOOPS);
		testTracker.LogEnd(endText);
	}
	testTracker.LogEnd();
	testTracker.LogStart();
	for (unsigned i = 0; i < TEST_TAKES; ++i) {
		testTracker.LogStart(startText);
result +=		test2(TEST_LOOPS);
		testTracker.LogEnd(endText);
	}
	testTracker.LogEnd();


	std::cout << "finished!" << result << LINE_SEPARATOR;
	std::cout << testTracker << LINE_SEPARATOR;

	/*float x1 = RNG32::Rand(0.0f, 100.0f);
	float y1 = RNG32::Rand(0.0f, 100.0f);
	float z1 = RNG32::Rand(0.0f, 100.0f);
	float w1 = RNG32::Rand(0.0f, 100.0f);
	float x2 = RNG32::Rand(0.0f, 100.0f);
	float y2 = RNG32::Rand(0.0f, 100.0f);
	float z2 = RNG32::Rand(0.0f, 100.0f);
	float w2 = RNG32::Rand(0.0f, 100.0f);

	QuaternionIntrinsics quatIn1(x1, y1, z1, w1);
	QuaternionIntrinsics quatIn2(x2, y2, z2, w2);
	QuaternionSimple quat1(x1, y1, z1, w1);
	QuaternionSimple quat2(x2, y2, z2, w2);

	QuaternionIntrinsics qIn3 = quatIn1 * quatIn2;
	QuaternionSimple q3 = quat1 * quat2;

	std::cout << qIn3;
	std::cout << q3;*/

	int x;
	std::cin >> x;

	return 0;
}
