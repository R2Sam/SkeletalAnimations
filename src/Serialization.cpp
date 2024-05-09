#include "Serialization.h"

std::vector<SegmentData> SerializeSegments(const std::vector<Segment*> segments)
{
	std::vector<SegmentData> data;

	for (Segment* segment: segments)
	{
		SegmentData segmentData;

		segmentData.ID = segment->ID;
	    segmentData.textureName = segment->textureName;
	    segmentData.offsetX = segment->offset.x;
	    segmentData.offsetY = segment->offset.y;
	    segmentData.scale = segment->scale;
	    segmentData.rotation = segment->rotation;
	    segmentData.positionX = segment->position.x;
	    segmentData.positionY = segment->position.y;
	    segmentData.topX = segment->topPos.x;
	    segmentData.topY = segment->topPos.y;
	    segmentData.bottomX = segment->bottomPos.x;
	    segmentData.bottomY = segment->bottomPos.y;
	    segmentData.angle = segment->angle;

	    segmentData.jointTop = segment->jointTopID;
	    segmentData.jointBottom = segment->jointBottomID;

	    data.push_back(segmentData);
	}

	return data;
}

std::vector<Segment> DeSerializeSegments(const std::vector<SegmentData>& data)
{
	std::vector<Segment> segments;

	for (const SegmentData& segmentData: data)
	{
		Segment segment;

        segment.ID = segmentData.ID;
        segment.textureName = segmentData.textureName;
        segment.offset.x = segmentData.offsetX;
        segment.offset.y = segmentData.offsetY;
        segment.scale = segmentData.scale;
        segment.rotation = segmentData.rotation;
        segment.position.x = segmentData.positionX;
        segment.position.y = segmentData.positionY;
        segment.topPos.x = segmentData.topX;
        segment.topPos.y = segmentData.topY;
        segment.bottomPos.x = segmentData.bottomX;
        segment.bottomPos.y = segmentData.bottomY;
        segment.angle = segmentData.angle;

        segment.jointTopID = segmentData.jointTop;
        segment.jointBottomID = segmentData.jointBottom;

        segments.push_back(segment);
	}

	return segments;
}

std::vector<JointData> SerializeJoints(const std::vector<Joint*> joints)
{
	std::vector<JointData> data;

	for (Joint* joint : joints)
	{
		JointData jointData;

		jointData.ID = joint->ID;
		jointData.positionX = joint->position.x;
		jointData.positionY = joint->position.y;

		for (Segment* segment : joint->segments)
		{
			jointData.segments.push_back(segment->ID);
		}

		data.push_back(jointData);
	}

	return data;
}

std::vector<Joint> DeSerializeJoints(const std::vector<JointData>& data)
{
	std::vector<Joint> joints;

    for (const JointData& jointData : data)
    {
        Joint joint;

        joint.ID = jointData.ID;
        joint.position.x = jointData.positionX;
        joint.position.y = jointData.positionY;

       	for (const unsigned int& id : jointData.segments)
       	{
       		joint.segmentIDs.push_back(id);
       	}

       	joints.push_back(joint);
    }

    return joints;
}

SkeletonData SerializeSkeleton(Skeleton* skeleton)
{
	SkeletonData skeletonData;

	skeletonData.segments = SerializeSegments(skeleton->GetSegments());
	skeletonData.joints = SerializeJoints(skeleton->GetJoints());

	return skeletonData;
}

Skeleton DeSerializeSkeleton(const SkeletonData& data)
{
	Skeleton skeleton;

	skeleton.SetSegments(DeSerializeSegments(data.segments));
	skeleton.SetJoints(DeSerializeJoints(data.joints));

	for (Segment* segment : skeleton.GetSegments())
	{
		for (Joint* joint : skeleton.GetJoints())
		{
			static unsigned int count = 0;
			if (segment->jointTopID == joint->ID)
			{
				segment->jointTop = joint;
				count++;
			}

			if (segment->jointBottomID == joint->ID)
			{
				segment->jointBottom = joint;
				count++;
			}

			if (count == 2)
			{
				break;
			}
		}
	}

	for (Joint* joint : skeleton.GetJoints())
	{
		for (Segment* segment : skeleton.GetSegments())
		{
			for (const unsigned int id : joint->segmentIDs)
			{
				if (id == segment->ID)
				{
					joint->segments.push_back(segment);
					break;
				}
			}
		}
	}

	return skeleton;
}