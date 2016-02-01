#include "OctreeSpacePartition.h"

#include "CollisionDetection.h"
#include "NCLDebug.h"

unsigned int OctreeSpacePartition::Capacity = 20;
unsigned int OctreeSpacePartition::MaxDepth = 8;

ObjectPool<OctreeSubPartition> OctreeSpacePartition::m_SubPartitionPool;
std::future<void> OctreeSpacePartition::m_DebugHandle;
std::vector<CollisionPair> OctreeSpacePartition::m_CollisionPairs;

#ifdef DEBUG_PHYSICS
#ifdef DEBUGDRAW_SPACEPARTITION

std::vector<SpacePartitionDebugLine> OctreeSpacePartition::m_DebugLines[2];
unsigned int OctreeSpacePartition::m_WritingDebugBufferIndex = 0;

#endif // DEBUGDRAW_SPACEPARTITION
#endif

bool OctreeSpacePartition::CheckBoundingOverlap(GameObject* obj1, GameObject* obj2) {
	if (obj1->IsBoundingSphere()) {
		if (obj2->IsBoundingSphere())
			return CollisionDetection::CheckSphereOverlap(obj1->Physics()->GetPosition(), obj1->GetBoundingRadius(), obj2->Physics()->GetPosition(), obj2->GetBoundingRadius());

		return CollisionDetection::CheckAABBSphereOverlap(obj2->Physics()->GetPosition(), obj2->GetBoundingHalfVolume(), obj1->Physics()->GetPosition(), obj1->GetBoundingRadius());
	} else {
		if (obj2->IsBoundingSphere())
			return CollisionDetection::CheckAABBSphereOverlap(obj1->Physics()->GetPosition(), obj1->GetBoundingHalfVolume(), obj2->Physics()->GetPosition(), obj2->GetBoundingRadius());

		return CollisionDetection::CheckAABBOverlap(obj1->Physics()->GetPosition(), obj1->GetBoundingHalfVolume(), obj2->Physics()->GetPosition(), obj2->GetBoundingHalfVolume());
	}
}

OctreeSpacePartition::OctreeSpacePartition(const Vec3Physics& center, const Vec3Physics& halfVolume, unsigned int depth) :
	m_Center(center), m_HalfVolume(halfVolume), m_Depth(depth), m_SubPartitions(nullptr) {

	assert(("SpacePartition halfVolume had a negative or zero value.", m_HalfVolume.x > 0, m_HalfVolume.y > 0, m_HalfVolume.z > 0));

	m_Population = 0;
}

OctreeSpacePartition::~OctreeSpacePartition(void) {
}

bool OctreeSpacePartition::CheckOverlap(GameObject* obj) const {
	if (obj->IsBoundingSphere())
		return (CollisionDetection::CheckAABBSphereOverlap(m_Center, m_HalfVolume, obj->Physics()->GetPosition(), obj->GetBoundingRadius()));
	else
		return CollisionDetection::CheckAABBOverlap(m_Center, m_HalfVolume, obj->Physics()->GetPosition(), obj->GetBoundingHalfVolume());
}

bool OctreeSpacePartition::CheckCanFit(GameObject* obj) const {
	if (obj->IsBoundingSphere())
		return CollisionDetection::CheckSphereInsideAABB(obj->Physics()->GetPosition(), obj->GetBoundingRadius(), m_Center, m_HalfVolume);
	else
		return CollisionDetection::CheckAABBInsideAABB(obj->Physics()->GetPosition(), obj->GetBoundingHalfVolume(), m_Center, m_HalfVolume);
}


void OctreeSpacePartition::BuildLocalCollisions(GameObject* obj) {
	for (GameObject*& go : m_Items) {
		if (CheckBoundingOverlap(go, obj))
			m_CollisionPairs.push_back(CollisionPair(go->Physics(), obj->Physics()));
	}
}

void OctreeSpacePartition::MoveDown() {
	for (GameObject*& go : m_Items) {
		if (m_SubPartitions->Move(go))
			go = nullptr;
	}
	m_Items.erase(std::remove(begin(m_Items), end(m_Items), nullptr), end(m_Items));
}

