function initialize() {
  var map = new google.maps.Map(document.getElementById('balloon-trajectory'), {});

  var ctaLayer = new google.maps.KmlLayer({
    url: 'http://www.bristol-seds.co.uk/assets/posts/buseds1/path.kml'
  });
  ctaLayer.setMap(map);
}

google.maps.event.addDomListener(window, 'load', initialize);
