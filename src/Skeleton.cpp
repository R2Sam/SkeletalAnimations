#include "Skeleton.h"
#include "Serialization.h"

void UpdateSegment(Segment& segment)
{
	segment.position = Vector2Scale(Vector2Add(segment.bottomPos, segment.topPos), 0.5);
	segment.angle = AngleFromPos(segment.bottomPos, segment.topPos);
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

Segment* Skeleton::AddSegment(const Segment& segment)
{
	_segments.push_back(segment);
	return &_segments[_segments.size() - 1];
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

Joint* Skeleton::AddJoint(const Joint& joint)
{
	_joints.push_back(joint);
	return &_joints[_joints.size() - 1];
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

		Rectangle rec = {0, 0, 5, 5};
		rec = CenteredRectangle(rec, segment.topPos);
		DrawRectangleRec(rec, Color{255, 0, 0, opacity});

		rec = CenteredRectangle(rec, segment.bottomPos);
		DrawRectangleRec(rec, Color{255, 0, 0, opacity});

		DrawLineEx(segment.topPos, segment.bottomPos, 2.5, Color{255, 0, 0, opacity});
	}
}

void Skeleton::DrawJoints(const unsigned int& opacity)
{
	for (Joint& joint : _joints)
	{
		if (joint.selected)
		{
			Rectangle rec = {0, 0, 10, 10};
			rec = CenteredRectangle(rec, joint.position);

			DrawRectangleRec(rec, Color{0, 0, 255, opacity});
		}

		else
		{
			Rectangle rec = {0, 0, 2.5, 2.5};
			rec = CenteredRectangle(rec, joint.position);

			DrawRectangleRec(rec, Color{0, 0, 255, opacity});
		}
	}
}