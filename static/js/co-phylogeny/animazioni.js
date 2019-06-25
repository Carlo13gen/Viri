var svg;

function animazione() {
    svg = d3.select("#svgCoPhylo");

    (evidenziamento_mouse_rect() |
        evidenziamento_mouse_parassita() |
        evidenziamento_mouse_edge()
    )

}

function evidenziamento_mouse_edge() {

    svg.selectAll("#pathParassite")
        .attr("opacity", 1)
        .on("mouseover", sopra_arco)
        .on("mousemove", valore_tooltip_edge)
        .on("mouseout", nascondimento_tooltip_edge)
    return true;
}

function evidenziamento_mouse_rect() {

    svg.selectAll("#rectHost")
        .attr("opacity", 1)
        .on("mouseover", sopra_rect)
        .on("mousemove", valore_tooltip_rect)
        .on("mouseout", nascondimento_tooltip_rect)
    return true;
}

function evidenziamento_mouse_parassita() {

    svg.selectAll("#circleParassite")
        .attr("opacity", 1)
        .on("mouseover", sopra_parassita)
        .on("mousemove", valore_tooltip_parassita)
        .on("mouseout", nascondimento_tooltip_parassita)
    return true;
}


function sopra_arco(d, i) {
    rimuoviTestoMouse(svg);

    //coordinate mouse
    var coordinates = [0, 0];
    coordinates = d3.mouse(this);
    var x = coordinates[0];
    var y = coordinates[1];

    var host = data["nodes"]
        .filter(function (d2) {
            return d2["id"].startsWith("H") && (d2["x"] <= x) && ((d2["width"] + d2["x"]) > x) && (d2["y"] <= y) && ((d2["y"] + d2["height"]) > y)
        })[0];
    try {
        evidenziaHost(host["id"], svg)

        aggiungereTestoMouse(("" + host.id), svg, x, y);

        /*       if (curveDefined()) {
                    aggiungereTestoMouse((""+host.id),svg,x,y);
                } else{
                    aggiungereTestoMouse((""+d.from +"-"+d.to),svg,x,y);
                    d3.select(this)
                        .style("stroke","red");
                }
        */
    } catch (err) {}
}

function valore_tooltip_edge(d, i) {

    rimuoviTestoMouse(svg);
    //coordinate mouse
    var coordinates = [0, 0];
    coordinates = d3.mouse(this);
    var x = coordinates[0];
    var y = coordinates[1];

    var host = data["nodes"]
        .filter(function (d2) {
            return d2["id"].startsWith("H") && (d2["x"] <= x) && ((d2["width"] + d2["x"]) > x) && (d2["y"] <= y) && ((d2["y"] + d2["height"]) > y)
        })[0];
    try {
        var result = evidenziaHost(host["id"], svg);
        listaPadre = result[1]
        listaFigli = result[0]

        aggiungereTestoMouse(("" + host.id), svg, x, y);

        /*        
                if (curveDefined()) {
                    aggiungereTestoMouse((""+host.id),svg,x,y);
                } else{
                    aggiungereTestoMouse((""+d.from +"-"+d.to),svg,x,y);
                    d3.select(this)
                        .style("stroke","red");
                }
        */
        var stringaListaFigli = ""
        listaFigli.forEach(function (d) {
            stringaListaFigli += ", " + d.toString().substring(1);
        })

        var stringaListaParassiti = ""
        findNodeParassiteInHost(host.id).forEach(function (d2) {
            stringaListaParassiti = stringaListaParassiti + ", " + d2.toString().substring(1)
        })


        tooltip.html("<table width='100%' ><tr><td>CURRENTLY HIGHLIGHTED NODE: </td><td> name = " + host.id.substring(1) + "</td><td>parasite nodes = " + stringaListaParassiti.substring(2) + "</td><td> parent = " + listaPadre.toString().substring(1) + "</td><td>children = " + stringaListaFigli.substring(2) + "</td></tr></table>").style("visibility", "visible");
    } catch (err) {

    }

}

function nascondimento_tooltip_edge(d, i) {

    rimuoviTestoMouse(svg);

    (togliereEvidenziazioneCircle(svg) |
        togliereEvidenziazionePath(svg) |
        togliereEvidenziazioneRect(svg)
    )
    d3.select(this)
        .style("stroke", "black")
        .classed("hover", false);

    tooltip.style("visibility", "hidden");
}

/* who calls this function? */

