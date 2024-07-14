var xml = createXmlHttpObject();
var speed_button = 0;
var motor_on = false;

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
  xhttp.open("POST", "STATE_BUTTON", false);
  xhttp.send();
}

function ReverseButton() {
  var xhttp = new XMLHttpRequest();
  document.getElementById("REVERSE-BUTTON").disabled = true;
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200)
      if (this.responseText == "DONE")
        document.getElementById("REVERSE-BUTTON").disabled = false;
  };
  xhttp.open("POST", "REVERSE_BUTTON", false);
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
  xhttp.open("PUT", "UPDATE_RPM?SPEED=" + speed_button, false);
  xhttp.send();
}

function UpdateRPMInput(value) {
  speed_button = checkSpeed(value);
}
function UpdateRPMButton() {
  UpdateRPM(speed_button);
  document.getElementById("RPM-SLIDE").value = speed_button;
  document.getElementById("RPM-SPAN").innerHTML = speed_button;
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
  xhttp.open("PUT", "SET_TIMER_HOUR?HOUR=" + time_hour, false);
  xhttp.send();
  xhttp = new XMLHttpRequest();
  xhttp.open("PUT", "SET_TIMER_MIN?MIN=" + time_min, false);
  xhttp.send();
  xhttp = new XMLHttpRequest();
  xhttp.open("PUT", "SET_TIMER_SEC?SEC=" + time_sec, false);
  xhttp.send();
  xhttp = new XMLHttpRequest();
  xhttp.open("POST", "SET_TIMER", false);
  xhttp.send();

  document.getElementById("TIME-HOUR").disabled = true;
  document.getElementById("TIME-MIN").disabled = true;
  document.getElementById("TIME-SEC").disabled = true;
  document.getElementById("TIME-BUTTON").disabled = true;
}
function DisableTimer() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "DISABLE_TIMER", false);
  xhttp.send();

  document.getElementById("TIME-HOUR").value = 0;
  document.getElementById("TIME-MIN").value = 0;
  document.getElementById("TIME-SEC").value = 0;
  document.getElementById("TIME-HOUR").disabled = false;
  document.getElementById("TIME-MIN").disabled = false;
  document.getElementById("TIME-SEC").disabled = false;
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
  motor_on = message === "ON";
  if (motor_on) {
    document.getElementById("STATE-BUTTON").innerHTML = "Стоп";
    document.getElementById("TIME-BUTTON").disabled = true;
    document.getElementById("TIME-DIS-BUTTON").disabled = true;
    document.getElementById("TIME-HOUR").disabled = true;
    document.getElementById("TIME-MIN").disabled = true;
    document.getElementById("TIME-SEC").disabled = true;
  } else {
    document.getElementById("STATE-BUTTON").innerHTML = "Старт";
    document.getElementById("TIME-BUTTON").disabled = false;
    document.getElementById("TIME-DIS-BUTTON").disabled = false;
    document.getElementById("TIME-HOUR").disabled = false;
    document.getElementById("TIME-MIN").disabled = false;
    document.getElementById("TIME-SEC").disabled = false;
  }

  xmldoc = xmlResponse.getElementsByTagName("TIME-LEFT");
  message = xmldoc[0].firstChild.nodeValue;
  document.getElementById("TIME-LEFT").innerHTML = message;
}

function process() {
  if (xml.readyState == 0 || xml.readyState == 4) {
    xml.open("GET", "xml", true);
    xml.onreadystatechange = response;
    xml.send();
  }
  setTimeout("process()", 200);
  setTimeout("UpdateGraph()", 200);
}

let chartT = new Highcharts.Chart({
  chart: { renderTo: "SPEED-CHART" },
  title: { text: "BME280 Temperature" },
  series: [
    {
      showInLegend: false,
      data: [],
    },
  ],
  plotOptions: {
    line: { animation: false, dataLabels: { enabled: true } },
    series: { color: "#059e8a" },
  },
  xAxis: { type: "datetime", dateTimeLabelFormats: { second: "%H:%M:%S" } },
  yAxis: { title: { text: "Temperature (Celsius)" } },
  credits: { enabled: false },
});

function UpdateGraph() {
  chartT.series[0].addPoint([1, 2]);
  // chartT.series[0].addPoint([1, 2], true, true, true);
  // var xhttp = new XMLHttpRequest();
  // xhttp.onreadystatechange = function () {
  //   if (this.readyState == 4 && this.status == 200) {
  //     var x = new Date().getTime(),
  //       y = parseFloat(this.responseText);
  //     if (chartT.series[0].data.length > 40) {
  //       chartT.series[0].addPoint([x, y], true, true, true);
  //     } else {
  //       chartT.series[0].addPoint([x, y], true, false, true);
  //     }
  //   }
  // };
  // xhttp.open("GET", "/temperature", true);
  // xhttp.send();
}
