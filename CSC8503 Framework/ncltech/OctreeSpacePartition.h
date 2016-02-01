#pragma once

#include <vector>
#include "GameObject.h"
#include <future>
#include "../Helpers/ObjectPool.h"
#include "constants.h"

typedef std::pair<PhysicsObject*, PhysicsObject*> CollisionPair;
typedef std::pair<Vec3Physics, Vec3Physics> SpacePartitionDebugLine;

class OctreeSubPartition;

class OctreeSpacePartition {
 protected:
	Vec3Physics m_Center;
	Vec3Physics m_HalfVolume;

	friend class OctreeSubPartition;

	static ObjectPool<OctreeSubPartition> m_SubPartitionPool;
	static std::future<void> m_DebugHandle;
	static std::vector<CollisionPair> m_CollisionPairs;
	//static unsigned int m_WorkingWakeupArrayIndex;
	//static std::vector<GameObject*> m_WakeupArray[2];

#ifdef DEBUG_PHYSICS
#ifdef DEBUGDRAW_SPACEPARTITION
	static unsigned int m_WritingDebugBufferIndex;
	static std::vector<SpacePartitionDebugLine> m_DebugLines[2];
#endif
#endif // DEBUG_PHYSICS
	size_t m_Population;

	unsigned int m_Depth;
	OctreeSubPartition* m_SubPartitions;

	OctreeSpacePartition() {}

	static bool CheckBoundingOverlap(GameObject* obj1, GameObject* obj2);


	bool CheckCanFit(GameObject* obj) const;
	bool CheckOverlap(GameObject* obj) const;

	/// <summary>
	/// Builds collision pairs for the current partition
	/// </summary>
	/// <param name="obj"></param>
	void BuildLocalCollisions(GameObject* obj);

	/// <summary>
	/// Builds collision pairs for current partition and any children
	/// </summary>
	/// <param name="obj"></param>
	void BuildCollisions(GameObject* obj);

	/// <summary>
	/// Stores debug lines of the current state
	/// </summary>
	void BuildDebugLines() const;

	/// <summary>
	/// Tries to partition current population into the children
	/// </summary>
	void MoveDown();

	void PushToChildren(GameObject* obj);

	/// <summary>
	/// Set method for pooling
	/// </summary>
	/// <param name="center"></param>
	/// <param name="halfVolume"></param>
	/// <param name="depth"></param>
	void Set(const Vec3Physics& center, const Vec3Physics& halfVolume, unsigned int depth);
	/// <summary>
	/// Delete method for pooling
	/// </summary>
	void Delete();

	void BuildDebug();

public:
	/// <summary>
	/// Attempt to place object without creating collision pairs
	/// </summary>
	/// <param name="obj"></param>
	/// <returns></returns>
	bool Move(GameObject* obj);

	std::vector<GameObject*> m_Items;

	bool Remove(GameObject* obj);
	inline size_t Population() { return m_Population; }

	void StartBuildDebug();
	void EndBuildDebug();

	void DebugDraw();

	//unsigned int GetDepth(unsigned int size = 0);

	static unsigned int Capacity;
	static unsigned int MaxDepth;

	OctreeSpacePartition(const Vec3Physics& center, const Vec3Physics& halfVolume, unsigned int depth = 0);

	~OctreeSpacePartition();

	inline std::vector<CollisionPair>& GetCollisionPairs() { return m_CollisionPairs; }

	inline const Vec3Physics& GetHalfVolume() const { return m_HalfVolume; }
	inline const Vec3Physics& GetCenter() const { return m_Center; }

	bool Push(GameObject* obj);

	MEM_ALIGN_NEW_DELETE
};

/// <summary>
/// Octree space partition children structure
/// </summary>
class OctreeSubPartition {
 public:
	OctreeSubPartition() {}

	void Set(OctreeSpacePartition& parent);

	OctreeSpacePartition space[2][2][2];

	void Delete();
	bool Remove(GameObject* obj);

	size_t Population();

	bool Push(GameObject* obj);
	void BuildCollisions(GameObject* obj);
	bool Move(GameObject* obj);
};