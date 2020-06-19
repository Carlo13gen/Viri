#/bin/bash

echo "killing:"
ps aux | grep "\-e" | grep portaGDTEucalipt
ps aux | grep "\-e" | grep portaGDTEucalipt | gawk -F" " '{print  $2}' | xargs kill -9

echo "killing:"
ps aux | grep "\-e" | grep portaInizializzazioneESalvataggio
ps aux | grep "\-e" | grep portaInizializzazioneESalvataggio | gawk -F" " '{print  $2}' | xargs kill -9

echo "killing:"
ps aux | grep "\-e" | grep project2
ps aux | grep "\-e" | grep project2 | gawk -F" " '{print  $2}' | xargs kill -9



