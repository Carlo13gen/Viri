// N.B.: Type is either "P" or "H"
function findParentIdsOfHostOrParasiteNode(idNode, type, graph) {
	var links = restitusciListaArchi(idNode.substring(0, 1),type,graph);
    return links
        .filter(function(d){ return d["from"].startsWith(type) && d["to"] == idNode}) 
        // Children
        .map(function(d) {return d["from"]});
}
