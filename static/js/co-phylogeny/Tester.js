var contatorePerScrittura = 0;

function curveHiden(){
	if( (document.getElementById("curveAlternate").checked).toString()=="true"){
		document.getElementById("curve").hidden=true;
		document.getElementById("curveSwitch").hidden=true;
	} else {
		document.getElementById("curve").hidden=false;
		document.getElementById("curveSwitch").hidden=false;
	}
}
function euristicheHiden(){
	if( (document.getElementById("euristicheAlternate").checked).toString()=="true"){
		document.getElementById("euristica1").hidden=true;
		document.getElementById("euristica2").hidden=true;
	} else {
		document.getElementById("euristica1").hidden=false;
		document.getElementById("euristica2").hidden=false;
	}
}




function test(nomeFileTest){
// funzioni di supporto
	function inizializzaFile(nomeFile, testo , curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica){
		contatorePerScrittura = contatorePerScrittura + 1;
		var i=true;
		var st;
			d3.json("http://localhost:8888/inizializzaFile/"+ nomeFile +"/"+contatorePerScrittura+"/"+ testo, function(error, json) {
				if (error) return console.warn(error);
				var data = json;
				var testo = writeTitle( curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica);
				salvaInFile(data[0],testo)
				ciclaFile(data[0], curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica);
			});
		return st;
	}
	
	function salvaInFile(nomeFile, testo){
		contatorePerScrittura = contatorePerScrittura + 1;
		d3.json("http://localhost:8888/salva/"+ nomeFile +"/"+contatorePerScrittura+"/"+ testo, function(error, dataOriginal) {
		});
	}
	
	
	// invocoEucalipt
	function invocaEucalipt(nomeFile, name, cospeciationCost, duplicationCost, hostswitchCost, sortingCost, numberMaxSolution, rootToRoot, rimuoviCicli, metodoRimuviCicli, soluzioniRandomiche, maximumHostSwitchDistance, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, euristica){
		d3.select("svg").remove();
		d3.select("#tooltip").remove();
	
		//richiamo il json
		d3.json("http://localhost:9000/graphs_gdt/"+name+"/"+cospeciationCost+"/"+duplicationCost+"/"+hostswitchCost+"/"+sortingCost+"/"+numberMaxSolution.toString()+"/"+rootToRoot+"/"+rimuoviCicli+"/"+metodoRimuviCicli+"/"+soluzioniRandomiche+"/"+ maximumHostSwitchDistance , function(error, dataOriginal) {
	
			if (error) throw error;
			
			var st=""
			//se qualcuno non ha soluzioni
			if (dataOriginal[0]["problem"] != undefined) {
				st= st + name + "Combination not found";
				var p = document.getElementById("problem");
				p.innerHTML = st;
				return 0;
			} else {
				st=st+"";
				var p = document.getElementById("problem");
				p.innerHTML = st;
			}
	
			var data = dataOriginal[0]["graph"][0][0];
			graphs_solution = dataOriginal[0]["graph"]
			numberSolutions = dataOriginal[0]["number"];

			
			d3.select("body")
				.datum(data);
			
			//var data = graphs_solution[0][0];
			//elaboraNodi(data);


			for (var i = numberSolutions-1; i >= 0; i--) {
				var data = graphs_solution[i][0];
				elaboraNodi(data);
				/*
				var dataMeta = graphs_solution[i][0];
				    //aggiorno le appartenze    
			    dataMeta.nodes.forEach(function (d){
        			id = d.id;
        			nodoParassita=findNodeById(id, data);
        			nodoParassita.belongs_to = d.belongs_to;
    			})
    			//aggiorno gli switch
    			dataMeta.links.forEach(function (d){
        			link = data.linksParassiteParassite.find(function(d2){return ((d2.from == d.from) && (d.to == d2.to));})
        			link.switch=d.switch
    			})
    			*/
				elabolaD3Soluzioni(nomeFile,name, i, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data)
			}

		});
	}
	
	
	function avviaTest(nomeFile){
		if(nomeFile==undefined){
			var nomeFile = document.getElementById("nomeFileTest").value;
		}
		if(nomeFile==""){
			var nomeFile = "testIndefinito";
		}

		//leggo valori impostati nel pannello
		var curve = curveDefined(curve);
		var curveSwitch = curveSwitchDefined(curveSwitch);
		var height = heightDefined(height);
		var width = widthDefined(width);
		var longLeaves = longLeavesDefined(longLeaves);
		var equalsLivelEqualsHeight = equalsLivelEqualsHeightDefined(equalsLivelEqualsHeight);
		var separatorBetweenHostNodesWidth = separatorBetweenHostNodesWidthDefined(separatorBetweenHostNodesWidth);
		var separatorBetweenHostNodesHight = separatorBetweenHostNodesHightDefined(separatorBetweenHostNodesHight);
		var hostNodeBoxHeight = hostNodeBoxHeightDefined(hostNodeBoxHeight);
		var hostLeafWidth = hostLeafWidthDefined( hostLeafWidth, width);
		var parasiteNodesRadius = parasiteNodesRadiusDefined(parasiteNodesRadius);
		var euristica = euristicaDefined(euristica);


		var maximumHostSwitchDistance = maximumHostSwitchDistanceDefined(maximumHostSwitchDistance); 
		var soluzioniRandomiche= metodoSoluzioniRandomDefined(soluzioniRandomiche);
		var metodoRimuviCicli = metodoRimuviCicliDefined(metodoRimuviCicli);
		var rimuoviCicli=rimuoviCicliDefined(rimuoviCicli);
		var rootToRoot = rootToRootDefined(rootToRoot);
		var cospeciationCost = cospeciationCostDefined(cospeciationCost);
		var duplicationCost = duplicationCostDefined(duplicationCost);
		var hostswitchCost = hostswitchCostDefined(hostswitchCost);
		var sortingCost = sortingCostDefined(sortingCost);
		var numberMaxSolution = numberMaxSolutionDefined(numberMaxSolution);
	

		//intestazione tabella
		var testo = writeInfo ( curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica);
		inizializzaFile(nomeFile, testo , curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica);
	}
	
	function ciclaFile(nomeFile, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica){

		// per un file nexus specifico(nell'esempio sotto "GL" )
		// elabolaEucaliptSoluzioni(nomeFile, "GL" , curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius , maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution);
		
		//Per ogni file nexus
		d3.json("http://localhost:8000/list_of_file", function(error, json) {
				if (error) return console.warn(error);
				var data = json;
				data.forEach(function(d){
					elabolaEucaliptSoluzioni(nomeFile,d , curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius , maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica);
				});
		});
	}

	function invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data){

		var testo = ridisegna(false,  curve, curveSwitch,longLeaves, equalsLivelEqualsHeight, width, height, hostLeafWidth, hostNodeBoxHeight, separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight, parasiteNodesRadius, euristica, data);
		totaleIncroci= testo[0];
		incrociTraNoHostSwitch = testo[1];
		incrociTraHostSwitchENonHostSwitch = testo[2];
		incrociTraHostSwitch = testo[3];
		//scrivo i risultati
		var testo = writeRecord( totaleIncroci, incrociTraNoHostSwitch, incrociTraHostSwitch,incrociTraHostSwitchENonHostSwitch, nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions,data);
		salvaInFile(nomeFile,testo);
	}

