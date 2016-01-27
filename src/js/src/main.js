
var LuxParking = {};

LuxParking.lastRss = null;
LuxParking.lastRssTimestamp = null;


LuxParking.sendError = function(type, err) {
  appMessageQueue.clear();
  appMessageQueue.send({
                       "type": type,
                       "method": METHOD.REPLY_ERROR,
                       "error": err
                       });
};


LuxParking.queryRss = function(callback, param) {
  var now = new Date();
  console.log("Now: " + now);
  console.log("lastRssTimestamp: " + LuxParking.lastRssTimestamp);
  console.log("Diff: " + (now - LuxParking.lastRssTimestamp));

  // if last query to the server was less than 30s ago, don't requery, use the cached version
  if (LuxParking.lastRssTimestamp && (now - LuxParking.lastRssTimestamp) < (30 * 1000)) {
    console.log("Using cached RSS");
    callback(LuxParking.lastRss, param);
    return;
  }
  
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
//      console.log("RSS version: " + data.rss.version);
//      console.log("Title: " + data.rss.channel.title);
//      console.log("Date: " + data.rss.channel.lastBuildDate);
      
      LuxParking.lastRss = data;
      LuxParking.lastRssTimestamp = new Date();
      
      callback(data, param);
    }
    else {
      console.log('Error');
    }
  };
  console.log("Sending HTTP request");
  req.send(null);
};


LuxParking.sendParkings = function(data, param) {
  var parkings = [];
  
  data.rss.channel.item.forEach(function(item) {
                                  var area = item.quartier[0].__content__;
                                  if (!param || area == param) {
                                    console.log("+Parking " + item.title + " (" + area + "): " + item.actuel + "/" + item.total);
                                    parkings.push(item);
                                  }
                                });
  
  appMessageQueue.send({
                       "type": TYPE.PARKING,
                       "method": METHOD.REPLY_COUNT,
                       "count": parkings.length,
                       "timestamp": LuxParking.lastRssTimestamp.toISOString()
                       });
  
  for (var i=0; i<parkings.length; i++){
    var parking = parkings[i];
    var area = parking.quartier[0].__content__;
    var free = parking.actuel ? parseInt(parking.actuel) : -1;
    var capacity = parking.total ? parseInt(parking.total) : -1;
    var trend = parking.tendance ? parseInt(parking.tendance) : 99; //anything other than -1, 0, 1 will do
    var open = parking.ouvert ? parseInt(parking.ouvert) : -1;
    appMessageQueue.send({
      "type": TYPE.PARKING,
      "method": METHOD.REPLY_ITEM,
      "index": i,
      "id": parking.id,
      "name": parking.title,
      "area": area,
      "open": open,
      "capacity": capacity,
      "free": free,
      "trend": trend
    });
  }
};


LuxParking.sendAreas = function(data, param) {
  var areas = [];
  data.rss.channel.item.forEach(function(item) {
                                  var area = item.quartier[0].__content__;
//                                  console.log("Parking " + item.title + " (" + area + "): " + item.actuel + "/" + item.total);
                                  if (areas.indexOf(area) < 0) {
                                    areas.push(area);
                                    console.log("+Area: " + area);
                                  }
                                });
  
  appMessageQueue.send({
                       "type": TYPE.AREA,
                       "method": METHOD.REPLY_COUNT,
                       "count": areas.length,
                       "timestamp": LuxParking.lastRssTimestamp.toISOString()
                       });
  
  for (var i=0; i<areas.length; i++) {
    appMessageQueue.send({
                         "type": TYPE.AREA,
                         "method": METHOD.REPLY_ITEM,
                         "index": i,
                         "name": areas[i]
                         });
  }
};


LuxParking.handleAppMessage = function(e) {
  console.log("Received message: " + JSON.stringify(e.payload));
  var msgType = e.payload.type;
  var msgMethod = e.payload.method;
  console.log("Type: " + msgType + ", method: " + msgMethod);

  switch (msgMethod) {
    case METHOD.REQUEST_GET:
      switch (msgType) {
        case TYPE.AREA:
          LuxParking.queryRss(LuxParking.sendAreas, null);
          break;
        case TYPE.PARKING:
          var area = e.payload.area;
          LuxParking.queryRss(LuxParking.sendParkings, area);
          break;
      }
      break;
  }
};

LuxParking.init = function() {
  console.log("App ready!");
  LuxParking.queryRss(LuxParking.sendAreas, null);
};



Pebble.addEventListener("ready", LuxParking.init);
Pebble.addEventListener("appmessage", LuxParking.handleAppMessage);




