//Dimensions of SVG window
var w = 400;
var h = 300;

// Import data
d3.text("/assets/posts/buseds1/balloon_coords.csv", function(text) {
//d3.text("http://0.0.0.0:4000/assets/posts/buseds1/balloon_coords.csv", function(text) {
    var rawData = d3.csv.parseRows(text);

    parsetime = d3.time.format("%H:%M:%S").parse;

    var data = [];
    // Convert non-date values to numbers
    for (var i = 0; i < rawData.length; i++) {
      data.push({"x": parsetime(rawData[i][0]), "y": Number(rawData[i][3])});
    }
    dataseries = [{"values": data, "key": "Altitude", "color": 'FireBrick'}];

    nv.addGraph(function() {
      var chart = nv.models.lineChart().margin({left: 80, right: 40});
      chart.xAxis.tickFormat(function (d) {
	return d3.time.format("%H:%M:%S")(new Date(d));
      });
      chart.xAxis.axisLabel('Time');
      chart.yAxis.axisLabel('Altitude (m)');
      chart.tooltips(false);

      chart.showLegend(false);
      d3.select("#alt-time")
      .attr("width", w).attr("height", h)
      .datum(dataseries).call(chart);
    nv.utils.windowResize(function() { chart.update() });
    return chart;
    });
});


