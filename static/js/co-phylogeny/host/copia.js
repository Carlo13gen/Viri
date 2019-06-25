function copia(graph,host2ListParasiteInHostSubTree,rootId,hostLeaf2ParasiteLeaf,longLeaves,hostLeafWidth,separatorBetweenHostNodesWidth, treeHeight){
	
	
	EuristicaRandom(graph, host2ListParasiteInHostSubTree,rootId);//lancio un euristica random per avere i valori di figlio destro e sinistri iniziali
		setHostNodesXCoordinates(graph, separatorBetweenHostNodesWidth, treeHeight); //aggiorno alle nuove coordinate
		setParasiteNodesXCoordinates(graph, hostLeaf2ParasiteLeaf, hostLeafWidth, longLeaves, "5");
		
 	livello=MaxDepth(graph["nodes"]);

	while(livello>=1){
		var nodi = JSON.parse(JSON.stringify(graph["nodes"]));  /*creo due nuovi array contenenti nodi e archi*/
		var links = JSON.parse(JSON.stringify(graph["links"]));
		etichettaPadreNeiFigli(nodi,graph); 	//aggiungo il campo padre nei figli
		aggiungiNodiDummy(nodi,links);        	//inserisco i nodi e gli archi dummy
		spostaArchiFoglie(nodi,links);
		superLinks=inserisciSuperLinksPrimoLivello(nodi,links); //creo i superLinks
		inserisciSuperLinksRimanenti(nodi,links,superLinks);
		assegnaBaricentro(nodi,links,superLinks);    //assegno il campo baricentro ad ogni nodo host non dummy
		euristicaBaricentro(nodi,livello);	
					//lancio l'euristica del baricentro
		copiaAttributoDestroSinistroAGraph(nodi,graph); //copio gli attributi right child e left child al grafo iniziale
		setHostNodesXCoordinates(graph, separatorBetweenHostNodesWidth, treeHeight); //aggiorno alle nuove coordinate
		setParasiteNodesXCoordinates(graph, hostLeaf2ParasiteLeaf, hostLeafWidth, longLeaves, "5");
	livello--;}
		 //rilancio l'euristica con le nuove coordinate
	
					
							
	/*alert(pippo.map(function(d){return d["id"].concat(d["baricentro"])}));*/
	
	
}

function copiaAttributoDestroSinistroAGraph(nodi,graph){
	nodi.forEach(function(d){if(!d["dummy"]){
		appoggio=findNodeById(d["id"],graph);
		appoggio["right_child"]=d["right_child"];
		appoggio["left_child"]=d["left_child"];}
	});
}


function trovaFigli(nodoId,nodi){
return nodi.filter(function(d){ 
			return d["padre"].toString()==nodoId.toString()})
			.map(function(v){return v["id"]});
}

function trovaNodoDaId(IdNodo,nodi){
return nodi.find(function(d){return d["id"]==IdNodo;});
}
function etichettaPadreNeiFigli(nodi,graph){
	nodi.forEach(function(v){
			
			v["padre"]=findParentIdsOfHostOrParasiteNode(v["id"],v["id"].substring(0,1),graph);});
}


function MaxDepth(nodi){
var maxDepth=0;
	nodi.forEach(function(d){
			temp=parseInt(d["depth"]);
			if (maxDepth<temp){
				 maxDepth=temp;}
		});
return maxDepth;
}
	



function aggiungiNodiDummy(nodi,links){
	maxDepth=MaxDepth(nodi);
		id=1;
		id2="dummy";
	function vai(i){
		d=nodi[i];
				if (d!=null){
				
				isDummy=d["dummy"];
				if (isDummy==undefined)
					isDummy=false;
				if (d["leafParassite"]||d["leafHost"]||isDummy){
					depth=parseInt(d["depth"]);
					if(depth<maxDepth){
						depth++;
						if(depth==maxDepth)
							depthReached=true;
							else
							depthReached=false;
						x=depth.toString();
						y=d["id"].substring(0,1).concat(id2.concat(id.toString()));
						nodi.push({"depth": x, "id": y ,"dummy":true,"padre":d["id"],"depthReached":depthReached});
						links.push({"dummy1":true, "from":d["id"],"to": y});
						
						id++;
					}
				}
			i++;
			vai(i);}
			}
		vai(0);
	
}

function spostaArchiFoglie(nodi,links){
maxDepth=MaxDepth(nodi);
nodi.forEach(function(d){ if(d["leafParassite"]||d["leafHost"]){
				depth=parseInt(d["depth"]);
				 if(depth<maxDepth){
					pippo=depth;
					temp=d["id"];
					while(pippo<maxDepth){
						temp=trovaFigli(temp,nodi);
						pippo++;
					}
					links.filter(function(v){return v["dummy"] && v["from"].toString()==d["id"].toString()}).map(function(x){x["from"]=temp}); 
					links.filter(function(v){return v["dummy"] && v["to"].toString()==d["id"].toString()}).map(function(x){x["to"]=temp}); 
				 }
			}
	});
}

