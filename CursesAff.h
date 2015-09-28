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

#ifndef	CURSES_AFF_HH
#define	CURSES_AFF_HH

#include       <curses.h>
#include	"Map.h"

#define UP_ARROW 259
#define DOWN_ARROW 258
#define RIGHT_ARROW 261
#define LEFT_ARROW 260
#define SHIFT_TAB 353

void	initCurses();
void	endCurses();
void	affMapCurses(Map *map);
int	handleCh(Map *map);

#endif
