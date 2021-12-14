//
// Created by Vladimir Ivanov on 04.06.2021.
//

#ifndef GRAPH_MAGICK_HPP
#define GRAPH_MAGICK_HPP

#include <Magick++.h>
void display(const void* data, size_t length);

class Maze;
void displayMaze(const Maze& maze);

#endif //GRAPH_MAGICK_HPP
