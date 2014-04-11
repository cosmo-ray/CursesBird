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

#ifndef	MAP_HH
#define MAP_HH

#define	MAP_W_SIZE 23
#define	MAP_H_SIZE 25
#define BORDER 0xFFF00000
#define NPIPE 0x000000FF // oposite of a pipe a pipe is ~PIPE
#define SIZE_HOLE 8
#define BIRDPOSX 5
#define MAP_THRESHOLD 4
#define Max_BONUS 8

typedef struct	{
  int	map[MAP_W_SIZE + MAP_THRESHOLD];
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
