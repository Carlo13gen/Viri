function mostraOpzioni(){
    function listaFile(){

        d3.json("http://localhost:8000/list_of_file", function(error, dataOriginal) {


            if (error) throw error;

            //var p = document.getElementById("select");
            //  p.innerHTML = st;

            grafic_output(document.getElementById("file_name"))

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
