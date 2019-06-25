function sistemare(data, host2ListParasite, host2parasiteSubtreeHeight,hostDeph2HeightMax, host2parasiteSubtreeHeight, treeHeight){
	togliEtichetta(data);
	var parassitaLivelloHost = creaMappaNodoParassitaLivelloHost(data);
	etichettareSistematiParassiti(data);
	etichettareSistematiHost(data,host2ListParasite);
	var nodiParassitaDaSistemare = creaListaNodiDaSistemare(data,"P");
	etichettaProfonditaParassiti(data);

	if(nodiParassitaDaSistemare.length==0){
		return [ host2parasiteSubtreeHeight,parassitaLivelloHost, hostDeph2HeightMax];
	}
	// ordino i nodi parassiti da sistemare dicendo che prima vengono quelli a profondità albero parassita minore e poi a parita quelli con depth host minore
	//console.log(nodiParassitaDaSistemare)	
	var nodiParassitaDaSistemare = nodiParassitaDaSistemare.sort(function(a, b){return compareNodiParassitaDaSistemare(a,b,data)});
	//console.log(nodiParassitaDaSistemare)	

	var host2Antenati = createHostToListAntenati(data);
	var parassita2Antenati = createParassiteToListAntenati(data);
	var result = sistemareSupporto( parassitaLivelloHost, data, host2Antenati,parassita2Antenati,nodiParassitaDaSistemare,host2ListParasite,host2parasiteSubtreeHeight);
    parassitaLivelloHost = result[0];
	host2parasiteSubtreeHeight = result[1];

	var nodiToLinkCiclo = data["nodes"].filter(function(d){return d["to_of_link_of_cycle"];})
					.filter(function(to){
						var from = findNodeById( (findParentIdsOfHostOrParasiteNode(to["id"],"P",data))[0],data )
						var profonditaFrom=profondita(host2Antenati,from,host2parasiteSubtreeHeight,parassitaLivelloHost);
						var profonditaTo=profondita(host2Antenati,to,host2parasiteSubtreeHeight,parassitaLivelloHost);
						return (profonditaTo < profonditaFrom) && (!(to["leafParassite"]));
					})
					.map(function(d){return d["id"];});

	var foglieCheSiPossonoSistemare = data["nodes"].filter(function(d){return d["to_of_link_of_cycle"];})
					.filter(function(to){return (to["leafParassite"]);
					})
					.map(function(d){return d["id"];});

	if( foglieCheSiPossonoSistemare.length > 0 ) {
		var result = sistemareSupporto( parassitaLivelloHost, data, host2Antenati,parassita2Antenati,foglieCheSiPossonoSistemare,host2ListParasite,host2parasiteSubtreeHeight);
    	parassitaLivelloHost = result[0];
		host2parasiteSubtreeHeight = result[1];
	}

	// sono solo i potenziali nodi che generano un ciclo e che rialzano la struttura.
	var archiCiclo = data.linksParassiteParassite
		.filter(function(d){
			return nodiToLinkCiclo.indexOf(d["to"]) != -1; 
		});

//	console.log(archiCiclo);
//	archiCiclo.forEach(function(d){console.log(d["to"],"-",d["from"])});


/*	
	data["links"].filter(function(d){
			if(d["to"].startsWith("H") || d["from"].startsWith("H") ){
				return false;
			} 
			try{
			
			var to = findNodeById(d["to"],data);
			var from = findNodeById(d["from"],data);
			var profonditaChild=profondita(host2Antenati,from,host2parasiteSubtreeHeight,parassitaLivelloHost);
			var profonditaParent=profondita(host2Antenati,to,host2parasiteSubtreeHeight,parassitaLivelloHost);
			console.log(profonditaChild,profonditaParent,((profonditaParent-1) < profonditaChild))
			return (profonditaParent) < profonditaChild;
			
			}catch(err){
				return false;
			}
		})
		.forEach(function(d){console.log(d.to,d.from)});
*/	
	//da aggiornare anche questi valori
	/* hostDeph2HeightMax prendiamo tutti gli host di un livello e calcoliamo quello che prima aveva valore maggiore*/
	var hostDeph2HeightMax = aggiornareHostDeph2HeightMax(host2parasiteSubtreeHeight, hostDeph2HeightMax, data, treeHeight);

    return [ host2parasiteSubtreeHeight,parassitaLivelloHost, hostDeph2HeightMax, archiCiclo ];
}

