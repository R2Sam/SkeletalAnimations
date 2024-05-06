#pragma once
#include "log.h"
#include "MyRaylib.h"

struct Joint;

struct Segment
{
	Texture2D texture;
	Vector2 offset;
	float scale;
	float rotation;

	Vector2 position;
	float length;
	float angle;

	std::vector<Joint*> joints;

	~Segment()
	{
		if (IsTextureReady(texture))
		{
			UnloadTexture(texture);
		}
	}
};

struct Joint
{
	Vector2 position;
	std::vector<Segment*> Segmets;
};

class Skeleton
{
private:

	std::vector<Segment> _segments;
	std::vector<Joint> _joints;

public:

	Skeleton();
	~Skeleton();

	std::vector<Segment*> GetSegments();
	void SetSegments(const std::vector<Segment>& segments);
	void AddSegment(const Segment& segment);

	std::vector<Joint*> GetJoints();
	void SetJoints(const std::vector<Joint>& joints);
	void AddJoint(const Joint& joint);

	void DrawTextures(const float& opacity);
	void DrawSegments(const float& opacity);
	void DrawJoints(const float& opacity);
};