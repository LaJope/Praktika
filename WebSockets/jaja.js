var Socket;
var MotorSpeed = 0;

function init() {
  Socket = new WebSocket("ws://" + window.location.hostname + ":81/");
  Socket.onmessage = function (event) {
    processCommand(event);
  };

  document
    .getElementById("STATE-BUTTON")
    .addEventListener("click", StateButton);
  document
    .getElementById("REVERSE-BUTTON")
    .addEventListener("click", ReverseButton);
  document
    .getElementById("RPM-INPUT")
    .addEventListener("input", UpdateRPMInput);
  document
    .getElementById("RPM-INPUT-BUTTON")
    .addEventListener("click", UpdateRPMInputButton);
}

function StateButton() {
  var message = { BUTTON: "STATE" };
  Socket.send(JSON.stringify(message));
}

function ReverseButton() {}

function checkSpeed(value) {
  if (value > 255) return 255;
  if (value < 0) return 0;
  return value;
}

function UpdateRPMInput(event) {
  MotorSpeed = checkSpeed(event.target.value);
}

function UpdateRPMInputButton() {
  var message = { SPEED: MotorSpeed };
  Socket.send(JSON.stringify(message));
  document.getElementById("RPM-SLIDE").value = MotorSpeed;
  document.getElementById("RPM-SPAN").innerHTML = MotorSpeed;
}

function processCommand(event) {
  let jsonMessage = JSON.parse(event.data);
  document.getElementById("RPM").innerHTML = jsonMessage.RPM;
  document.getElementById("CURR").innerHTML = jsonMessage.CURR;
}

window.onload = function (event) {
  init();
};
