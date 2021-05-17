#include "board.h"

#include <cassert>
#include <fstream>
#include <iostream>

#include "ncursespp/color.h"

Board::Board()
	: boardView{ncurses::Rect{{}, {0, context.get_height() - 1}}}
	, statusLine{ncurses::Rect{{0, context.get_height() - 1}, {}}}
{
	lists.back().width = boardView.get_width();
	context.register_palette(ncurses::Palette{1}, ncurses::Color::LightCyan, ncurses::Color::Black);
	boardView.set_palette(ncurses::Palette{1});
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

	for (auto& list: lists)
	{
		list.width = listWidth;
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
	auto x = 0;
	for (auto i = 0ul; i < lists.size(); i++)
	{
		auto& list = lists[i];
		boardView.mvaddnstr({x, 0}, list.title, list.width);
		if (list.title.length() > static_cast<std::size_t>(list.width))
		{
			boardView.mvaddstr({x + list.width - 1, 0}, ">");
		}
		boardView.set_attributes_in_rect({ncurses::Attribute::Reverse}, {{x, 0}, {list.width, 1}});

		auto y = 1;
		for (auto const& card: list.cards)
		{
			boardView.mvaddnstr({x, y}, card.title, list.width);
			if (card.title.length() > static_cast<std::size_t>(list.width))
			{
				boardView.mvaddstr({x + list.width - 1, y}, ">");
			}
			y++;
		}

		if (i == cursorPosition.list)
		{
			boardView.set_attributes_in_rect(
				{ncurses::Attribute::Reverse},
				{{x, static_cast<int>(cursorPosition.card) + 1}, {list.width, 1}}
			);
		}
		x += list.width + 1;
	}
	boardView.refresh();

	statusLine.erase();
	statusLine.addstr(
		std::to_string(cursorPosition.list) + ", " + std::to_string(cursorPosition.card)
	);
	statusLine.refresh();
}

void Board::showMessage(std::string const& message)
{
	statusLine.erase();
	statusLine.addstr(message);
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

		if (ch == 'h')
		{
			if (cursorPosition.list > 0)
			{
				cursorPosition.list--;
				if (lists[cursorPosition.list].cards.size() == 0)
				{
					cursorPosition.card = 0;
				}
				else
				{
					cursorPosition.card = std::min(cursorPosition.card, lists[cursorPosition.list].cards.size() - 1);
				}
			}
		}
		if (ch == 'l')
		{
			if (cursorPosition.list + 1 < lists.size())
			{
				cursorPosition.list++;
				if (lists[cursorPosition.list].cards.size() == 0)
				{
					cursorPosition.card = 0;
				}
				else
				{
					cursorPosition.card = std::min(cursorPosition.card, lists[cursorPosition.list].cards.size() - 1);
				}
			}
		}
		if (ch == 'j')
		{
			if (cursorPosition.card + 1 < lists[cursorPosition.list].cards.size())
			{
				cursorPosition.card++;
			}
		}
		if (ch == 'k')
		{
			if (cursorPosition.card > 0)
			{
				cursorPosition.card--;
			}
		}

		if (ch == ncurses::Key::Resize)
		{
			onResize();
		}
		repaint();
	}
	return 0;
}
