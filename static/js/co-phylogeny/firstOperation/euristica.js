 //Euristica
    function Euristica(graph, host2ListParasiteInHostSubTree){
        function rapporto(listaParassiti1, listaParassiti2, archi){
            function supporto(from, to, archi){
                //restituisce quanti sono gli archi che vanno da una lista di nodi parassiti all'altra
                return archi
                            .filter(function(d) {return from.indexOf(d["from"]) > -1})
                            .filter(function(d) {return to.indexOf(d["to"]) > -1})
                            .length;
            }
            var listeDiListeParassiti = [listaParassiti1,listaParassiti2]
            var listaValoreSupporto = listeDiListeParassiti.map( function (d){
                if(d==listaParassiti1){
                    return supporto(listaParassiti1, listaParassiti2, archi);
                }else{
                    return supporto(listaParassiti2, listaParassiti1, archi);
                }
                }
            )

            return listaValoreSupporto[1]+listaValoreSupporto[0]

//            return supporto(listaParassiti1, listaParassiti2, archi) + supporto(listaParassiti2, listaParassiti1, archi);
        }
    

        function indicareFiglioDestroESinistro(nodoId, parassitiADestra, parassitiASinistra, graph, host2ListParasiteInHostSubTree, archi){
            var sonsIds = findChildrenIdsOfHostOrParasiteNode(nodoId, "H", graph);
            

                // Leaves
            if (sonsIds.length == 0) {
            
            }
            else{

                var tendenze = sonsIds.map(function(d){
                    var listParassiteChild = host2ListParasiteInHostSubTree.get(d);
                    var rapportoChildAntDestro= rapporto(listParassiteChild, parassitiADestra, archi);
                    var rapportoChildAntSinistra= rapporto(listParassiteChild, parassitiASinistra, archi);
                    return [(rapportoChildAntDestro - rapportoChildAntSinistra),listParassiteChild]; 
                })

                var destro;
                var sinistro;

                if( tendenze[0][0] > tendenze[1][0] ){
                        destro=sonsIds[0];
                        sinistro=sonsIds[1];
                        var listParasiteDestro=tendenze[0][1];
                        var listParasiteSinistro=tendenze[1][1];
                }else{
                        destro=sonsIds[1];
                        sinistro=sonsIds[0];
                        var listParasiteDestro=tendenze[1][1];
                        var listParasiteSinistro=tendenze[0][1];
                }

                sonsIds.forEach(function(d){
                    if(d==destro){
                        indicareFiglioDestroESinistro(d, parassitiADestra, parassitiASinistra.concat(listParasiteSinistro), graph, host2ListParasiteInHostSubTree, archi);
                        childDestro= findNodeById(destro,graph);
                        childDestro["right_child"]=true;
                        childDestro["left_child"]=false;
                    } else {
                        indicareFiglioDestroESinistro(d, parassitiADestra.concat(listParasiteDestro), parassitiASinistra, graph, host2ListParasiteInHostSubTree, archi);
                        childSinistro= findNodeById(sinistro,graph);
                        childSinistro["right_child"]=false;
                        childSinistro["left_child"]=true;
                    }
                });
            }
        }
        var rootsConnector = graph["links"].filter(function(d) {return d["roots_connector"] == true})[0];
        var rootIdHost = rootsConnector["to"].startsWith("H") ? rootsConnector["to"] : rootsConnector["from"];
        var archiParassiti = graph.linksParassiteParassite.filter(function(d){return d["switch"]==true && d["from"].startsWith("P") && d["to"].startsWith("P") })
        indicareFiglioDestroESinistro(rootIdHost, [], [], graph, host2ListParasiteInHostSubTree, archiParassiti);
        
        var rootHost= findNodeById(rootIdHost,graph);
        rootHost["right_child"]=false;
        rootHost["left_child"]=false;
              
    }

