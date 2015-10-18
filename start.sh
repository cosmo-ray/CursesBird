#!/bin/bash

gnome-terminal -h  > /dev/null
HAS_GTERM=$?

xterm -h > /dev/null
HAS_XTERM=$?

if [ $HAS_XTERM ] ; then
    xterm -title 'Curses Bird' ./Curses\ Bird
elif [ $HAS_GTERM ] ; then
    gnome-terminal -e './Curses\ Bird' -t 'curses bird' --hide-menubar
fi
