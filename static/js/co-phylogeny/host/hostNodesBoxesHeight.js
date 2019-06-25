    // This algorithm works in two phases.
    // 1. It first assigns a default height to each node
    // 2. It scales the heights according to the height of the canvas
    function setHostNodesBoxesHeight(graph, treeDepth, hostDeph2HeightMax, hostNodeBoxHeight, height, separatorBetweenHostNodesHight, host2parasiteSubtreeHeight, equalsLivelEqualsHeight) {

        //plane & suplane
        var depthHostWithDepthSubTreeHost=0;
        for (i=0;i<treeDepth;i++) {
               depthHostWithDepthSubTreeHost += hostDeph2HeightMax.get(i)+1;
        }
        
        var scaleFactor = (height - separatorBetweenHostNodesHight * (treeDepth)) / (depthHostWithDepthSubTreeHost * hostNodeBoxHeight);

        // This code implements step 2
        if(equalsLivelEqualsHeight){

        graph.nodiHost
            .forEach(function(d) {
                d["height"] =(hostNodeBoxHeight*(1+hostDeph2HeightMax.get(d["depth"])))* scaleFactor; 

        });

        }else{

        graph.nodiHost
            .forEach(function(d) {
                d["height"] =(hostNodeBoxHeight*(1+ host2parasiteSubtreeHeight.get(d["id"])))* scaleFactor; 
        });
        }
        // We need to scale the nodeHeight as well. For the y coordinates that will be assigned later
        hostNodeBoxHeight *= scaleFactor;

        return [hostNodeBoxHeight, scaleFactor]

        }