bool OctreeSpacePartition::Move(GameObject* obj) {
	if (CheckCanFit(obj)) {
		++m_Population;
		if (m_SubPartitions) {
			if (m_SubPartitions->Move(obj))
				return true;
		}
		m_Items.push_back(obj);
		return true;
	}
	return false;
}

void OctreeSpacePartition::BuildCollisions(GameObject* obj) {
	if (CheckOverlap(obj)) {
		BuildLocalCollisions(obj);
		if (m_SubPartitions)
			m_SubPartitions->BuildCollisions(obj);
	}
}

void OctreeSpacePartition::PushToChildren(GameObject* obj) {
	BuildLocalCollisions(obj);
	m_SubPartitions->BuildCollisions(obj);
	//store locally if children can not fit the object
	if (!m_SubPartitions->Push(obj))
		m_Items.push_back(obj);
}

bool OctreeSpacePartition::Push(GameObject* obj) {
	if (CheckCanFit(obj)) {
		++m_Population;
		if (m_SubPartitions)
			PushToChildren(obj);
		else {
			//attempt to partition further
			if ((m_Items.size() + 1) > Capacity && m_Depth < MaxDepth) {
				m_SubPartitions = m_SubPartitionPool.New();
				//update new parent and locations
				m_SubPartitions->Set(*this);
				MoveDown();
				PushToChildren(obj);
			} else {
				BuildLocalCollisions(obj);
				m_Items.push_back(obj);
			}
		}
		return true;
	}
	return false;
}


bool OctreeSpacePartition::Remove(GameObject* obj) {
	if (CheckCanFit(obj)) {
		size_t childrenPopulation = 0;
		if (m_SubPartitions) {
			if (!m_SubPartitions->Remove(obj))
				FindSwapErase(m_Items, obj);
			childrenPopulation = m_SubPartitions->Population();
			//cleanup empty leafs
			if (childrenPopulation == 0) {
				m_SubPartitionPool.Delete(m_SubPartitions);
				m_SubPartitions = nullptr;
			}
		} else
			FindSwapErase(m_Items, obj);
		//reset population measure
		m_Population = m_Items.size() + childrenPopulation;
		return true;
	}
	return false;
}

void OctreeSpacePartition::Set(const Vec3Physics& center, const Vec3Physics& halfVolume, unsigned int depth) {
	m_Center = center;
	m_HalfVolume = halfVolume;
	m_Depth = depth;
	m_SubPartitions = nullptr;
	m_Population = 0;
}

void OctreeSpacePartition::Delete() {
	m_Items.clear();
	if (m_SubPartitions)
		m_SubPartitionPool.Delete(m_SubPartitions);
	m_SubPartitions = nullptr;
	m_Population = 0;
}


void OctreeSpacePartition::BuildDebug() {
#ifdef  DEBUG_PHYSICS
#ifdef DEBUGDRAW_SPACEPARTITION
	m_WritingDebugBufferIndex = (m_WritingDebugBufferIndex + 1) % 2;
	m_DebugLines[m_WritingDebugBufferIndex].clear();
	BuildDebugLines();
#endif
#endif //  DEBUG_PHYSICS
}

void OctreeSpacePartition::StartBuildDebug() {
#ifdef  DEBUG_PHYSICS
#ifdef DEBUGDRAW_SPACEPARTITION
	m_DebugHandle = std::move(std::async(std::launch::async, &OctreeSpacePartition::BuildDebug, this));
#endif
#endif //  DEBUG_PHYSICS
}
void OctreeSpacePartition::EndBuildDebug() {
#ifdef  DEBUG_PHYSICS
#ifdef DEBUGDRAW_SPACEPARTITION
	if (m_DebugHandle.valid())
		m_DebugHandle.get();
#endif
#endif //  DEBUG_PHYSICS
}

