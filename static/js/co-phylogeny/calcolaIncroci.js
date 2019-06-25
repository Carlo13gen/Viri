function xPiuASinistraDestra (nodo1, nodo2){
	if(nodo1["x"] < nodo2["x"]){
		return [nodo1["x"], nodo2["x"]]
	} else {
		return [nodo2["x"], nodo1["x"]]
	}
}

function xPiuASinistraDestraCurvo(toLink,fromLink, parasiteNodesRadius){
	var toX = toLink["x"];

	if( toLink["x"] > fromLink["x"]){ var fromX = fromLink["x"] + parasiteNodesRadius;  }
	if( toLink["x"] < fromLink["x"]){ var fromX = fromLink["x"] - parasiteNodesRadius;  }
	if( toLink["x"] == fromLink["x"]){ var fromX = fromLink["x"];  }

	if(toX < fromX){
		return [toX, fromX]
	} else {
		return [fromX, toX]
	}
}

function yPiuAltaBassaCurvo(toLink,fromLink, parasiteNodesRadius){
	var fromY = fromLink["y"];

	if( toLink["y"] > fromLink["y"]){ var toY = toLink["y"] - parasiteNodesRadius;  }
	if( toLink["y"] < fromLink["y"]){ var toY = toLink["y"] + parasiteNodesRadius;  }
	if( toLink["y"] == fromLink["y"]){ var toY = toLink["y"];  }

	if(toY < fromY){
		return [toY, fromY]
	} else {
		return [fromY, toY]
	}
}

function yPiuAltaBassa (nodo1, nodo2){
	if(nodo1["y"] < nodo2["y"]){
		return [nodo1["y"], nodo2["y"]]
	} else {
		return [nodo2["y"], nodo1["y"]]
	}
}

// riscontri preliminari servono per vedere se condividono uno stesso quadrante
function riscontroPreliminareCurvo(toLink1,fromLink1,toLink2,fromLink2, parasiteNodesRadius){
	var result = xPiuASinistraDestraCurvo(toLink1,fromLink1, parasiteNodesRadius);
	var xPiuASinistraLink1 = result[0];
	var xPiuADestraLink1 = result[1];

	var result = xPiuASinistraDestraCurvo(toLink2,fromLink2, parasiteNodesRadius);
	var xPiuASinistraLink2 = result[0];
	var xPiuADestraLink2 = result[1];

	var result = yPiuAltaBassaCurvo(toLink1, fromLink1, parasiteNodesRadius);
	var yPiuAltaLink1 = result[0];
	var yPiuBassaLink1 = result[1];

	var result = yPiuAltaBassaCurvo(toLink2, fromLink2, parasiteNodesRadius);
	var yPiuAltaLink2 = result[0];
	var yPiuBassaLink2 = result[1];

	if(xPiuADestraLink2 < xPiuASinistraLink1){
		return false;
	}

	if(xPiuADestraLink1 < xPiuASinistraLink2){
		return false;
	}

	if(yPiuBassaLink2 < yPiuAltaLink1){
		return false;
	}

	if(yPiuBassaLink1 < yPiuAltaLink2){
		return false;
	}

	return true;
}

function riscontroPreliminareDrittoCurvo(toLinkDritto,fromLinkDritto,toLinkCurvo,fromLinkCurvo, parasiteNodesRadius){
	var result = xPiuASinistraDestra(toLinkDritto,fromLinkDritto);
	var xPiuASinistraLink1 = result[0];
	var xPiuADestraLink1 = result[1];

	var result = xPiuASinistraDestraCurvo(toLinkCurvo,fromLinkCurvo, parasiteNodesRadius);
	var xPiuASinistraLink2 = result[0];
	var xPiuADestraLink2 = result[1];

	var result = yPiuAltaBassa(toLinkDritto, fromLinkDritto);
	var yPiuAltaLink1 = result[0];
	var yPiuBassaLink1 = result[1];

	var result = yPiuAltaBassaCurvo(toLinkCurvo, fromLinkCurvo, parasiteNodesRadius);
	var yPiuAltaLink2 = result[0];
	var yPiuBassaLink2 = result[1];

	if(xPiuADestraLink2 < xPiuASinistraLink1){
		return false;
	}

	if(xPiuADestraLink1 < xPiuASinistraLink2){
		return false;
	}

	if(yPiuBassaLink2 < yPiuAltaLink1){
		return false;
	}

	if(yPiuBassaLink1 < yPiuAltaLink2){
		return false;
	}

	return true;
}

