function computeSVGPathHost(id, data, hostNodeBoxHeight, separatorBetweenHostNodesWidth, treeHeight, parasiteNodesRadius){
    var nodo = findNodeById(id, data);
    var x = nodo["x"];
    var y = nodo["y"];
    var width = nodo["width"];
    var height = nodo["height"]; 

    var spigolo1 = x + " " + y +" ";
//    return "M "+ spigolo1 + "h " + width +" v "+ height + " h " + (0-width) +" z"; 


    var isLeafHost = nodo["leafHost"];
    var isRight = nodo["right_child"];
    var isLeft = nodo["left_child"];

    if ((!(isRight))&(!(isLeft))) {

        var spigolo1 = (x + parasiteNodesRadius ) + " " + y + " ";
        var spigolo2 = (x+width - parasiteNodesRadius ) + " " + y + " ";
        var spigolo3 = (x+width) + " " + (y+height) + " ";
        var spigolo4 = x + " " + (y+height) +" ";
        var punto1 = (x+(width/2)) + " " + y + " "; 

        return "M "+ punto1 + "L "+ spigolo2 +"L " + spigolo3 + "L "+ spigolo4 + "L " +spigolo1 + "z";
    }

    var nodoParent=findNodeById((findParentIdsOfHostOrParasiteNode(id, "H", data)[0]),data);
    var separetor = (separatorBetweenHostNodesWidth*(treeHeight - nodoParent["depth"]))/2;
    var separatoreZero=false;
    if (separetor == 0){
        separatoreZero=true;
        separetor = parasiteNodesRadius;
    }

    if (isRight) {
        var spigolo1 = x + " " + y +" ";
        var spigolo2 = (x+width) + " " + y + " ";
        var spigolo3 = (x+width) + " " + (y+height) + " ";
        var spigolo4 = x + " " + (y+height) +" ";
        var punto1 = (x+(width/2)) + " " + y + " "; 
        var puntoDiFlesso = (x) + " " + (y + (hostNodeBoxHeight/2)) + " ";
        var punto21= (x + width - parasiteNodesRadius) + " " + y+" ";
        var spigolo1 = (x-separetor) + " " + y +" ";
        if(separatoreZero){
            var spigolo1 = (x) + " " + y +" ";
            var spigolo4 = (x + separetor) + " " + (y+height) +" ";
        }
        return "M "+ punto21 + "L " + spigolo3 + "L "+ spigolo4 + "C "+puntoDiFlesso+puntoDiFlesso+spigolo1+ "z";
    }
    if (isLeft){
        var punto1 = (x+(width/2)) + " " + y + " "; 
        var puntoDiFlesso = (x+width) + " " + (y + (hostNodeBoxHeight/2)) + " ";
        if(separatoreZero){
            var spigolo1 = (x + parasiteNodesRadius) + " " + y + " ";
            var spigolo2 = (x+width) + " " + y + " ";
            var spigolo3= (x+width-separetor) + " " + (y+height) + " "
            var spigolo4 = x + " " + (y+height) +" ";
        }else{
            var spigolo1 = (x + parasiteNodesRadius) + " " + y + " ";
            var spigolo2 = (x+width+separetor) + " " + y + " ";
            var spigolo3 = (x+width) + " " + (y+height) + " ";
            var spigolo4 = x + " " + (y+height) +" ";
        }

        return "M "+ punto1 + "L "+ spigolo2 +"C " +puntoDiFlesso+puntoDiFlesso+ spigolo3 + "L "+ spigolo4 + "L " +spigolo1+ "z";
    }




/*
    var isLeafHost = nodo["leafHost"];
    var isRight = nodo["right_child"];
    var isLeaf = nodo["left_child"];


    var spigolo2 = (x+width) + " " + y + " ";
    var spigolo3 = (x+width) + " " + (y+height) + " ";
    var spigolo4 = x + " " + (y+height) +" ";

    var punto1 = (x+(width/2)) + " " + y + " ";    
    var punto2 = (x+width) + " " + (y+(height/2)) + " ";
    var punto3 = (x+(width/2)) + " " + (y+height) + " ";
    var punto4 = x + " " + (y+(height/2)) + " ";

    if ((!(isRight))&(!(isLeaf))) {
        return "M "+ punto1 + "L "+ spigolo2 +"L " + spigolo3 + "L "+ spigolo4 + "L " +spigolo1 + "z";
    }

    var nodoParent=findNodeById((findParentIdsOfHostOrParasiteNode(id, "H", data)[0]),data);
    var separetor = (separatorBetweenHostNodesWidth*(treeHeight - nodoParent["depth"]))/2;


//    return "M "+ punto1 + "C "+ spigolo2 + spigolo2 + punto2 +"C " + spigolo3 + spigolo3 + punto3 + "C "+ spigolo4 + spigolo4 + punto4 + "C " +spigolo1+spigolo1+punto1+ "z";
    if (isRight) {
        var spigolo1 = (x-separetor) + " " + y +" ";
        return "M "+ punto1 + "L "+ spigolo2 + "L " + spigolo3 + "L "+ spigolo4 + "L "+spigolo1+ "z";
    }
    if (isLeaf){
        var spigolo2 = (x+width+separetor) + " " + y + " ";
        return "M "+ punto1 + "L "+ spigolo2 +"L " + spigolo3 + "L "+ spigolo4 + "L " +spigolo1+ "z";
    }
*/


/**chele
    var isLeafHost = nodo["leafHost"];
    var isRight = nodo["right_child"];
    var isLeaf = nodo["left_child"];


    var spigolo2 = (x+width) + " " + y + " ";
    var spigolo3 = (x+width) + " " + (y+height) + " ";
    var spigolo4 = x + " " + (y+height) +" ";

    var punto1 = (x+(width/2)) + " " + y + " ";    
    var punto2 = (x+width) + " " + (y+(height/2)) + " ";
    var punto3 = (x+(width/2)) + " " + (y+height) + " ";
    var punto4 = x + " " + (y+(height/2)) + " ";

    if ((!(isRight))&(!(isLeaf))) {
        return "M "+ punto1 + "C "+ spigolo2 + spigolo2 + punto2 +"C " + spigolo3 + spigolo3 + punto3 + "C "+ spigolo4 + spigolo4 + punto4 + "C " +spigolo1+spigolo1+punto1+ "z";
    }

    var nodoParent=findNodeById((findParentIdsOfHostOrParasiteNode(id, "H", data)[0]),data);
    var separetor = (separatorBetweenHostNodesWidth*(treeHeight - nodoParent["depth"]))/2;


//    return "M "+ punto1 + "C "+ spigolo2 + spigolo2 + punto2 +"C " + spigolo3 + spigolo3 + punto3 + "C "+ spigolo4 + spigolo4 + punto4 + "C " +spigolo1+spigolo1+punto1+ "z";
    if (isRight) {
        var spigolo1 = (x-separetor) + " " + y +" ";
        return "M "+ punto1 + "C "+ spigolo2 + spigolo2 + punto2 +"C " + spigolo3 + spigolo3 + punto3 + "L "+ spigolo4 + "L "+spigolo1+ "z";
    }
    if (isLeaf){
        var spigolo2 = (x+width+separetor) + " " + y + " ";
        return "M "+ punto1 + "L "+ spigolo2 +"L " + spigolo3 + "C "+ spigolo4 + spigolo4 + punto4 + "C " +spigolo1+spigolo1+punto1+ "z";
    }
*/

/*
    var isLeafHost = nodo["leafHost"];
    var isRight = nodo["right_child"];
    var isLeaf = nodo["left_child"];

    var spigolo2 = (x+width) + " " + y + " ";
    var spigolo3 = (x+width) + " " + (y+height) + " ";
    var spigolo4 = x + " " + (y+height) +" ";

    var punto1 = (x+(width/2)) + " " + y + " ";    
    var punto2 = (x+width) + " " + (y+(height/4)) + " ";
    var punto3 = (x+(width/2)) + " " + (y+height) + " ";
    var punto4 = x + " " + (y+(height/4)) + " ";

    var punto32 = (x+(width/2)-(hostNodeBoxHeight/2)) + " " + (y+height) + " ";
    var punto38 = (x+(width/2)+(hostNodeBoxHeight/2)) + " " + (y+height) + " ";
    var punto3H = (x+(width/2)) + " " + (y+height-(hostNodeBoxHeight/3)) + " ";
    var spigoloInterno2 = (x+(width/2)-(hostNodeBoxHeight/2))+ " " + (y+height-(hostNodeBoxHeight/3)) + " ";
    var spigoloInterno8 = (x+(width/2)+(hostNodeBoxHeight/2))+ " " + (y+height-(hostNodeBoxHeight/3)) + " ";
    if (isLeafHost) {
        if (isRight) {
            return "M "+ spigolo1 + "h " + width +" v "+ height + " h " + (0-width) +" z";
        } 
        else {
            return "M "+ punto1 + "C "+ spigolo2 + spigolo2 + punto2 +"C " + spigolo3 + spigolo3 + punto3 + "C "+ spigolo4 + spigolo4 + punto4 + "C " +spigolo1+spigolo1+punto1+ "z";
        }
    } else {

        return "M "+ punto1 + "C "+ spigolo2 + spigolo2 + punto2 +"C " + spigolo3 + spigolo3 + punto38 + "C " + spigoloInterno8 + spigoloInterno8 + punto3H + "C " + spigoloInterno2 + spigoloInterno2 + punto32 + "C "+ spigolo4 + spigolo4 + punto4 + "C " +spigolo1+spigolo1+punto1+ "z";
    }
*/   
}

/*




*/