function compareNodiParassitaDaSistemare(idNodo1,idNodo2,data){
	var nodo1 = findNodeById(idNodo1,data);
	var nodo2 = findNodeById(idNodo2,data);
	if ( nodo1["leafParassite"] ){
		return -1;
	}
	if ( nodo2["leafParassite"] ){
		return +1;
	}

	if (nodo1["depth"] == nodo2["depth"]) {
		var nodo1Host = findNodeById(nodo1["belongs_to"],data);
		var nodo2Host = findNodeById(nodo2["belongs_to"],data);
		return nodo1Host["depth"] - nodo2Host["depth"];
	}else{
		return nodo1["depth"] - nodo2["depth"];
	}
}



function etichettaProfonditaParassiti(data){
	function etichettaSupporto(nodo,data,numero){
		nodo["depth"] = numero;
		var depthChild = numero + 1;
		var childId = findChildrenIdsOfHostOrParasiteNode(nodo["id"],"P",data);
		if(childId.length>0){
			childId
			.forEach(function(d){
				var nodoChild = findNodeById(d,data);
				etichettaSupporto(nodoChild, data,depthChild);
				});
		}
	}
	var rootParassita = findNodeById(findRoot(data,"P"),data);
	etichettaSupporto(rootParassita,data,0);	
}


function sistemareSupporto(parassitaLivelloHost, data,host2Antenati,parassita2Antenati,nodiParassitaDaSistemare,host2ListParasite,host2parasiteSubtreeHeight){
 /*	var nodiDaSistemabili = nodiParassitaDaSistemare.filter(function(d){return sistemabile(d,data,parassita2Antenati,host2Antenati); });

	var i=0;
	var nodoDaSistemareId=undefined;
	while( nodoDaSistemareId == undefined && nodiDaSistemabili.length > i ){
		nodoDaSistemareId = nodiDaSistemabili[i];
		i=i+1;
	}
 */

	var i=0;
	var nodoDaSistemareId=undefined;
	var isSistemabile = false;
	while( nodoDaSistemareId == undefined ||(nodiParassitaDaSistemare.length > i && !(isSistemabile))){
		nodoDaSistemareId = nodiParassitaDaSistemare[i];
		isSistemabile = sistemabile(nodoDaSistemareId,data,parassita2Antenati,host2Antenati);
		i=i+1;
	}
	//console.log(isSistemabile, nodoDaSistemareId,sistemabile(nodoDaSistemareId,data,parassita2Antenati,host2Antenati));



	
	//se non esistono nodi sistemabili prendi l'ultimo e consideralo sistemato
	if (!(isSistemabile)) {
		// i nodi parassiti precedenti sono sicuro che sono sistemati ma l'host del figlio? prendere il nodo da sistemare il cui discendente è in un nodo host sistemato
		var nodoDaSistemareId = nodiParassitaDaSistemare[0];
		
		var nodoDaSistemare = findNodeById(nodoDaSistemareId,data);
		//console.log(nodoDaSistemareId);
		var nodoParentId = findParentIdsOfHostOrParasiteNode(nodoDaSistemare["id"],"P",data);
		var nodoParent = findNodeById(nodoParentId,data);

		nodoDaSistemare["sistemato"]=true;
		nodoDaSistemare["to_of_link_of_cycle"] = true;	
	//controllare e propagare il sistemato negli host
	if (controlloHostSistemato( nodoDaSistemare.belongs_to, data, host2ListParasite)){
		propagaHostSistemato( nodoDaSistemare.belongs_to, data, host2ListParasite)
	}
	} else {
		var nodoDaSistemare = findNodeById(nodoDaSistemareId,data);
		var nodoParentId = findParentIdsOfHostOrParasiteNode(nodoDaSistemare["id"],"P",data);
		var nodoParent = findNodeById(nodoParentId,data);

		var result = sistemareNodo(nodoParent,nodoDaSistemare,parassitaLivelloHost, host2parasiteSubtreeHeight,data, host2Antenati);
		var parassitaLivelloHost = result[0];
		var host2parasiteSubtreeHeight = result[1];
	//controllare e propagare il sistemato negli host
	if (controlloHostSistemato( nodoDaSistemare.belongs_to, data, host2ListParasite)){
		propagaHostSistemato( nodoDaSistemare.belongs_to, data, host2ListParasite)
	}
	}

	
		

	var nuovaListaNodiDaSistemare = ([].concat(nodiParassitaDaSistemare.filter(function(d){return !(d == nodoDaSistemareId); }))).sort(function(a, b){return compareNodiParassitaDaSistemare(a,b,data)});
	if (nuovaListaNodiDaSistemare.length == 0){
		return [parassitaLivelloHost, host2parasiteSubtreeHeight];
	}
	return sistemareSupporto(parassitaLivelloHost, data,host2Antenati,parassita2Antenati,nuovaListaNodiDaSistemare,host2ListParasite,host2parasiteSubtreeHeight)
}

