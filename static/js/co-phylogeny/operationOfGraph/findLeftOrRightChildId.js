// The third parameter should be a string: "left_child" or "right_child"
function findLeftOrRightChildId(node, graph, left_or_right_child) {

    var sonsIds = findChildrenIdsOfHostOrParasiteNode(node["id"], "H", graph);

    return graph.nodiHost
        .filter(function(d) { return d[left_or_right_child] == true
        							&& sonsIds.indexOf(d["id"]) != -1 })
        .map(function(d) { return d["id"] })[0]
}