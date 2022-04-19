#ifndef SRC_BOARD_H_
#define SRC_BOARD_H_

#include <filesystem>
#include <string>
#include <vector>

#include "ncursespp/keys.h"
#include "ncursespp/ncurses.h"
#include "ncursespp/window.h"

class Board
{
public:
	struct CursorPosition
	{
		std::size_t list;
		std::size_t card;
	};

	struct Card
	{
		std::string title;
		std::string summary{};
		std::vector<std::string> description{};
	};
	struct List
	{
		std::string title;
		std::vector<Card> cards{};
		int width{0};
	};

	Board();

	void open(std::filesystem::path);
	void dump() const;

	int mainLoop();

private:
	ncurses::Ncurses context{};

	ncurses::Window boardView;
	ncurses::Window statusLine;

	void handleKey(ncurses::Key);
	void repaint();

	void resizeLists();
	void onResize();

	void showMessage(std::string const&);

	bool doQuit{false};

	CursorPosition cursorPosition{0, 0};
	std::vector<List> lists{{"New list"}};
};

#endif // SRC_BOARD_H_