function aggiornareHostDeph2HeightMax(host2parasiteSubtreeHeight, hostDeph2HeightMax, data, treeHeight){
	var listaLivelli =[];
	var i=0;
	while( i < treeHeight ){
		listaLivelli.push(i);
		i++;
	}

	var nodiHost = data.nodiHost;

	listaLivelli.forEach(
		function(d){
			
			var nuovoValore = d3.max(nodiHost.filter(function(d2){return d2.depth == d })
				.map(function(d2){return host2parasiteSubtreeHeight.get(d2.id); }));
			
			hostDeph2HeightMax.set(d,nuovoValore);
			
		});
	return hostDeph2HeightMax;
}

function sistemabile(idNodo,data,parassita2Antenati,host2Antenati){
	//tutti gli antenati parassiti sono sistemati	
	function sistemabilePerIParassiti(idNodo,data,parassita2Antenati){
		return ((parassita2Antenati.get(idNodo)	
			.filter(
				function(d){
					var nodo = findNodeById(d,data);
					return nodo["sistemato"]==false;
				})
			.length) == 0);
	}
	// per quanto riguarda l'host devo vedere se il parent dell'host contenuto è sistemato
	function sistemabilePerGliHost(idNodo,data,host2Antenati){
		return ((host2Antenati.get( findNodeById(idNodo,data)["belongs_to"] )	
			.filter(
				function(d){
					var nodo = findNodeById(d,data);
					return nodo["sistemato"]==false;
				})
			.length) == 0);		
	}
	// sistemabile per gli host del genitore
	//console.log(findParentIdsOfHostOrParasiteNode(idNodo,"P",data)[0]);

	return (sistemabilePerIParassiti(idNodo,data,parassita2Antenati) && sistemabilePerGliHost(idNodo,data,host2Antenati) && sistemabilePerGliHost((findParentIdsOfHostOrParasiteNode(idNodo,"P",data)[0]),data,host2Antenati));
}

function creaListaNodiDaSistemare(data,type){
	if(type == "H"){
		var nodi= data.nodiHost;
	}else{
		var nodi= data.nodiParassiti;
	}

	return nodi.filter(function(d){return d.sistemato == false;})
				.map(function(d){return d.id});
}

function etichettareSistematiHost(data,host2ListParasite){
	propagaHostSistemato(findRoot(data,"H"),data,host2ListParasite); 
}

function findRoot(data,type){
  	var rootsConnector = data["links"].filter(function(d) {return d["roots_connector"] == true})[0];
    var rootId = rootsConnector["to"].startsWith(type) ? rootsConnector["to"] : rootsConnector["from"];
    return rootId;
}

function etichettareSistematiParassiti(data){
	var rootParassita = findNodeById(findRoot(data,"P"),data);
	rootParassita["sistemato"] = true;

	data.linksParassiteParassite.filter(function(d){return d["switch"] == false})
		.map(function(d) {return d["to"] })
		.forEach(function (d) {
			var nodo = findNodeById(d,data);
			nodo["sistemato"]=true;
		});
}

function togliEtichetta(data){
	data["nodes"].forEach(function(d){d["sistemato"]=false});
}

// immaginare che tutti i sottoalberi dei parassiti contenuti nell'host hanno una loro radice a livello 0(il primo in alto)
function creaMappaNodoParassitaLivelloHost(data){
	var parassitaLivelloHost=d3.map();
	// funzione per assegnare i valori
	function recursive(nodo,data,numero){
		var childId = findChildrenIdsOfHostOrParasiteNode(nodo["id"],"P",data);
		if(childId.length>0){
			numero=numero+1;
			childId
			.forEach(function(d){
				var nodoChild = findNodeById(d,data);
				if (nodoChild["belongs_to"]==nodo["belongs_to"]){
					parassitaLivelloHost.set(nodoChild["id"],numero);
					recursive(nodoChild,data,numero);
				} else {
					parassitaLivelloHost.set(nodoChild["id"],0);
					recursive(nodoChild,data,0);
				}

			});
		}
	}
	//prendo la root e lancio la funzione
    var rootId = findRoot(data,"P");
    var rootNode = findNodeById(rootId, data);
    parassitaLivelloHost.set(rootId,0);
	recursive(rootNode,data,0);

    return parassitaLivelloHost;
}

function controlloHostSistemato(idNodo,data,host2ListParasite){
	return ((host2ListParasite.get(idNodo)	
			.filter(
				function(d){
					var nodo = findNodeById(d,data);
					return nodo["sistemato"]==false;
				})
			.length) == 0);
}

