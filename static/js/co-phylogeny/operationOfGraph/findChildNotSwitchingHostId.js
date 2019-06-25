function findChildNotSwitchingHostId(children, graph) {
            
/*
    // Isolate links of parasite tree
    var parasiteLinks = graph["links"].filter(function(link) {
        return link["from"].startsWith("P") && link["to"].startsWith("P")
    });
    
    // Return the id of the first child that does not switch host
    for (i = 0; i < children.length; i++) {
        for (var j = 0; j < parasiteLinks.length; j++) {
            if (parasiteLinks[j]["to"] == children[i] && parasiteLinks[j]["switch"] == false) {
                console.log(children[i]);
                return children[i]
            }
        }
    }
*/    

    var child = graph["links"]
            .filter(function(link) {return  link["switch"] == false 
                                            && link["from"].startsWith("P")
                                            && (children.indexOf(link["to"]) != -1)
                                            })[0]["to"];

    if(child != undefined){
        return child;
    }
    

    throw "No non-host-switching child found"
}