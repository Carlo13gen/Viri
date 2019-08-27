function mostraOpzioni(){
    function listaFile(){

        d3.json("http://localhost:8000/list_of_file", function(error, dataOriginal) {
            st="";
            stForNav=""


            if (error) throw error;
            dataOriginal.forEach(function(d){
                st=st.concat("<option>").concat(d).concat("</option>");
                stForNav=stForNav.concat('<li><a onclick="cambiaFile(\''+d+'\')">  ').concat(d).concat("</a></li>")
            });
            //var p = document.getElementById("select");
            //  p.innerHTML = st;
            var p = document.getElementById("ulListaFile");
            p.innerHTML = stForNav;
            var p = document.getElementById("mostraNomeFile");
            p.innerHTML = dataOriginal[0]+'<span class="caret"></span>';
            var p = document.getElementById("mostraNomeFile").setAttribute("value", dataOriginal[0]);
            var username = document.getElementById("title_user").textContent.trim()
            console.log(username)
            var foo = sessionStorage
            console.log(foo)
            grafic_output(dataOriginal[0])

        });
    }

    listaFile();

}




function cambiaFile(nomeFile){
    console.log(nomeFile)
    var username = document.getElementById("title_user").textContent.trim()
    console.log(username)
    var p = document.getElementById("mostraNomeFile");
    p.innerHTML = nomeFile+'<span class="caret"></span>';
    var p = document.getElementById("mostraNomeFile").setAttribute("value", nomeFile);
    var foo = document.cookie.toString()
    console.log(foo)
    grafic_output(nomeFile, username)

}



function getListaFileNexus(){
    var listaFileNexus=[];
    function listaFile(){

        d3.json("http://localhost:8000/list_of_file", function(error, dataOriginal) {

            dataOriginal.forEach(function(d){
                listaFileNexus.push(d)
            });
        });
    }

    listaFile();
    return listaFileNexus;
}
