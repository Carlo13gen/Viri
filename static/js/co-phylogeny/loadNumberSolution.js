function loadNumberSolution(number) {
    function supportLoadNumberSolution(number){
    	//var st="Testo del ";
    	var stForNav = "";
        for (var i = 1; i <= number; i++) {
            //st=st.concat("<option>").concat(i).concat("</option>");
            stForNav=stForNav.concat('<li><a onclick="cambiaNumero('+i+')">  ').concat(i).concat("</a></li>")
            }
        return stForNav;
    }

    var optionMenuTendina=supportLoadNumberSolution(number);
    var p = document.getElementById("numberSolutionGraph");
    p.innerHTML = optionMenuTendina;
    var p = document.getElementById("mostraNumeroSoluzione");
    p.innerHTML = 1+'<span class="caret"></span>';
    var p = document.getElementById("mostraNumeroSoluzione").setAttribute("value", 1);

}