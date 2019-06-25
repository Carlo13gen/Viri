    function setHostNodesDepths(graph) {

        function setDepthOfChildren(node, graph, currentDepth) {

            var sonsIds = findChildrenIdsOfHostOrParasiteNode(node["id"], "H", graph);

            if (sonsIds.length > 0) {
                sonsIds.forEach(function(id) {
                    var son = findNodeById(id, graph);
                    son["depth"] = currentDepth + 1;
                    setDepthOfChildren(son, graph, son["depth"])
                })
            }
        }
        
        var rootsConnector = graph["links"].filter(function(d) {return d["roots_connector"] == true})[0];
        var rootIdHost = rootsConnector["to"].startsWith("H") ? rootsConnector["to"] : rootsConnector["from"];
        var rootNode = findNodeById(rootIdHost, graph);
        rootNode["depth"] = 0;
        setDepthOfChildren(rootNode, graph, rootNode["depth"]);

    }
