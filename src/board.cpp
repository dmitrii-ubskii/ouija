#include "board.h"

#include <cassert>
#include <fstream>
#include <iostream>

Board::Board()
	: boardView{ncurses::Rect{{}, {0, context.get_height() - 1}}}
	, statusLine{ncurses::Rect{{0, context.get_height() - 1}, {}}}
{
	listViews.push_back(ncurses::Window{boardView.get_rect()});
	context.raw(true);
	context.refresh();
	repaint();
}

std::string getParagraph(std::ifstream& input)
{
	auto paragraph = std::string{};
	for (std::string lineBuffer; std::getline(input, lineBuffer); )
	{
		if (paragraph != "")
		{
			if (lineBuffer == "")
			{
				break;
			}
			paragraph += " " + lineBuffer;
		}
		else if (lineBuffer == "")
		{
			continue;
		}
		else
		{
			paragraph = lineBuffer;
		}
	}

	return paragraph;
}

void Board::open(std::filesystem::path filePath)
{
	auto fileHandler = std::ifstream{filePath};

	auto expectList = true;

	lists.clear();
	while (true)
	{
		auto paragraph = getParagraph(fileHandler);
		if (paragraph == "")
		{
			break;
		}

		if (expectList)
		{
			if (not paragraph.starts_with("# "))
			{
				std::cerr << "Expected list title, got '" << paragraph.substr(0, 20) << "'...";
				return;
			}
			expectList = false;
		}

		if (paragraph.starts_with("# "))
		{
			lists.push_back({paragraph.substr(2)});
		}
		else if (paragraph.starts_with("## "))
		{
			lists.back().cards.push_back({paragraph.substr(3)});
		}
		else
		{
			auto& lastCard = lists.back().cards.back();
			if (lastCard.summary == "")
			{
				lastCard.summary = paragraph;
			}
			else
			{
				lastCard.description.push_back(paragraph);
			}
		}
	}

	listViews.clear();
	for (auto i = 0ul; i < lists.size(); i++)
	{
		listViews.emplace_back(ncurses::Rect{{0, 0}, {1, boardView.get_height()}});
		listViews.back().set_color(ncurses::Color::LightCyan, ncurses::Color::Black);
	}
	resizeLists();
	repaint();
}

void Board::dump() const
{
	for (auto const& list: lists)
	{
		std::cout << "# " << list.title << "\n\n";
		for (auto const& card: list.cards)
		{
			std::cout << "## " << card.title << "\n\n";
			if (card.summary != "")
			{
				std::cout << card.summary << "\n\n";
				if (not card.description.empty())
				{
					for (auto const& line: card.description)
					{
						std::cout << line << "\n\n";
					}
				}
			}
			else
			{
				assert(card.description.empty());
			}
		}
	}
}

void Board::resizeLists()
{
	auto numLists = static_cast<int>(lists.size());
	auto listWidth = (boardView.get_width() - (numLists - 1)) / numLists;
	auto x = 0;

	for (auto& view: listViews)
	{
		view.resize({listWidth, boardView.get_height()});
		view.move_to({x, 0});
		x += listWidth + 1;
	}
}

void Board::onResize()
{
	auto termSize = context.get_size();
	boardView.resize({termSize.w, std::max(1, termSize.h - 1)});
	statusLine.resize({termSize.w, 1});
	statusLine.move_to({0, termSize.h - 1});

	resizeLists();
}

void Board::repaint()
{
	boardView.erase();
	boardView.refresh();
	for (auto i = 0ul; i < lists.size(); i++)
	{
		auto& list = lists[i];
		auto& view = listViews[i];
		view.erase();
		view.add_attributes({ncurses::Attribute::Reverse});
		view.mvaddnstr({0, 0}, list.title, view.get_width());
		while (view.get_cursor().x > 0)
		{
			view.addstr(" ");
		}
		if (list.title.length() > static_cast<std::size_t>(view.get_width()))
		{
			view.mvaddstr({view.get_width() - 1, 0}, ">");
		}
		view.remove_attributes({ncurses::Attribute::Reverse});
		auto y = 1;
		for (auto const& card: list.cards)
		{
			view.mvaddnstr({0, y}, card.title, view.get_width());
			if (card.title.length() > static_cast<std::size_t>(view.get_width()))
			{
				view.mvaddstr({view.get_width() - 1, y}, ">");
			}
			y++;
		}
		view.refresh();
	}
	statusLine.erase();
	statusLine.mvaddstr(
		{},
		std::to_string(context.get_width()) + "x" + std::to_string(context.get_height()) + " | " +
		std::to_string(boardView.get_width()) + "x" + std::to_string(boardView.get_height()) + " | " +
		std::to_string(statusLine.get_width()) + "x" + std::to_string(statusLine.get_height())
	);
	statusLine.refresh();
}

int Board::mainLoop()
{
	while (not doQuit)
	{
		ncurses::Key ch;
		ch = statusLine.getch();
		if (ch == ncurses::Key::Ctrl({'c'}))  // Ctrl+C
		{
			break;
		}
		if (ch == ncurses::Key::Resize)
		{
			onResize();
		}
		repaint();
	}
	return 0;
}
