function salva(){
	//get svg element.
	var svg = document.getElementById("spazioSvg");
	console.log(svg)
  var html = d3.select("#spazioSvg")
        .attr("version", 1.1)
        .attr("xmlns", "http://www.w3.org/2000/svg")
        .node().parentNode.innerHTML;

  //console.log(html);
  var imgsrc = 'data:image/svg+xml;base64,'+ btoa(html);
  var img = '<img src="'+imgsrc+'">'; 
  d3.select("#svgdataurl").html(img);
}

function changeTest(){
  var rimuoviCicli=rimuoviCicliDefined();
  if (rimuoviCicli=="true"){
    grafic();
  }

}
function changeRandomSolution(){
  if (numberMaxSolutionDefined().toString()!="NaN"){
    grafic();    
  }
}






var host2isRightIsLeaf = d3.map();

function bloccato(){
  if(bloccatoDefined()){
    data["nodes"]
      .filter(function(d){return d.id.startsWith("H")})
      .forEach(
        function(d){
          host2isRightIsLeaf.set(d["id"],[d["right_child"],d["left_child"]] );
        });
  }
}

function changeNexusFile(){
  if(bloccatoDefined()){
    bloccatoTogliere();
  }
  grafic();
}

function getListaBloccata(){
  return host2isRightIsLeaf;  
}