function inserisciSuperLinksPrimoLivello(nodi,links){
	var superLinks=[];
	maxDepth=MaxDepth(nodi);
	maxDepth2=maxDepth-1;
	depthToString=maxDepth2.toString();
	links.forEach(function(d){if(d["dummy"] && !d["roots_connector"]){
					if(existInSuperLinks(superLinks,d)){
						superLinks.filter(function(v){return v["from"].toString()==d["from"].toString() && v["to"].toString()==d["to"].toString()}).map(function(x){
							
							temp=parseInt(x["peso"]); 
							temp++;
							temp2=temp.toString();
							x["peso"]=temp2;});
					}else{
					da=trovaNodoDaId(d["from"],nodi);
					a=trovaNodoDaId(d["to"],nodi);
					superLinks.push({"from":da["id"],"to":a["id"],"peso":"1","dummy":true,"depth":maxDepth.toString()});
					superLinks.push({"from":da["padre"],"to":a["padre"],"peso":"1","dummy":true,"depth":depthToString});
					}
				}});
return superLinks;
}


function inserisciSuperLinksRimanenti(nodi,links,superLinks){
	maxDepth=MaxDepth(nodi);
	maxDepth--;
	function appoggio(i,depth){		
			depthString=depth.toString();
			d=superLinks[i];
			if(d!=null){
				if(d["depth"]==depthString){
					nodoFrom=trovaNodoDaId(d["from"],nodi);
					nodoTo=trovaNodoDaId(d["to"],nodi);
					if(existSuperLinksParent(superLinks,nodoFrom["padre"],nodoTo["padre"])){
					superLinks.filter(function(x){return x["from"].toString()==nodoFrom["padre"].toString() && x["to"].toString()==nodoTo["padre"].toString()})
						.map(function(y){
						temp=parseInt(d["peso"]);
						temp2=parseInt(y["peso"]);
						temp3=temp+temp2;
						temp4=temp3.toString();
						y["peso"]=temp4;});

					}else{
					depth3=depth-1;
					depthToString=depth3.toString();
				superLinks.push({"from":nodoFrom["padre"],"to":nodoTo["padre"],"peso":d["peso"],"dummy":true,"depth":depthToString});
					}
				}
			i++;
			appoggio(i,depth);
			}
		}
		for(depth=maxDepth;depth>1;depth--){
		appoggio(0,depth);}
		
		
}



function assegnaBaricentro(nodi,links,superLinks){
	depth=MaxDepth(nodi);
	while(depth>=1){
		depthToString=depth.toString();
		
		nodi.filter(function(d){return d["id"].substring(0,1)=="H"&& d["depth"]==depthToString && d["dummy"]!=true})
				.forEach(function(v){collegamenti=superLinks.filter(function(y){return y["from"].toString()==v["id"].toString()});
							cont=0;
							var pesoPerX=0;
							collegamenti.forEach(function(z){cont++;
											nodo=trovaNodoDaId(z["to"],nodi);
											while(nodo["dummy"]){
												idPadre=nodo["padre"];
												padre=trovaNodoDaId(idPadre,nodi);
												nodo=padre;
											}
											  pesoPerX=pesoPerX+parseInt(z["peso"])*nodo["x"];});
							baricentro=pesoPerX/cont;
							v["baricentro"]=baricentro;});
		depth--;}
						     
						
}
function euristicaBaricentro(nodi,depth){
		
		depthToString=depth.toString();
		
		nodi.filter(function(d){return d["id"].substring(0,1)=="H" && d["depth"].toString()==depthToString.toString() && d["dummy"]!=true})
			.forEach(function(v){	
						sonsIds=trovaFigli(v["padre"],nodi);
						if(v["left_child"]){
							
							if(sonsIds[0].toString()==v["id"].toString()){
								destro=trovaNodoDaId(sonsIds[1],nodi);
								if(v["baricentro"]>destro["baricentro"]){
									v["left_child"]=false;
									v["right_child"]=true;
									destro["left_child"]=true;
									destro["right_child"]=false;
									}
							}else{
							destro=trovaNodoDaId(sonsIds[0],nodi);
								if(v["baricentro"]>destro["baricentro"]){
									v["left_child"]=false;
									v["right_child"]=true;
									destro["left_child"]=true;
									destro["right_child"]=false;
									}
							}
						}else{ 
								if(sonsIds[0].toString()==v["id"].toString()){
								sinistro=trovaNodoDaId(sonsIds[1],nodi);
								if(sinistro["baricentro"]>v["baricentro"]){
									
									v["left_child"]=true;
									v["right_child"]=false;
									sinistro["left_child"]=false;
									sinistro["right_child"]=true;
									}
							}else{
								
								
								sinistro=trovaNodoDaId(sonsIds[0],nodi);
								if(sinistro["baricentro"]>v["baricentro"]){
									v["left_child"]=true;
									v["right_child"]=false;
									sinistro["left_child"]=false;
									sinistro["right_child"]=true;
									}}
							}});
		
}
function existInSuperLinks(array,link){
	appoggio=array.filter(function(d){return d["from"].toString()==link["from"].toString() && d["to"].toString()==link["to"].toString()});
	return appoggio.length!=0;

}
function existSuperLinksParent(array,idFromPadre,idToPadre){
	appoggio=array.filter(function(d){return d["from"].toString()==idFromPadre.toString() && d["to"].toString()==idToPadre.toString()});
	return appoggio.length!=0;

}

