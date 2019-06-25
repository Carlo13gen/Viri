#/bin/bash

echo "killing:"
ps aux | grep xterm | grep iconic  

ps aux | grep xterm | grep iconic | gawk -F" " '{print  $2}' | xargs kill -9


