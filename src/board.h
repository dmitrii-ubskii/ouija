#ifndef SRC_BOARD_H_
#define SRC_BOARD_H_

#include <filesystem>
#include <string>
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

	void showMessage(std::string const&);

	bool doQuit{false};

	struct CursorPosition
	{
		std::size_t list;
		std::size_t card;
	} cursorPosition{0, 0};

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
