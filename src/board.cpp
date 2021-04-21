#include "board.h"

#include <cassert>
#include <fstream>
#include <iostream>

Board::Board()
	: boardView{ncurses::Rect{{}, {0, context.get_height() - 1}}}
	, statusLine{ncurses::Rect{{0, context.get_height() - 1}, {}}}
{
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

void Board::repaint()
{
	boardView.erase();
	boardView.refresh();
	statusLine.erase();
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
		repaint();
	}
	return 0;
}
