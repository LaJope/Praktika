var xml = createXmlHttpObject();
var speed_button = 0;
var time_hour = 0;
var time_min = 0;
var time_sec = 0;

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
      document.getElementById("STATE-BUTTON").innerHTML =
        this.responseText == "ON" ? "Старт" : "Стоп";
    }
  };
  xhttp.open("PUT", "STATE_BUTTON", false);
  xhttp.send();
}

function ReverseButton() {
  var xhttp = new XMLHttpRequest();
  document.getElementById("REVERSE-BUTTON").disabled = true;
  xhttp.onreadystatechange = function () {
    if (
      this.readyState == 4 &&
      this.status == 200 &&
      this.responseText === "DONE"
    )
      document.getElementById("REVERSE-BUTTON").disabled = false;
  };
  xhttp.open("PUT", "REVERSE_BUTTON", false);
  xhttp.send();
}

function UpdateRPM(value) {
  var xhttp = new XMLHttpRequest();
  UpdateRPMInput(value);
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      speed_button = this.responseText;
      document.getElementById("RPM").innerHTML = this.responseText;
      document.getElementById("RPM-SLIDE").value = Number(this.responseText);
      document.getElementById("RPM-INPUT").value = Number(this.responseText);
    }
  };
  xhttp.open("PUT", "UPDATE_RPM?VALUE=" + speed_button, true);
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
function UpdateHour(value) {
  time_hour = value;
}
function UpdateMin(value) {
  time_min = value;
}
function UpdateSec(value) {
  time_sec = value;
}

function response() {
  var dt = new Date();
  document.getElementById("TIME").innerHTML = dt.toLocaleTimeString();
  document.getElementById("DATE").innerHTML = dt.toLocaleDateString();

  var xmlResponse = xml.responseXML;
  var xmldoc;
  var message;

  xmldoc = xmlResponse.getElementsByTagName("RPM");
  message = xmldoc[0].firstChild.nodeValue;
  document.getElementById("RPM").innerHTML = message;

  xmldoc = xmlResponse.getElementsByTagName("VOLT");
  message = xmldoc[0].firstChild.nodeValue;
  document.getElementById("VOLT").innerHTML = message;

  xmldoc = xmlResponse.getElementsByTagName("CURR");
  message = xmldoc[0].firstChild.nodeValue;
  document.getElementById("CURR").innerHTML = message;
}

function process() {
  if (xml.readyState == 0 || xml.readyState == 4) {
    xml.open("PUT", "xml", true);
    xml.onreadystatechange = response;
    xml.send(null);
  }
  setTimeout("process()", 100);
}
