/**
 * Robot - cleaner: shall clean (ie move over) all available zones (cells) in a room
 */

#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <map>
#include <set>

#include "trace.h"

using namespace std;

struct Coor {
	size_t x = 0;
	size_t y = 0;
};

ostream& operator<<(ostream& os, const Coor& c) {
	return os << '(' << c.y << ',' << c.x << ')';
}

class Room {
public:
	using Map = vector<string>;
	explicit Room(vector<string> input)
		: size{input[0].length(), input.size()}
		, map_(std::move(input))
	{}

	bool isAvailable(Coor pos) const {
		return pos.x < size.x &&
				pos.y < size.y &&
				map_[pos.y][pos.x] != 'x';
	}

	auto copyMap() const { return map_; }

	const Coor size;
private:
	Map map_;
};

class RobotInTheRoom {
public:
	RobotInTheRoom(Room room, Coor start);
	void turn();
	bool move(); // update coor
	void doClean(); // provide visible output
private:
	const Room room_;
	Coor pos_;
	enum class Dir {E, S, W, N};
	Dir dir_;
	Room::Map state_;

	char& stateAt(Coor pos) { return state_[pos.y][pos.x]; }
	void markState();
};

RobotInTheRoom::RobotInTheRoom(Room room, Coor start)
	: room_(std::move(room))
	, pos_(std::move(start))
	, dir_(Dir::N) // TODO use random
	, state_(room_.copyMap())
	{}

void RobotInTheRoom::turn() {
	dir_ = static_cast<Dir>((static_cast<int>(dir_) + 1) % 4);
}

namespace {
[[maybe_unused]]
void printState(const Room::Map& state) {
	cout << "----\n";
	for (auto& s : state) cout << s << endl;
	cout << "----\n";
}
}

void RobotInTheRoom::markState() {
	assert(room_.isAvailable(pos_));
//	auto xy = stateAt(pos_);
	if (state_[pos_.y][pos_.x] == ' ')
		state_[pos_.y][pos_.x] = '1';
	else state_[pos_.y][pos_.x]++;
}

void RobotInTheRoom::doClean() {
	assert(room_.isAvailable(pos_));
	assert(state_[pos_.y][pos_.x] == ' ' || state_[pos_.y][pos_.x] == '1');
//	if(state_[pos_.y][pos_.x] != ' ') throw logic_error("state_[pos_.y][pos_.x] != ' '");
//	state_[pos_.y][pos_.x] = '*';
//	cout << "doClean>" << pos_ << endl;
//	printState(state_);
}

bool RobotInTheRoom::move() {
	switch (dir_) {
		case Dir::E:
			if (!room_.isAvailable({pos_.x + 1, pos_.y})) return false;
			pos_.x += 1;
			break;
		case Dir::S:
			if (!room_.isAvailable({pos_.x, pos_.y + 1})) return false;
			pos_.y += 1;
			break;
		case Dir::W:
			if (!room_.isAvailable({pos_.x - 1, pos_.y})) return false; // overflow is OK
			pos_.x -= 1;
			break;
		case Dir::N:
			if (!room_.isAvailable({pos_.x, pos_.y - 1})) return false;
			pos_.y -= 1;
			break;
	}
	cout << pos_ << endl;
	getchar();
	markState();
	printState(state_);
	return true;
}

using Set = set<pair<int, int>>;
Set visited;
using Pos = pair<int, int>;

ostream& operator<<(ostream& os, const Pos& c) {
	return os << '[' << c.first << ',' << c.second << ']';
}
void run(RobotInTheRoom& r, Pos pos, int dir = 0) {
	r.doClean();
	for (auto direction = 0; direction != 4; ++direction) {
		Pos next = (dir == 0 ? Pos{pos.first + 1, pos.second} :
		            (dir == 1 ? Pos{pos.first, pos.second + 1} :
		             (dir == 2 ? Pos{pos.first - 1, pos.second} :
		              Pos{pos.first, pos.second - 1})
		            )
		);
		auto [it, inserted] = visited.insert(next);
//		cout << next << "; " << (inserted ? "true" : "false") << endl;
		if (inserted && r.move()) {
			run(r, next, dir);
			r.turn();
			r.turn();
		}
		r.turn();
		dir = (dir + 1) % 4;
	}
	r.turn();
	r.turn();
	r.move(); // get back
}

void test() {
	vector<string> room_input = {
			"          ",
			"  x xx    ",
	        "xx x xx   ",
	        "  xxx x   ",
	        "x    x   x",
	        "x xx x   x",
	        "   x      ",
	};
//	vector<string> room_input = {
//			"        ",
//			"  x  x  ",
//	        "   x    ",
//	        "  xxx   ",
//	        "  xxx  x",
//	        "   x    ",
//	};

	Room room(room_input);
	Coor start = {0, 0};

	RobotInTheRoom rr(room, start);
	run(rr, {start.x, start.y});
}


int main() {
	test();
	return 0;
}