function propagaHostSistemato(idNodo,data,host2ListParasite){
	var nodo = findNodeById(idNodo,data);
	nodo["sistemato"]= true;
	var childId = findChildrenIdsOfHostOrParasiteNode(idNodo,"H",data);
	if(childId.length>0){
		childId
			.forEach(function(d){
				if(controlloHostSistemato(d,data,host2ListParasite)){
					propagaHostSistemato(d,data,host2ListParasite);
				}
			});
	}		
}

function creaListaAntenati(data,type){
	function iterazione(idNodo,data,host2Antenati,lista){
    	var listaSupporto=[].concat(lista);
    	host2Antenati.set(idNodo,listaSupporto);
    	var listaSupporto2 = [].concat(listaSupporto).concat([idNodo]);

    	var childId = findChildrenIdsOfHostOrParasiteNode(idNodo,type,data);
    	if(childId.length>0){
      		childId.forEach(
        		function(d){
    	    		iterazione(d,data,host2Antenati,listaSupporto2);
        		});
    	}
  	}

  	var rootId = findRoot(data,type);
    var host2Antenati = d3.map();
    iterazione(rootId,data,host2Antenati,[]);

    return host2Antenati;
}

//funzione funziona
function createHostToListAntenati(data){
	return creaListaAntenati(data,"H");
}

//funzione funziona
function createParassiteToListAntenati(data){
	return creaListaAntenati(data,"P");
}

function profondita(host2Antenati,nodo,host2parasiteSubtreeHeight,parassitaLivelloHost){
	var listaAntenati = host2Antenati.get(nodo["belongs_to"]);
	var contatore = 0 + parassitaLivelloHost.get(nodo["id"]);
	listaAntenati.forEach(function(d){
		var daAggiungere = host2parasiteSubtreeHeight.get(d)+1;
		contatore = contatore + daAggiungere;
	});
	return contatore;
}

//ritorna la profondità massima che avrà il sottoalbero
function abbassareSottoAlbero(nodo,parassitaLivelloHost,data, livelliDaAbbassare){
		
		var attualeLivello = parassitaLivelloHost.get(nodo["id"]);
		attualeLivello = attualeLivello + livelliDaAbbassare;
		parassitaLivelloHost.set(nodo["id"],attualeLivello);
		var maxProfonditaRaggiunta=attualeLivello;

		var childId = findChildrenIdsOfHostOrParasiteNode(nodo["id"],"P",data);
		if(childId.length>0){
			var result = d3.max(childId.map(function(d){
				var nodoChild = findNodeById(d,data);
				if(nodoChild["belongs_to"]==nodo["belongs_to"] ){
					return abbassareSottoAlbero(nodoChild,parassitaLivelloHost,data,livelliDaAbbassare);	
				}
			}));

			if(maxProfonditaRaggiunta<result){
				maxProfonditaRaggiunta=result;
			}
		}
		return maxProfonditaRaggiunta;
}

//funzione da implementare
function sistemareNodo(nodoParent,nodo,parassitaLivelloHost, host2parasiteSubtreeHeight,data, host2Antenati){
	
	//			0-aspettare che l'host parent dell'host in cui è contenuto il parassita sia sistemato

	//			1-abbassa il nodo fino a che è necessario per renderlo un livello sotto al genitore
	var profonditaChild=profondita(host2Antenati,nodo,host2parasiteSubtreeHeight,parassitaLivelloHost);
	var profonditaParent=profondita(host2Antenati,nodoParent,host2parasiteSubtreeHeight,parassitaLivelloHost);

	if ( ((profonditaParent) < profonditaChild) && (!(profonditaParent == profonditaChild)) ) {
		//è già sistemato
		nodo["sistemato"]=true;
		return [parassitaLivelloHost, host2parasiteSubtreeHeight];
	} else {
		var livelliDaAbbassare = (profonditaParent+1) - profonditaChild;
	//			2-abbassa di livello tutti i discendenti di quel parassita contenuti nell'host.
		var profonditaRaggiunta = abbassareSottoAlbero(nodo,parassitaLivelloHost,data,livelliDaAbbassare);
	}

	//			3-controllo se abbiamo sforato e in caso affermativo allungare la box.
	var nodoHost=findNodeById(nodo["belongs_to"],data);

	if(host2parasiteSubtreeHeight.get(nodoHost["id"]) < profonditaRaggiunta){
		host2parasiteSubtreeHeight.set(nodoHost["id"],profonditaRaggiunta);
	}

	nodo["sistemato"]=true;
	return [parassitaLivelloHost, host2parasiteSubtreeHeight];
}
