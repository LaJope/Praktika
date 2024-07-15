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
