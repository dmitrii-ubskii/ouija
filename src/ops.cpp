#include "ops.h"

OperatorResult moveCursor(OperatorArgs args)
{
	auto result = OperatorResult{.cursorMoved=true, .cursorPosition=args.cursorPosition};
	switch (args.key)
	{
		case ncurses::Key{'h'}: case ncurses::Key::Left:
			if (args.cursorPosition.list == 0)
			{
				return {};
			}

			result.cursorPosition.list--;
			if (args.lists[result.cursorPosition.list].cards.size() == 0)
			{
				result.cursorPosition.card = 0;
			}
			else
			{
				result.cursorPosition.card = std::min(
					result.cursorPosition.card,
					args.lists[result.cursorPosition.list].cards.size() - 1
				);
			}
			break;

		case ncurses::Key{'l'}: case ncurses::Key::Right:
			if (args.cursorPosition.list >= args.lists.size() - 1)
			{
				return {};
			}

			result.cursorPosition.list++;
			break;

		case ncurses::Key{'j'}: case ncurses::Key::Down:
			result.cursorPosition.card++;
			break;

		case ncurses::Key{'k'}: case ncurses::Key::Up:
			if (result.cursorPosition.card > 0)
			{
				result.cursorPosition.card--;
			}
			break;

		case ncurses::Key{'^'}:
			result.cursorPosition.card = 0;
			break;

		case ncurses::Key{'$'}:
			result.cursorPosition.card = args.lists[result.cursorPosition.list].cards.size();
			break;

		default:
			return {};
	}

	if (args.lists[result.cursorPosition.list].cards.size() == 0)
	{
		result.cursorPosition.card = 0;
	}
	else
	{
		result.cursorPosition.card = std::min(
			result.cursorPosition.card,
			args.lists[result.cursorPosition.list].cards.size() - 1
		);
	}

	return result;
}