function sopra_rect(d, i) {
    rimuoviTestoMouse(svg)
    evidenziaHost(d["id"], svg); 

    var coordinates = [0, 0];      // prepare an array of two values
    coordinates = d3.mouse(this);  // take the current position of the mouse
    var x = coordinates[0];        // current x-coordinate of the mouse
    var y = coordinates[1];        // current y-coordinate of the mouse


    aggiungereTestoMouse(("" + d.id), svg, x, y); // this call has no apparent effect
}

function nascondimento_tooltip_rect(d, i) {
    rimuoviTestoMouse(svg);
    (togliereEvidenziazioneCircle(svg) |
        togliereEvidenziazionePath(svg) |
        togliereEvidenziazioneRect(svg)
    )
    d3.select(this)
        .classed("hover", false)
        .attr("stroke-width", "0px");


    tooltip.style("visibility", "hidden");
}

function valore_tooltip_rect(d, i) {

    rimuoviTestoMouse(svg);
    var listaFigli = findChildrenIdsOfHostOrParasiteNode(d.id, "H", data);
    var listaPadre = findParentIdsOfHostOrParasiteNode(d.id, "H", data);

    var coordinates = [0, 0];
    coordinates = d3.mouse(this);
    var x = coordinates[0];
    var y = coordinates[1];

    aggiungereTestoMouse(("" + d.id), svg, x, y)
    var stringaListaFigli = ""
    listaFigli.forEach(function (d) {
        stringaListaFigli += ", " + d.toString().substring(1);
    })

    var stringaListaParassiti = ""
    findNodeParassiteInHost(d.id).forEach(function (d2) {
        stringaListaParassiti = stringaListaParassiti + ", " + d2.toString().substring(1)
    })

    tooltip.html("<table width='100%' ><tr><td>CURRENTLY HIGHLIGHTED NODE: </td><td> name = " + d.id.substring(1) + "</td><td>parasite nodes = " + stringaListaParassiti.substring(2) + "</td><td> parent = " + listaPadre.toString().substring(1) + "</td><td>children = " + stringaListaFigli.substring(2) + "</td></tr></table>").style("visibility", "visible");

    //tooltip.html("<table width='100%' ><tr><td> name: # " + d.id.substring(1) + "</td><td> nodes parasite:" + stringaListaParassiti.substring(2) + "</td><td> parent:" + listaPadre.toString().substring(1) + "</td><td> child:" + stringaListaFigli.substring(2) + "</td></tr></table>").style("visibility", "visible");
}

function sopra_parassita(d, i) {
    rimuoviTestoMouse(svg);
    var host = d["belongs_to"]
    evidenziaHost(host, svg);

    var listaFigli = findChildrenIdsOfHostOrParasiteNode(d.id, "P", data);
    var listaPadre = findParentIdsOfHostOrParasiteNode(d.id, "P", data);

    // metto in evidenza i nodi
    svg.selectAll("#circleParassite")
        .style("stroke", "black")
        .style("stroke-width", "2")
        .attr("opacity", "0.30");

    (sottolinea(svg.selectAll("#circleParassite").filter(function (d3) {
            return (listaFigli.indexOf(d3.id) > -1);
        }), "red") |
        sottolinea(svg.selectAll("#circleParassite").filter(function (d3) {
            return (listaPadre.indexOf(d3.id) > -1);
        }), "yellow") |
        sottolinea(svg.selectAll("#circleParassite").filter(function (d3) {
            return d3.id == d.id;
        }), "blue")
    )
    // metto in evidenza gli archi
    svg.selectAll("#pathParassite")
        .style("stroke", "black")
        .style("stroke-width", "1")
        .attr("opacity", "0.30");
    try {
        (sottolinea(svg.selectAll("#pathParassite").filter(function (d3) {
                return (d3.from == d.id);
            }), "red") |
            sottolinea(svg.selectAll("#pathParassite").filter(function (d3) {
                return (d3.to == d.id);
            }), "yellow")
        )
    } catch (err) {}
    //testo parent

    (infoNodoPadreOFiglio(listaPadre, svg) |
        infoNodoPadreOFiglio(listaFigli, svg))

    //testo nodo
    var coordinates = [0, 0];
    coordinates = d3.mouse(this);
    var x = coordinates[0];
    var y = coordinates[1];


    aggiungereTestoMouse(("prova" + d.id), svg, x, y);
}

function nascondimento_tooltip_parassita(d, i) {
    rimuoviTestoMouse(svg);
    (togliereEvidenziazioneCircle(svg) |
        togliereEvidenziazionePath(svg) |
        togliereEvidenziazioneRect(svg)
    )
    d3.select(this)
        .classed("hover", false)
        .attr("stroke-width", "0px");

    rimuoviInfoNodoParassita(svg);
    tooltip.style("visibility", "hidden");
}

