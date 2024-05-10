#include "LogWindow.h"

LogWindow::LogWindow()
{

}

LogWindow::~LogWindow()
{

}

void LogWindow::Init(const LogWindowRectangle& logWindowRectangle, const unsigned int& fontSize, const Color& fontColor)
{
	_logWindowRectangle = logWindowRectangle;
	_rectangle = CenteredRectangle(_logWindowRectangle.windowRec, _logWindowRectangle.pos);
	_fontSize = fontSize;
	_fontColor = fontColor;

	SetTextLineSpacing(_fontSize);
	_visibleLinesMax = (_logWindowRectangle.windowRec.height - _logWindowRectangle.yMargin * 2) / _fontSize;
}

void LogWindow::Init(const LogWindowRectangle& logWindowRectangle, const unsigned int& fontSize, const Color& fontColor, const Font& font)
{
	_logWindowRectangle = logWindowRectangle;
	_rectangle = CenteredRectangle(_logWindowRectangle.windowRec, _logWindowRectangle.pos);
	_fontSize = fontSize;
	_fontColor = fontColor;
	_font = font;
	_customFont = true;

	SetTextLineSpacing(_fontSize);
	_visibleLinesMax = (_logWindowRectangle.windowRec.height - _logWindowRectangle.yMargin * 2) / _fontSize;
}

void LogWindow::SetRec(const LogWindowRectangle& logWindowRectangle)
{
	_logWindowRectangle = logWindowRectangle;
	_rectangle = CenteredRectangle(_logWindowRectangle.windowRec, _logWindowRectangle.pos);

	_visibleLinesMax = (_logWindowRectangle.windowRec.height - _logWindowRectangle.yMargin * 2) / _fontSize;
}

void LogWindow::ParseLine(const std::string& line)
{
	std::vector<std::string> words = WordList(line);
	
	std::string currentLine;

	for (const std::string& word : words)
	{
		std::string potentialLine = currentLine + word + " ";

		if (!_customFont)
		{
			if (MeasureText(potentialLine.c_str(), _fontSize) > _rectangle.width - _logWindowRectangle.xMargin)
			{
				_logMessages.push_back(currentLine);
				currentLine = word + " ";
			}

			else
			{
				currentLine = potentialLine;
			}
		}

		else if (MeasureTextEx( _font, potentialLine.c_str(), _fontSize, _fontSize).x > _rectangle.width - _logWindowRectangle.xMargin)
		{
			_logMessages.push_back(currentLine);
			currentLine = word + " ";
		}

		else
		{
			currentLine = potentialLine;
		}
	}

	if (!currentLine.empty())
	{
		_logMessages.push_back(currentLine);
	}
}

void LogWindow::AddMessage(const std::string& message)
{
	std::string line;
	for (const char& c : message)
	{
		line += c;

		if (c == '\n')
		{
			line.pop_back();
			ParseLine(line);

			line.clear();
		}
	}

	if (!line.empty())
	{
		ParseLine(line);
	}

	if (_logMessages.size() > 1)
	{
		if (_logMessages[_logMessages.size() - 2][0] == '>')
		{
			_logMessages[_logMessages.size() - 2].erase(_logMessages[_logMessages.size() - 2].begin());
			_logMessages[_logMessages.size() - 2].shrink_to_fit();
		}
	}

	_logMessages[_logMessages.size() - 1].insert(_logMessages[_logMessages.size() - 1].begin(), '>');

	if (_logMessages.size() > _visibleLinesMax)
	{
		_logMessagesToShow.clear();
		_logMessagesToShow.shrink_to_fit();

		for (int i = _logMessages.size() - _visibleLinesMax; i < _logMessages.size(); i++)
		{
			_logMessagesToShow.push_back(_logMessages[i]);
		}
	}

	else
	{
		_logMessagesToShow = _logMessages;
	}

	_scrollPosition = _logMessages.size();
}

void LogWindow::Clear()
{
	_logMessages.clear();
	_logMessagesToShow.clear();
}

void LogWindow::Update(const Vector2& mousePos ,const float& mouseScroll)
{
	if (CheckCollisionPointRec(mousePos, _rectangle) && mouseScroll != 0 && _logMessages.size() > _logMessagesToShow.size())
	{
		unsigned int newScrollPos = _scrollPosition;

		if (mouseScroll < 0)
		{
			newScrollPos++;
			newScrollPos = std::clamp(newScrollPos,  _visibleLinesMax, (unsigned int)_logMessages.size());

			if (newScrollPos != _scrollPosition)
			{
				_scrollPosition = newScrollPos;

				_logMessagesToShow.push_back(_logMessages[_scrollPosition - 1]);
				_logMessagesToShow.erase(_logMessagesToShow.begin());
				_logMessagesToShow.shrink_to_fit();
			}
		}

		else
		{
			newScrollPos--;
			newScrollPos = std::clamp(newScrollPos,  _visibleLinesMax, (unsigned int)_logMessages.size());

			if (newScrollPos != _scrollPosition)
			{
				_scrollPosition = newScrollPos;

				_logMessagesToShow.insert(_logMessagesToShow.begin(), _logMessages[_scrollPosition - _visibleLinesMax]);
				_logMessagesToShow.pop_back();
				_logMessagesToShow.shrink_to_fit();
			}
		}
	}
}

void LogWindow::Draw()
{
	std::string totalText;

	for (const std::string& line : _logMessagesToShow)
	{
		totalText += line + " \n";
	}

	DrawRectangleRec(_rectangle, _logWindowRectangle.color);

	if (!_customFont)
	{
		DrawText(totalText.c_str(), _rectangle.x + _logWindowRectangle.xMargin, _rectangle.y + _logWindowRectangle.xMargin, _fontSize, _fontColor);
	}
	
	else
	{
		DrawTextEx(_font, totalText.c_str(), Vector2{_rectangle.x + _logWindowRectangle.xMargin, _rectangle.y + _logWindowRectangle.xMargin}, _fontSize, _fontSize, _fontColor);
	}
}