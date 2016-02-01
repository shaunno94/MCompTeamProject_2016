#include "RebuildingOctreeSpacePartition.h"
#include <cassert>
#include <future>
#include "NCLDebug.h"
#include "CollisionDetection.h"
#include "Helpers/common.h"

bool CheckBoundingOverlap(GameObject* obj1, GameObject* obj2) {
	if (obj1->IsBoundingSphere()) {
		if (obj2->IsBoundingSphere())
			return CollisionDetection::CheckSphereOverlap(obj1->Physics()->GetPosition(), obj1->GetBoundingRadius(), obj2->Physics()->GetPosition(), obj2->GetBoundingRadius());

		const Vec3Physics& center = obj2->Physics()->GetPosition();
		const Vec3Physics& halfVolume = obj2->GetBoundingHalfVolume();
		return CollisionDetection::CheckAABBSphereOverlap(center - halfVolume, center + halfVolume, obj1->Physics()->GetPosition(), obj1->GetBoundingRadius());
	} else {
		if (obj2->IsBoundingSphere()) {
			const Vec3Physics& center = obj1->Physics()->GetPosition();
			const Vec3Physics& halfVolume = obj1->GetBoundingHalfVolume();
			return CollisionDetection::CheckAABBSphereOverlap(center - halfVolume, center + halfVolume, obj2->Physics()->GetPosition(), obj2->GetBoundingRadius());
		}

		return CollisionDetection::CheckAABBOverlap(obj1->Physics()->GetPosition(), obj1->GetBoundingHalfVolume(), obj2->Physics()->GetPosition(), obj2->GetBoundingHalfVolume());
	}
}

unsigned int RebuildingOctreeSpacePartition::Capacity = 20;
unsigned int RebuildingOctreeSpacePartition::MaxDepth = 7;

std::vector<RebuildingOctreeSpacePartition*> RebuildingOctreeSpacePartition::m_Leafs;
ObjectPool<RebuildingOctreeSubPartition> RebuildingOctreeSpacePartition::m_SubPartitionPool;
std::future<void> RebuildingOctreeSpacePartition::m_ClearHandle;
std::vector<CollisionPair> RebuildingOctreeSpacePartition::m_BroadphaseCollisionPairAssemblyThreadData[BROADPHASE_ASSEMBLY_THREADS];

#ifdef DEBUG_PHYSICS
#ifdef DEBUGDRAW_SPACEPARTITION

std::vector<SpacePartitionDebugLine> RebuildingOctreeSpacePartition::m_DebugLines[2];
unsigned int RebuildingOctreeSpacePartition::m_WritingDebugBufferIndex = 0;

#endif // DEBUGDRAW_SPACEPARTITION
#endif


RebuildingOctreeSpacePartition::RebuildingOctreeSpacePartition(const Vec3Physics& center, const Vec3Physics& halfVolume, unsigned int depth) :
	m_Center(center), m_HalfVolume(halfVolume), m_Depth(depth), m_SubPartitions(nullptr) {

	assert(("SpacePartition halfVolume had a negative or zero value.", m_HalfVolume.x > 0, m_HalfVolume.y > 0, m_HalfVolume.z > 0));
}

RebuildingOctreeSpacePartition::~RebuildingOctreeSpacePartition() {
}


void RebuildingOctreeSpacePartition::DebugDraw() {
#ifdef DEBUG_PHYSICS
#ifdef DEBUGDRAW_SPACEPARTITION
	for (const SpacePartitionDebugLine& line : m_DebugLines[(m_WritingDebugBufferIndex + 1) % 2])
		NCLDebug::DrawThickLine(line.first, line.second, 0.02f, Vec4Graphics::ONES);
#endif
#endif // DEBUG_PHYSICS
}

