
#include "../Helpers/MeasuringTimer.h"
#include <vector>
#include "FixedSizeAllocator.h"
#include "STLManagedAllocator.h"
#include "AlignmentWrapper.h"


unsigned dataSize = 10000;
std::vector<int*> test1data;
std::vector<int*> test2data;
FixedSizeAllocator allocator(4, 50, 5);

void test1(unsigned int testLoops, MeasuringTimer& tracker)
{
	static const char* const startText = "LoopStart";

	tracker.LogStart(startText);
	for (unsigned j = 0; j < testLoops; ++j) {
		test1data.push_back(reinterpret_cast<int*>(allocator.Allocate()));
	}
	for (unsigned j = 0; j < testLoops; ++j) {
		allocator.Free(test1data[j]);
		test1data.clear();
	}
	tracker.LogEnd();
}

void test2(unsigned int testLoops, MeasuringTimer& tracker)
{
	static const char* const startText = "LoopStart2";

	tracker.LogStart(startText);
	for (unsigned j = 0; j < testLoops; ++j) {
		test2data.push_back(new int());
	}
	for (unsigned j = 0; j < testLoops; ++j) {
		delete test2data[j];
		test2data.clear();
	}
	tracker.LogEnd();
}


int main2(int argc, char** argv)
{
	/*unsigned TEST_TAKES = 3;
	unsigned TEST_LOOPS = 100;
	static const char* const startText = "TakeStart";
	static const char* const endText = "TakeEnd";

	MeasuringTimer testTracker(100);

	testTracker.LogStart();
	for (unsigned i = 0; i < TEST_TAKES; ++i) {
		testTracker.LogStart(startText);
		test1(TEST_LOOPS, testTracker);
		testTracker.LogEnd(endText);
	}
	testTracker.LogEnd();
	testTracker.LogStart();
	for (unsigned i = 0; i < TEST_TAKES; ++i) {
		testTracker.LogStart(startText);
		test2(TEST_LOOPS, testTracker);
		testTracker.LogEnd(endText);
	}
	testTracker.LogEnd();


	std::cout << "finished!" << LINE_SEPARATOR;
	std::cout << testTracker << LINE_SEPARATOR;*/

	std::vector<int, STLManagedAllocator<int, 16>> vec;
	vec.push_back(10);
	vec.push_back(20);
	vec.push_back(30);
	vec.push_back(40);
	std::cout << &vec[0] << " : " << vec[0] << LINE_SEPARATOR;
	std::cout << &vec[1] << " : " << vec[1] << LINE_SEPARATOR;
	std::cout << &vec[2] << " : " << vec[2] << LINE_SEPARATOR;
	std::cout << &vec[3] << " : " << vec[3] << LINE_SEPARATOR;

	AlignmentWrapper<double, 12> test(10);

	std::cout << test.value << LINE_SEPARATOR;
	std::cout << sizeof(test.value) << LINE_SEPARATOR;
	std::cout << sizeof(test.padding) << LINE_SEPARATOR;
	std::cout << sizeof(test) << LINE_SEPARATOR;

	int x;
	std::cin >> x;

	return 0;
}