//COSA FAR VARIARE: EUCALIPT
	function elabolaEucaliptSoluzioni(nomeFile,nomeNexus, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica){
		// VALORI CHE SI POSSONO MODIFICARE QUI SONO: cospeciationCost, duplicationCost, hostswitchCost, sortingCost, numberMaxSolution, rootToRoot, rimuoviCicli, metodoRimuviCicli, soluzioniRandomiche, maximumHostSwitchDistance
		// ATTENZIONE: se si vogliono effettuare i test su un singolo file FILE.NEX andare a modificare CICLA FILE

		//ora posso moficare i valori e poi invocare eucalipt con la funzione sotto
		invocaEucalipt(nomeFile, nomeNexus , cospeciationCost, duplicationCost, hostswitchCost, sortingCost, numberMaxSolution, rootToRoot, rimuoviCicli, metodoRimuviCicli, soluzioniRandomiche, maximumHostSwitchDistance , curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, euristica);
	}
//COSA FAR VARIARE: D3
	function elabolaD3Soluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution,euristica, numberSolutions, data){
		equalsLivelEqualsHeight=true;

		longLeaves=true;

		euristica="1";
		// VALORI CHE SI POSSONO MODIFICARE QUI SONO: curve, curveSwitch,longLeaves, equalsLivelEqualsHeight, width, height, hostLeafWidth, hostNodeBoxHeight, separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight, parasiteNodesRadius, euristica
		curve= true;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= true;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		longLeaves=false;
		// VALORI CHE SI POSSONO MODIFICARE QUI SONO: curve, curveSwitch,longLeaves, equalsLivelEqualsHeight, width, height, hostLeafWidth, hostNodeBoxHeight, separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight, parasiteNodesRadius, euristica
		curve= true;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= true;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		euristica="2";
		longLeaves=true;
		// VALORI CHE SI POSSONO MODIFICARE QUI SONO: curve, curveSwitch,longLeaves, equalsLivelEqualsHeight, width, height, hostLeafWidth, hostNodeBoxHeight, separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight, parasiteNodesRadius, euristica
		curve= true;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= true;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		longLeaves=false;
		// VALORI CHE SI POSSONO MODIFICARE QUI SONO: curve, curveSwitch,longLeaves, equalsLivelEqualsHeight, width, height, hostLeafWidth, hostNodeBoxHeight, separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight, parasiteNodesRadius, euristica
		curve= true;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= true;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		euristica="3";
		longLeaves=true;
		// VALORI CHE SI POSSONO MODIFICARE QUI SONO: curve, curveSwitch,longLeaves, equalsLivelEqualsHeight, width, height, hostLeafWidth, hostNodeBoxHeight, separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight, parasiteNodesRadius, euristica
		curve= true;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= true;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		longLeaves=false;
		// VALORI CHE SI POSSONO MODIFICARE QUI SONO: curve, curveSwitch,longLeaves, equalsLivelEqualsHeight, width, height, hostLeafWidth, hostNodeBoxHeight, separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight, parasiteNodesRadius, euristica
		curve= true;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= true;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

longLeaves=true;

		euristica="5";
		// VALORI CHE SI POSSONO MODIFICARE QUI SONO: curve, curveSwitch,longLeaves, equalsLivelEqualsHeight, width, height, hostLeafWidth, hostNodeBoxHeight, separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight, parasiteNodesRadius, euristica
		curve= true;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= true;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		longLeaves=false;
		// VALORI CHE SI POSSONO MODIFICARE QUI SONO: curve, curveSwitch,longLeaves, equalsLivelEqualsHeight, width, height, hostLeafWidth, hostNodeBoxHeight, separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight, parasiteNodesRadius, euristica
		curve= true;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= true;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		equalsLivelEqualsHeight=false;


		longLeaves=true;

		euristica="1";
		// VALORI CHE SI POSSONO MODIFICARE QUI SONO: curve, curveSwitch,longLeaves, equalsLivelEqualsHeight, width, height, hostLeafWidth, hostNodeBoxHeight, separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight, parasiteNodesRadius, euristica
		curve= true;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= true;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		longLeaves=false;
		// VALORI CHE SI POSSONO MODIFICARE QUI SONO: curve, curveSwitch,longLeaves, equalsLivelEqualsHeight, width, height, hostLeafWidth, hostNodeBoxHeight, separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight, parasiteNodesRadius, euristica
		curve= true;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= true;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		euristica="2";
		longLeaves=true;
		// VALORI CHE SI POSSONO MODIFICARE QUI SONO: curve, curveSwitch,longLeaves, equalsLivelEqualsHeight, width, height, hostLeafWidth, hostNodeBoxHeight, separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight, parasiteNodesRadius, euristica
		curve= true;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= true;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		longLeaves=false;
		// VALORI CHE SI POSSONO MODIFICARE QUI SONO: curve, curveSwitch,longLeaves, equalsLivelEqualsHeight, width, height, hostLeafWidth, hostNodeBoxHeight, separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight, parasiteNodesRadius, euristica
		curve= true;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= true;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		euristica="3";
		longLeaves=true;
		// VALORI CHE SI POSSONO MODIFICARE QUI SONO: curve, curveSwitch,longLeaves, equalsLivelEqualsHeight, width, height, hostLeafWidth, hostNodeBoxHeight, separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight, parasiteNodesRadius, euristica
		curve= true;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= true;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		longLeaves=false;
		// VALORI CHE SI POSSONO MODIFICARE QUI SONO: curve, curveSwitch,longLeaves, equalsLivelEqualsHeight, width, height, hostLeafWidth, hostNodeBoxHeight, separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight, parasiteNodesRadius, euristica
		curve= true;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= true;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

longLeaves=true;

		euristica="5";
		// VALORI CHE SI POSSONO MODIFICARE QUI SONO: curve, curveSwitch,longLeaves, equalsLivelEqualsHeight, width, height, hostLeafWidth, hostNodeBoxHeight, separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight, parasiteNodesRadius, euristica
		curve= true;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= true;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		longLeaves=false;
		// VALORI CHE SI POSSONO MODIFICARE QUI SONO: curve, curveSwitch,longLeaves, equalsLivelEqualsHeight, width, height, hostLeafWidth, hostNodeBoxHeight, separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight, parasiteNodesRadius, euristica
		curve= true;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=true;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= true;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

		curve= false;
		curveSwitch=false;
		invocaRidisegnaESalvaRecordSoluzioni(nomeFile,name, solutionNumber, curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutions, data);

	}

