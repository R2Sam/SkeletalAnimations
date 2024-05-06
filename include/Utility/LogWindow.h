#pragma once
#include "log.h"
#include "MyRaylib.h"

struct LogWindowRectangle
{
	Rectangle windowRec;
	Vector2 pos;
	Color color;
	unsigned int xMargin;
	unsigned int yMargin;
};

class LogWindow
{
private:

	LogWindowRectangle _logWindowRectangle;
	Rectangle _rectangle;

	unsigned int _fontSize;
	unsigned int _visibleLinesMax;
	unsigned int  _scrollPosition;

	Font _font;
	Color _fontColor;
	bool _customFont = false;

	std::vector<std::string> _logMessages;
	std::vector<std::string> _logMessagesToShow;

	void ParseLine(const std::string& line);

public:

	LogWindow();
	~LogWindow();

	void Init(const LogWindowRectangle& logWindowRectangle, const unsigned int& fontSize, const Color& fontColor);
	void Init(const LogWindowRectangle& logWindowRectangle, const unsigned int& fontSize, const Color& fontColor, const Font& font);

	void SetRec(const LogWindowRectangle& logWindowRectangle);

	void AddMessage(const std::string& message);
	void Clear();

	void Update(const Vector2& mousePos ,const float& mouseScroll);
	void Draw();
};