void OctreeSpacePartition::DebugDraw() {
#ifdef DEBUG_PHYSICS
#ifdef DEBUGDRAW_SPACEPARTITION
	for (const SpacePartitionDebugLine& line : m_DebugLines[(m_WritingDebugBufferIndex + 1) % 2])
		NCLDebug::DrawThickLine(line.first, line.second, 0.05f, Vec4Graphics::ONES);
#endif
#endif // DEBUG_PHYSICS
}

void OctreeSpacePartition::BuildDebugLines() const {
	//only build lines on the leaf nodes
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


//unsigned int OctreeSpacePartition::GetDepth(unsigned int size) {
//	++size;
//	if (m_SubPartitions) {
//		size =
//		  std::max(
//		    m_SubPartitions->space[0][0][0].GetDepth(size),
//		    std::max(
//		      m_SubPartitions->space[1][0][0].GetDepth(size),
//		      std::max(
//		        m_SubPartitions->space[0][1][0].GetDepth(size),
//		        std::max(
//		          m_SubPartitions->space[1][1][0].GetDepth(size),
//		          std::max(
//		            m_SubPartitions->space[0][0][1].GetDepth(size),
//		            std::max(
//		              m_SubPartitions->space[1][0][1].GetDepth(size),
//		              std::max(
//		                m_SubPartitions->space[0][1][1].GetDepth(size),
//		                m_SubPartitions->space[1][1][1].GetDepth(size)
//		              )
//		            )
//		          )
//		        )
//		      )
//		    )
//		  );
//	}
//	return size;
//}


bool OctreeSubPartition::Push(GameObject* obj) {
	bool result = space[0][0][0].Push(obj);
	result = result || space[0][0][1].Push(obj);
	result = result || space[0][1][0].Push(obj);
	result = result || space[0][1][1].Push(obj);
	result = result || space[1][0][0].Push(obj);
	result = result || space[1][0][1].Push(obj);
	result = result || space[1][1][0].Push(obj);
	result = result || space[1][1][1].Push(obj);
	return result;
}

void OctreeSubPartition::Set(OctreeSpacePartition& parent) {
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

void OctreeSubPartition::Delete() {
	space[0][0][0].Delete();
	space[0][0][1].Delete();
	space[0][1][0].Delete();
	space[0][1][1].Delete();
	space[1][0][0].Delete();
	space[1][0][1].Delete();
	space[1][1][0].Delete();
	space[1][1][1].Delete();
}

bool OctreeSubPartition::Remove(GameObject* obj) {
	bool result = space[0][0][0].Remove(obj);
	result = result || space[0][0][1].Remove(obj);
	result = result || space[0][1][0].Remove(obj);
	result = result || space[0][1][1].Remove(obj);
	result = result || space[1][0][0].Remove(obj);
	result = result || space[1][0][1].Remove(obj);
	result = result || space[1][1][0].Remove(obj);
	result = result || space[1][1][1].Remove(obj);
	return result;
}


void OctreeSubPartition::BuildCollisions(GameObject* obj) {
	space[0][0][0].BuildCollisions(obj);
	space[0][0][1].BuildCollisions(obj);
	space[0][1][0].BuildCollisions(obj);
	space[0][1][1].BuildCollisions(obj);
	space[1][0][0].BuildCollisions(obj);
	space[1][0][1].BuildCollisions(obj);
	space[1][1][0].BuildCollisions(obj);
	space[1][1][1].BuildCollisions(obj);
}

bool OctreeSubPartition::Move(GameObject* obj) {
	bool result = space[0][0][0].Move(obj);
	result = result || space[0][0][1].Move(obj);
	result = result || space[0][1][0].Move(obj);
	result = result || space[0][1][1].Move(obj);
	result = result || space[1][0][0].Move(obj);
	result = result || space[1][0][1].Move(obj);
	result = result || space[1][1][0].Move(obj);
	result = result || space[1][1][1].Move(obj);
	return result;
}
size_t OctreeSubPartition::Population() {
	return
	  space[0][0][0].Population() +
	  space[0][0][1].Population() +
	  space[0][1][0].Population() +
	  space[0][1][1].Population() +
	  space[1][0][0].Population() +
	  space[1][0][1].Population() +
	  space[1][1][0].Population() +
	  space[1][1][1].Population();
}
