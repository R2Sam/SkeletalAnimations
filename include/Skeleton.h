#pragma once
#include "log.h"
#include "MyRaylib.h"

struct Joint;

struct Segment
{
	unsigned int ID;

	Texture2D texture;
	std::string textureName;
	Vector2 offset;
	float scale;
	float rotation;

	Vector2 position;
	Vector2 topPos;
	Vector2 bottomPos;
	float angle;

	Joint* jointTop = nullptr;
	Joint* jointBottom = nullptr;
	unsigned int jointTopID;
	unsigned int jointBottomID;
};

void  UpdateSegment(Segment& segment);

struct Joint
{
	unsigned int ID;
	bool selected = false;

	Vector2 position;
	std::vector<Segment*> segments;
	std::vector<unsigned int> segmentIDs;
};

class Skeleton;

class Skeleton
{
private:

	std::vector<Segment> _segments;
	std::vector<Joint> _joints;

public:

	Skeleton();
	Skeleton(Skeleton* skeleton);
	~Skeleton();

	std::vector<Segment*> GetSegments();
	void SetSegments(const std::vector<Segment>& segments);
	Segment* AddSegment(const Segment& segment);

	std::vector<Joint*> GetJoints();
	void SetJoints(const std::vector<Joint>& joints);
	Joint* AddJoint(const Joint& joint);

	void DrawTextures(const unsigned int& opacity);
	void DrawSegments(const unsigned int& opacity);
	void DrawJoints(const unsigned int& opacity);
};