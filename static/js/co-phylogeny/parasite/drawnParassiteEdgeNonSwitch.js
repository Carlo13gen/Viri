function drawnParassiteEdgeNonSwitch(data, parasiteTreeLinksGroupEnter, hostLeafWidth, hostNodeBoxHeight, curve, parasiteNodesRadius){
// Parasite links (non switch): filtering
            var nonSwitchEdges = data.linksParassiteParassite
                .filter(function(d){return d["switch"] == false});

            // Parasite links (non switch): data binding
            var parasiteLinksNonSwitchDataBinding = parasiteTreeLinksGroupEnter.selectAll(".parasiteLinkNonSwitch")
                .data(nonSwitchEdges, function(d){ return d["from"] + d["to"]});

            // Parasite links (non switch): Enter
            var nonSwitchLink = parasiteLinksNonSwitchDataBinding.enter()
                .append("svg:path")
                .classed("parasiteLinkNonSwitch", true)
                .attr("id","pathParassite")
                .attr("name", "edge")
                .attr("fill-opacity", "0")
                .style("stroke","black")
                .style("stroke-width","1")
                .attr("d", function(d) {
                

                    var from = findNodeById(d["from"], data);
                    var to = findNodeById(d["to"], data);
                
                if(curve){
                    return computeSVGPathNonSwitchEdgesParasiteTree(from, to, hostNodeBoxHeight, parasiteNodesRadius)//, hostLeafWidth, hostNodeBoxHeight)
                }else{
                    return "M " + from["x"] + " " + from["y"] + " " + "L " + to["x"] + " " + to["y"]
                }
                });
}