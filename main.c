/*
  Copyright (c) 2014, Matthias Gatto
  All rights reserved.
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
  * Neither the name of Matthias Gatto nor the
  names of its contributors may be used to endorse or promote products
  derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include	<string.h>
#include	<stdlib.h>
#include	"CursesAff.h"
#include	"Map.h"

#define NORMAL_MODE 1
#define	SLOW_MODE -1
#define	FAST_MODE 3

int	score = 0;
int	mode = NORMAL_MODE;

int	getBitdFlag(int pos)
{
  return ((1 << pos));
}

void	initMap(Map *map)
{
  int i;
  for (i = 0; i < MAP_W_SIZE  + MAP_THRESHOLD; ++i)
    {
      map->map[i] = BORDER;
    }
  map->birdPosY = 9;
  map->bird = getBitdFlag(map->birdPosY);
  map->nbrBonus = 0;
}

void	initCurses()
{
  initscr();
  noecho();
  cbreak();
  //noraw();
  //nodelay(1);
  keypad(stdscr, TRUE);
  timeout(0);
  curs_set(0);
  start_color();
  init_pair(1, COLOR_BLACK, COLOR_WHITE);
}

void	endCurses()
{
  endwin();
}

int	getRandDecalage()
{
  int ret = (rand() % 2) + 1;

  if (!(rand() % 4))
    return (0);
  if (rand() % 2)
    return (ret);
  return (-ret);
}

void	addPipe(Map *map, int pos)
{
  int pipe = NPIPE;
  static int posPipe = 6;

  posPipe += getRandDecalage();
  if (posPipe <= 0)
    posPipe = 0;
  else if (posPipe >= MAP_H_SIZE - SIZE_HOLE)
    posPipe = MAP_H_SIZE - SIZE_HOLE;
  if (pos >= (MAP_W_SIZE + MAP_THRESHOLD))
    return ;
  //printw("pipe: 0x%08x\n", pipe << 4);
  map->map[pos] = ~(pipe << posPipe);
}

void	affMapCurses(Map *map)
{
  int i;
  int i2;
  char line[MAP_H_SIZE + 1];

  move(0,0);
  /* printw("bird: 0x%08x\n", map->bird); */
  /* for (i = 0; i < MAP_H_SIZE; ++i) */
  /*   { */
  /*     printw("map col:%d: 0x%08x\n", i, map->map[i]); */
  /*   } */
  printw("Score: %d\n", score);
  for (i2 = MAP_H_SIZE - 1; i2 >= 0; --i2)
    {
      strncpy(line, "                                                                                                          ", MAP_W_SIZE);
      line[MAP_W_SIZE] = 0;
      for (i = 0; i < MAP_W_SIZE; ++i)
	{
	  if ((1 << i2) & map->map[i])
	    line[i] = '#';
	}
      if (i2 == map->birdPosY)
	line[BIRDPOSX] = '>';
      printw("%s\n", line);
    }
  refresh();
}

void	waitForUnpause()
{
  printw("the Game is in pause\n");
  while (getch() != 'p');
  erase();
}

char	waitForSomething(const char *str)
{
  int i = 0;
  char ch = getch();

  for (i = 0; i < strlen(str); ++i)
    {
      if (ch == str[i])
	return (ch);
    }
}

int	handleCh(Map *map)
{
  int	hasDirBeenPress = 0;
  int	ch;

  while ((ch = getch()) != ERR)
    {
      if (ch == UP_ARROW && !hasDirBeenPress)
	{
	  map->birdPosY += 1;
	  map->bird = getBitdFlag(map->birdPosY);
	  hasDirBeenPress = 1;
	}
      else if (ch == DOWN_ARROW && !hasDirBeenPress)
	{
	  map->birdPosY -= 1;
	  map->bird = getBitdFlag(map->birdPosY);
	  hasDirBeenPress = 1;
	}
      else if (ch == 'q')
	{
	  endCurses();
	  exit(1);
	}
      else if (ch == LEFT_ARROW)
	mode = SLOW_MODE;
      else if (ch == RIGHT_ARROW)
	mode = FAST_MODE;
      else if (ch == 'p')
	waitForUnpause();
    }
}

void	rollingMap(Map *map)
{
  int	i;

  for (i = 0;i < MAP_W_SIZE + MAP_THRESHOLD - 1; ++i)
    {
      map->map[i] = map->map[i + 1];
    }
  addPipe(map, MAP_W_SIZE + MAP_THRESHOLD - 1);
}

int	checkCol(Map *map)
{
  return (map->bird & map->map[BIRDPOSX]);
}


static int	askToReplay()
{
  char ret;
  move(0,0);
  erase();
  printw("You just lose the game with %d point, do you want to replay ?\n"
	 "Press Y if you want to try again, N if you don't", score);
  ret = waitForSomething("yYnN");
  if (ret == 'y' || ret == 'Y')
    return (1);
  else if (ret == 'n' || ret == 'N')
    return (0);
  usleep(200000);
  askToReplay();
}

static int	getScoreModifier()
{
  return (mode);
}

static int	getSpeedModifier(int speed)
{
  if (mode == NORMAL_MODE)
    return (speed);
  if (mode == SLOW_MODE)
    return (speed * 2);
  if (mode == FAST_MODE)
    return (speed / 2);
}

static int	doGame()
{
  Map	map;
  int	speed = 150000;

  srand(time(0));
  initMap(&map);
  while (!checkCol(&map))
    {
      handleCh(&map);
      score += getScoreModifier();
      if (speed > 40000)
	speed -= 300;
      // sleep
      rollingMap(&map);
      affMapCurses(&map);
      usleep(getSpeedModifier(speed));
      affMapCurses(&map);
      mode = NORMAL_MODE;
    }
  return (askToReplay());
}

int	main()
{
  initCurses();
  while (doGame())
    score = 0;
  endCurses();
  return (0);
}
