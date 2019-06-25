function drawnParassiteEdgeSwitch(data, parasiteTreeLinksGroupEnter, hostLeafWidth, hostNodeBoxHeight, curve, parasiteNodesRadius, archiCiclo){
                // Parasite links (switch): filter
            var switchEdges = data.linksParassiteParassite
                .filter(function(d){return d["switch"] == true})
                .filter(function(d){return archiCiclo.indexOf(d) == -1 });


            // Parasite links (switch): data binding
            var parasiteLinkSwitchDataBinding = parasiteTreeLinksGroupEnter.selectAll(".parasiteLinkSwitch")
                .data(switchEdges, function(d) {return d["from"] + d["to"]});

            // Parasite links (switch): enter
            var switchLink = parasiteLinkSwitchDataBinding.enter()
                .append("svg:path")
                .classed("parasiteLinkSwitch", true)
                .attr("id","pathParassite")
                .attr("fill-opacity", "0")
                .style("stroke","black")
                .style("stroke-width","1")
                .attr("d", function(d) {

                    var from = findNodeById(d["from"], data);
                    var to = findNodeById(d["to"], data);
                    if( curve){
                        return computeSVGPathSwitchEdgesParasiteTree(from, to, hostNodeBoxHeight, parasiteNodesRadius)//, hostLeafWidth, hostNodeBoxHeight);
                    }else{
                        return "M " + from["x"] + " " + from["y"] + " " + "L " + to["x"] + " " + to["y"]
                    }
               
                })
                //.style("stroke","red")
                .style("stroke-dasharray", ("8, 2"));

}

function drawnParassiteEdgeSwitchCicle(data, parasiteTreeLinksGroupEnter, hostLeafWidth, hostNodeBoxHeight, curve, parasiteNodesRadius, archiCiclo){
                // Parasite links (switch): filter
            var switchEdges = data.linksParassiteParassite
                .filter(function(d){return d["switch"] == true})
                .filter(function(d){return archiCiclo.indexOf(d) != -1 });


            // Parasite links (switch): data binding
            var parasiteLinkSwitchDataBinding = parasiteTreeLinksGroupEnter.selectAll(".parasiteLinkSwitch")
                .data(switchEdges, function(d) {return d["from"] + d["to"]});

            // Parasite links (switch): enter
            var switchLink = parasiteLinkSwitchDataBinding.enter()
                .append("svg:path")
                .classed("parasiteLinkSwitch", true)
                .attr("id","pathParassiteCicle")
                .attr("fill-opacity", "0")
                .style("stroke","#ff9900")
                .style("stroke-width","3")
                .attr("d", function(d) {

                    var from = findNodeById(d["from"], data);
                    var to = findNodeById(d["to"], data);
                    if( curve){
                        return computeSVGPathSwitchEdgesParasiteTree(from, to, hostNodeBoxHeight, parasiteNodesRadius)//, hostLeafWidth, hostNodeBoxHeight);
                    }else{
                        return "M " + from["x"] + " " + from["y"] + " " + "L " + to["x"] + " " + to["y"]
                    }
               
                })
                //.style("stroke","red")
                .style("stroke-dasharray", ("8, 2"));

}