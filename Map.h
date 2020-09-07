/* Copyright 2015 Matthias Gatto
 *
 * This file is part of Curses Bird.
 *
 * Curses Bird is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as published
 * by the Free Software Foundation.
 *
 * Curses Bird is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Butterfly.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef	MAP_HH
#define MAP_HH

#include "circular-buffer.h"

#define	MAP_W_SIZE 23
#define	MAP_H_SIZE 25
#define BORDER 0xFFF00000
#define NPIPE 0x000000FF // oposite of a pipe is ~PIPE
#define SIZE_HOLE 8
#define BIRDPOSX 5
#define MAP_THRESHOLD 4
#define Max_BONUS 8

REGISTER_CB(map_cb, int, MAP_W_SIZE + MAP_THRESHOLD)

typedef struct	{
  struct map_cb map;
  int	bonus[8];
  int	nbrBonus;
  int	bird;
  int	birdPosY;
} Map;

void	initMap(Map *map);
int	getBitdFlag(int pos);
void	addPipe(Map *map, int pos);
void	rollingMap(Map *map);

#endif