void RebuildingOctreeSpacePartition::BuildDebugLines() const {
	if (m_SubPartitions) {
		m_SubPartitions->space[0][0][0].BuildDebugLines();
		m_SubPartitions->space[1][0][0].BuildDebugLines();
		m_SubPartitions->space[0][1][0].BuildDebugLines();
		m_SubPartitions->space[1][1][0].BuildDebugLines();
		m_SubPartitions->space[0][0][1].BuildDebugLines();
		m_SubPartitions->space[1][0][1].BuildDebugLines();
		m_SubPartitions->space[0][1][1].BuildDebugLines();
		m_SubPartitions->space[1][1][1].BuildDebugLines();
		return;
	}
	std::vector<SpacePartitionDebugLine>& currentDebugBuffer = m_DebugLines[m_WritingDebugBufferIndex];

	Vec3Physics volume = m_HalfVolume * 2.0f;
	Vec3Physics volumeX = Vec3Physics(volume.x, 0.0f, 0.0f);
	Vec3Physics volumeZ = Vec3Physics(0.0f, 0.0f, volume.z);

	Vec3Physics corners[8];
	corners[0] = m_Center - m_HalfVolume;
	corners[1] = corners[0] + volumeX;
	corners[2] = corners[1] + volumeZ;
	corners[3] = corners[2] - volumeX;

	corners[4] = corners[0] + Vec3Physics(0.0f, volume.y, 0.0f);
	corners[5] = corners[4] + volumeX;
	corners[6] = corners[5] + volumeZ;
	corners[7] = corners[6] - volumeX;

	currentDebugBuffer.push_back(SpacePartitionDebugLine(corners[0], corners[1]));
	currentDebugBuffer.push_back(SpacePartitionDebugLine(corners[1], corners[2]));
	currentDebugBuffer.push_back(SpacePartitionDebugLine(corners[2], corners[3]));
	currentDebugBuffer.push_back(SpacePartitionDebugLine(corners[3], corners[0]));

	currentDebugBuffer.push_back(SpacePartitionDebugLine(corners[4], corners[5]));
	currentDebugBuffer.push_back(SpacePartitionDebugLine(corners[5], corners[6]));
	currentDebugBuffer.push_back(SpacePartitionDebugLine(corners[6], corners[7]));
	currentDebugBuffer.push_back(SpacePartitionDebugLine(corners[7], corners[4]));

	currentDebugBuffer.push_back(SpacePartitionDebugLine(corners[0], corners[4]));
	currentDebugBuffer.push_back(SpacePartitionDebugLine(corners[1], corners[5]));
	currentDebugBuffer.push_back(SpacePartitionDebugLine(corners[2], corners[6]));
	currentDebugBuffer.push_back(SpacePartitionDebugLine(corners[3], corners[7]));
}


void RebuildingOctreeSpacePartition::StartClear() {
	m_ClearHandle = std::move(std::async(std::launch::async, &RebuildingOctreeSpacePartition::Clear, this));
}

void RebuildingOctreeSpacePartition::EndClear() {
	if (m_ClearHandle.valid())
		m_ClearHandle.get();
}


void RebuildingOctreeSpacePartition::Clear() {

#ifdef  DEBUG_PHYSICS
#ifdef DEBUGDRAW_SPACEPARTITION
	m_WritingDebugBufferIndex = (m_WritingDebugBufferIndex + 1) % 2;
	m_DebugLines[m_WritingDebugBufferIndex].clear();

	for (RebuildingOctreeSpacePartition* leaf : m_Leafs)
		leaf->BuildDebugLines();
#endif
#endif //  DEBUG_PHYSICS

	m_Leafs.clear();
	m_Leafs.push_back(this);

	for (int i = 0; i < BROADPHASE_ASSEMBLY_THREADS; ++i)
		m_BroadphaseCollisionPairAssemblyThreadData[i].clear();

	Delete();
}


void RebuildingOctreeSpacePartition::BuildContainedCollisionPairs(std::vector<CollisionPair>& out) {
	for (auto occupantIt1 = m_Occupants.begin(); occupantIt1 != m_Occupants.end(); ++occupantIt1) {
		GameObject* occupantObj1 = *occupantIt1;
		for (auto occupantIt2 = occupantIt1 + 1; occupantIt2 != m_Occupants.end(); ++occupantIt2) {
			GameObject* occupantObj2 = *occupantIt2;
			if (CheckBoundingOverlap(occupantObj1, occupantObj2)) {
				out.push_back(CollisionPair(occupantObj1->Physics(), occupantObj2->Physics()));
				if (!occupantObj1->Physics()->AtRest())
					occupantObj2->Physics()->WakeUp();
				else if (!occupantObj2->Physics()->AtRest())
					occupantObj1->Physics()->WakeUp();
			}
		}
	}

	for (auto currentObj : m_Occupants) {
		for (auto overlapIt : m_Overlaps) {
			if (CheckBoundingOverlap(currentObj, overlapIt)) {
				out.push_back(CollisionPair(currentObj->Physics(), overlapIt->Physics()));
				if (!currentObj->Physics()->AtRest())
					overlapIt->Physics()->WakeUp();
				else if (!overlapIt->Physics()->AtRest())
					currentObj->Physics()->WakeUp();
			}
		}
	}
}

