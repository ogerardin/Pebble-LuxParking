Pebble.addEventListener("ready",
    function(e) {
        console.log("App ready!");
    }
);

Pebble.addEventListener("appmessage",
    function(e) {
        console.log("Received message: " + JSON.stringify(e.payload));
        var msgType = e.payload.messageType;
        console.log("Type: " + msgType);

        switch(msgType) {
        case 2:
          var area = e.payload.area;
          queryRss(queryArea, area);
          break;
        case 3:
          queryRss(getAreas, null);
          break;
        }
    }
);

function queryArea(data, param) {
  data.rss.channel.item.forEach(function(item) {
    console.log("Parking " + item.title + " (" + item.quartier[0].__content__ + "): " + item.actuel + "/" + item.total);
    //Pebble.sendAppMessage({"messageType": 1, "parkingCapacity":item.total, "parkingOccupancy": item.actuel});
  });
}

function getAreas(data, param) {
  var areas = [];
  data.rss.channel.item.forEach(function(item) {
//    console.log("Parking " + item.title + " (" + item.quartier[0].__content__ + "): " + item.actuel + "/" + item.total);
    var area = item.quartier[0].__content__;
    if (areas.indexOf(area) < 0) {
      areas.push(area);
      console.log("+Ajout quartier: " + area);
    }
    //Pebble.sendAppMessage({"messageType": 1, "area": area});
  });
}


function queryRss(callback, param) {
  var req = new XMLHttpRequest();
  // Data is provided by Ville de Luxembourg as RSS 2.0, which is actually XML. Since it is nearly impossible to parse XML using
  // PebbleKit JS (no DOM access), we use an online XML2JSON service; see http://bitzesty.com/2011/04/11/xml-to-json-api-proxy-for-quick-mash-ups/
  req.open("GET", "http://xml2json.herokuapp.com/?url=http://service.vdl.lu/rss/circulation_guidageparking.php&callback=dummy", true);
  req.onload = function(e) {
    if (req.readyState == 4 && req.status == 200) {
      console.log("Received HTTP response");
      // xml2json forces us to specify a callback; to get the pure JSON data we have to strip the callback from the response
      // NOTE: we could also use the name of a real callback and call eval(req.responseText) to invoke the callback, but then we wouldn't be 
      // able to pass extra arguments to the callback.
      var json = req.responseText.slice("dummy(".length, -1);
      var data = JSON.parse(json);
      console.log("RSS version: " + data.rss.version);
      console.log("Title: " + data.rss.channel.title);
      callback(data, param);
    }
    else {
      console.log('Error');
    }
  };
  console.log("Sending HTTP request");
  req.send(null);
}
