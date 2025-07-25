void construct_site(WiFiClient &client) {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html lang="da">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Portal? Odder!</title>
    <style>
      /* Enchanted background */
      body {
        margin: 0;
        padding: 0;
        font-family: 'Palatino Linotype','Book Antiqua','Georgia', serif;
        color: #eee;
        background: radial-gradient(circle at top, #1a1a2e, #0f0f1e) no-repeat fixed;
        background-size: cover;
      }
      h1 {
        text-align: center;
        font-size: 1.5em;
        margin: 1rem 0;
        text-shadow: 0 0 10px #fffa;
      }
      h2 {
        text-align: center;
        font-size: 1em;
        margin: 1rem 0;
        text-shadow: 0 0 10px #fffa;
      }
      table {
        width: 90%;
        margin: 1rem auto;
        border-collapse: separate;
        border-spacing: 0;
        border-radius: 8px;
        overflow: hidden;
        box-shadow: 0 0 15px rgba(0,0,0,0.5);
      }
      th, td {
        padding: 0.75rem 1rem;
        text-align: center;
        background: rgba(10, 10, 30, 0.8);
      }
      th {
        background: rgba(20, 20, 50, 0.9);
        font-size: 1em;
      }
      tr:hover td {
        background: rgba(30, 30, 60, 0.9);
      }
      select, button {
        font-size: 1em;
        padding: 0.5rem 1rem;
        margin: 0.2rem;
        border: none;
        border-radius: 5px;
        cursor: pointer;
        box-shadow: 0 0 8px rgba(255,255,255,0.2);
        transition: all 0.2s ease-in-out;
      }
      select {
        background: rgba(40, 40, 70, 0.9);
        color: #fff;
      }
      button {
        background: linear-gradient(135deg, #6b33d9, #b880f9);
        color: #fff;
        position: relative;
      }
      button:hover {
        transform: scale(1.05) rotate(-1deg);
        box-shadow: 0 0 12px #b880f9, 0 0 20px #6b33d9;
      }
      /* Sparkle animation */
      @keyframes sparkle {
        0%, 100% { opacity: 0.7; transform: translateY(0) scale(1); }
        50% { opacity: 1; transform: translateY(-2px) scale(1.1); }
      }
      button::after {
        content: '✨';
        position: absolute;
        top: -6px;
        right: -6px;
        font-size: 0.8em;
        animation: sparkle 2s infinite;
      }
      .commit-btn {
        display: block;
        margin: 2rem auto;
        font-size: 1.2em;
      }
      .mode-buttons {
        display: flex;
        justify-content: center;
        margin: 2rem auto;
        gap: 1rem;
      }
      .mode-btn {
        padding: 0.7rem 1.5rem;
        font-size: 1rem;
        border-radius: 8px;
        border: none;
        background: linear-gradient(135deg, #3e3e60, #505075);
        color: #eee;
        cursor: pointer;
        box-shadow: 0 0 10px rgba(0,0,0,0.3);
        transition: all 0.2s ease;
      }
      .mode-btn:hover {
        background: linear-gradient(135deg, #5a5a80, #7070a0);
      }
      .mode-btn.selected {
        background: linear-gradient(135deg, #8b5cf6, #c084fc);
        box-shadow: 0 0 15px #c084fc;
      }
      .color-picker-container {
        display: flex;
        align-items: center;
        justify-content: center;
        margin: 2rem auto;
        gap: 1rem;
        font-size: 1.2em;
        color: #fff;
        background: rgba(40, 30, 80, 0.3);
        padding: 1rem;
        border-radius: 12px;
        box-shadow: 0 0 12px #0004;
      }
      .color-picker-container input[type="color"] {
        width: 3rem;
        height: 3rem;
        border: none;
        padding: 0;
        background: none;
        cursor: pointer;
        box-shadow: 0 0 8px #fff8;
        border-radius: 8px;
      }
    </style>
  </head>
  <body>
    <h1>🔮 Portal Kontrolpanel 🔮</h1>
  )rawliteral";

  html += R"rawliteral(
  <div class="mode-buttons">
    <button id="mode0" onclick="setMode(0)" class="mode-btn selected">Normal</button>
    <button id="mode1" onclick="setMode(1)" class="mode-btn">Konstant Tændt</button>
  </div>
  <div class="mode-buttons">
    <button id="mode2" onclick="setMode(2)" class="mode-btn">Blink</button>
    <button id="mode3" onclick="setMode(3)" class="mode-btn">Konstant Slukket</button>
  </div>
  )rawliteral"; 

  html += "<div class=\"color-picker-container\">";
  char hexColor[8];
  sprintf(hexColor, "#%02X%02X%02X", maincolor.red, maincolor.green, maincolor.blue);

  html += "<input type='color' id='rgb' value='" + String(hexColor) + "'>";
  html += "<label> Hvid: <input type='range' id='white' min='0' max='255' value='" + String(maincolor.white) + "'></label>";
  html += "<button onclick='sendColor()'>Sæt Farve</button></div>";

  if (prevIDs[0] != "00000000") {
    html += R"rawliteral(
    <h2>Seneste RFID-Tags</h2>
    <table>
      <tr><th>#</th><th>ID</th><th>Gem</th></tr>
    )rawliteral";
    for (int i = 0; i < MAX_IDS; i++) {
      if (prevIDs[i] == "00000000") break;
      html += "<tr><td>";
      html += String(i);
      html += "</td><td>";
      html += prevIDs[i];
      html += "</td><td><button onclick=\"addCard(";
      html += String(i);
      html += ")\">Gem</button></td></tr>";
    }
    html += "</table>";
  }

  html += R"rawliteral(
    <h2>Gemte RFID-Tags</h2>
    <table>
      <tr><th>#</th><th>Kort-ID</th><th>Tilstand</th><th>Slet</th></tr>
  )rawliteral";

  const char* labels[5] = {"Ingen","Bl&aring","R&oslashd","Gr&oslashn", "Selvvalgt"};
  for (int i = 0; i < ramEntryCount; i++) {
    char idBuf[ENTRY_ID_LEN + 1];
    memcpy(idBuf, ramEntries[i].id, ENTRY_ID_LEN);
    idBuf[ENTRY_ID_LEN] = '\0';

    html += "<tr><td>";
    html += String(i);
    html += "</td><td>";
    html += String(idBuf);
    html += "</td><td><select onchange=\"editValue(";
    html += String(i);
    html += ",this.value)\">";

    for (int v = 0; v < 5; v++) {
      html += "<option value=\"";
      html += String(v);
      html += "\"";
      if (ramEntries[i].data == v) html += " selected";
      html += ">";
      html += labels[v];
      html += "</option>";
    }

    html += "</select></td><td><button onclick=\"deleteEntry(";
    html += String(i);
    html += ")\">Slet</button></td></tr>";
  }

  html += R"rawliteral(
    </table>
    <button class="commit-btn" onclick="commitChanges()">Gem til Fast Hukommelse</button>
  )rawliteral";

   html += R"rawliteral(
  <script>
    function sendRequest(path) {
      return fetch(path + '&t=' + Date.now(), { method: 'GET' });
    }
    function editValue(i, v) {
      sendRequest(`/edit?i=${i}&v=${v}`);
    }
    function deleteEntry(i) {
      sendRequest(`/delete?i=${i}`)
        .then(() => location.reload());
    }
    function addCard(i) {
      sendRequest(`/addcard?i=${i}`)
        .then(() => location.reload());
    }
    function commitChanges() {
      sendRequest('/commit')
        .then(() => alert('Ændringer er gemt!'));
    }
    window.onload = () => {
      history.replaceState({}, '', '/');
      document.getElementById("mode0").classList.add("selected");
      }

  function sendColor() {
        const rgbHex = document.getElementById("rgb").value;
        const white = document.getElementById("white").value;

        // Convert #RRGGBB to r, g, b
        const r = parseInt(rgbHex.substr(1, 2), 16);
        const g = parseInt(rgbHex.substr(3, 2), 16);
        const b = parseInt(rgbHex.substr(5, 2), 16);

        const url = `/setcolor?r=${r}&g=${g}&b=${b}&w=${white}`;
        fetch(url)
          .then(res => res.text())
          .then(text => console.log("Server response:", text));
      }

  let currentMode = 0;

  function setMode(mode) {
    if (mode === currentMode) return;
    currentMode = mode;
    // Highlight selected button
    for (let i = 0; i < 4; i++) {
      document.getElementById(`mode${i}`).classList.remove('selected');
    }
    document.getElementById(`mode${mode}`).classList.add('selected');
    // Send GET request to /force0, /force1, or /force2
    fetch(`/force?i=${mode}&t=` + Date.now(), { method: "GET" });
  }

  
  </script>
  </body>
  </html>
  )rawliteral";

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=UTF-8");
  client.print("Content-Length: ");
  client.println(html.length());
  client.println("Connection: close");
  client.println();
  client.print(html);
}
