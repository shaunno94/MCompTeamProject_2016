#pragma once

#include <vector>
#include "GameObject.h"
#include <future>
#include "../Helpers/ObjectPool.h"
#include "constants.h"
#include "OctreeSpacePartition.h"

class RebuildingOctreeSubPartition;

typedef std::pair<Vec3Physics, Vec3Physics> SpacePartitionDebugLine;

class RebuildingOctreeSpacePartition {
 protected:
	friend class RebuildingOctreeSubPartition;

	static ObjectPool<RebuildingOctreeSubPartition> m_SubPartitionPool;
	static std::vector<RebuildingOctreeSpacePartition*>m_Leafs;
	static std::future<void> m_ClearHandle;
	static std::vector<CollisionPair> m_BroadphaseCollisionPairAssemblyThreadData[BROADPHASE_ASSEMBLY_THREADS];

#ifdef DEBUG_PHYSICS
#ifdef DEBUGDRAW_SPACEPARTITION
	static unsigned int m_WritingDebugBufferIndex;
	static std::vector<SpacePartitionDebugLine> m_DebugLines[2];
#endif
#endif // DEBUG_PHYSICS

	Vec3Physics m_Center;
	Vec3Physics m_HalfVolume;

	unsigned int m_Depth;
	RebuildingOctreeSubPartition* m_SubPartitions;

	std::vector<GameObject*> m_Occupants;
	std::vector<GameObject*> m_Overlaps;


	RebuildingOctreeSpacePartition() {}

	void Clear();

	void PushOverlap(GameObject* obj);
	void PushOccupant(GameObject* obj);

	bool CheckIsOccupant(GameObject* obj) const;
	bool CheckHasOverlap(GameObject* obj) const;

	void BuildContainedCollisionPairs(std::vector<CollisionPair>& out);

	void BuildCollisionPairsBatch(unsigned int start, unsigned int end, std::vector<CollisionPair>* out);

	void BuildDebugLines() const;

 public:
	static void DebugDraw();

	static unsigned int Capacity;
	static unsigned int MaxDepth;

	RebuildingOctreeSpacePartition(const Vec3Physics& center, const Vec3Physics& halfVolume, unsigned int depth = 0);

	~RebuildingOctreeSpacePartition();


	inline const Vec3Physics& GetHalfVolume() const { return m_HalfVolume; }
	inline const Vec3Physics& GetCenter() const { return m_Center; }

	void Push(GameObject* obj);

	void Set(const Vec3Physics& center, const Vec3Physics& halfVolume, unsigned int depth);
	void Delete();

	void StartClear();
	void EndClear();

	void BuildCollisionPairs(std::vector<CollisionPair>& out);
};

class RebuildingOctreeSubPartition {
 public:
	RebuildingOctreeSubPartition() {}

	void Set(RebuildingOctreeSpacePartition& parent);

	RebuildingOctreeSpacePartition space[2][2][2];

	void Delete();

	void Push(GameObject* obj);
	void PushOverlap(GameObject* obj);
	void PushOccupant(GameObject* obj);
};