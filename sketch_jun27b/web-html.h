const char PAGE_MAIN[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="ru" class="js-focus-visible">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=2.0" />
    <title>Панель управления барабанной установкой</title>
    <style>
    html {
  font-family: Arial, Helvetica, sans-serif;
}

.footer {
  width: 100%;
  display: flex;
  flex-wrap: wrap;
  justify-content: center;
  gap: 10px 60px;
  padding: 10px;
  margin-bottom: 0px;
  border-top: solid 2px;
  border-radius: 10px;
  background-color: blanchedalmond;
}

.header {
  width: 100%;
  display: flex;
  flex-wrap: wrap;
  justify-content: center;
  gap: 10px 60px;
  padding: 10px;
  border-bottom: solid 2px;
  border-radius: 10px;
  background-color: blanchedalmond;
}

.fixed-top {
  position: fixed;
  top: 0;
  right: 0;
  left: 0;
  z-index: 1030;
}

.header-title {
  font-size: 35px;
  font-weight: 600;
  text-wrap: inherit;
}

.time-and-date {
  display: grid;
  grid-template: 2 2;
  gap: 0px 12px;
  font-size: 20px;
  font-weight: 500;
  text-align: center;
}

.heading {
  grid-row: 1;
  text-wrap: nowrap;
}

.header-data {
  grid-row: 2;
}

.container {
  margin-top: 170px;
  margin-bottom: 200px;
  overflow: auto;
  display: flex;
  flex-wrap: wrap;
  justify-content: center;
  column-gap: 40px;
}

.category {
  display: grid;
  justify-content: center;
  text-align: center;
  margin-bottom: 40px;
}

.category-title {
  font-size: 30px;
  font-weight: 600;
}

.param-div,
table {
  border: solid 2px;
  border-radius: 10px;
  text-align: center;
}

th {
  padding: 5px;
  font-weight: 600;
  background-color: blanchedalmond;
  border: 1px solid;
  border-radius: 10px;
}

td {
  padding: 5px;
  background-color: blanchedalmond;
  border: 1px solid;
  border-radius: 10px;
}

.param-div {
  padding: 10px;
  background-color: blanchedalmond;
}

.rpm-slider {
  display: block;
  width: 100%;
  background: red;
}

.category-text {
  margin-bottom: 5px;
}

.control-panel {
  display: grid;
  justify-content: center;
  text-align: center;
  margin-bottom: 0px;
}

.control-el {
  padding: 10px 0px;
}

.control-button {
  width: 80%;
  padding: 6px;
  margin: 6px;
  border-radius: 10px;
  font-size: 20px;
  font-weight: 700;
  background-color: white;
  border-radius: 10px;
  border: solid 2px;
}

.control-button:disabled,
.control-button[disabled] {
  border: 1px solid #999999;
  background-color: #cccccc;
  color: #666666;
}

.fixed-bottom {
  position: fixed;
  bottom: 0px;
  z-index: 1030;
}

.time-input {
  display: inline-block;
  width: 60px;
  padding: 2px;
  padding-left: 6px;
  border: solid 2px;
  border-radius: 10px;
}

.rpm-input {
  display: inline-block;
  width: 60px;
  padding: 2px;
  padding-left: 6px;
  border: solid 2px;
  border-radius: 10px;
}

.rpm-button {
  display: inline-block;
  width: 60%;
  padding: 6px;
  margin: 6px;
  border-radius: 10px;
  font-size: 15px;
  font-weight: 700;
  background-color: white;
  border-radius: 10px;
  border: solid 2px;
}
    </style>
    <script type="text/javascript">
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

var start_slider_speed = 0;
function SliderMouseDown(value) {
  start_slider_speed = value;
}
function SliderMouseUp(value) {
  UpdateRPM(value);
}
function UpdateRPMSlider(value) {
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

  </script>
  </head>

  <body style="background-color: #eeeeee" onload="process()">
    <header>
      <div class="header fixed-top">
        <div class="header-title">Панель управления</div>
        <div class="time-and-date">
          <div class="heading">Дата</div>
          <div class="header-data" id="DATE">дд/мм/гггг</div>
          <div class="heading">Время</div>
          <div class="header-data" id="TIME">00:00:00</div>
        </div>
      </div>
    </header>
    <main>
      <div class="container">
        <div class="category">
          <div class="category-title">Показания датчиков</div>
          <div>
            <table>
              <tr>
                <th colspan="1" class="heading" style="font-size: 20px">
                  Измерение
                </th>
                <th colspan="1" class="heading" style="font-size: 20px">
                  Показание датчика
                </th>
              </tr>
              <tr>
                <td class="table-text">RPM (от 0 до 255)</td>
                <td class="table-data" id="RPM">0</td>
              </tr>
              <tr>
                <td class="table-text">Сила тока</td>
                <td class="table-data" id="CURR">0</td>
              </tr>
              <tr>
                <td class="table-text">Напряжение</td>
                <td class="table-data" id="VOLT">0</td>
              </tr>
            </table>
          </div>
        </div>
        <div class="category" style="gap: 10px 0px">
          <div class="category-title">Задание параметров</div>
          <div class="param-div">
            <div class="category-text">Скорость вращения барабана</div>
            <input
              type="range"
              class="rpm-slider"
              min="0"
              max="255"
              value="0"
              width="0%"
              id="RPM-SLIDE"
              onmousedown="SliderMouseDown(this.value)"
              onmouseup="SliderMouseUp(this.value)"
              oninput="UpdateRPMSlider(this.value)"
            />
            <input
              type="number"
              class="rpm-input"
              min="0"
              max="255"
              value="0"
              id="RPM-INPUT"
              oninput="UpdateRPMInput(this.value)"
            />
            <button
              type="button"
              class="rpm-button"
              onclick="UpdateRPMButton()"
            >
              Обновить скорость
            </button>
          </div>
          <div class="param-div">
            <div class="category-text">Время вращения барабана</div>
            <input
              type="number"
              class="time-input"
              min="0"
              max="23"
              value="0"
              placeholder="час"
              id="TIME-HOUR"
              oninput="UpdateHour(this.value)"
            />
            <span>:</span>
            <input
              type="number"
              class="time-input"
              min="0"
              max="59"
              value="0"
              placeholder="мин"
              id="TIME-MIN"
              oninput="UpdateMin(this.value)"
            />
            <span>:</span>
            <input
              type="number"
              class="time-input"
              min="0"
              max="59"
              value="0"
              placeholder="сек"
              id="TIME-SEC"
              oninput="UpdateSec(this.value)"
            />
          </div>
        </div>
      </div>
    </main>
    <footer>
      <div class="footer control-panel fixed-bottom">
        <div>
          <div class="control-el">
            <button
              type="button"
              class="control-button"
              id="STATE-BUTTON"
              onclick="StateButton()"
            >
              Старт
            </button>
            <button
              type="button"
              class="control-button"
              id="REVERSE-BUTTON"
              onclick="ReverseButton()"
            >
              Смена направления вращения
            </button>
          </div>
        </div>
      </div>
    </footer>
  </body>
</html>
)=====";
