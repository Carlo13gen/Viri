function setParasiteNodesXCoordinates(graph, hostLeaf2ParasiteLeaf, hostLeafWidth, longLeaves, euristica) {

	//set position of leaf
	function setInitialLeafNodeXCoordinate(graph){
		graph.nodiParassiti.filter(function(d){return d["leafParassite"]==true}).forEach(function(node) {
						var hostNode = findNodeById(node["belongs_to"], graph);
						node["x"] = (hostNode["x"])+(hostLeafWidth)/2;
					});
	}


	function setLeafNodeXCoordinate(graph){
		function compareLeafParassite(a,b,widthHostNode,xHostCoordinate){
			var xb = xHostCoordinate;
			var xa = xb + widthHostNode;
			var parentA = findNodeById(findParentIdsOfHostOrParasiteNode(a, "P", graph),graph);
			var parentB = findNodeById(findParentIdsOfHostOrParasiteNode(b, "P", graph),graph);
			var x1 = parentA["x"];
			var x2 = parentB["x"];
			var y1 = parentA["y"];
			var y2 = parentB["y"];
			
			var nodoA=findNodeById(a,graph);
			if ((longLeaves)|| (nodoA["y"]>y1) ) {
				if((( x1 > xb ) && ( xb > x2 )) || (( x2 > xb ) && ( xb > x1 ))){return x1 - x2;}
				if((( x1 > xa ) && ( xa > x2 )) || (( x2 > xa ) && ( xa > x1 ))){return x1 - x2;}
				if ((y1-y2)==0) {
					return x1-x2;
				}else{
					if (x1>xa){
						return y1 - y2;
					} else {
						if (x1<xb){ 
							return y2 - y1;
						} else {
							return x1-x2;
						}
					}
				}
			} else {
				if((( x1 > xb ) && ( xb > x2 )) || (( x2 > xb ) && ( xb > x1 ))){return x1 - x2;}
				if((( x1 > xa ) && ( xa > x2 )) || (( x2 > xa ) && ( xa > x1 ))){return x1 - x2;}
				if ((y1-y2)==0) {
					return x1-x2;
				}else{
					if (x1>xa){
						return y2 - y1;
					} else {
						if (x1<xb){ 
							return y1 - y2;
						} else {
							return x1-x2;
						}
					}
				}
			}
		}

		if ((euristica == "3")||(euristica == "2")||(!((graph.nodiHost[0]["right_child_gdt"])==undefined))){
			if ((euristica == "2")||(!((graph.nodiHost[0]["right_child_gdt"])==undefined))){
				graph.nodiParassiti
					.filter(function(d){return d["leafParassite"]==true})
					.forEach(function(node) {
						node["x"] += (hostLeafWidth * ( node.MaxPosition-(node.position+1)))//(listParassite.indexOf(node["id"])));
				});
			}else{
				graph.nodiParassiti
					.filter(function(d){return d["leafParassite"]==true})
					.forEach(function(node) {
						node["x"] += (hostLeafWidth * ( node.MaxPosition-(node.position_solution+1)))//(listParassite.indexOf(node["id"])));
				});
			}
   		}else{
				graph.nodiHost
				.filter(function(d){return d["leafHost"]==true && (hostLeaf2ParasiteLeaf.get(d["id"]).length>1)})
				.forEach(function(node) {
					var listParassite = hostLeaf2ParasiteLeaf.get(node["id"])
										   .sort(function(a, b){return compareLeafParassite(a,b,node["width"],node["x"])});
					hostLeaf2ParasiteLeaf.set(node["id"],listParassite);
			});
				graph.nodiParassiti
				.filter(function(d){return d["leafParassite"]==true})
				.forEach(function(node) {
						var hostNode = findNodeById(node["belongs_to"], graph);
						var listParassite = hostLeaf2ParasiteLeaf.get(hostNode["id"]);
						node["x"] += (hostLeafWidth*(listParassite.indexOf(node["id"])));
			});
		}
	}



	function setNodeXCoordinate(node, graph) {

		var sonsIds = findChildrenIdsOfHostOrParasiteNode(node["id"], "P", graph);

		// Leaves
		if (sonsIds.length == 0) {
			return node["x"]
		}
		else {


			var childrenSwitchingHost = numberOfChildrenSwitchingHost(sonsIds, graph);
			switch (childrenSwitchingHost) {

				// No children that switch host
				case 0:
					var sumOfXCoordinatesOfChildren = 0;
					// Recursive calls to continue the exploration
					sonsIds.forEach(function(sonId) {
						sumOfXCoordinatesOfChildren += setNodeXCoordinate(findNodeById(sonId, graph), graph)
					});
					// Take the average x coordinate among the children
					node["x"] = sumOfXCoordinatesOfChildren / sonsIds.length;
					return node["x"];
					break;

				// One child switching host
				case 1:
					// Recursive calls to continue the exploration
					sonsIds.forEach(function(sonId) {
						setNodeXCoordinate(findNodeById(sonId, graph), graph)
					});
					// Assign as x coordinate, the one of the child that is not switching host
					node["x"] = findNodeById(findChildNotSwitchingHostId(sonsIds, graph), graph)["x"];
					return node["x"];
					break;

				// Two children switching host
				case 2:
					var sumOfXCoordinatesOfChildren = 0;
					// Recursive calls to continue the exploration
					sonsIds.forEach(function(sonId) {
						sumOfXCoordinatesOfChildren += setNodeXCoordinate(findNodeById(sonId, graph), graph)
					});

					var hostNode=graph.nodiHost.filter(function(d){return d["id"]==d["belongs_to"]})[0]; 

					// Take the average x coordinate among the children
					node["x"] = hostNode["x"]+(((sumOfXCoordinatesOfChildren / sonsIds.length)/width)*hostNode["width"]);
					return node["x"];
					//throw "Two children switching host";
					break;

			}


		}
	}
	setInitialLeafNodeXCoordinate(graph);
	
	var rootsConnector = graph["links"].filter(function(d) {return d["roots_connector"] == true})[0];
	var rootId = rootsConnector["to"].startsWith("P") ? rootsConnector["to"] : rootsConnector["from"];
	var rootNode = findNodeById(rootId, graph);


	setNodeXCoordinate(rootNode, graph);
	setLeafNodeXCoordinate(graph);
	setNodeXCoordinate(rootNode, graph);
}