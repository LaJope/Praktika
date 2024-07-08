var xml = createXmlHttpObject();
var speed_button = 0;
var time_hour = 0;
var time_min = 0;
var time_sec = 0;

function sleep(ms) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}

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
    sleep(100);
  };
  xhttp.open("PUT", "REVERSE_BUTTON", false);
  xhttp.send();
}

function checkSpeed(value) {
  if (value < 0) value = 0;
  if (value > 255) value = 255;
  return value;
}

var start_slider_speed = 0;
function SliderMouseDown(value) {
  start_slider_speed = checkSpeed(value);
}
function SliderMouseUp(value) {
  UpdateRPM(checkSpeed(value));
}
function UpdateRPMSlider(value) {
  value = checkSpeed(value);
  if (Math.abs(start_slider_speed - value) % 10 == 0) {
    UpdateRPM(value);
  }
}

function UpdateRPM(value) {
  var xhttp = new XMLHttpRequest();
  UpdateRPMInput(value);
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      speed_button = Number(this.responseText);
      document.getElementById("RPM-INPUT").value = speed_button;
    }
  };
  xhttp.open("PUT", "UPDATE_RPM?VALUE=" + speed_button, true);
  xhttp.send();
}

function UpdateRPMInput(value) {
  speed_button = checkSpeed(value);
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
  UpdateRPMOnRefresh(message);

  xmldoc = xmlResponse.getElementsByTagName("VOLT");
  message = xmldoc[0].firstChild.nodeValue;
  document.getElementById("VOLT").innerHTML = message;

  xmldoc = xmlResponse.getElementsByTagName("CURR");
  message = xmldoc[0].firstChild.nodeValue;
  document.getElementById("CURR").innerHTML = message;
}

var UpdateRPMOnRefresh = (function () {
  var executed = false;
  return function (message) {
    if (!executed) {
      executed = true;
      document.getElementById("RPM-SLIDER").innerHTML = message;
      document.getElementById("RPM-INPUT").innerHTML = message;
    }
  };
})();

function process() {
  if (xml.readyState == 0 || xml.readyState == 4) {
    xml.open("PUT", "xml", true);
    xml.onreadystatechange = response;
    xml.send();
  }
  setTimeout("process()", 50);
}
