function changeNumberSolution(numberSolutionGraph, disegna) {
	if(numberSolutionGraph == undefined ){
    	var numberSolutionGraph = numberSolutionGraphDefined();
        console.log(numberSolutionGraph)
    }

    numberSolutionGraph -=1; 
    data = graphs_solution[numberSolutionGraph][0]
    /*
    dataMeta = graphs_solution[numberSolutionGraph][0];
    data = graphs_solution[0][0];


    //aggiorno le appartenze    
    dataMeta.nodes.forEach(function (d){
        id = d.id;
        nodoParassita=findNodeById(id, data);
        nodoParassita.belongs_to = d.belongs_to;
    })
    //aggiorno gli switch
    dataMeta.links.forEach(function (d){
        link = data.linksParassiteParassite.find(function(d2){return ((d2.from == d.from) && (d.to == d2.to));})
        link.switch=d.switch
    })
    */

    if((disegna)||(disegna==undefined)){
    	elaboraNodi(data);
    	ridisegna(disegna);
	}
}

function cambiaNumero(numero){
    console.log(numero)

        var p = document.getElementById("mostraNumeroSoluzione");
        p.innerHTML = numero+'<span class="caret"></span>';
        var p = document.getElementById("numberSolutionGraph").setAttribute("value", numero);
        changeNumberSolution(numero);

}


function getNumberSolution(){
	return numberSolution;
}