// global variable visible to all java functions
var xmlHttp = createXmlHttpObject();

// function to create XML object
function createXmlHttpObject() {
  if (window.XMLHttpRequest) {
    xmlHttp = new XMLHttpRequest();
  } else {
    xmlHttp = new ActiveXObject("Microsoft.XMLHTTP");
  }
  return xmlHttp;
}

// function to handle button press from HTML code above
// and send a processing string back to server
// this processing string is use in the .on method
function ButtonPress0() {
  var xhttp = new XMLHttpRequest();
  var message;
  // if you want to handle an immediate reply (like status from the ESP
  // handling of the button press use this code
  // since this button status from the ESP is in the main XML code
  // we don't need this
  // remember that if you want immediate processing feedbac you must send it
  // in the ESP handling function and here
  /*
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      message = this.responseText;
      // update some HTML data
    }
  }
  */

  xhttp.open("PUT", "BUTTON_0", false);
  xhttp.send();
}

// function to handle button press from HTML code above
// and send a processing string back to server
// this processing string is use in the .on method
function ButtonPress1() {
  var xhttp = new XMLHttpRequest();
  /*
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("button1").innerHTML = this.responseText;
    }
  }
  */
  xhttp.open("PUT", "BUTTON_1", false);
  xhttp.send();
}

function UpdateSlider(value) {
  var xhttp = new XMLHttpRequest();
  // this time i want immediate feedback to the fan speed
  // yea yea yea i realize i'm computing fan speed but the point
  // is how to give immediate feedback
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      // update the web based on reply from  ESP
      document.getElementById("fanrpm").innerHTML = this.responseText;
    }
  };
  // this syntax is really weird the ? is a delimiter
  // first arg UPDATE_SLIDER is use in the .on method
  // server.on("/UPDATE_SLIDER", UpdateSlider);
  // then the second arg VALUE is use in the processing function
  // String t_state = server.arg("VALUE");
  // then + the controls value property
  xhttp.open("PUT", "UPDATE_SLIDER?VALUE=" + value, true);
  xhttp.send();
}

// function to handle the response from the ESP
function response() {
  var message;
  var barwidth;
  var currentsensor;
  var xmlResponse;
  var xmldoc;
  var dt = new Date();
  var color = "#e8e8e8";

  // get the xml stream
  xmlResponse = xmlHttp.responseXML;

  // get host date and time
  document.getElementById("time").innerHTML = dt.toLocaleTimeString();
  document.getElementById("date").innerHTML = dt.toLocaleDateString();

  // A0
  xmldoc = xmlResponse.getElementsByTagName("B0"); //bits for A0
  message = xmldoc[0].firstChild.nodeValue;

  if (message > 2048) {
    color = "#aa0000";
  } else {
    color = "#0000aa";
  }

  barwidth = message / 40.95;
  document.getElementById("b0").innerHTML = message;
  document.getElementById("b0").style.width = barwidth + "%";
  // if you want to use global color set above in <style> section
  // other wise uncomment and let the value dictate the color
  //document.getElementById("b0").style.backgroundColor=color;
  //document.getElementById("b0").style.borderRadius="5px";

  xmldoc = xmlResponse.getElementsByTagName("V0"); //volts for A0
  message = xmldoc[0].firstChild.nodeValue;
  document.getElementById("v0").innerHTML = message;
  document.getElementById("v0").style.width = barwidth + "%";
  // you can set color dynamically, maybe blue below a value, red above
  document.getElementById("v0").style.backgroundColor = color;
  //document.getElementById("v0").style.borderRadius="5px";

  // A1
  xmldoc = xmlResponse.getElementsByTagName("B1");
  message = xmldoc[0].firstChild.nodeValue;
  if (message > 2048) {
    color = "#aa0000";
  } else {
    color = "#0000aa";
  }
  document.getElementById("b1").innerHTML = message;
  width = message / 40.95;
  document.getElementById("b1").style.width = width + "%";
  document.getElementById("b1").style.backgroundColor = color;
  //document.getElementById("b1").style.borderRadius="5px";

  xmldoc = xmlResponse.getElementsByTagName("V1");
  message = xmldoc[0].firstChild.nodeValue;
  document.getElementById("v1").innerHTML = message;
  document.getElementById("v1").style.width = width + "%";
  document.getElementById("v1").style.backgroundColor = color;
  //document.getElementById("v1").style.borderRadius="5px";

  xmldoc = xmlResponse.getElementsByTagName("LED");
  message = xmldoc[0].firstChild.nodeValue;

  if (message == 0) {
    document.getElementById("btn0").innerHTML = "Turn ON";
  } else {
    document.getElementById("btn0").innerHTML = "Turn OFF";
  }

  xmldoc = xmlResponse.getElementsByTagName("SWITCH");
  message = xmldoc[0].firstChild.nodeValue;
  document.getElementById("switch").style.backgroundColor = "rgb(200,200,200)";
  // update the text in the table
  if (message == 0) {
    document.getElementById("switch").innerHTML = "Switch is OFF";
    document.getElementById("btn1").innerHTML = "Turn ON";
    document.getElementById("switch").style.color = "#0000AA";
  } else {
    document.getElementById("switch").innerHTML = "Switch is ON";
    document.getElementById("btn1").innerHTML = "Turn OFF";
    document.getElementById("switch").style.color = "#00AA00";
  }
}

// general processing code for the web page to ask for an XML steam
// this is actually why you need to keep sending data to the page to
// force this call with stuff like this
// server.on("/xml", SendXML);
// otherwise the page will not request XML from the ESP, and updates will not happen
function process() {
  if (xmlHttp.readyState == 0 || xmlHttp.readyState == 4) {
    xmlHttp.open("PUT", "xml", true);
    xmlHttp.onreadystatechange = response;
    xmlHttp.send(null);
  }
  // you may have to play with this value, big pages need more porcessing time, and hence
  // a longer timeout
  setTimeout("process()", 100);
}
