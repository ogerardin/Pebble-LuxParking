Pebble.addEventListener("ready",
    function(e) {
        console.log("Hello world! - Sent from your javascript application.");
    }
);

Pebble.addEventListener("appmessage",
    function(e) {
        console.log("Received message: " + JSON.stringify(e.payload));
        var msgType = e.payload["messageType"];
        console.log("Type: " + msgType);
                        
        switch(msgType) {
        case 2:
          var area = e.payload["area"];
          queryArea(area);
          break;
        }
    }
);

function processData(json) {
  console.log("RSS version: " + json.rss.version);
  console.log("Title: " + json.rss.channel.title);
  json.rss.channel.item.forEach(function(item) {
    console.log("Item: " + item.title);
    //Pebble.sendAppMessage({"messageType": 1, "parkingCapacity":item.total, "parkingOccupancy": item.actuel});
  });
}


function queryArea(area) {
  var req = new XMLHttpRequest();
  // Data is provided as RSS 2.0 which is actually XML. Since it is nearly impossible to parse XML using
  // PebbleKit JS (no DOM access), we use an online XML2JSON service; see http://bitzesty.com/2011/04/11/xml-to-json-api-proxy-for-quick-mash-ups/
  req.open("GET", "http://xml2json.herokuapp.com/?url=http://service.vdl.lu/rss/circulation_guidageparking.php&callback=processData", true);
  req.onload = function(e) {
    if (req.readyState == 4 && req.status == 200) {
      if(req.status == 200) {
        console.log("Received HTTP response");
        // response is a JSONP callback (xml2json requirement) so we call eval() to invoke the callback
        eval(req.responseText);
      }
      else {
        console.log('Error');
      }
    }
  }
  req.send(null);
}
