#ifndef SRC_BOARD_H_
#define SRC_BOARD_H_

#include <filesystem>
#include <vector>

#include "ncursespp/ncurses.h"
#include "ncursespp/window.h"

class Board
{
public:
	Board();

	void open(std::filesystem::path);
	void dump() const;

	int mainLoop();

private:
	ncurses::Ncurses context{};

	ncurses::Window boardView;
	ncurses::Window statusLine;

	void repaint();

	void resizeLists();
	void onResize();

	bool doQuit{false};

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
	};
	std::vector<List> lists{{"New list"}};
	std::vector<ncurses::Window> listViews{};
};

#endif // SRC_BOARD_H_
