#include "Skeleton.h"
#include "Serialization.h"

void UpdateSegment(Segment& segment)
{
	segment.position = Vector2Scale(Vector2Add(segment.bottomPos, segment.topPos), 0.5);
	segment.angle = AngleFromPos(segment.bottomPos, segment.topPos);
	segment.topCorners.first = Vector2Add(Vector2Rot(segment.thinkness / 2, segment.angle - 90), segment.topPos);
	segment.topCorners.second = Vector2Add(Vector2Rot(segment.thinkness / 2, segment.angle + 90), segment.topPos);
	segment.bottomCorners.first = Vector2Add(Vector2Rot(segment.thinkness / 2, segment.angle - 90), segment.bottomPos);
	segment.bottomCorners.second = Vector2Add(Vector2Rot(segment.thinkness / 2, segment.angle + 90), segment.bottomPos);
}

Skeleton::Skeleton()
{
	
}

Skeleton::Skeleton(Skeleton* skeleton)
{
	*this = DeSerializeSkeleton(SerializeSkeleton(skeleton));
}

Skeleton::~Skeleton()
{

}

std::vector<Segment*> Skeleton::GetSegments()
{
	std::vector<Segment*> segments;

	for (Segment& segment : _segments)
	{
		segments.push_back(&segment);
	}

	return segments;
}

void Skeleton::SetSegments(const std::vector<Segment>& segments)
{
	_segments = segments;
}

Segment* Skeleton::AddSegment(Segment& segment)
{
	segmentsIDindex++;
	segment.ID = segmentsIDindex;
	_segments.push_back(segment);
	return &(_segments[_segments.size() - 1]);
}

std::vector<Joint*> Skeleton::GetJoints()
{
	std::vector<Joint*> joints;

	for (Joint& joint : _joints)
	{
		joints.push_back(&joint);
	}

	return joints;
}

void Skeleton::SetJoints(const std::vector<Joint>& joints)
{
	_joints = joints;
}

Joint* Skeleton::AddJoint(Joint& joint)
{
	jointsIDindex++;
	joint.ID = jointsIDindex;
	_joints.push_back(joint);
	return &(_joints[_joints.size() - 1]);
}

void Skeleton::DrawTextures(const unsigned int& opacity)
{
	for (Segment& segment : _segments)
	{
		if (IsTextureReady(segment.texture))
		{
			DrawTextureRotScale(segment.texture, Vector2Add(segment.position, segment.offset), segment.angle + segment.rotation, segment.scale, Color{255, 255, 255, opacity});
		}
	}
}

void Skeleton::DrawSegments(const unsigned int& opacity)
{
	for (Segment& segment : _segments)
	{
		if (segment.selectedTop)
		{
			segment.recTop = {0, 0, 10, 10};
		}

		else
		{
			segment.recTop = {0, 0, 5, 5};
		}

		DrawRectangleRec(CenteredRectangle(segment.recTop, segment.topPos), Color{255, 0, 0, opacity});

		if (segment.selectedBottom)
		{
			segment.recBot = {0, 0, 10, 10};
		}

		else
		{
			segment.recBot = {0, 0, 5, 5};
		}

		DrawRectangleRec(CenteredRectangle(segment.recBot, segment.bottomPos), Color{255, 0, 0, opacity});

		if (segment.selected)
		{
			segment.thinkness = 5;
		}

		else
		{
			segment.thinkness = 2.5;
		}

		DrawLineEx(segment.topPos, segment.bottomPos, segment.thinkness, Color{255, 0, 0, opacity});
	}
}

void Skeleton::DrawJoints(const unsigned int& opacity)
{
	for (Joint& joint : _joints)
	{
		if (joint.selected)
		{
			joint.rec = {0, 0, 10, 10};
		}

		else
		{
			joint.rec = {0, 0, 5, 5};
		}
		DrawRectangleRec(CenteredRectangle(joint.rec, joint.position), Color{0, 0, 255, opacity});
	}
}