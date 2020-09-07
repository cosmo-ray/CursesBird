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

#include	<unistd.h>
#include	<string.h>
#include	<stdlib.h>
#include	<time.h>
#include        <stdio.h>
#include        <errno.h>
#include	"CursesAff.h"
#include	"Map.h"

#define NORMAL_MODE 1
#define	SLOW_MODE -1
#define	FAST_MODE 3

int	score = 0;
int	mode = NORMAL_MODE;

int	getBirdFlag(int pos)
{
  return ((1 << pos));
}

void	initMap(Map *map)
{
  int i;

  cb_init(map->map, map_cb);
  for (i = 0; i < MAP_W_SIZE  + MAP_THRESHOLD; ++i)
    {
      cb_set(map->map, i, BORDER);
    }
  map->birdPosY = 9;
  map->bird = getBirdFlag(map->birdPosY);
  map->nbrBonus = 0;
}

void	initCurses()
{
  initscr();
  noecho();
  cbreak();
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

int	getRandGap()
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

  posPipe += getRandGap();
  if (posPipe <= 0)
    posPipe = 0;
  else if (posPipe >= MAP_H_SIZE - SIZE_HOLE)
    posPipe = MAP_H_SIZE - SIZE_HOLE;
  if (pos >= (MAP_W_SIZE + MAP_THRESHOLD))
    return ;
  //printw("pipe: 0x%08x\n", pipe << 4);
  cb_set(map->map, pos ,~(pipe << posPipe));
}

void	affMapCurses(Map *map)
{
  int i;
  int i2;
  char line[MAP_H_SIZE + 1];

  move(0,0);
  printw("Score: %d\n", score);
  for (i2 = MAP_H_SIZE - 1; i2 >= 0; --i2)
    {
      strncpy(line, "                                                                                                          ", MAP_W_SIZE);
      line[MAP_W_SIZE] = 0;
      for (i = 0; i < MAP_W_SIZE; ++i)
	{
	  if ((1 << i2) & cb_get(map->map, i))
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
  erase();
  printw("the Game is in pause\n");
  while (getch() != 'p');
}

char	waitForSomething(const char *str)
{
  int i = 0;
  char ch;

again:
  ch = getch();

  for (i = 0; i < strlen(str); ++i)
    {
      if (ch == str[i])
	return (ch);
    }
  goto again;
  return 0;
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
	  map->bird = getBirdFlag(map->birdPosY);
	  hasDirBeenPress = 1;
	}
      else if (ch == DOWN_ARROW && !hasDirBeenPress)
	{
	  map->birdPosY -= 1;
	  map->bird = getBirdFlag(map->birdPosY);
	  hasDirBeenPress = 1;
	}
      else if (ch == 'q')
	{
	  endCurses();
	  exit(1);
	}
      else if (ch == LEFT_ARROW)
	if (mode == SLOW_MODE)
		mode = NORMAL_MODE;
	else
		mode = SLOW_MODE;
      else if (ch == RIGHT_ARROW)
	if (mode == FAST_MODE)
		mode = NORMAL_MODE;
	else
                mode = FAST_MODE;
      else if (ch == 'p')
	waitForUnpause();
    }
}

void	rollingMap(Map *map)
{
  cb_start_incr(map->map);
  addPipe(map, MAP_W_SIZE + MAP_THRESHOLD - 1);
}

int	checkCol(Map *map)
{
  return (map->bird & cb_get(map->map, BIRDPOSX));
}


static int	askToReplay(char *name)
{
  char ret;
  static char nickname[5][256];
  static int points[5];
  FILE *fhs;

  fhs = fopen("./highscore.txt", "r" );
  erase();
  if (fhs) {
    for (int  i = 0; i < 5; ++i) {
      fscanf(fhs, "%s : %d\n", nickname[i], &points[i]);
    }
    fclose(fhs);
  }

  fhs = fopen("./highscore.txt", "w+" );
  int xPos = COLS / 2 - sizeof("You just lose the game with %d point, do you want to replay ?") / 2;

  move(12, 0);

  for (int  i = 0; i < 5; ++i) {
    char tmp[256];
    if (score > points[i]) {
      strcpy(tmp, nickname[i]);
      strcpy(nickname[i], name);
      strcpy(name, tmp);
      points[i] += score;
      score = points[i] - score;
      points[i] -= score;
      move(3, xPos);
      printw("New High Score ! ");
    }
    fprintf(fhs, "%s : %d\n", nickname[i][0] ? nickname[i] : "none", points[i]);
  }
  fprintf(fhs, "-----------------------------------------\n");
  move(5, xPos);
  printw("----------- HIGH SCORES -------------");

  for (int yPos = 6, i = 0 ; i < 5 ; i++, yPos++) {
      move (yPos, xPos);
      printw(" %s:%d ", nickname[i][0] ? nickname[i] : "none", points[i] );
  }
  fclose(fhs);
 again:
  move(0, xPos);
  printw("You just lose the game with %d point, do you want to replay ?",
	 score);
  move(1, xPos);
  printw("Press Y if you want to try again, N if you don't");
  refresh();
  ret = waitForSomething("yYnN");
  if (ret == 'y' || ret == 'Y')
    return (1);
  else if (ret == 'n' || ret == 'N')
    return (0);
  usleep(200000);
  goto again;
}

static int enterYourName()
{
  static char name[256];

  erase();
  printw("Enter your name :)\n");
  echo();
  nocbreak();
  timeout(-1);
  getnstr(name, 256);
  timeout(0);
  noecho();
  cbreak();
  return askToReplay(name);
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
    }
  return (enterYourName());
}

int	main()
{
  int ret;

  initCurses();
  while ((ret = doGame()) > 0) {
    score = 0;
    mode = NORMAL_MODE;
  }
  endCurses();
  return (ret);
}