function riscontroPreliminare(toLink1,fromLink1,toLink2,fromLink2){
	var result = xPiuASinistraDestra(toLink1,fromLink1);
	var xPiuASinistraLink1 = result[0];
	var xPiuADestraLink1 = result[1];

	var result = xPiuASinistraDestra(toLink2,fromLink2);
	var xPiuASinistraLink2 = result[0];
	var xPiuADestraLink2 = result[1];

	var result = yPiuAltaBassa(toLink1, fromLink1);
	var yPiuAltaLink1 = result[0];
	var yPiuBassaLink1 = result[1];

	var result = yPiuAltaBassa(toLink2, fromLink2);
	var yPiuAltaLink2 = result[0];
	var yPiuBassaLink2 = result[1];

	if(xPiuADestraLink2 < xPiuASinistraLink1){
		return false;
	}

	if(xPiuADestraLink1 < xPiuASinistraLink2){
		return false;
	}

	if(yPiuBassaLink2 < yPiuAltaLink1){
		return false;
	}

	if(yPiuBassaLink1 < yPiuAltaLink2){
		return false;
	}

	return true;
}

// calcola incroci in base ai tipi di archi
function incrocioArcoDrittoEArcoDritto(link1, link2, data){
	if (( ( link1["to"]==link2["from"] )||
		( link2["to"]==link1["from"] )||
		( link2["from"]==link1["from"] )
		)){
		return false;
	}




	var toLink1 = findNodeById(link1["to"],data);
	var fromLink1 = findNodeById(link1["from"],data);
	var toLink2 = findNodeById(link2["to"],data);
	var fromLink2 = findNodeById(link2["from"],data);

	if(!(riscontroPreliminare(toLink1,fromLink1,toLink2,fromLink2))){
		return false;
	}

	//calcolare incrocio
	// la x in cui si incrociano gli archi è x = q2 - q1 / m1 - m2
	// calcolarla e vedere se la x è compresa tra i due estremi di entrambi gli archi, se si incrociano

	function calcoloMLink(fromLink, toLink){
		return ((toLink["y"] - fromLink["y"]) / (toLink["x"] - fromLink["x"]))
	}
	function calcoloQLink(fromLink, mLink){
		return (fromLink["y"] - ( mLink * fromLink["x"]));
	}

	var result = xPiuASinistraDestra(toLink1,fromLink1);
	var xPiuASinistraLink1 = result[0];
	var xPiuADestraLink1 = result[1];

	var result = xPiuASinistraDestra(toLink2,fromLink2);
	var xPiuASinistraLink2 = result[0];
	var xPiuADestraLink2 = result[1];


	var m1 = calcoloMLink( fromLink1, toLink1 );
	var m2 = calcoloMLink( fromLink2, toLink2 );

	var q1 = calcoloQLink( fromLink1, m1 ); 
	var q2 = calcoloQLink( fromLink2, m2 ); 


	if(( m2 == Infinity )&&( m1 == Infinity )){
		return true;	
	}

	if(m1 == Infinity){
		// l'arco 1 è una retta verticale
		var result = yPiuAltaBassa(toLink1, fromLink1);
		var yPiuAltaLink1 = result[0];
		var yPiuBassaLink1 = result[1];
		var xIncrocio = fromLink1["x"];
		var yIncrocio = m2*xIncrocio + q2;

		return  ( yPiuAltaLink1 < yIncrocio) && ( yIncrocio < yPiuBassaLink1 );
	}

	if(m2 == Infinity){
		// l'arco 2 è una retta verticale
		// l'arco 1 è una retta verticale
		var result = yPiuAltaBassa(toLink2, fromLink2);
		var yPiuAltaLink2 = result[0];
		var yPiuBassaLink2 = result[1];
		var xIncrocio = fromLink2["x"];
		var yIncrocio = m1*xIncrocio + q1;

		return  ( yPiuAltaLink2 < yIncrocio) && ( yIncrocio < yPiuBassaLink2 );
	}

	if(m1 == m2){
		// sono parallele
		return false;
	}


	var xIncrocio = ((q2 - q1)/( m1 - m2 ));

	// se la x è compresa in entrambi gli archi allora incrocia
	return ((xPiuASinistraLink1 < xIncrocio) && (xPiuASinistraLink2 < xIncrocio) && (xPiuADestraLink1 > xIncrocio) && (xPiuADestraLink2 > xIncrocio));
}


