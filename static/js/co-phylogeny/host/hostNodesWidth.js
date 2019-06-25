function setHostNodesWidth(graph, hostLeaf2ParasiteLeaf, hostLeafWidth, separatorBetweenHostNodesWidth, width, host2parasiteSubtreeHeight, parasite2HeightInSubtree, host2ListParasite, treeHeight) {

        // This algorithm works in two steps:
        // 1. Bottom up traversal: assign the width to the leaves using the default leafWidth and then
        //  assign the width to the parents as the sum of the width of the children + the separatorWidth
        // 2. Top down traversal: scale the boxes to fit them into the canvas size. (If the root is too big, the scale
        //  factor will be < 1, otherwise, if it's too small, the scale factor will be > 1

        // This function implements step 1
/*    
    function maxNumberNodeParasiteInSubTree(node){
        // calcola quanti nodi parassiti al massimo sono da allineare in uno stesso livello


        var sottolivelli = graph["nodes"].filter(function(d){return host2ListParasite.get(node["id"]).indexOf(d["id"])!=-1;})
                            .map(function(d) {return parasite2HeightInSubtree.get(d["id"]); })
                            .sort(function(a,b){return a-b;})

        var larghezzaSottolivelli=[];
        var costante=sottolivelli[0];
        var contatore=0;
        for (var i = 0; i < sottolivelli.length ; i++) {
            if (sottolivelli[i]==costante) 
                { contatore =contatore + 1 ;}
            else { 
                larghezzaSottolivelli.push(contatore);
                contatore = 1;
                costante = sottolivelli[i];
            }
        }
        larghezzaSottolivelli.push(contatore);
        

        return d3.max(larghezzaSottolivelli);

    }  
*/




        function setInitialWidth(node, graph) {


            var sonsIds = findChildrenIdsOfHostOrParasiteNode(node["id"], "H", graph);


            // Exit condition
            if (sonsIds.length == 0) {
                var numberParasite = hostLeaf2ParasiteLeaf.get(node["id"]).length;
                if(numberParasite==0){ numberParasite = 1;}
                node["width"] = hostLeafWidth * numberParasite;
                return node["width"];
            }

            // Node is not a leaf. Find width of children
            node["width"] = 0;
            sonsIds.forEach(function(sonId) {
                node["width"] += setInitialWidth(findNodeById(sonId, graph), graph);
            });

            // Sum the separatorWidth
            //node["width"] += Math.pow(separatorBetweenHostNodesWidth, ((treeHeight - node["depth"])));
            node["width"] += separatorBetweenHostNodesWidth * (treeHeight - node["depth"]);
            return node["width"];
        }

        var rootNode = graph.nodiHost
                        .filter(function(n){ return  n["right_child"]==false && 
                                                     n["left_child"]==false;})[0];
        
        setInitialWidth(rootNode, graph);

        // scaleFactor > 1 if rootNode["width"] < width, otherwise scaleFactor < 1.
        var scaleFactor = (width -5) / rootNode["width"];
        
        graph.nodiHost
            .forEach(function(d) {
                d["width"] *= scaleFactor; 
        });


        // Scale also the separatorWidth, it will be used to compute the x coordinates of the boxes later
        separatorBetweenHostNodesWidth *= scaleFactor;
        hostLeafWidth *=scaleFactor;
        return [hostLeafWidth, separatorBetweenHostNodesWidth,scaleFactor];
    }