
var appMessageQueue = {
  queue: [],
  numTries: 0,
  maxTries: 5,
  working: false,
  clear: function() {
      this.queue = [];
      this.working = false;
  },
  isEmpty: function() {
      return this.queue.length === 0;
  },
  nextMessage: function() {
      return this.isEmpty() ? {} : this.queue[0];
  },
  send: function(message) {
      if (message) this.queue.push(message);
      if (this.working) return;
      if (this.queue.length > 0) {
        this.working = true;
        var ack = function() {
          appMessageQueue.numTries = 0;
          appMessageQueue.queue.shift();
          appMessageQueue.working = false;
          appMessageQueue.send();
        };
        var nack = function() {
          appMessageQueue.numTries++;
          appMessageQueue.working = false;
          appMessageQueue.send();
        };
        if (this.numTries >= this.maxTries) {
          console.log('Failed sending AppMessage: ' + JSON.stringify(this.nextMessage()));
          ack();
        }
        console.log('Sending AppMessage: ' + JSON.stringify(this.nextMessage()));
        Pebble.sendAppMessage(this.nextMessage(), ack, nack);
      }
  }
};


var LuxParking = {};

LuxParking.sendError = function(type, err) {
  appMessageQueue.clear();
  appMessageQueue.send({
                       "type": type,
                       "method": 4, // REPLY_ERROR
                       "error": err
                       });
};


LuxParking.queryRss = function(callback, param) {
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


LuxParking.sendParkings = function(data, param) {
  var parkings = [];
  
  data.rss.channel.item.forEach(function(item) {
                                  var area = item.quartier[0].__content__;
                                  console.log("Parking " + item.title + " (" + area + "): " + item.actuel + "/" + item.total);
                                  if (!param || area == param) {
                                    console.log("+Ajout parking: " + item.title);
                                    parkings.push(item);
                                  }
                                });
  
  appMessageQueue.send({
                       "type": 1, // PARKING
                       "method": 2, // REPLY_COUNT
                       "count": parkings.length
                       });
  
  for (var i=0; i<parkings.length; i++){
    var parking = parkings[i];
    var area = parking.quartier[0].__content__;
    appMessageQueue.send({
                         "type": 1, // PARKING,
                         "method": 3, // REPLY_ITEM
                         "index": i,
                         "id": parking.id,
                         "name": parking.title,
                         "area": area,
                         "capacity": parseInt(parking.total),
                         "occupancy": parseInt(parking.actuel)
                         });
  }
}


LuxParking.sendAreas = function(data, param) {
  var areas = [];
  data.rss.channel.item.forEach(function(item) {
                                  var area = item.quartier[0].__content__;
                                  console.log("Parking " + item.title + " (" + area + "): " + item.actuel + "/" + item.total);
                                  if (areas.indexOf(area) < 0) {
                                    areas.push(area);
                                    console.log("+Ajout quartier: " + area);
                                  }
                                });
  
  appMessageQueue.send({
                       "type": 2, // AREA
                       "method": 2, // REPLY_COUNT
                       "count": areas.length
                       });
  
  for (var i=0; i<areas.length; i++) {
    appMessageQueue.send({
                         "type": 2, // AREA,
                         "method": 3, // REPLY_ITEM
                         "index": i,
                         "name": areas[i]
                         });
  }
}


LuxParking.handleAppMessage = function(e) {
  console.log("Received message: " + JSON.stringify(e.payload));
  var msgType = e.payload.type;
  var msgMethod = e.payload.method;
  console.log("Type: " + msgType + ", method: " + msgMethod);

  switch (msgMethod) {
    case 1: // REQUEST_GET
      switch (msgType) {
        case 2: // AREA
          LuxParking.queryRss(LuxParking.sendAreas, null);
          break;
        case 1: // PARKING
          var area = e.payload.area;
          LuxParking.queryRss(LuxParking.sendParkings, area);
          break;
      }
      break;
  }
}

LuxParking.init = function() {
  console.log("App ready!");
  LuxParking.queryRss(LuxParking.sendAreas, null);
}



Pebble.addEventListener("ready", LuxParking.init);
Pebble.addEventListener("appmessage", LuxParking.handleAppMessage);