function incrocioArcoCurvoEArcoCurvo(link1,link2,data, parasiteNodesRadius){
	var numeroIncroci=0;
	var toLink1 = findNodeById(link1["to"],data);
	var fromLink1 = findNodeById(link1["from"],data);
	var toLink2 = findNodeById(link2["to"],data);
	var fromLink2 = findNodeById(link2["from"],data);

	if(!(riscontroPreliminareCurvo(toLink1,fromLink1,toLink2,fromLink2, parasiteNodesRadius))){
		return numeroIncroci;
	}

	var result = xPiuASinistraDestraCurvo(toLink1,fromLink1, parasiteNodesRadius);
	var xPiuASinistraLink1 = result[0];
	var xPiuADestraLink1 = result[1];

	var result = xPiuASinistraDestraCurvo(toLink2,fromLink2, parasiteNodesRadius);
	var xPiuASinistraLink2 = result[0];
	var xPiuADestraLink2 = result[1];

	var result = yPiuAltaBassaCurvo(toLink1, fromLink1, parasiteNodesRadius);
	var yPiuAltaLink1 = result[0];
	var yPiuBassaLink1 = result[1];

	var result = yPiuAltaBassaCurvo(toLink2, fromLink2, parasiteNodesRadius);
	var yPiuAltaLink2 = result[0];
	var yPiuBassaLink2 = result[1];

	// caso 77
		/* 	x=x
			y=y
			una delle due x interseca l'asse orizontale dell'altra
		*/
	// caso L7
		/* 	x=x
			y=y
			una delle due x interseca l'asse orizontale dell'altra o entrambe
		*/

	// caso 7L
		/* 	x=x
			y=y
			una delle due x interseca l'asse orizontale dell'altra o entrambe
		*/

	// caso LL
		/* 	x=x
			y=y
			una delle due x interseca l'asse orizontale dell'altra
		*/


	/* quindi nel caso generale 
			x=x
			y=y
			una delle due x interseca l'asse orizontale dell'altra o entrambe
	*/

	// y=y
	if(fromLink1["y"] == fromLink2["y"]){ 
		numeroIncroci = numeroIncroci+1;
	}
	
	
	// x=x
	if(toLink1["x"] == toLink2["x"]){ 
		numeroIncroci = numeroIncroci+1;
	}

	if (!((toLink1["x"] == toLink2["x"]) && (fromLink1["y"] == fromLink2["y"]))){
		// vedo se i due punti potenziali di incrocio esistono

		// punto incrocio 1
		var xIncrocio = toLink2["x"];
		var yIncrocio = fromLink1["y"];
		if (( xPiuASinistraLink1 < xIncrocio ) && ( xIncrocio < xPiuADestraLink1 ) &&
			( yPiuAltaLink2 < yIncrocio ) && ( yIncrocio < yPiuBassaLink2 )){
			numeroIncroci = numeroIncroci + 1;
		}


		// punto incrocio 2
		var xIncrocio = toLink1["x"];
		var yIncrocio = fromLink2["y"];
		if (( xPiuASinistraLink2 < xIncrocio ) && ( xIncrocio < xPiuADestraLink2 ) &&
			( yPiuAltaLink1 < yIncrocio ) && ( yIncrocio < yPiuBassaLink1 )){
			numeroIncroci = numeroIncroci + 1;
		}
	
	}

	// controllo se un incrocio è dato dal fatto che uno è figlio dell'altro o viceversa o sono fratelli
	if (( ( link1["to"]==link2["from"] )||
		( link2["to"]==link1["from"] )||
		( link2["from"]==link1["from"] )
		)){
		numeroIncroci = numeroIncroci - 1;
	}
	//console.log(numeroIncroci);
	return numeroIncroci;

}

