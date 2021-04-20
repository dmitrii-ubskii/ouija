#include "board.h"

int main()
{
	Board board;
	board.open("example_boards/example.kanban");
	board.dump();

	return 0;
}
