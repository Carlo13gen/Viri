function ridisegna(disegna,  curve, curveSwitch,longLeaves, equalsLivelEqualsHeight, width, height, hostLeafWidth, hostNodeBoxHeight, separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight, parasiteNodesRadius,euristica, graph){
    var millisecondStart = new Date();
    if(graph == undefined){
        var graph = data;
    }

    /*
    if (graph != undefined){
        var graph = graph;
    } else {
        console.log(graph);
        var graph = graph;
        var graph = graph;
    }

      var nodiParassiti;
        var nodiHost;
        
        var linksFromTo
        var linksParassiteParassite
        var linksHostParassite  
        var linksParassiteHost
        var linksHostHost
*/
    var euristica = euristicaDefined(euristica);
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
   
    

    //var separatorBetweenHostNodesHight=0;
    var down = true;
            /*** PRE-PROCESSING ***/
            // Set nodes depths
            setHostNodesDepths(graph);
            // Compute tree depth
            var treeHeight = computeHeightOfHostTree(graph);
            var mappings = computeMappings(graph, treeHeight);
            var host2parasiteSubtreeHeight = mappings[0];
            var parasite2HeightInSubtree = mappings[1];
            var hostDeph2HeightMax = mappings[2];
            var hostLeaf2ParasiteLeaf = mappings[3]
            var host2ListParasiteInHostSubTree = mappings[4];
            var host2ListParasite = mappings[5];
            separatorBetweenHostNodesWidth=separatorBetweenHostNodesWidth/2;
            /*** END PRE-PROCESSING ***/

            /*** if Bloccato ***/
            if((bloccatoDefined()) && ((disegna==undefined) || (!(disegna)))){
                var host2isRightIsLeaf = getListaBloccata();
                if(host2isRightIsLeaf==undefined){
                }else {
                graph.nodiHost
                .forEach(
                    function(d){
                        var result = host2isRightIsLeaf.get(d["id"]);
                        if(result!=undefined){
                        d["right_child"] = result[0];
                        d["left_child"] = result[1];
                    }
                    else {
                        setRightChildAndLeftChild(graph, host2ListParasiteInHostSubTree, euristica,hostLeaf2ParasiteLeaf,longLeaves,hostLeafWidth,separatorBetweenHostNodesWidth, treeHeight);
                    }
                    }
                );
            }
            } else {

            /*** Euristica ***/
            
            setRightChildAndLeftChild(graph, host2ListParasiteInHostSubTree, euristica,hostLeaf2ParasiteLeaf,longLeaves,hostLeafWidth,separatorBetweenHostNodesWidth, treeHeight);
	
            }
            /*** END Euristica ***/
            

            //if(down){            
                var result = sistemare(graph, host2ListParasite, host2parasiteSubtreeHeight, hostDeph2HeightMax, host2parasiteSubtreeHeight,treeHeight)
                var parassitaLivelloHost=result[1];
                host2parasiteSubtreeHeight= result[0];
                hostDeph2HeightMax = result[2];
                var archiCiclo = result[3]
            /*
            }else{
                var parassitaLivelloHost=undefined;
            }
            */

            // Select the svg element, if it exists.
            d3.select("svg").remove();
            d3.select("#tooltip").remove();
            d3.select("#spazioSvg").append("svg")
                .attr("id", "svgCoPhylo");
            var svg = d3.select("#svgCoPhylo")
            
            /*** HOST TREE ***/
            // Set width for each node of the host tree
            var result = setHostNodesWidth(graph, hostLeaf2ParasiteLeaf, hostLeafWidth, separatorBetweenHostNodesWidth, width, host2parasiteSubtreeHeight, parasite2HeightInSubtree, host2ListParasite, treeHeight);
                hostLeafWidth = result[0]; 
                separatorBetweenHostNodesWidth = result[1];
            var scaleFactorWidth = result[2];

            // Set x coordinates of nodes of host tree
            setHostNodesXCoordinates(graph, separatorBetweenHostNodesWidth, treeHeight);

            // Set fixed height for each box of the host tree
            var result2 = setHostNodesBoxesHeight(graph, treeHeight, hostDeph2HeightMax, hostNodeBoxHeight, height, separatorBetweenHostNodesHight, host2parasiteSubtreeHeight, equalsLivelEqualsHeight);
                hostNodeBoxHeight = result2[0];
                var scaleFactorHeight2 = result2[1];

            // Set y coordinates of the nodes of the host tree
            setHostNodesBoxesYCoordinates(graph, treeHeight, hostDeph2HeightMax, hostNodeBoxHeight, separatorBetweenHostNodesHight, host2parasiteSubtreeHeight, equalsLivelEqualsHeight);
            // OPTIONAL: Extend leaves height to the maximum available
            
            var scaleFactorHeight=height/(d3.max(graph.nodiHost
                                    .map(function(d){return d["y"]+d["height"]})));

            graph.nodiHost
                                    .forEach(function(d){ 
                                        d["y"] *= scaleFactorHeight ;
                                        d["height"] *= scaleFactorHeight;
                                        d["heightOriginal"] = d["height"]
                                        })
            hostNodeBoxHeight *= scaleFactorHeight;

            parasiteNodesRadius *= d3.min([scaleFactorWidth, (scaleFactorHeight*scaleFactorHeight2)])

            if (longLeaves) 
                extendHostNodesBoxesHeight(graph, height);
            
            /*** END HOST TREE ***/

            /*** PARASITE TREE NODES ***/
            // Set y coordinate of parasite nodes
            setParasiteNodesYCoordinates(graph, parasite2HeightInSubtree, host2parasiteSubtreeHeight, hostNodeBoxHeight, parassitaLivelloHost, down);
            // Set x coordinate of parasite nodes
            setParasiteNodesXCoordinates(graph, hostLeaf2ParasiteLeaf, hostLeafWidth, longLeaves, euristica);

            //*** END PARASITE TREE NODES ***/


            if ((disegna)||(disegna==undefined)) {
                drawnHost(svg, height, width, graph, hostNodeBoxHeight, separatorBetweenHostNodesWidth, treeHeight ,parasiteNodesRadius);
                drawnParassiteNodes(svg, graph, parasiteNodesRadius)
                var parasiteTreeLinksGroupEnter = svg.insert("g", "#parasiteTreeNodes").attr("id", "parasiteTreeLinks");
                drawnParassiteEdgeNonSwitch(graph, parasiteTreeLinksGroupEnter, hostLeafWidth, hostNodeBoxHeight, curve, parasiteNodesRadius);            
                drawnParassiteEdgeSwitch(graph, parasiteTreeLinksGroupEnter, hostLeafWidth, hostNodeBoxHeight, curveSwitch, parasiteNodesRadius, archiCiclo);
                drawnParassiteEdgeSwitchCicle(graph, parasiteTreeLinksGroupEnter, hostLeafWidth, hostNodeBoxHeight, curveSwitch, parasiteNodesRadius, archiCiclo);
                if(document.getElementById("tooltip") == undefined){
                    tooltip = d3.select("body")
                        .append("div")
                        .attr("id", "tooltip")
                        .style("visibility", "visible");
                }
                animazione();
                if(bloccatoDefined()){
                    var host2isRightIsLeaf = getListaBloccata();
                    var result = host2isRightIsLeaf.get(graph.nodiHost[0]["id"]);
                    if(result==undefined){
                        bloccato();
                    }
                }
                var millisecondEnd = new Date();
                graph["tempoDiCalcoloPerDisegnare"] = ((millisecondEnd - millisecondStart)/1000) - graph["tempoEuristicaRapp"];
            } else{ 
                //return contaIncroci(graph, curve, curveSwitch, parasiteNodesRadius);
                var millisecondEnd = new Date();
                graph["tempoDiCalcoloPerDisegnare"] = ((millisecondEnd - millisecondStart)/1000) - graph["tempoEuristicaRapp"];
                var result = contaIncroci(graph, curve, curveSwitch, parasiteNodesRadius);
                var contatoreTotale = result[0];
                var contatoreNoSwitch = result[1];
                var contatoreSwitchNoSwitch = result[2];
                var contatoreSwitchSwitch = result[3];
                
                return [contatoreTotale,contatoreNoSwitch,contatoreSwitchNoSwitch,contatoreSwitchSwitch]

            //return "TOT:"+contatoreTotale + " incrociNoSwitch:" + contatoreNoSwitch+ " incrociSwitchNoSwitch:"+contatoreSwitchNoSwitch+" incrociSwitchSwitch:"+ contatoreSwitchSwitch+" numberSolution:"+ numberSolution ;

            }


            
}
