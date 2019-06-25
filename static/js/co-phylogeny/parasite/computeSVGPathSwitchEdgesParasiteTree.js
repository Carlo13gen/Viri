    function computeSVGPathSwitchEdgesParasiteTree(from, to, hostNodeBoxHeight, parasiteNodesRadius){//, hostLeafWidth, hostNodeBoxHeight) {

/*
        function sgn(x) {
            return (x > 0) - (x < 0);
        }

        var deltaY = sgn(from["y"] - to["y"]) * (hostNodeBoxHeight/2);
        
        if(deltaY<0){
            var deltaX = sgn(from["x"] - to["x"]) * (hostNodeBoxHeight/2);

            if ((from["x"] - to["x"])>deltaY) {
                var puntoDiAppoggio = (to["x"]) + " " + (from["y"] - deltaY) + " ";
            } else {
                var puntoDiAppoggio = (to["x"]) + " " + (to["y"]) + " ";
            }

            var startingPoint = from["x"] + " " + from["y"] + " ";
            var firstControlPoint = to["x"] + " " + from["y"] + " ";
            var secondControlPoint = to["x"] + " " + (from["y"] - (deltaY/2)) + " ";
            var finalPoint = to["x"] + " " + to["y"];


        return "M " + startingPoint + "C " + firstControlPoint + secondControlPoint + puntoDiAppoggio +"L "+finalPoint;
        } else { 
            var deltaX = sgn(from["x"] - to["x"]) * (hostNodeBoxHeight/2);
            var deltaX2 = sgn(from["x"] - to["x"]) * (parasiteNodesRadius/2);
            
            if ((from["x"] - to["x"])<deltaY) {
                var puntoDiAppoggio = (to["x"]+deltaX2) + " " + (from["y"] - deltaY) + " ";
            } else {
                var puntoDiAppoggio = (to["x"]) + " " + (to["y"]) + " ";
            }

            //var puntoDiAppoggio = (to["x"]+deltaX2) + " " + (from["y"] - deltaY) + " ";

            var startingPoint = from["x"] + " " + from["y"] + " ";
            var firstControlPoint = (to["x"]+deltaX2) + " " + from["y"] + " ";
            var secondControlPoint = (to["x"]+deltaX2) + " " + (from["y"] - (deltaY/2)) + " ";
            var finalPoint = (to["x"]+deltaX2) + " " + to["y"];

        return "M " + startingPoint + "C " + firstControlPoint + secondControlPoint + puntoDiAppoggio +"L "+finalPoint;
        }
*/

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

        if(((from["y"] - to["y"])*sgn(from["y"] - to["y"]))<5){
            deltaY=0;
        }
        if(((from["x"] - to["x"])*sgn(from["x"] - to["x"]))<5){
            deltaX=0;
        }

        var startingPoint = from["x"] + " " + from["y"] + " ";
        //var controPoint = to["x"] + " " + from["y"] + " ";
        var firstControlPoint = (to["x"] + deltaX) + " " + (from["y"] - deltaY*2) + " ";
        //var secondControlPoint = (to["x"]) + " " + (from["y"] + deltaY) + " ";
        var secondControlPoint = (to["x"] + deltaX*2) + " " + (from["y"] - deltaY) + " ";
        var finalPoint = to["x"] + " " + to["y"];
        return "M " + startingPoint + "C " + firstControlPoint + secondControlPoint + finalPoint;
        //        return "M " + startingPoint + "C " + firstControlPoint + secondControlPoint + secondControlPoint + "S " + firstControlPoint+ finalPoint;
        //        return "M " + startingPoint + "C " + firstControlPoint + secondControlPoint + firstControlPoint + "S " + firstControlPoint+ finalPoint;
*/

        function sgn(x) {
            return (x > 0) - (x < 0);
        }

        var deltaY = sgn(from["y"] - to["y"]) * (hostNodeBoxHeight/2);
        var deltaX = sgn(from["x"] - to["x"]) * (hostNodeBoxHeight/2);
        var deltaX2 = sgn(from["x"] - to["x"]) *(parasiteNodesRadius/2);
        var deltaY2 = sgn(from["y"] - to["y"]) *(parasiteNodesRadius/2);

        var puntoDiAppoggio = (to["x"]+deltaX2) + " " + (from["y"] - deltaY) + " ";

        var spostamentoOriziontale = 0;

        if( ((from["y"] - to["y"])*sgn(from["y"] - to["y"]))> (hostNodeBoxHeight/2)){
            var secondControlPoint = (to["x"]+deltaX2) + " " + (from["y"] - (deltaY/2)) + " ";
        } else {
            var secondControlPoint = (to["x"]+deltaX2) + " " + (to["y"]) + " ";
        }


        if(deltaX2==0){
            var startingPoint = (from["x"]) + " " + (from["y"]-deltaY2) + " ";
        }else{
            var startingPoint = (from["x"]-deltaX2) + " " + from["y"] + " ";
        }
        
        var firstControlPoint = (to["x"]+deltaX2) + " " + from["y"] + " ";

        var finalPoint = (to["x"]) + " " + (to["y"] + deltaY2);


        return "M " + startingPoint + "h "+ spostamentoOriziontale+ "C " + firstControlPoint + secondControlPoint + puntoDiAppoggio +"L "+finalPoint;



    }