    function computeMappings(graph, treeHeight) {
        var parasite2HeightInSubtree = d3.map();
        var hostDeph2HeightMax = d3.map();
        var host2parasiteSubtreeHeight = d3.map();
        var hostLeaf2ParasiteLeafNumber =d3.map();
        var hostLeaf2ParasiteLeaf =d3.map();
        var host2ListParasite=d3.map();
        var host2ListParasiteInHostSubTree=d3.map();
        var host2ListParasiteInHostSubTreeSwithLinkStartOrEnd = d3.map();
        
        function traverseParasiteTree(node, graph) {
            

            var hostCurrentParasite = node["belongs_to"];
            var sonsIds = findChildrenIdsOfHostOrParasiteNode(node["id"], "P", graph);

            // Leaves
            if (sonsIds.length == 0) {
                
                node["leafParassite"]=true;

                if(findChildrenIdsOfHostOrParasiteNode(hostCurrentParasite,"H", graph).length==0){
                    var oldNumber=hostLeaf2ParasiteLeafNumber.get(hostCurrentParasite);
                    if (oldNumber==undefined) {
                        hostLeaf2ParasiteLeaf.set(hostCurrentParasite,[node["id"]]);
                        oldNumber=1;
                    } else {
                        var oldListParassiteLeaf=hostLeaf2ParasiteLeaf.get(hostCurrentParasite);
                        oldListParassiteLeaf.push(node["id"]);
                        hostLeaf2ParasiteLeaf.set(hostCurrentParasite,oldListParassiteLeaf);
                        oldNumber += 1;
                    }
                    hostLeaf2ParasiteLeafNumber.set(hostCurrentParasite,oldNumber);
                }
                
                var oldList=host2ListParasite.get(hostCurrentParasite);
                if (oldList==undefined) {
                        oldList=[node["id"]];
                } else {
                        oldList.push(node["id"]);
                }
                host2ListParasite.set(hostCurrentParasite, oldList);   

                var heightInSubtree = 0;
                var parasiteSubTreeHeight = 0;

                parasite2HeightInSubtree.set(node["id"], heightInSubtree);
                if (host2parasiteSubtreeHeight.get(hostCurrentParasite) == undefined)
                    host2parasiteSubtreeHeight.set(hostCurrentParasite, parasiteSubTreeHeight);

                var nodeHostOfCurrentNode = findNodeById(hostCurrentParasite, graph);
                /*if( findChildrenIdsOfHostOrParasiteNode(nodeHostOfCurrentNode, "H", graph).length == 0){
                    console.log(nodeHostOfCurrentNode["depth"]-treeHeight);
                    return nodeHostOfCurrentNode["depth"]-treeHeight;
                }*/
                return 0;
            }

            // Non leaves
            else {
                node["leafParassite"]=false;

                var heightInSubTreeCurrentNode = d3.max(sonsIds.map(function(d){
                    var child = findNodeById(d,graph);
                    var hostChild = child["belongs_to"];
                    if (hostCurrentParasite != hostChild) {
                        var heightInSubTreeChild = traverseParasiteTree(child, graph);
                        heightInSubTreeChild = -1;
                    } else {
                        var heightInSubTreeChild = traverseParasiteTree(child, graph);
                    }
                    return heightInSubTreeChild;
                })) + 1;

                parasite2HeightInSubtree.set(node["id"], heightInSubTreeCurrentNode);

                var parasiteSubtreeHeightOfHostOfCurrentNode = host2parasiteSubtreeHeight.get(hostCurrentParasite);

                if ( parasiteSubtreeHeightOfHostOfCurrentNode == undefined ||
                    parasiteSubtreeHeightOfHostOfCurrentNode < heightInSubTreeCurrentNode) {
                    host2parasiteSubtreeHeight.set(hostCurrentParasite, heightInSubTreeCurrentNode);
                }

                var nodeHostOfCurrentNode=findNodeById(hostCurrentParasite,graph);
                var depthHostOfCurrentNode=nodeHostOfCurrentNode["depth"];
                var hostHeightMaxOfDethOfCurrentNode = hostDeph2HeightMax.get(depthHostOfCurrentNode);
                
/*
                if( findChildrenIdsOfHostOrParasiteNode(nodeHostOfCurrentNode, "H", graph).length == 0){
                    var heightInSubTreeCurrentNode2=heightInSubTreeCurrentNode - (treeHeight - (nodeHostOfCurrentNode["depth"]));
                }else{
                    var heightInSubTreeCurrentNode2=heightInSubTreeCurrentNode;
                }
*/                
                if ( hostHeightMaxOfDethOfCurrentNode == undefined ||
                        hostHeightMaxOfDethOfCurrentNode < heightInSubTreeCurrentNode) {
                        hostDeph2HeightMax.set(depthHostOfCurrentNode, heightInSubTreeCurrentNode);
                    }

                
                var oldList=host2ListParasite.get(hostCurrentParasite);
                if (oldList==undefined) {
                        oldList=[node["id"]];
                } else {
                        oldList.push(node["id"]);
                }
                host2ListParasite.set(hostCurrentParasite, oldList);
                return heightInSubTreeCurrentNode
            }


        }
        
        function controlHost(node, graph, depthHostOfCurrentNode, listParassiteStartOrEndSwitch){
            var sonsIds = findChildrenIdsOfHostOrParasiteNode(node["id"], "H", graph);

            function operazioneSulNodo(node){
                var hostHeightMaxOfDethOfCurrentNode = hostDeph2HeightMax.get(depthHostOfCurrentNode);
                if (( hostHeightMaxOfDethOfCurrentNode == undefined )||( hostHeightMaxOfDethOfCurrentNode < 0 ) ){
                    hostDeph2HeightMax.set(depthHostOfCurrentNode, 0);
                }
                var parasiteSubtreeHeightOfHostOfCurrentNode = host2parasiteSubtreeHeight.get(node["id"]);

                if ( parasiteSubtreeHeightOfHostOfCurrentNode == undefined ) {
                    host2parasiteSubtreeHeight.set(node["id"],0);
                }

                var oldList=host2ListParasite.get(node["id"]);
                if (oldList==undefined) {
                    oldList=[];
                    host2ListParasite.set(node["id"],oldList);
                }
            }
            // Leaves
            if (sonsIds.length == 0) {
                node["leafHost"]=true;
                operazioneSulNodo(node);


                var numberParassiteInLeaf= hostLeaf2ParasiteLeafNumber.get(node["id"]);
                if ( numberParassiteInLeaf == undefined ) {
                    numberParassiteInLeaf=1;
                    hostLeaf2ParasiteLeafNumber.set(node["id"],numberParassiteInLeaf);
                    hostLeaf2ParasiteLeaf.set(node["id"],[]);
                }

                var oldList=host2ListParasite.get(node["id"]);
                host2ListParasiteInHostSubTree.set(node["id"],oldList);
                var listParassite = oldList.filter(function (d){return listParassiteStartOrEndSwitch.indexOf(d) > -1})
    			host2ListParasiteInHostSubTreeSwithLinkStartOrEnd.set(node["id"],listParassite);
                return oldList;

            }

            // Non leaves
            else {
                node["leafHost"] = false;

                var listsParassiteChild = sonsIds.map(function(d){
                    var child = findNodeById(d, graph);
                    var listParassiteChild=controlHost(child, graph, depthHostOfCurrentNode+1, listParassiteStartOrEndSwitch);
                    return listParassiteChild;
                })
                
                operazioneSulNodo(node);
                var oldList=host2ListParasite.get(node["id"]);
                var listParassite=oldList.concat(listsParassiteChild[0], listsParassiteChild[1]);
                host2ListParasiteInHostSubTree.set(node["id"],listParassite);
				var listParassite2 = listParassite.filter(function (d){return listParassiteStartOrEndSwitch.indexOf(d) > -1})
    			host2ListParasiteInHostSubTreeSwithLinkStartOrEnd.set(node["id"],listParassite2);
                return listParassite;

            }
        }


        var rootsConnector = graph["links"].filter(function(d) {return d["roots_connector"] == true})[0];
        var rootId = rootsConnector["to"].startsWith("P") ? rootsConnector["to"] : rootsConnector["from"];
        var rootNode = findNodeById(rootId, graph);
        listParassiteStartOrEndSwitch = []; 
        graph.linksParassiteParassite.filter(function(d){return d["switch"]==true })
        				.forEach(function(d){ 
        					listParassiteStartOrEndSwitch.push(d["from"]);
        					listParassiteStartOrEndSwitch.push(d["to"]);
        				 })
       	listParassiteStartOrEndSwitch = Array.from(new Set( listParassiteStartOrEndSwitch ));
        traverseParasiteTree(rootNode, graph);

        var rootIdHost = rootsConnector["to"].startsWith("H") ? rootsConnector["to"] : rootsConnector["from"];
        var rootNodeHost = findNodeById(rootIdHost, graph);
        controlHost(rootNodeHost, graph, 0, listParassiteStartOrEndSwitch);
        
        // hostLeaf2ParasiteLeafNumber,host2ListParasite
        return [host2parasiteSubtreeHeight, parasite2HeightInSubtree, hostDeph2HeightMax, hostLeaf2ParasiteLeaf, host2ListParasiteInHostSubTree, host2ListParasite];
    }