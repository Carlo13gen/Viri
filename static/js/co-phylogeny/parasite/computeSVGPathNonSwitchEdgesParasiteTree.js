    function computeSVGPathNonSwitchEdgesParasiteTree(from, to, hostNodeBoxHeight, parasiteNodesRadius){//, hostLeafWidth, hostNodeBoxHeight) {


        function sgn(x) {
            return (x > 0) - (x < 0);
        }

        var deltaY = sgn(from["y"] - to["y"]) * (hostNodeBoxHeight/2);
        var deltaX = sgn(from["x"] - to["x"]) * (hostNodeBoxHeight/2);
        var deltaX2 = sgn(from["x"] - to["x"]) *(parasiteNodesRadius);
        var deltaY2 = sgn(from["y"] - to["y"]) *(parasiteNodesRadius);


        var puntoDiAppoggio = to["x"] + " " + (from["y"] - deltaY) + " ";

        var spostamentoOriziontale = 0;
        
/*
        if( ((from["x"] - to["x"])*sgn(from["x"] - to["x"]))> (hostNodeBoxHeight/2)){
            var spostamentoOriziontale = (to["x"]-from["x"])+deltaX;
        } else {
            var spostamentoOriziontale = 0;
        }
*/

        if( ((from["y"] - to["y"])*sgn(from["y"] - to["y"]))> (hostNodeBoxHeight/2)){
                var secondControlPoint = to["x"] + " " + (from["y"] - (deltaY/2)) + " ";
                } else {
                var secondControlPoint = to["x"] + " " + (to["y"]) + " ";
            }


        if(deltaX2==0){
            var startingPoint = (from["x"]) + " " + (from["y"]-deltaY2) + " ";
        }else{
            var startingPoint = (from["x"]-deltaX2) + " " + from["y"] + " ";
        }

        var firstControlPoint = to["x"] + " " + from["y"] + " ";

        var finalPoint = to["x"] + " " + (to["y"]+deltaY2);


        return "M " + startingPoint + "h "+ spostamentoOriziontale+ "C " + firstControlPoint + secondControlPoint + puntoDiAppoggio +"L "+finalPoint;


/*


        var CONSTANT_CURVE_X = 5
        var CONSTANT_CURVE_Y = 5
//        var CONSTANT_CURVE_X =  hostLeafWidth/5;
//        var CONSTANT_CURVE_Y = hostNodeBoxHeight/5;


        function sgn(x) {
            return (x > 0) - (x < 0);
        }

        var deltaX = sgn(from["x"] - to["x"]) * CONSTANT_CURVE_X;
        var deltaY = sgn(from["y"] - to["y"]) * CONSTANT_CURVE_Y;


        var startingPoint = from["x"] + " " + from["y"] + " ";
        //var controPoint = to["x"] + " " + from["y"] + " ";
        var firstControlPoint = (to["x"] + deltaX) + " " + from["y"] + " ";
        var secondControlPoint = (to["x"]) + " " + (from["y"] + deltaY) + " ";
        var finalPoint = to["x"] + " " + to["y"];
        return "M " + startingPoint + "C " + firstControlPoint + secondControlPoint + finalPoint;
        
*/

    }
   