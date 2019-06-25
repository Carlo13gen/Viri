    function drawnHost(svg, height, width, data, hostNodeBoxHeight, separatorBetweenHostNodesWidth, treeHeight, parasiteNodesRadius){

    // Update the outer dimensions
            svg.attr("width", width).attr("height", height);

            // Append host tree group
            var hostTreeGroupEnter = svg.append("g").attr("id", "hostTree");

            // Host nodes: filtering
            var hostNodes = data.nodiHost;

            // Host nodes: Data binding
            var hostNodesDataBinding = hostTreeGroupEnter.selectAll(".hostNode")
                .data(hostNodes, function(d) {return d["id"]});
/*
            // Host nodes: Enter
            var rect = hostNodesDataBinding.enter()
                .append("svg:rect")
                .attr("id","rectHost")
                .classed("hostNode", true)
                .attr("name", function(d) {return d["id"]})
                .attr("x", function(d) { return d["x"]})
                .attr("y", function(d) { return d["y"]})
                .attr("width", function(d) { return d["width"] })
                .attr("height", function(d) { return d["height"] })
                .style("fill", '#6666ff')
                .append("svg:title")
                .style("stroke","white")
                .style("stroke-width","2")
                .text(function(d) {return d["id"]});
*/

            // Host nodes: Enter
            var rect = hostNodesDataBinding.enter()
                .append("svg:path")
                .attr("id","rectHost")
                .classed("hostNode", true)
                .style("stroke","white")
                .style("stroke-width","2")
                .attr("fill-opacity", "100")
                //.style("fill", '#6666ff')
                .style("fill", '#bde7ff')
                .attr("d", function(d) {
                        return computeSVGPathHost(d["id"], data, hostNodeBoxHeight, separatorBetweenHostNodesWidth, treeHeight, parasiteNodesRadius);
                });
    }