function setHostNodesBoxesYCoordinates(graph, treeHeight, hostDeph2HeightMax, hostNodeBoxHeight, separatorBetweenHostNodesHight, host2parasiteSubtreeHeight, equalsLivelEqualsHeight) {

        function setYCoordinatesOfChildren(yParent, node, graph) {

            var sonsIds = findChildrenIdsOfHostOrParasiteNode(node["id"], "H", graph);

            if (sonsIds.length > 0) {
                sonsIds.forEach(function(id) {
                    var son = findNodeById(id, graph);
                    son["y"] = yParent;
                    if(equalsLivelEqualsHeight){
                        heightMaxOfParassiteSubTreeInThisLevel =hostDeph2HeightMax.get(son["depth"]);
                    }else{
                        heightMaxOfParassiteSubTreeInThisLevel =host2parasiteSubtreeHeight.get(son["id"]);
                    }                    

                        ySon=((heightMaxOfParassiteSubTreeInThisLevel + 1)*hostNodeBoxHeight)+yParent+separatorBetweenHostNodesHight;
                    setYCoordinatesOfChildren(ySon, son, graph, son["depth"])
                })
            }
        }

        var rootNode = graph.nodiHost
                        .filter(function(n){ return n["right_child"]==false && 
                                                     n["left_child"]==false;})[0]; 
                    
        rootNode["y"] = 0;
        yChildOfRootHost=((hostDeph2HeightMax.get(rootNode["depth"])+1) * hostNodeBoxHeight) +separatorBetweenHostNodesHight;
        setYCoordinatesOfChildren(yChildOfRootHost, rootNode, graph, rootNode["depth"]);

    }