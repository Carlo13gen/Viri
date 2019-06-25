function findNodeById(id, graph) {
    //console.log(id)

    try{
        if(id.startsWith("H")){
            var nodi=graph.nodiHost;
        }else{
            var nodi=graph.nodiParassiti;
        }
    }catch(err){
        if(id[0].startsWith("H")){
            var nodi=graph.nodiHost;
        }else{
            var nodi=graph.nodiParassiti;
        }
    }
    var node = nodi.find(function(d){return d["id"]==id;});
	if(node != undefined){
		return node;
	}
    
    throw "Node not found"

}

function elaboraNodi(graph){
    //if (graph.nodiParassiti == undefined){
        graph.nodiParassiti = graph["nodes"].filter(function(d){return d["id"].startsWith("P");});
        graph.nodiHost = graph["nodes"].filter(function(d){return d["id"].startsWith("H");});
        graph.linksParassiteParassite =graph["links"].filter(function(d){ return d["to"].startsWith("P") && d["from"].startsWith("P")});
        graph.linksHostParassite= graph["links"].filter(function(d){ return d["to"].startsWith("P") && d["from"].startsWith("H")});
        graph.linksParassiteHost= graph["links"].filter(function(d){ return d["to"].startsWith("H") && d["from"].startsWith("P")});
        graph.linksHostHost= graph["links"].filter(function(d){ return d["to"].startsWith("H") && d["from"].startsWith("H")});
    //}
}