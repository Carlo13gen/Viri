function setParasiteNodesYCoordinates(graph, parasite2HeightInSubtree, host2parasiteSubtreeHeight, hostNodeBoxHeight, parassitaLivelloHost, down) {
        //console.log(parassitaLivelloHost);
        
        var nodesParasite = graph.nodiParassiti;
        
        nodesParasite.filter(function(d){return d["leafParassite"] == true })
                        .forEach(function(d){
                            var hostNode = findNodeById(d["belongs_to"], graph);
                            d["y"]=(hostNode["y"]+hostNode["height"])- hostNodeBoxHeight/2;
                        });

        nodesParasite.filter(function(d){return d["leafParassite"] == false })
                        .forEach(function(node){
                            var hostNode = findNodeById(node["belongs_to"], graph);
                           // if(down){
                                node["y"] = hostNode["y"]+(hostNodeBoxHeight*(0.5+(parassitaLivelloHost.get(node["id"]))));
                           /* }else{
                                var hostNodeBoxHeight = hostNode["height"] / (host2parasiteSubtreeHeight.get(hostNode["id"]) +1);
                                //node["y"] = hostNode["y"] + (hostNodeBoxHeight*(0.5+(host2parasiteSubtreeHeight.get(hostNode["id"])-parasite2HeightInSubtree.get(node["id"]))));
                                node["y"] = hostNode["y"]+hostNode["height"] - (hostNodeBoxHeight*(0.5+(parasite2HeightInSubtree.get(node["id"]))));
                                //node["y"] = hostNode["y"]+(hostNodeBoxHeight*(0.5+(parassitaLivelloHost.get(node["id"]))));
                            }
                            */
                        });

}