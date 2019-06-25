d3.custom = d3.custom || {};

d3.custom.cophy = function() {
    // Parameters
    var width = window.innerWidth;
    var height = window.innerHeight;
    var hostLeafWidth = width / 20;
    var hostNodeBoxHeight = 100;
    var separatorBetweenHostNodesWidth = 1;
    var separatorBetweenHostNodesHight = 2;
    var longLeaves = true;
    var equalsLivelEqualsHeight = false;
    var curve = false;
    var disegna = true;
    var curveSwitch = false;
    var euristica = "1";

    var parasiteNodesRadius = 22;

    function chart(selection) {
        selection.each(function(data) {
            // Select the svg element, if it exists.
            var svg = d3.select(this).selectAll("svg").data([data]);
            // Otherwise, create the svg
            svg.enter().append("svg");
            ridisegna(disegna, curve, curveSwitch,longLeaves, equalsLivelEqualsHeight, width, height, hostLeafWidth, hostNodeBoxHeight, separatorBetweenHostNodesWidth, separatorBetweenHostNodesHight, parasiteNodesRadius,euristica);
        });
    }

    // Getter/setters
    chart.width = function(_) {
        if (!arguments.length) return width;
        width = _;
        return chart;
    };

    chart.euristica = function(_) {
        if (!arguments.length) return euristica;
        euristica = _;
        return chart;
    };
    
    chart.height = function(_) {
        if (!arguments.length) return height;
        height = _;
        return chart;
    };

    chart.leafWidth = function(_) {
        if (!arguments.length) return hostLeafWidth;
        hostLeafWidth = _;
        return chart
    };

    chart.separatorWidth = function(_) {
        if (!arguments.length) return separatorBetweenHostNodesWidth;
        separatorBetweenHostNodesWidth = _;
        return chart
    };

    chart.separatorBetweenHostNodesWidth = function(_) {
        if (!arguments.length) return separatorBetweenHostNodesWidth;
        separatorBetweenHostNodesWidth = _;
        return chart
    };

    chart.separatorHight = function(_) {
        if (!arguments.length) return separatorBetweenHostNodesHight;
        separatorBetweenHostNodesHight = _;
        return chart
    };
    chart.separatorBetweenHostNodesHight = function(_) {
        if (!arguments.length) return separatorBetweenHostNodesHight;
        separatorBetweenHostNodesHight = _;
        return chart
    };

    chart.longLeaves = function(_) {
        if (!arguments.length) return longLeaves;
        longLeaves = _;
        return chart
    };
    
    chart.equalsLivelEqualsHeight = function(_) {
        if (!arguments.length) return equalsLivelEqualsHeight;
        equalsLivelEqualsHeight = _;
        return chart
    };
    
    chart.parasiteNodesRadius = function(_) {
        if (!arguments.length) return parasiteNodesRadius;
        parasiteNodesRadius = _;
        return chart
    };

    chart.curve = function(_) {
        if (!arguments.length) return curve;
        curve = _;
        return chart
    };

    chart.curveSwitch = function(_) {
        if (!arguments.length) return curveSwitch;
        curveSwitch = _;
        return chart
    };

    chart.disegna = function(_) {
        if (!arguments.length) return disegna;
        disegna = _;
        return chart
    };

    return chart;
};
