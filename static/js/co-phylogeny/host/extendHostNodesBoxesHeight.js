function extendHostNodesBoxesHeight(graph, height) {

    graph.nodiHost
    	.filter(function(n){return n["leafHost"]==true; })
    	.forEach(function(n) {
            n["height"]=height - n["y"];
        });
}
