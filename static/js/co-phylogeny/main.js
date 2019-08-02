function grafic(disegna, curve, curveSwitch, name, numberMaxSolution, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost, sortingCost, longLeaves, equalsLivelEqualsHeight, separatorBetweenHostNodesWidth,separatorBetweenHostNodesHight, height, width, rimuoviCicli, metodoRimuviCicli, soluzioniRandomiche, euristica ){
    var maximumHostSwitchDistance = maximumHostSwitchDistanceDefined(maximumHostSwitchDistance); 
    var soluzioniRandomiche= metodoSoluzioniRandomDefined(soluzioniRandomiche);
    var metodoRimuviCicli = metodoRimuviCicliDefined(metodoRimuviCicli);
    var rimuoviCicli=rimuoviCicliDefined(rimuoviCicli);
    var name = nameDefined(name);
    var number = numberDefined(number);
    var rootToRoot = rootToRootDefined(rootToRoot);
    var cospeciationCost = cospeciationCostDefined(cospeciationCost);
    var duplicationCost = duplicationCostDefined(duplicationCost);
    var hostswitchCost = hostswitchCostDefined(hostswitchCost);
    var sortingCost = sortingCostDefined(sortingCost);
    var numberMaxSolution = numberMaxSolutionDefined(numberMaxSolution);
    var curve = curveDefined(curve);
    var curveSwitch = curveSwitchDefined(curveSwitch);
    var euristica = euristicaDefined(euristica);

    if ( disegna==undefined ){
        var disegna=true;
    }

    d3.select("svg").remove();
    d3.select("#tooltip").remove();
    //console.log(("graphs_gdt/"+name+"/"+cospeciationCost+"/"+duplicationCost+"/"+hostswitchCost+"/"+sortingCost+"/"+numberMaxSolution.toString()+"/"+rootToRoot+"/"+rimuoviCicli+"/"+metodoRimuviCicli+"/"+soluzioniRandomiche+"/"+ maximumHostSwitchDistance));
	d3.json("http://localhost:9000/graphs_gdt/"+name+"/"+cospeciationCost+"/"+duplicationCost+"/"+hostswitchCost+"/"+sortingCost+"/"+numberMaxSolution.toString()+"/"+rootToRoot+"/"+rimuoviCicli+"/"+metodoRimuviCicli+"/"+soluzioniRandomiche+"/"+ maximumHostSwitchDistance , function(error, dataOriginal) {

        if (error) throw error;
        // Assumption: we only want to draw the first graph_analysis for now.
        // TODO: Is this decision final?
        if (dataOriginal[0]["problem"] != undefined) {
            st="Combination not found";
            var p = document.getElementById("problem");
            p.innerHTML = st;
            return 0;
        } else {
            st="";
            var p = document.getElementById("problem");
            p.innerHTML = st;
        }

        setTimeout(function(){
        numberReset();
        },0)

        data = dataOriginal[0]["graph"][0][0];
        graphs_solution = dataOriginal[0]["graph"];
        elaboraNodi(data);
        // End assumption
        numberSolution = dataOriginal[0]["number"];
        
        setTimeout(function(){
        loadNumberSolution(numberSolution);        
        },0)
        
        // Data contains an array of graphs, each contains both the host and the guest trees
        var vis = d3.custom.cophy()
            .width(width)
            .height(height)
            .longLeaves(longLeaves)
            .equalsLivelEqualsHeight(equalsLivelEqualsHeight)
            .separatorBetweenHostNodesWidth(separatorBetweenHostNodesWidth)
            .curve(curve)
            .curveSwitch(curveSwitch)
            .separatorBetweenHostNodesHight(separatorBetweenHostNodesHight)
            .disegna(disegna)
            .euristica(euristica);

        // bind the dom element to the chart.
        d3.select("body")
            .datum(data)
            .call(vis);

    });


}

function grafic_output(disegna, curve, curveSwitch, name, numberMaxSolution, rootToRoot, cospeciationCost, duplicationCost, hostswitchCost, sortingCost, longLeaves, equalsLivelEqualsHeight, separatorBetweenHostNodesWidth,separatorBetweenHostNodesHight, height, width, rimuoviCicli, metodoRimuviCicli, soluzioniRandomiche, euristica, username ){
    var maximumHostSwitchDistance = maximumHostSwitchDistanceDefined(maximumHostSwitchDistance);
    var soluzioniRandomiche= metodoSoluzioniRandomDefined(soluzioniRandomiche);
    var metodoRimuviCicli = metodoRimuviCicliDefined(metodoRimuviCicli);
    var rimuoviCicli=rimuoviCicliDefined(rimuoviCicli);
    var name = nameDefined(name);
    var number = numberDefined(number);
    var rootToRoot = rootToRootDefined(rootToRoot);
    var cospeciationCost = cospeciationCostDefined(cospeciationCost);
    var duplicationCost = duplicationCostDefined(duplicationCost);
    var hostswitchCost = hostswitchCostDefined(hostswitchCost);
    var sortingCost = sortingCostDefined(sortingCost);
    var numberMaxSolution = numberMaxSolutionDefined(numberMaxSolution);
    var curve = curveDefined(curve);
    var curveSwitch = curveSwitchDefined(curveSwitch);
    var euristica = euristicaDefined(euristica);

    if ( disegna==undefined ){
        var disegna=true;
    }

    d3.select("svg").remove();
    d3.select("#tooltip").remove();
    //console.log(("graphs_gdt/"+name+"/"+cospeciationCost+"/"+duplicationCost+"/"+hostswitchCost+"/"+sortingCost+"/"+numberMaxSolution.toString()+"/"+rootToRoot+"/"+rimuoviCicli+"/"+metodoRimuviCicli+"/"+soluzioniRandomiche+"/"+ maximumHostSwitchDistance));
	d3.json("http://localhost:1998/graphs_output_gdt/"+name, function(error, dataOriginal) {

        if (error) throw error;
        // Assumption: we only want to draw the first graph_analysis for now.
        // TODO: Is this decision final?
        if (dataOriginal[0]["problem"] != undefined) {
            st="Combination not found";
            var p = document.getElementById("problem");
            p.innerHTML = st;
            return 0;
        } else {
            st="";
            var p = document.getElementById("problem");
            p.innerHTML = st;
        }

        setTimeout(function(){
        numberReset();
        },0)

        data = dataOriginal[0]["graph"][0][0];
        graphs_solution = dataOriginal[0]["graph"];
        elaboraNodi(data);
        // End assumption
        numberSolution = dataOriginal[0]["number"];

        setTimeout(function(){
        loadNumberSolution(numberSolution);
        },0)

        // Data contains an array of graphs, each contains both the host and the guest trees
        var vis = d3.custom.cophy()
            .width(width)
            .height(height)
            .longLeaves(longLeaves)
            .equalsLivelEqualsHeight(equalsLivelEqualsHeight)
            .separatorBetweenHostNodesWidth(separatorBetweenHostNodesWidth)
            .curve(curve)
            .curveSwitch(curveSwitch)
            .separatorBetweenHostNodesHight(separatorBetweenHostNodesHight)
            .disegna(disegna)
            .euristica(euristica);

        // bind the dom element to the chart.
        d3.select("body")
            .datum(data)
            .call(vis);

    });
}


