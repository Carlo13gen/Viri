function numberOfChildrenSwitchingHost(children, graph) {

/*
    var numberOfChildrenSwitchingHost = 0;
    children.forEach(function(childId) {
        graph["links"].forEach(function(link) {
            if (link["to"] == childId && link["switch"] == true)
                numberOfChildrenSwitchingHost += 1;
        })
    });
    
    return numberOfChildrenSwitchingHost
*/
    return  graph.linksParassiteParassite
                .filter(function(link){return link["switch"] == true && ((children.indexOf(link["to"]) != -1));})
                .length;
    
}