void RebuildingOctreeSpacePartition::BuildCollisionPairsBatch(unsigned int start, unsigned int end, std::vector<CollisionPair>* out) {
	for (unsigned int i = start; i < end; ++i)
		m_Leafs[i]->BuildContainedCollisionPairs(*out);
}

void RebuildingOctreeSpacePartition::BuildCollisionPairs(std::vector<CollisionPair>& out) {
	size_t leafCount = m_Leafs.size();
	size_t leafsInBatch = leafCount / BROADPHASE_ASSEMBLY_THREADS;

	std::future<void> asyncHandles[BROADPHASE_ASSEMBLY_THREADS];
	for (size_t i = 0; i < BROADPHASE_ASSEMBLY_THREADS; ++i) {
		size_t start = i * leafsInBatch;
		asyncHandles[i] = std::move(std::async(std::launch::async, &RebuildingOctreeSpacePartition::BuildCollisionPairsBatch, this, start, start + leafsInBatch, &m_BroadphaseCollisionPairAssemblyThreadData[i]));
	}

	BuildCollisionPairsBatch(BROADPHASE_ASSEMBLY_THREADS * leafsInBatch, leafCount, &out);

	for (int i = 0; i < BROADPHASE_ASSEMBLY_THREADS; ++i) {
		asyncHandles[i].get();
		out.insert(out.end(), m_BroadphaseCollisionPairAssemblyThreadData[i].begin(), m_BroadphaseCollisionPairAssemblyThreadData[i].end());
	}
}

void RebuildingOctreeSpacePartition::Set(const Vec3Physics& center, const Vec3Physics& halfVolume, unsigned int depth) {
	m_Center = center;
	m_HalfVolume = halfVolume;
	m_Depth = depth;
	m_SubPartitions = nullptr;
}

void RebuildingOctreeSpacePartition::Delete() {
	m_Occupants.clear();
	m_Overlaps.clear();
	if (m_SubPartitions)
		m_SubPartitionPool.Delete(m_SubPartitions);
	m_SubPartitions = nullptr;
}

bool RebuildingOctreeSpacePartition::CheckHasOverlap(GameObject* obj) const {
	if (obj->IsBoundingSphere())
		return (CollisionDetection::CheckAABBSphereOverlap(m_Center - m_HalfVolume, m_Center + m_HalfVolume, obj->Physics()->GetPosition(), obj->GetBoundingRadius()));
	else
		return CollisionDetection::CheckAABBOverlap(m_Center, m_HalfVolume, obj->Physics()->GetPosition(), obj->GetBoundingHalfVolume());
}


bool RebuildingOctreeSpacePartition::CheckIsOccupant(GameObject* obj) const {
	const Vec3Physics& minVol = m_Center - m_HalfVolume;
	const Vec3Physics& maxVol = m_Center + m_HalfVolume;
	const Vec3Physics& point = obj->Physics()->GetPosition();

	if (point.x >= minVol.x && point.x < maxVol.x &&
	    point.y >= minVol.y && point.y < maxVol.y &&
	    point.z >= minVol.z && point.z < maxVol.z)
		return true;

	return false;
}

void RebuildingOctreeSpacePartition::PushOverlap(GameObject* obj) {
	if (CheckHasOverlap(obj))
		m_Overlaps.push_back(obj);
}
void RebuildingOctreeSpacePartition::PushOccupant(GameObject* obj) {
	if (CheckIsOccupant(obj))
		m_Occupants.push_back(obj);
	else
		PushOverlap(obj);
}

void RebuildingOctreeSpacePartition::Push(GameObject* obj) {
	//check if object should be disregarded completely
	if (CheckHasOverlap(obj))
		//push objects onto tree leafs
		if (m_SubPartitions)
			m_SubPartitions->Push(obj);
		else {
			//partition further if conditions are met
			if ((m_Occupants.size() + 1) > Capacity && m_Depth < MaxDepth) {
				//get sub-partition from pool
				m_SubPartitions = m_SubPartitionPool.New();
				//update new parent and locations
				m_SubPartitions->Set(*this);

				//update leafs record
				SwapErase(m_Leafs, this);
				m_Leafs.push_back(&m_SubPartitions->space[0][0][0]);
				m_Leafs.push_back(&m_SubPartitions->space[1][0][0]);
				m_Leafs.push_back(&m_SubPartitions->space[0][1][0]);
				m_Leafs.push_back(&m_SubPartitions->space[1][1][0]);
				m_Leafs.push_back(&m_SubPartitions->space[0][0][1]);
				m_Leafs.push_back(&m_SubPartitions->space[1][0][1]);
				m_Leafs.push_back(&m_SubPartitions->space[0][1][1]);
				m_Leafs.push_back(&m_SubPartitions->space[1][1][1]);

				//push occupants to children (they can still be occupants or overlaps)
				for (GameObject* occupant : m_Occupants)
					m_SubPartitions->PushOccupant(occupant);
				m_Occupants.clear();
				//push overlaps to children (they can only be overlaps)
				for (GameObject* overlap : m_Overlaps)
					m_SubPartitions->PushOverlap(overlap);
				m_Overlaps.clear();

				//add new element (has to go through Push method because this can trigger further partitioning)
				m_SubPartitions->Push(obj);
			} else {
				//try upgrading overlap to occupant (overlap is already tested at the start of this method)
				if (CheckIsOccupant(obj))
					m_Occupants.push_back(obj);
				else
					m_Overlaps.push_back(obj);
			}
		}
}


