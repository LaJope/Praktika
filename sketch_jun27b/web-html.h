const char PAGE_MAIN[] PROGMEM = R"=====(


<!DOCTYPE html>
<html lang="ru">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=2.0" />
    <title>Панель управления барабанной установкой</title>
    <style>
      html {
        font-family: Arial, Helvetica, sans-serif;
      }
      .header {
        display: flex;
        flex-wrap: wrap;
        justify-content: center;
        /* justify-content: space-between; */
        gap: 10px 60px;
        padding: 10px;
        margin-bottom: 20px;
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
        position: relative;
        top: 170px;
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
      table {
        border: solid 2px;
        border-radius: 10px;
      }
      th {
        padding: 5px;
        font-weight: 600;
      }
      td {
        padding: 5px;
      }

      .control-el {
        /* border: solid 2px;
    border-radius: 10px; */
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
      }

      .fixed-bottom {
        position: fixed;
        bottom: 0px;
        z-index: 1030;
      }
      .category.fixed-bottom {
        width: 100%;
        border-top: solid 2px;
        border-radius: 10px;
        background-color: blanchedalmond;
      }
    </style>

    <script type="text/javascript">
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

        xhttp.open("PUT", "BUTTON_1", false);
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
        document.getElementById("switch").style.backgroundColor =
          "rgb(200,200,200)";
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
    </script>
  </head>

  <body>
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
              <colgroup>
                <col
                  span="1"
                  style="background-color: blanchedalmond; width: 50%"
                />
                <col
                  span="1"
                  style="background-color: blanchedalmond; width: 50%"
                />
              </colgroup>
              <col span="2" />
              <col span="2" />
              <tr>
                <th colspan="1" class="heading" style="font-size: 20px">
                  Измерение
                </th>
                <th colspan="1" class="heading" style="font-size: 20px">
                  Показание датчика
                </th>
              </tr>
              <tr>
                <td class="table-text">RPM</td>
                <!-- <td class="table-data" id="RPM">55</td> -->
                <td class="table-data" id="B0">55</td>
              </tr>
              <tr>
                <td class="table-text">Сила тока</td>
                <!-- <td class="table-data" id="CURR">55</td> -->
                <td class="table-data" id="B1">55</td>
              </tr>
              <tr>
                <td class="table-text">Напряжение</td>
                <td class="table-data" id="DIFF">55</td>
              </tr>
            </table>
          </div>
        </div>
        <div class="category">
          <div class="category-title">Задание параметров</div>
          <div>PLACEHOLDER</div>
        </div>
        <div class="category fixed-bottom">
          <div class="category-title">Управление</div>
          <div>
            <div class="control-el">
              <button
                type="button"
                class="control-button"
                id="START-BUTTON"
                onclick="StartButton()"
              >
                Старт
              </button>
              <button
                type="button"
                class="control-button"
                id="STOP-BUTTON"
                onclick="StopButton()"
              >
                Стоп
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
      </div>
    </main>

    <footer></footer>
  </body>
</html>



)=====";