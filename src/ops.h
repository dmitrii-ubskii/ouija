#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include "ncursespp/keys.h"

#include "board.h"

struct OperatorArgs
{
	ncurses::Key const key;

	Board::CursorPosition const cursorPosition;
	std::vector<Board::List> const& lists;
};

struct OperatorResult
{
	bool cursorMoved{false};
	Board::CursorPosition cursorPosition{0, 0};

	std::string message{""};
};

using OperatorFunction = OperatorResult(*)(OperatorArgs args);

OperatorResult moveCursor(OperatorArgs args);

static auto const normalOps = std::unordered_map<ncurses::Key, OperatorFunction>{
	{ncurses::Key{'h'}, moveCursor},
	{ncurses::Key{'j'}, moveCursor},
	{ncurses::Key{'k'}, moveCursor},
	{ncurses::Key{'l'}, moveCursor},
	{ncurses::Key::Left, moveCursor},
	{ncurses::Key::Down, moveCursor},
	{ncurses::Key::Up, moveCursor},
	{ncurses::Key::Right, moveCursor},
	{ncurses::Key{'$'}, moveCursor},
	{ncurses::Key{'^'}, moveCursor},
};
