/**
 * PebbleKit JS Implementation
 * Have the Pebble make requests on back-button press. Save the
 * timestamp on Pebble and make this request no more than once every min.
 *
 * When a request is received, download schedule.xml and respond to Pebble
 * with the title of the currently playing show.
 */
Pebble.addEventListener("ready", function (e) {
    // Do init here.
});

Pebble.addEventListener("appmessage", function(e) {
    // Assume all messages are requesting schedule data.
    // Respond with the currently playing show.
    var xmlparser,
        xmldoc,
        titleArr,
        currentShow,
        req = new XMLHttpRequest();

    req.onload = function (e) {
        if (req.readyState == 4 && req.status == 200) {
            xmlparser = new DOMParser();
            xmldoc = xmlparser.parseFromString(req.responseText, "application/xml");
            titleArr = xmldoc.getElementsByTagName("showTitle");
            currentShow = titleArr[titleArr.length - 1].firstChild.substringData(0,100).replace("<!--[CDATA[", "").replace("]]-->", "");
            Pebble.sendAppMessage({"0":currentShow});
        }
    }
    req.open('GET', 'http://www.tvo.org/schedule.xml', true);
    req.send();
});