//Scrivere Info:
	function writeInfo ( curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica){
		return "cospeciationCost:"+cospeciationCost+";duplicationCost:"+duplicationCost+";hostswitchCost:"+hostswitchCost+";sortingCost"+sortingCost +";rimuoviCicli:"+rimuoviCicli+";metodoRimuviCicli:"+metodoRimuviCicli+"(1:Stolzer,2012;2:Tofigh,2011);soluzioniRandomiche:"+soluzioniRandomiche+";maximumHostSwitchDistance:"+maximumHostSwitchDistance+"(Nan=infinty);width:"+width+";height:"+height+";separatorBetweenHostNodesWidth:"+separatorBetweenHostNodesWidth+";separatorBetweenHostNodesHight:"+separatorBetweenHostNodesHight +";euristica:1-rapp;2-gdtUniversal;3-gdtForSolution";
	}

//COSA SCRIVERE: TITOLO
	function writeTitle( curve, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica){
		//Limitazioni:
		//		- solo una riga
		//		- usare lo spazio come delimitatore
		var testo = "fileNex soluzioni equalsLivelEqualsHeight euristica longLeaves curveNonSwitch curveSwith nodiHostOriginali nodiParassitiOriginali nodiHostAggiuntiEucalipt nodiParassitiAggiuntiEucalipt nodiHost nodiParassiti TotIncroci incrociTraNonSwitch incrociTraSwitchENoSwitch incrociTraSwitch tempoEucalipt tempoPlanarita tempoEurusticaGDTUniversal tempoEurusticaGDTForSolution tempoEuristicaRapp tempoDiCalcoloPerDisegnare tempoEuristicaSort"
		return testo;
	}