void RebuildingOctreeSubPartition::Push(GameObject* obj) {
	space[0][0][0].Push(obj);
	space[0][0][1].Push(obj);
	space[0][1][0].Push(obj);
	space[0][1][1].Push(obj);
	space[1][0][0].Push(obj);
	space[1][0][1].Push(obj);
	space[1][1][0].Push(obj);
	space[1][1][1].Push(obj);
}

void RebuildingOctreeSubPartition::Set(RebuildingOctreeSpacePartition& parent) {
	Vec3Physics subSpaceHalfVolume = parent.m_HalfVolume * 0.5f;
	Vec3Physics firstSubCenter = parent.GetCenter() - subSpaceHalfVolume;
	unsigned int newDepth = parent.m_Depth + 1;

	space[0][0][0].Set(firstSubCenter, subSpaceHalfVolume, newDepth);
	space[0][0][1].Set(firstSubCenter + Vec3Physics(0.0f, 0.0f, parent.m_HalfVolume.z),								subSpaceHalfVolume,		newDepth);
	space[0][1][0].Set(firstSubCenter + Vec3Physics(0.0f, parent.m_HalfVolume.y, 0.0f),								subSpaceHalfVolume,		newDepth);
	space[0][1][1].Set(firstSubCenter + Vec3Physics(0.0f, parent.m_HalfVolume.y, parent.m_HalfVolume.z),				subSpaceHalfVolume,		newDepth);
	space[1][0][0].Set(firstSubCenter + Vec3Physics(parent.m_HalfVolume.x, 0.0f, 0.0f),								subSpaceHalfVolume,		newDepth);
	space[1][0][1].Set(firstSubCenter + Vec3Physics(parent.m_HalfVolume.x, 0.0f, parent.m_HalfVolume.z),				subSpaceHalfVolume,		newDepth);
	space[1][1][0].Set(firstSubCenter + Vec3Physics(parent.m_HalfVolume.x, parent.m_HalfVolume.y, 0.0f),				subSpaceHalfVolume,		newDepth);
	space[1][1][1].Set(firstSubCenter + Vec3Physics(parent.m_HalfVolume.x, parent.m_HalfVolume.y, parent.m_HalfVolume.z), subSpaceHalfVolume, newDepth);
}

void RebuildingOctreeSubPartition::Delete() {
	space[0][0][0].Delete();
	space[0][0][1].Delete();
	space[0][1][0].Delete();
	space[0][1][1].Delete();
	space[1][0][0].Delete();
	space[1][0][1].Delete();
	space[1][1][0].Delete();
	space[1][1][1].Delete();
}

void RebuildingOctreeSubPartition::PushOverlap(GameObject* obj) {
	space[0][0][0].PushOverlap(obj);
	space[0][0][1].PushOverlap(obj);
	space[0][1][0].PushOverlap(obj);
	space[0][1][1].PushOverlap(obj);
	space[1][0][0].PushOverlap(obj);
	space[1][0][1].PushOverlap(obj);
	space[1][1][0].PushOverlap(obj);
	space[1][1][1].PushOverlap(obj);
}
void RebuildingOctreeSubPartition::PushOccupant(GameObject* obj) {
	space[0][0][0].PushOccupant(obj);
	space[0][0][1].PushOccupant(obj);
	space[0][1][0].PushOccupant(obj);
	space[0][1][1].PushOccupant(obj);
	space[1][0][0].PushOccupant(obj);
	space[1][0][1].PushOccupant(obj);
	space[1][1][0].PushOccupant(obj);
	space[1][1][1].PushOccupant(obj);
}
