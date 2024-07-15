const char PAGE_MAIN[] PROGMEM = R"=====(<!DOCTYPE html>

<!DOCTYPE html>
<html lang="ru" class="js-focus-visible">
  <head>
    <meta charset="UTF-8" />
    <meta
      name="viewport"
      content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no"
    />
    <title>Панель управления барабанной установкой</title>
    <style>html {
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
.time-control {
  display: flex;
  justify-content: center;
  margin-top: 10px;
  column-gap: 20px;
}
.time-button {
  background-color: white;
  border: solid 2px black;
  border-radius: 10px;
  padding: 4px 8px;
  font-weight: 600;
}
.time-left {
  margin-top: 10px;
}

    </style>
    <script type="text/javascript">
    var Socket;
var MotorSpeed = 0;
var StartRPMSliderValue = 0;

var TimeHour = 0;
var TimeMin = 0;
var TimeSec = 0;

function setDisabled(id, value) {
  let element = document.getElementById(id);
  if (element) element.disabled = value;
}

function init() {
  Socket = new WebSocket("ws://" + window.location.hostname + ":81/");
  Socket.onmessage = function (event) {
    processCommand(event);
  };
}

function StateButton() {
  var message = {
    type: "CHANGE-STATE",
  };
  Socket.send(JSON.stringify(message));
  console.log("STATE");
}

function ReverseButton() {
  setDisabled("REVERSE-BUTTON", true);
  var message = {
    type: "CHANGE-ROTATION",
  };
  Socket.send(JSON.stringify(message));
  console.log("REVERSE");
}

function checkSpeed(value) {
  if (value > 255) return 255;
  if (value < 0) return 0;
  return value;
}

function UpdateRPMInput(value) {
  value = checkSpeed(value);
  MotorSpeed = value;
  document.getElementById("RPM-INPUT").value = value;
  console.log("UPDATE INPUT" + value);
}

function UpdateRPMInputButton() {
  var message = {
    type: "CHANGE-SPEED",
    speed: MotorSpeed,
  };
  Socket.send(JSON.stringify(message));
  document.getElementById("RPM-SLIDE").value = MotorSpeed;
  document.getElementById("RPM-INPUT").value = MotorSpeed;
  document.getElementById("RPM-SPAN").innerHTML = MotorSpeed;
}

function RPMSliderPointerDown(value) {
  StartRPMSliderValue = checkSpeed(value);
}
function RPMSliderPointerUp(value) {
  MotorSpeed = checkSpeed(value);
  UpdateRPMInputButton();
}
function RPMSliderPointerMove(value) {
  value = checkSpeed(value);
  document.getElementById("RPM-SPAN").innerHTML = value;
  if (Math.abs(value - StartRPMSliderValue) % 10 == 0) {
    MotorSpeed = value;
    UpdateRPMInputButton();
  }
}

function UpdateHour(value) {
  TimeHour = value;
  console.log("HOUR " + value);
}
function UpdateMin(value) {
  TimeMin = value;
  console.log("MIN " + value);
}
function UpdateSec(value) {
  TimeSec = value;
  console.log("SEC " + value);
}
function UpdateTimerButton() {
  setDisabled("TIME-HOUR", true);
  setDisabled("TIME-MIN", true);
  setDisabled("TIME-SEC", true);
  setDisabled("TIME-BUTTON", true);
  setDisabled("TIME-DIS-BUTTON", false);
  var message = {
    type: "UPDATE-TIMER",
    hours: TimeHour,
    minutes: TimeMin,
    seconds: TimeSec,
  };
  Socket.send(JSON.stringify(message));
  console.log("SET TIMER " + TimeHour + ":" + TimeMin + ":" + TimeSec);
}
function DisableTimerButton() {
  setDisabled("TIME-HOUR", false);
  setDisabled("TIME-MIN", false);
  setDisabled("TIME-SEC", false);
  setDisabled("TIME-BUTTON", false);
  setDisabled("TIME-DIS-BUTTON", true);
  document.getElementById("TIME-HOUR").value = 0;
  document.getElementById("TIME-MIN").value = 0;
  document.getElementById("TIME-SEC").value = 0;
  TimeHour = 0;
  TimeMin = 0;
  TimeSec = 0;
  var message = {
    type: "DISABLE-TIMER",
  };
  Socket.send(JSON.stringify(message));
  console.log("DISABLE TIMER");
}

function processCommand(event) {
  let jsonMessage = JSON.parse(event.data);
  document.getElementById("RPM").innerHTML = jsonMessage.RPM;
  document.getElementById("VOLT").innerHTML = jsonMessage.VOLT;
  document.getElementById("CURR").innerHTML = jsonMessage.CURR;
  document.getElementById("STATE-BUTTON").innerHTML = (jsonMessage.MOTOR_STATE === "ON" ? "Стоп" : "Старт");
  let timerState = jsonMessage.TIMER_STATE === "ON";
  setDisabled("TIME-HOUR", timerState);
  setDisabled("TIME-MIN", timerState);
  setDisabled("TIME-SEC", timerState);
  setDisabled("TIME-BUTTON", timerState);
  setDisabled("TIME-DIS-BUTTON", !timerState);
  document.getElementById("TIME-LEFT").innerHTML = jsonMessage.TIME_LEFT;
}

window.onload = function (event) {
  init();
};


</script>
  </head>

  <body style="background-color: #eeeeee">
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
        <div class="top-row">
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
              <div class="category-text">
                Скорость вращения барабана (<span id="RPM-SPAN">0</span>)
              </div>
              <input
                type="range"
                class="rpm-slider"
                min="0"
                max="255"
                value="0"
                width="0%"
                id="RPM-SLIDE"
                onpointerdown="RPMSliderPointerDown(this.value)"
                onpointerup="RPMSliderPointerUp(this.value)"
                onpointermove="RPMSliderPointerMove(this.value)"
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
              <button type="button" class="rpm-button" id="RPM-INPUT-BUTTON" onclick="UpdateRPMInputButton()">
                Обновить скорость
              </button>
            </div>
            <div class="param-div">
              <div class="category-text">Время вращения барабана</div>
              <input
                type="number"
                class="time-input"
                min="0"
                max="17"
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
              <div class="time-control">
                <button
                  type="button"
                  class="time-button"
                  id="TIME-BUTTON"
                  onclick="UpdateTimerButton()"
                >
                  Задать время
                </button>
                <button
                  type="button"
                  class="time-button"
                  id="TIME-DIS-BUTTON"
                  onclick="DisableTimerButton()"
                >
                  Сбросить
                </button>
              </div>
              <div class="time-left">
                Осталось времени: <span id="TIME-LEFT">00:00:00</span>
              </div>
            </div>
          </div>
        </div>
        <div class="category">
          <div class="category-title">Графики</div>
          <div class="chart" id="SPEED-CHART"></div>
        </div>
      </div>
    </main>
    <footer>
      <div class="footer control-panel fixed-bottom">
        <div>
          <div class="control-el">
            <button type="button" class="control-button" id="STATE-BUTTON" onclick="StateButton()">
              Старт
            </button>
            <button type="button" class="control-button" id="REVERSE-BUTTON" onclick="ReverseButton()">
              Смена направления вращения
            </button>
          </div>
        </div>
      </div>
    </footer>
  </body>
</html>

)=====";