//COSA SCRIVERE: RIGA
	function writeRecord( totaleIncroci, incrociTraNoHostSwitch, incrociTraHostSwitch,incrociTraHostSwitchENonHostSwitch, nomeFile,nameNexus, solutionNumber, curveNonSwitch, curveSwitch, height, width, longLeaves, equalsLivelEqualsHeight,separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight,hostNodeBoxHeight, hostLeafWidth, parasiteNodesRadius, maximumHostSwitchDistance, soluzioniRandomiche, metodoRimuviCicli, rimuoviCicli, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost ,sortingCost, numberMaxSolution, euristica, numberSolutionsFind, data){
		//Limitazioni:
		//		- solo una riga
		//		- usare lo spazio come delimitatore
		
		//per qualunque altro parametro ricordare che nell'oggetto data c'è tutto il grafo 
		//		- data.nodes tutti gli oggetti nodo(con tutti i loro parametri)
		//		- data.links tutti gli oggetti archi(con tutti i loro parametri)
		//		- data.tempoEucalipt
		//		- data.tempoPlanarita
		//		- data.tempoEurusticaGDT
		//		- data.tempoEuristicaRapp
		//		- data.tempoDiCalcoloPerDisegnare

		var contatoreNodiHost= data.nodiHost.length;
		var contatoreNodiHostAggiunti= data.nodiHost.filter(function(d){return d.id.startsWith("H!H")}).length;

		var contatoreParassiti= data.nodiParassiti.length;
		var contatoreParassitiAggiunti= data.nodiParassiti.filter(function(d){return d.id.startsWith("P!P")}).length;
		
		var contatoreParassitiOriginali = contatoreParassiti -contatoreParassitiAggiunti;
		var contatoreNodiHostOriginali = contatoreNodiHost - contatoreNodiHostAggiunti;
		var testo = ""+nameNexus+" "+solutionNumber+" "+equalsLivelEqualsHeight+" "+euristica+" "+longLeaves+" "+curveNonSwitch+" "+curveSwitch+" "+contatoreNodiHostOriginali+" "+contatoreParassitiOriginali +" "+contatoreNodiHostAggiunti+" " + contatoreParassitiAggiunti +" "+contatoreNodiHost+" "+contatoreParassiti +" "+totaleIncroci+ " "+incrociTraNoHostSwitch+ " "+incrociTraHostSwitchENonHostSwitch+ " "+incrociTraHostSwitch+ " "+ data.tempoEucalipt + " " + data.tempoPlanarita + " " + data.tempoEurusticaGDT + " "+ data.tempoEurusticaGDTForSolution + " "+ data.tempoEuristicaRapp + " "+ data.tempoDiCalcoloPerDisegnare+ " "+ data.tempoEuristicaSort ;
		
		return testo;
	}

//AVVIARE TEST
	avviaTest(nomeFileTest);
}
