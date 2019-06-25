function drawnParassiteNodes(svg, data, parasiteNodesRadius){
                // Append the parasite tree group
            var parasiteTreeNodesGroupEnter = svg.append("g").attr("id", "parasiteTreeNodes");

            // Parasite nodes: filtering
            var parasiteNodes = data.nodiParassiti;

            // Parasite nodes: Data binding
            var parasiteNodesDataBinding = parasiteTreeNodesGroupEnter.selectAll(".parasiteNode")
                .data(parasiteNodes, function(d) {return d["id"]});

            // Parasite nodes: Enter
            var circle = parasiteNodesDataBinding.enter()
                .append("svg:circle")
                .attr("id","circleParassite")
                .classed("parasiteNode", true)
                .attr("name", function(d) {return d["id"]})
                .attr("cx", function(d) {return d["x"]})
                .attr("cy", function(d) { return d["y"]})
                .attr("r", parasiteNodesRadius)
                .attr("fill", "white")
                .append("svg:title")
                .style("stroke","black")
                .style("stroke-width","1")
                .text(function(d){return d["id"]});

            // At this point some nodes have been drawn in the same positions
}