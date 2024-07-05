var xml = createXmlHttpObject();
var speed_button = 0;

function createXmlHttpObject() {
  if (window.XMLHttpRequest) {
    xml = new XMLHttpRequest();
  } else {
    xml = new ActiveXObject("Microsoft.XMLHTTP");
  }
  return xml;
}

function StateButton() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("STATE-BUTTON").innerHTML
        = this.responseText == "ON" ? "Старт" : "Стоп";
    }
  };
  xhttp.open("PUT", "STATE_BUTTON", false);
  xhttp.send();
}

function ReverseButton() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("PUT", "REVERSE_BUTTON", false);
  xhttp.send();
}

function UpdateRPM(value) {
  var xhttp = new XMLHttpRequest();
  if (value < 0) value = 0;
  if (value > 255) value = 255;
  speed_button = value;
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("RPM").innerHTML = this.responseText;
    }
  };
  document.getElementById("RPM").innerHTML = value;
  document.getElementById("RPM-BUTT").value = value;
  document.getElementById("RPM-SLIDE").value = value;
  xhttp.open("PUT", "UPDATE_RPM?VALUE=" + value, true);
  xhttp.send();
}

function UpdateRPMInput(value) {
  if (value < 0) value = 0;
  if (value > 255) value = 255;
  speed_button = value;
}

function UpdateRPMButton() {
  UpdateRPM(speed_button);
}

function response() {
  var message;
  var barwidth;
  var currentsensor;
  var xmlResponse;
  var xmldoc;
  var dt = new Date();
  var color = "#e8e8e8";
  xmlResponse = xml.responseXML;
  document.getElementById("TIME").innerHTML = dt.toLocaleTimeString();
  document.getElementById("DATE").innerHTML = dt.toLocaleDateString();
  xmldoc = xmlResponse.getElementsByTagName("RPM");
  message = xmldoc[0].firstChild.nodeValue;
  document.getElementById("RPM").innerHTML = message;
  xmldoc = xmlResponse.getElementsByTagName("CURR");
  message = xmldoc[0].firstChild.nodeValue;
  document.getElementById("CURR").innerHTML = message;
  xmldoc = xmlResponse.getElementsByTagName("DIFF");
  message = xmldoc[0].firstChild.nodeValue;
  document.getElementById("DIFF").innerHTML = message;
}

function process() {
  if (xml.readyState == 0 || xml.readyState == 4) {
    xml.open("PUT", "xml", true);
    xml.onreadystatechange = response;
    xml.send(null);
  }
  setTimeout("process()", 100);
}