//Euristica Random

  function EuristicaRandom(graph, host2ListParasiteInHostSubTree,rootId){
	function indicareFiglioDestroESinistro(nodoId, parassitiADestra, parassitiASinistra, graph, host2ListParasiteInHostSubTree){
		var sonsIds = findChildrenIdsOfHostOrParasiteNode(nodoId, "H", graph);
            

                // Leaves
                if (sonsIds.length == 0) {
            
            }
            else{ 
		var destro;
		var sinistro;
	  	var random_boolean = Math.random() >= 0.5;
		if (random_boolean){
			destro=sonsIds[0];
                        sinistro=sonsIds[1];
			var listParasiteDestro=host2ListParasiteInHostSubTree.get(destro);
			var listParasiteSinistro=host2ListParasiteInHostSubTree.get(sinistro);
		}else{
			destro=sonsIds[1];
                        sinistro=sonsIds[0];
			var listParasiteDestro=host2ListParasiteInHostSubTree.get(destro);
			var listParasiteSinistro=host2ListParasiteInHostSubTree.get(sinistro);
			} sonsIds.forEach(function(d){
                    if(d==destro){
                        indicareFiglioDestroESinistro(d, parassitiADestra, parassitiASinistra.concat(listParasiteSinistro), graph, host2ListParasiteInHostSubTree);
                        childDestro= findNodeById(destro,graph);
                        childDestro["right_child"]=true;
                        childDestro["left_child"]=false;
                    } else {
                        indicareFiglioDestroESinistro(d, parassitiADestra.concat(listParasiteDestro), parassitiASinistra, graph, host2ListParasiteInHostSubTree);
                        childSinistro= findNodeById(sinistro,graph);
                        childSinistro["right_child"]=false;
                        childSinistro["left_child"]=true;
                    }
                });
            }
        }
       
       indicareFiglioDestroESinistro(rootId, [], [], graph, host2ListParasiteInHostSubTree);
        
        var rootHost= findNodeById(rootId,graph);
        rootHost["right_child"]=false;
        rootHost["left_child"]=false;
              
    }
		



    function setRightChildAndLeftChild(graph, host2ListParasiteInHostSubTree, euristica,hostLeaf2ParasiteLeaf,longLeaves,hostLeafWidth,separatorBetweenHostNodesWidth, treeHeight){
        if((graph.nodiHost[0]["right_child_gdt"])==undefined){
            if(euristica=="1"){
                var millisecondStart = new Date();        
                Euristica(graph, host2ListParasiteInHostSubTree);
		var millisecondEnd = new Date();
                graph["tempoEuristicaRapp"] = (millisecondEnd- millisecondStart) /1000 ;
            }
            if(euristica == "2"){
                //il grafo è planare
                graph.nodiHost
                    .forEach(
                        function(d){
                            d["right_child"] = d["right_child_euristica2"];
                            d["left_child"] = d["left_child_euristica2"];
                        });    
                graph["tempoEuristicaRapp"] = 0;
            }
            if(euristica == "3"){
                //il grafo è planare
                graph.nodiHost
                    .forEach(
                        function(d){
                            d["right_child"] = d["right_child_euristica2_solution"];
                            d["left_child"] = d["left_child_euristica2_solution"];
                        });    
                graph["tempoEuristicaRapp"] = 0;
            }
            if(euristica=="4"){
                var millisecondStart = new Date();        
		var rootsConnector = graph["links"].filter(function(d) {return d["roots_connector"] == true})[0];
      	 var rootIdHost = rootsConnector["to"].startsWith("H") ? rootsConnector["to"] : rootsConnector["from"];
                EuristicaRandom(graph, host2ListParasiteInHostSubTree,rootIdHost);
                var millisecondEnd = new Date();
                graph["tempoEuristicaRandom"] = (millisecondEnd- millisecondStart) /1000 ;
            }
	     if(euristica=="5"){
                var millisecondStart = new Date();  
      		var rootsConnector = graph["links"].filter(function(d) {return d["roots_connector"] == true})[0];
      	 var rootIdHost = rootsConnector["to"].startsWith("H") ? rootsConnector["to"] : rootsConnector["from"];
                copia(graph, host2ListParasiteInHostSubTree,rootIdHost,hostLeaf2ParasiteLeaf,longLeaves,hostLeafWidth,separatorBetweenHostNodesWidth, treeHeight);
                var millisecondEnd = new Date();
                graph["tempoEuristicaRandom"] = (millisecondEnd- millisecondStart) /1000 ;
            }


        } else {
            //il grafo è planare
            graph.nodiHost
            .forEach(
                function(d){
                   d["right_child"] = d["right_child_gdt"];
                   d["left_child"] = d["left_child_gdt"];
                });
            
            graph["tempoEuristicaRapp"] = 0;

        }


    }