function incrocioArcoDrittoEArcoCurvo(linkDritto, linkCurvo, data, parasiteNodesRadius){
	function calcoloMLink(fromLink, toLink){
		return ((toLink["y"] - fromLink["y"]) / (toLink["x"] - fromLink["x"]))
	}

	function calcoloQLink(fromLink, mLink){
		return (fromLink["y"] - ( mLink * fromLink["x"]));
	}

	var toLinkDritto = findNodeById(linkDritto["to"],data);
	var fromLinkDritto = findNodeById(linkDritto["from"],data);
	var toLinkCurvo = findNodeById(linkCurvo["to"],data);
	var fromLinkCurvo = findNodeById(linkCurvo["from"],data);

	var numeroIncroci=0;
	if(!(riscontroPreliminareDrittoCurvo(toLinkDritto,fromLinkDritto,toLinkCurvo,fromLinkCurvo, parasiteNodesRadius))){
		return numeroIncroci;
	}

	// il dritto è una verticale
	var mDritto = calcoloMLink( fromLinkDritto, toLinkDritto );
	
	if(mDritto == Infinity){
		// l'arco Dritto è una retta verticale
		// o l'arco verticalmente coincide o arco dritto incrocia con la parte orizontale
		var result = yPiuAltaBassa(toLinkDritto, fromLinkDritto);
		var yPiuAltaLinkDritto = result[0];
		var yPiuBassaLinkDritto = result[1];
		var yIncrocio = fromLinkCurvo["y"];

		// o l'arco verticalmente coincide o arco dritto incrocia con la parte orizontale
		if ((toLinkCurvo["x"] == toLinkDritto["x"]) || ((yPiuAltaLinkDritto < yIncrocio) && (yIncrocio < yPiuBassaLinkDritto))){
			return 1;
		}else{
			return 0;
		}; 
	}

	var qDritto = calcoloQLink( fromLinkDritto, mDritto );

	// retta incrocia asse verticale
		//	- xIncrocio = xAsseVerticale calcolo la yIncrocio e poi vedo se è nel range y dei due segmenti
	var xIncrocio = toLinkCurvo["x"];
	var yIncrocio = (xIncrocio * mDritto) + qDritto;

	var result = yPiuAltaBassaCurvo(toLinkCurvo, fromLinkCurvo, parasiteNodesRadius);
	var yPiuAltaLinkCurvo = result[0];
	var yPiuBassaLinkCurvo = result[1];
	var result = yPiuAltaBassa(toLinkDritto, fromLinkDritto);
	var yPiuAltaLinkDritto = result[0];
	var yPiuBassaLinkDritto = result[1];

	if ((yPiuAltaLinkCurvo < yIncrocio) && ( yIncrocio < yPiuBassaLinkCurvo ) &&
			(yPiuAltaLinkDritto < yIncrocio) && ( yIncrocio < yPiuBassaLinkDritto )){
		numeroIncroci = numeroIncroci + 1;
	}

	// retta incrocia asse orizontale
	var yIncrocio = fromLinkCurvo["y"];
	var xIncrocio = ((yIncrocio - qDritto)/mDritto);

	var result = xPiuASinistraDestraCurvo(toLinkCurvo,fromLinkCurvo, parasiteNodesRadius);
	var xPiuASinistraLinkCurvo = result[0];
	var xPiuADestraLinkCurvo = result[1];
	var result = xPiuASinistraDestra(toLinkDritto,fromLinkDritto);
	var xPiuASinistraLinkDritto = result[0];
	var xPiuADestraLinkDritto = result[1];


	if (( xPiuASinistraLinkCurvo < xIncrocio )&&( xIncrocio < xPiuADestraLinkCurvo)&&
		( xPiuASinistraLinkDritto < xIncrocio )&&( xIncrocio < xPiuADestraLinkDritto)){
		numeroIncroci = numeroIncroci + 1;
	}



	// togliere 1 se sono parenti
	if (( ( linkDritto["to"]==linkCurvo["from"] )||
		( linkCurvo["to"]==linkDritto["from"] )||
		( linkCurvo["from"]==linkDritto["from"] )
		)){
		numeroIncroci = numeroIncroci - 1;
	}

	return numeroIncroci;
}

