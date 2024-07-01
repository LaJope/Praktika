var xml = createXmlHttpObject();

function createXmlHttpObject() {
  if (window.XMLHttpRequest) {
    xml = new XMLHttpRequest();
  } else {
    xml = new ActiveXObject("Microsoft.XMLHTTP");
  }
  return xml;
}

function StartButton() {
  var xhttp = new XMLHttpRequest();
  // var message;
  // xhttp.onreadystatechange = function() {
  //   if (this.readyState == 4 && this.status == 200) {
  //     message = this.responseText;
  //     // update some HTML data
  //   }
  // }

  xhttp.open("PUT", "BUTTON_0", false);
  xhttp.send();
}

function StopButton() {
  var xhttp = new XMLHttpRequest();
  // var message;
  // xhttp.onreadystatechange = function() {
  //   if (this.readyState == 4 && this.status == 200) {
  //     message = this.responseText;
  //     // update some HTML data
  //   }
  // }

  xhttp.open("PUT", "STOP_BUTTON", false);
  xhttp.send();
}

function ReverseButton() {
  var xhttp = new XMLHttpRequest();
  // var message;
  // xhttp.onreadystatechange = function() {
  //   if (this.readyState == 4 && this.status == 200) {
  //     message = this.responseText;
  //     // update some HTML data
  //   }
  // }

  xhttp.open("PUT", "REVERSE_BUTTON", false);
  xhttp.send();
}

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
