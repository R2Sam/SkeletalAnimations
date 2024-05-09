#pragma once
#include "log.h"
#include "MyRaylib.h"

#include "cereal/archives/binary.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/vector.hpp"

#include "Skeleton.h"

struct SegmentData
{
	unsigned int ID;

	std::string textureName;
	float offsetX;
	float offsetY;
	float scale;
	float rotation;

	float positionX;
	float positionY;
	float topX;
	float topY;
	float bottomX;
	float bottomY;
	float angle;

	unsigned int jointTop;
	unsigned int jointBottom;

	template<class Archive>
	void serialize(Archive& ar)
	{
	    ar(ID, textureName, offsetX, offsetY, scale, rotation, positionX, positionY, topX, topY, bottomX, bottomY, angle, jointTop, jointBottom);
	}
};

struct JointData
{
	unsigned int ID;

	float positionX;
	float positionY;

	std::vector<unsigned int> segments;

	template<class Archive>
	void serialize(Archive& ar)
	{
	    ar(ID, positionX, positionY, segments);
	}
};

struct SkeletonData
{
	std::vector<SegmentData> segments;
	std::vector<JointData> joints;

	template<class Archive>
	void serialize(Archive& ar)
	{
	    ar(segments, joints);
	}
};

std::vector<SegmentData> SerializeSegments(const std::vector<Segment*> segments);
std::vector<Segment> DeSerializeSegments(const std::vector<SegmentData>& data);

std::vector<JointData> SerializeJoints(const std::vector<Joint*> joints);
std::vector<Joint> DeSerializeJoints(const std::vector<JointData>& data);

SkeletonData SerializeSkeleton(Skeleton* skeleton);
Skeleton DeSerializeSkeleton(const SkeletonData& data);