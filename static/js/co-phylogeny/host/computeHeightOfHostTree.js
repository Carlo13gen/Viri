    function computeHeightOfHostTree(graph) {

        return 1 + d3.max(graph.nodiHost.map(function(d) { return d["depth"] }));

    }
