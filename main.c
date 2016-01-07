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

#include	<string.h>
#include	<stdlib.h>
#include        <stdio.h>
#include        <errno.h> 
#include	"CursesAff.h"
#include	"Map.h"

#define NORMAL_MODE 1
#define	SLOW_MODE -1
#define	FAST_MODE 3

int	score = 0;
int	mode = NORMAL_MODE;
FILE *fhs;   

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



static int	askToReplay()
{
  char ret;
  char nickname[15][15];
  int points[5] = {0};
  int i = 0;
  int yPos = 5;
  int sj6 = score;  
  fhs = fopen("highscore.txt", "rw+" );
  if (fhs == NULL)
  {
        perror("Error read");
 	return -1;
  }
  fscanf(fhs, "%s%d", &nickname[0], &points[0]);
  fscanf(fhs, "%s%d", &nickname[1], &points[1]);
  fscanf(fhs, "%s%d", &nickname[2], &points[2]);
  fscanf(fhs, "%s%d", &nickname[3], &points[3]);
  fscanf(fhs, "%s%d", &nickname[4], &points[4]);
       
  int xPos = COLS / 2 - sizeof("You just lose the game with %d point, do you want to replay ?") / 2;

  erase();
  move(0, xPos);
  printw("You just lose the game with %d point, do you want to replay ?",
	 score);
  while (i < 5)
  {
  if (sj6 > points[i])
  {
	strcpy(nickname[i], name);
  	points[i] = sj6;
  	move(2, xPos);
  	printw(" New High Score ! Nice work young Padawan  !");
	break;  
  }
    i++;
  }
  i = 0;	
  move(1, xPos);
  printw("Press Y if you want to try again, N if you don't");
  move(3, xPos); 
  printw("HIGH SCORES");
  move(5, xPos);
  for ( i = 0 ; i < 5 ; i++, yPos++)
  {
	move (yPos, xPos);
	printw(" %s:%d ",nickname[i], points[i] ); 
  }
  fclose(fhs);
  ret = waitForSomething("yYnN");
  if (ret == 'y' || ret == 'Y')
    return (1);
  else if (ret == 'n' || ret == 'N')
    return (0);
  usleep(200000);
  return askToReplay();
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
  return (askToReplay());
}

int	main()
{
  int ret;

  initCurses();
  while ((ret = doGame()) > 0)
    score = 0;
  endCurses();
  return (ret);
}