function valore_tooltip_parassita(d, i) {
    rimuoviTestoMouse(svg);
    var coordinates = [0, 0];
    coordinates = d3.mouse(this);
    var x = coordinates[0];
    var y = coordinates[1];


    aggiungereTestoMouse(("" + d.id), svg, x, y);

    var listaFigli = findChildrenIdsOfHostOrParasiteNode(d.id, "P", data);
    var listaPadre = findParentIdsOfHostOrParasiteNode(d.id, "P", data);
    stringaListaFigli = ""
    listaFigli.forEach(function (d2) {
        stringaListaFigli = stringaListaFigli + ", " + d2.toString().substring(1)
    })

    //tooltip.html("<table width='100%' ><tr><td> name: " + d.id.substring(1) + "</td><td> belongs to:" + d.belongs_to.substring(1) + "</td><td> parent:" + listaPadre.toString().substring(1) + "</td><td> child:" + stringaListaFigli.substring(2) + "</td></tr></table>").style("visibility", "visible");
    tooltip.html("<table width='100%' ><tr><td>CURRENTLY HIGHLIGHTED NODE: </td><td> name = " + d.id.substring(1) + "</td><td>belongs to = " + d.belongs_to.substring(1) + "</td><td> parent = " + listaPadre.toString().substring(1) + "</td><td>children = " + stringaListaFigli.substring(2) + "</td></tr></table>").style("visibility", "visible");

}

function findNodeParassiteInHost(idHost) {
    return data["nodes"].filter(function (d) {
            return d.belongs_to == idHost
        })
        .map(function (d) {
            return d.id
        })
}

function evidenziaHost(idHost, svg) {
    var listaFigliHost = findChildrenIdsOfHostOrParasiteNode(idHost, "H", data);
    var listaPadreHost = findParentIdsOfHostOrParasiteNode(idHost, "H", data);

    svg.selectAll("#rectHost")

        .style("stroke", "white")
        .style("stroke-width", "2")
        .transition()
        .duration(120)
        .attr("opacity", "0.30");

    (trasformaRect([idHost], "1", svg) |
        trasformaRect(listaFigliHost, "0.70", svg) |
        trasformaRect(listaPadreHost, "0.70", svg)
    )
    return [listaFigliHost, listaPadreHost];
}

function togliereEvidenziazioneRect(svg) {
    svg.selectAll("#rectHost")
        .style("stroke", "white")
        .style("stroke-width", "2")
        .transition()
        .duration(120)
        .attr("opacity", "1")
    return true;
}

function togliereEvidenziazioneCircle(svg) {
    svg.selectAll("#circleParassite")
        .attr("opacity", "1")
        .style("stroke-width", "2")
        .style("stroke", "black");
    return true;
}

function togliereEvidenziazionePath(svg) {
    svg.selectAll("#pathParassite")
        .attr("opacity", "1")
        .style("stroke-width", "1")
        .style("stroke", "black");
    return true;
}

function rimuoviTestoMouse(svg) {
    svg.select("#testoMouse").remove();
    return true;
}

function aggiungereTestoMouse(text, svg, x, y) {
    svg.append("text")
        .attr("id", "testoMouse")
        .attr("x", (x + 3).toString())
        .attr("y", (y + 3).toString())
        .on("mouseover", sopra_mouse)
        .on("mousemove", sopra_mouse)
        .on("mouseout", sopra_mouse)
        .text("" + text.substring(1));
    return true;
}

function sopra_mouse(d, i) {
    rimuoviTestoMouse(svg);
}


function sottolinea(elemento, colore) {
    elemento.style("stroke", colore)
        .style("stroke-width", "4")
        .attr("opacity", "1");
    return 1;
}

function infoNodoPadreOFiglio(lista, svg) {
    lista.forEach(function (d3) {
        var nodo = findNodeById(d3, data);
        svg.append("text")
            .attr("id", "infoNodo")
            .attr("x", (nodo.x).toString())
            .attr("y", (nodo.y).toString())
            .text("" + nodo.id.substring(1));
    })
    return true;
}

function rimuoviInfoNodoParassita(svg) {
    svg.selectAll("#infoNodo").remove()
}

function trasformaRect(lista, opacita, svg) {
    svg.selectAll("#rectHost")
        .filter(function (d3) {
            return (lista.indexOf(d3.id)) > -1;
        })
        .transition()
        .duration(120)
        .attr("opacity", opacita);
    return true;
}
