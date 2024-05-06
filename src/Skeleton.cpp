#include "Skeleton.h"

Skeleton::Skeleton()
{

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

void Skeleton::AddSegment(const Segment& segment)
{
	_segments.push_back(segment);
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

void Skeleton::AddJoint(const Joint& joint)
{
	_joints.push_back(joint);
}

void Skeleton::DrawTextures(const float& opacity)
{
	for (Segment& segment : _segments)
	{
		DrawTextureRotScale(segment.texture, Vector2Add(segment.position, segment.offset), segment.rotation, segment.scale, Color{255, 255, 255, opacity});
	}
}

void Skeleton::DrawSegments(const float& opacity)
{
	for (Segment& segment : _segments)
	{
		float rotRad = DEG2RAD * segment.angle;
		Vector2 direction = {sinf(rotRad), -cosf(rotRad)};

		Vector2 pos1 = Vector2Add(segment.position, Vector2Scale(direction, segment.length / 2));
		Vector2 pos2 = Vector2Subtract(segment.position, Vector2Scale(direction, segment.length / 2));

		DrawLineEx(pos1, pos2, 10, Color{255, 0, 0, opacity});
	}
}

void Skeleton::DrawJoints(const float& opacity)
{
	for (Joint& joint : _joints)
	{
		Rectangle rec = {0, 0, 10};
		rec = CenteredRectangle(rec, joint.position);
		DrawRectangleRec(rec, Color{0, 0, 255, opacity});
	}
}