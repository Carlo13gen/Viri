
// N.B.: Type is either "P" or "H"
function findChildrenIdsOfHostOrParasiteNode(idNode, type, graph) {
	var links = restitusciListaArchi(type, idNode.substring(0, 1),graph);

    return links
        .filter(function(d){ return d["to"].startsWith(type) && d["from"] == idNode}) 
        // Children
        .map(function(d) {return d["to"]});
}

function restitusciListaArchi(typeTo,typeFrom,graph) {

	if (typeTo=="H"){
		if(typeFrom == "H"){
			return graph.linksHostHost;
		}else{
			return graph.linksParassiteHost;
		}

	}else{

		if(typeFrom == "H"){
			return graph.linksHostParassite;
		}else{
			return graph.linksParassiteParassite;
		}
	}
}