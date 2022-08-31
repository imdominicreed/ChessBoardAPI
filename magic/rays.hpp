#pragma once
#include "../util/bitutil.hpp"

enum {
  NORTH,
  SOUTH,
  EAST,
  WEST,
  NORTH_WEST,
  NORTH_EAST,
  SOUTH_WEST,
  SOUTH_EAST
};
unsigned long long get_ray(int sq, int dir);
void init_rays();