// la funzione che conta gli incroci e l'altra che invoca la funzione in base ai tipi di archi
function contaIncroci(data, curve, curveSwitch,parasiteNodesRadius){
	// vede per ogni arco se incrocia un un altro arco quindi abbiamo un doppio ciclo for
	var contatore = 0;
	var contatoreNoSwitch=0
	var contatoreSwitchSwitch =0
	var contatoreSwitchNoSwitch =0
	
	var archiParassiti = data["links"].filter(function(d){
			return d["to"].startsWith("P") && d["from"].startsWith("P") 
		});

	for (var i = 0; i < archiParassiti.length; i++) {
		var c=i+1;
		for (var j = c; j < archiParassiti.length; j++) {
			var quanteVolteIncrociano = incrocia(data, curve, curveSwitch, archiParassiti[i], archiParassiti[j], parasiteNodesRadius);
			if( quanteVolteIncrociano > 0 ){
				//console.log(archiParassiti[i]["from"],"-",archiParassiti[i]["to"]," con ",archiParassiti[j]["from"],"-",archiParassiti[j]["to"]);
				if((archiParassiti[i]["switch"])&&(archiParassiti[j]["switch"])){
					contatoreSwitchSwitch = contatoreSwitchSwitch + quanteVolteIncrociano;
				}
				if(((archiParassiti[i]["switch"])&&(!(archiParassiti[j]["switch"])))||((!(archiParassiti[i]["switch"]))&&(archiParassiti[j]["switch"]))){
					contatoreSwitchNoSwitch = contatoreSwitchNoSwitch + quanteVolteIncrociano;
				}
				if((!(archiParassiti[i]["switch"]))&&(!(archiParassiti[j]["switch"]))){
					contatoreNoSwitch = contatoreNoSwitch + quanteVolteIncrociano;
				}

				contatore = contatore +quanteVolteIncrociano;
			}
		}
	}

	return [contatore, contatoreNoSwitch, contatoreSwitchNoSwitch, contatoreSwitchSwitch]
}

function incrocia(data, curve, curveSwitch, link1, link2, parasiteNodesRadius){
	function isCurvo( link, curve, curveSwitch ) {
		//console.log(link["switch"]);
		if(link["switch"] == true){
			return curveSwitch;
		}else{
			return curve;
		}

	}
		// in base ai parametri di curve invoca una funzione sopra descritta
	// 1- capire quali link fanno una curva
	var link1IsCurvo = isCurvo( link1, curve, curveSwitch );
	var link2IsCurvo = isCurvo( link2, curve, curveSwitch );

	// 2- scegliere la funzione	
	if (link1IsCurvo && link2IsCurvo) { return incrocioArcoCurvoEArcoCurvo(link1, link2, data, parasiteNodesRadius); }
	if ((!(link1IsCurvo)) && link2IsCurvo) { return incrocioArcoDrittoEArcoCurvo(link1, link2, data, parasiteNodesRadius); }
	if (link1IsCurvo && (!(link2IsCurvo))) { return incrocioArcoDrittoEArcoCurvo(link2, link1, data, parasiteNodesRadius); }
	if ((!(link1IsCurvo)) && (!(link2IsCurvo))) {if(incrocioArcoDrittoEArcoDritto(link1, link2, data)){return 1;}else{return 0}; }
}