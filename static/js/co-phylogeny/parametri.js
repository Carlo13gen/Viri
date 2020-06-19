function metodoSoluzioniRandomDefined(soluzioniRandomiche){
    if (soluzioniRandomiche==undefined) {
        var soluzioniRandomiche = (document.getElementById("soluzioniRandomiche").checked).toString()=="true";
    }
    if (numberMaxSolutionDefined().toString()=="NaN"){
        var soluzioniRandomiche = false;    
    }

    return soluzioniRandomiche;
}

function maximumHostSwitchDistanceDefined(maximumHostSwitchDistance){
    if( maximumHostSwitchDistance ==undefined){
        //if not set is NaN
        var maximumHostSwitchDistance = parseFloat(document.getElementById("maximumHostSwitchDistance").value);
    }
    return maximumHostSwitchDistance;
}



function euristicaDefined(euristica){
    if (euristica==undefined) {
        if ((document.getElementById("euristica1").checked).toString()=="true"){
            var euristica = "1";
        }else{
            if ((document.getElementById("euristica3").checked).toString()=="true"){
                var euristica = "3";
            }else{
		 if ((document.getElementById("euristica4").checked).toString()=="true"){
                var euristica = "4";
	  	 }else{
			if ((document.getElementById("euristica5").checked).toString()=="true"){
                	var euristica = "5";
		}else{
            	    var euristica = "2";
            	}
}
        }
    }
}
    return euristica;
}




function metodoRimuviCicliDefined(metodoRimuviCicli){
    if (metodoRimuviCicli==undefined) {
        if ((document.getElementById("test2").checked).toString()=="true"){
            var metodoRimuviCicli = "2";
        }
        else{
            var metodoRimuviCicli = "1";
        }
    }
    return metodoRimuviCicli;
}

function curveDefined(curve){
    if(curve==undefined){
        var curve = (document.getElementById("curve").checked).toString()=="true";
    }
    return curve;
}

function curveSwitchDefined(curveSwitch){
    if(curveSwitch==undefined){
        var curveSwitch = (document.getElementById("curveSwitch").checked).toString()=="true";
    }
    return curveSwitch;
}

function bloccatoDefined(){
    var bloccato = (document.getElementById("bloccato").checked).toString()=="true";
    return bloccato;
}

function bloccatoTogliere(){
        var bloccato = (document.getElementById("bloccato").checked)=false;
}

function heightDefined(height){
    if(height==undefined){
        var heightString = document.getElementById("height").value;
        if (heightString==""){
            var height = window.innerHeight-150;
        }else{
            var height = parseFloat(heightString);
        }
    }
    return height;
}

function widthDefined(width) {
    if(width==undefined){
        var widthString = document.getElementById("width").value;
        if (widthString==""){
            var width = window.innerWidth-20;
        }else{
            var width = parseFloat(widthString);
        }
    }
    return width;
}

function longLeavesDefined(longLeaves) {
    if(longLeaves==undefined){
        var longLeaves = document.getElementById("longLeaves").checked;
    }
    return longLeaves;
}

function equalsLivelEqualsHeightDefined(equalsLivelEqualsHeight) {
    if(equalsLivelEqualsHeight==undefined){
        var equalsLivelEqualsHeight = document.getElementById("equalsLivelEqualsHeight").checked;
    }
    return equalsLivelEqualsHeight;
}

function separatorBetweenHostNodesWidthDefined(separatorBetweenHostNodesWidth) {
    if(separatorBetweenHostNodesWidth==undefined){
        var separatorBetweenHostNodesWidth = document.getElementById("select5").value;
        if (separatorBetweenHostNodesWidth==""){
            var separatorBetweenHostNodesWidth = "0";
        }
    }
    return parseFloat(separatorBetweenHostNodesWidth);
}

function separatorBetweenHostNodesHightDefined(separatorBetweenHostNodesHight) {
    if(separatorBetweenHostNodesHight==undefined){
        var separatorBetweenHostNodesHight = document.getElementById("select6").value;
        if (separatorBetweenHostNodesHight==""){
            var separatorBetweenHostNodesHight = "0";
        }
    }

    return parseFloat(separatorBetweenHostNodesHight);
}

function hostNodeBoxHeightDefined(hostNodeBoxHeight) {
    if(hostNodeBoxHeight == undefined ){
        var hostNodeBoxHeight = 100;
    }
    return hostNodeBoxHeight;
}

function hostLeafWidthDefined(hostLeafWidth, width) {
    if(hostLeafWidth==undefined){
        var hostLeafWidth =100;
    }
    return hostLeafWidth;
}

function parasiteNodesRadiusDefined(parasiteNodesRadius) {
    if (parasiteNodesRadius == undefined) {
        var parasiteNodesRadius = 22;
    }
    return parasiteNodesRadius;
}

function nameDefined(name) {
    if(name==undefined){
        var name = document.getElementById("file_name").getAttribute("value");
        //var name = document.getElementById("select").value;

    }
    return name;
}

function numberDefined(number) {
    if(number==undefined){
        var number = 0;
    }
    return number;
}

function numberReset() {
        var p=document.getElementById("numberSolutionGraph");
        p.value="1";
}

function rootToRootDefined(rootToRoot) {
    if (rootToRoot == undefined) {
        var rootToRoot = (document.getElementById("rootToRoot").checked).toString();
    }
    return rootToRoot;
}

function cospeciationCostDefined(cospeciationCost) {
    if( cospeciationCost ==undefined){
        var cospeciationCost = (document.getElementById("cc").value);
        if(cospeciationCost == ""){
            cospeciationCost="-1";
        }
    }
    return cospeciationCost;
}

function duplicationCostDefined(duplicationCost) {
    if( duplicationCost ==undefined){
        var duplicationCost = (document.getElementById("cd").value);
        if(duplicationCost == ""){
            duplicationCost ="1";
        }

    }
    return duplicationCost;
}

function hostswitchCostDefined(hostswitchCost) {
    if( hostswitchCost ==undefined){
        var hostswitchCost = (document.getElementById("ch").value);
        if( hostswitchCost == ""){
            hostswitchCost ="1";
        }
    }
    return hostswitchCost;
}

function sortingCostDefined(sortingCost) {
    if( sortingCost ==undefined){
        var sortingCost = (document.getElementById("cs").value);
        if( sortingCost == ""){
            sortingCost = "1";
        }
    }
    return sortingCost;
}

function numberMaxSolutionDefined(numberMaxSolution) {
    if(numberMaxSolution==undefined){
        var numberMaxSolution = parseFloat(document.getElementById("numberMaxSolution").value);
    }
    return numberMaxSolution;
}

function numberSolutionGraphDefined(numberSolutionGraph){
    if(numberSolutionGraph == undefined){
        
        var numberSolutionGraph = parseInt(document.getElementById("numberSolutionGraph").value);
    }
    return numberSolutionGraph;
}

function rimuoviCicliDefined(rimuoviCicli){
    if (rimuoviCicli == undefined) {
        var rimuoviCicli = (document.getElementById("rimuoviCicli").checked).toString();
    }
    return rimuoviCicli;
}


