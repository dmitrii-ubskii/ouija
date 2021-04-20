#ifndef SRC_BOARD_H_
#define SRC_BOARD_H_

#include <filesystem>
#include <vector>

class Board
{
public:
	void open(std::filesystem::path);

	void dump() const;

private:
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
	std::vector<List> lists;
};

#endif // SRC_BOARD_H_
