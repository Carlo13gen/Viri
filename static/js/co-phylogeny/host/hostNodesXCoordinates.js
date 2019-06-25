    function setHostNodesXCoordinates(graph, separatorBetweenHostNodesWidth, treeHeight) {
        
        function setXCoordinateOfChildren(node, graph) {

            var leftChildId = findLeftOrRightChildId(node, graph, "left_child");
            var rightChildId = findLeftOrRightChildId(node, graph, "right_child");

            var leftChild = findNodeById(leftChildId, graph);
            leftChild["x"] = node["x"];

            var rightChild = findNodeById(rightChildId, graph);
            //rightChild["x"] = leftChild["x"] + leftChild["width"] +	Math.pow(separatorBetweenHostNodesWidth, ((treeHeight - node["depth"])))
            rightChild["x"] = leftChild["x"] + leftChild["width"] + separatorBetweenHostNodesWidth * (treeHeight - node["depth"]);


            // Recursive calls
            var children=[leftChild, rightChild];
            children.forEach(function (d){
                if (findChildrenIdsOfHostOrParasiteNode(d["id"], "H", graph).length != 0)
                    setXCoordinateOfChildren(d, graph);
            });

/*
            if (findChildrenIdsOfHostOrParasiteNode(leftChild, "H", graph).length != 0)
                setXCoordinateOfChildren(leftChild, graph);

            if (findChildrenIdsOfHostOrParasiteNode(rightChild, "H", graph).length != 0)
                setXCoordinateOfChildren(rightChild, graph);
*/
        }

        var rootNode = graph.nodiHost
                        .filter(function(n){ return  n["right_child"]==false && 
                                                     n["left_child"]==false;})[0];
        rootNode["x"] = 5;
        
        setXCoordinateOfChildren(rootNode, graph)
    }