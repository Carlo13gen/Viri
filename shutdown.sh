#/bin/bash

echo "killing:"
ps aux | grep gnome-terminal

ps aux | grep gnome-terminal | gawk -F" " '{print  $2}' | xargs kill -9
z

