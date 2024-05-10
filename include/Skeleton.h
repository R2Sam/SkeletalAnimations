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
	Rectangle recTop;
	Rectangle recBot;
	float thinkness;
	std::pair<Vector2, Vector2> topCorners;
	std::pair<Vector2, Vector2> bottomCorners;
	float angle;

	bool selected = false;
	bool selectedTop = false;
	bool selectedBottom = false;

	unsigned int jointTopID = 0;
	unsigned int jointBottomID = 0;
};

void  UpdateSegment(Segment& segment);

struct Joint
{
	unsigned int ID;
	bool selected = false;

	Vector2 position;
	Rectangle rec;
};

class Skeleton;

class Skeleton
{
private:

	std::vector<Segment> _segments;
	std::vector<Joint> _joints;

public:

	unsigned int segmentsIDindex = 0;
	unsigned int jointsIDindex = 0;

	Skeleton();
	Skeleton(Skeleton* skeleton);
	~Skeleton();

	std::vector<Segment*> GetSegments();
	void SetSegments(const std::vector<Segment>& segments);
	Segment* AddSegment(Segment& segment);

	std::vector<Joint*> GetJoints();
	void SetJoints(const std::vector<Joint>& joints);
	Joint* AddJoint(Joint& joint);

	void DrawTextures(const unsigned int& opacity);
	void DrawSegments(const unsigned int& opacity);
	void DrawJoints(const unsigned int& opacity);
};