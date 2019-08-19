#!/usr/bin/env bash
$TERM -e "python3 portaGDTEucalipt.py" & $TERM -e "python3 portaFunzioniSupporto.py" & $TERM -e "python3 portaGDTEucaliptPreCalcolati.py" & $TERM -e "python3 project2.py" & $TERM -e "python portaInizializzazioneESalvataggio.py" &


echo "Open a browser and connecto to http://localhost:5000"
