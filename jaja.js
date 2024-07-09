var xml = createXmlHttpObject();
var speed_button = 0;
var time_hour = 0;
var time_min = 0;
var time_sec = 0;
var motor_state = "OFF";

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
  document.getElementById("RPM-SPAN").innerHTML = value;
  if (Math.abs(start_slider_speed - value) % 8 == 0) {
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

function SetTimer() {
  var xhttp = new XMLHttpRequest();
  xhttp.open(
    "PUT",
    "SET_TIMER?VALUE=" + (time_hour * 3600 + time_min * 60 + time_sec),
    true
  );
  xhttp.send();
}
function DisableTimer() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("PUT", "DISABLE_TIMER", true);
  document.getElementById("TIME-HOUR").value = "0";
  document.getElementById("TIME-MIN").value = "0";
  document.getElementById("TIME-SEC").value = "0";
  xhttp.send();
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

  xmldoc = xmlResponse.getElementsByTagName("STATE");
  message = xmldoc[0].firstChild.nodeValue;
  motor_state = message;
  if (motor_state == "ON") {
    document.getElementById("STATE-BUTTON").innerHTML = "Стоп";
    document.getElementById("TIME-BUTTON").disabled = true;
    document.getElementById("TIME-DIS-BUTTON").disabled = true;
  } else {
    document.getElementById("STATE-BUTTON").innerHTML = "Старт";
    document.getElementById("TIME-BUTTON").disabled = false;
    document.getElementById("TIME-DIS-BUTTON").disabled = false;
  }
}

function process() {
  if (xml.readyState == 0 || xml.readyState == 4) {
    xml.open("PUT", "xml", true);
    xml.onreadystatechange = response;
    xml.send();
  }
  setTimeout("process()", 50);
}
