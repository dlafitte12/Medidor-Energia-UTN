/**
 * @file index_h.h
 * @brief HTML and CSS content for the Web HMI
 * @details Stored in PROGMEM to save RAM.
 */

 // --- WEB INTERFACE (Stored in Flash Memory - PROGMEM) ---
// Dashboard design optimized for mobile and desktop responsiveness

#ifndef INDEX_H_H
#define INDEX_H_H

#include <Arduino.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Medidor de Energía - UTN</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: 'Segoe UI', sans-serif; text-align: center; background-color: #eceff1; color: #333; margin: 20px; }
    .card { background: #f5f7f9; padding: 20px; margin: 15px auto; max-width: 500px; border-radius: 12px; box-shadow: 0 4px 10px rgba(0,0,0,0.1); }
    .grid { display: grid; grid-template-columns: 1fr 1fr; gap: 10px 40px; padding: 10px; justify-content: center; width: fit-content; margin: 0 auto; text-align: left; }
    .value { font-size: 1.2rem; font-weight: bold; color: #1565c0; }
    .unit { font-size: 0.9rem; margin-left: 4px; color: #666; }
    .controls-container { display: flex; justify-content: space-around; align-items: center; margin-top: 20px; padding: 10px; border-top: 1px solid #ddd; }
    .control-item { display: flex; flex-direction: column; align-items: center; gap: 8px; font-weight: bold; font-size: 0.9rem; }
    .switch { position: relative; display: inline-block; width: 54px; height: 28px; }
    .switch input { opacity: 0; width: 0; height: 0; }
    .slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; transition: .4s; border-radius: 34px; }
    .slider:before { position: absolute; content: ""; height: 20px; width: 20px; left: 4px; bottom: 4px; background-color: white; transition: .4s; border-radius: 50%; }
    input:checked + .slider { background-color: #2196F3; }
    input:checked + .slider:before { transform: translateX(26px); }
    .btn-reset { background-color: #d32f2f; color: white; border: none; padding: 12px 25px; border-radius: 8px; cursor: pointer; transition: 0.3s; font-weight: bold; text-transform: uppercase; }
    .btn-reset:hover { background-color: #b71c1c; }
    .hist-box { border: 1px solid #b0bec5; background-color: #eceff1; padding: 12px; border-radius: 8px; margin: 10px 0; text-align: center; }
    .vfd-style { background-color: #f1f8e9; border-color: #c5e1a5; }
    h2 { color: #1a237e; margin-bottom: 5px; }
    h3 { margin-bottom: 15px; border-bottom: 2px solid #1a237e; padding-bottom: 5px; display: inline-block; }
    h4 { margin: 10px 0; color: #555; text-transform: uppercase; font-size: 0.8rem; letter-spacing: 1px; }
  </style>
  <meta charset="UTF-8">
</head><body>
  <h2>Analizador de consumo energético</h2>
  <p style="margin-top:0; color:#666; font-weight: bold;">Electrónica II - UTN FRRo</p>

  <div class="card">
    <h3>Parámetros Eléctricos</h3>
    <div class="grid">
      <div>V: <span id="v" class="value">0</span> <span class="unit">V</span></div>
      <div>I: <span id="i" class="value">0</span> <span class="unit">A</span></div>
      <div>P: <span id="p" class="value">0</span> <span class="unit">W</span></div>
      <div>Q: <span id="q" class="value">0</span> <span class="unit">VAR</span></div>
      <div>S: <span id="s" class="value">0</span> <span class="unit">VA</span></div>
      <div>f: <span id="f" class="value">0</span> <span class="unit">Hz</span></div>
      <div style="grid-column: span 2; text-align: center; margin-top: 5px;">fdp: <span id="fdp" class="value">0</span></div>
    </div>
  </div>

  <div class="card" style="border-top: 6px solid #1a237e;">
    <h3>Consumo Proyectado</h3>
    <div id="proy" style="font-size: 2.5rem; font-weight: bold; color: #1a237e; margin: 10px 0;">0,00</div>
    <span class="unit" style="font-size: 1.2rem;">kWh</span>

    <h4 style="margin-top:25px;">Consumos Históricos</h4>
    <div class="hist-box">
      <strong>Presión regulada por válvula</strong><br>
      <span id="h_valv" class="value">0,00</span> <span class="unit">kWh</span>
    </div>
    <div class="hist-box vfd-style">
      <strong>Presión regulada por velocidad</strong><br>
      <span id="h_vfd" class="value">0,00</span> <span class="unit">kWh</span>
    </div>

    <div class="controls-container">
      <div class="control-item">
        <span>Regulación por VDF</span>
        <label class="switch">
          <input type="checkbox" id="vfd_switch" onchange="toggleVFD()">
          <span class="slider"></span>
        </label>
      </div>
      <div class="control-item">
        <span>Reset históricos</span>
        <button class="btn-reset" onclick="resetData()">RESET</button>
      </div>
    </div>
  </div>

  <script>
    function format(num) { 
      return parseFloat(num).toLocaleString('es-AR', {minimumFractionDigits: 2, maximumFractionDigits: 2}); 
    }
    
    setInterval(function ( ) {
      fetch('/data').then(response => response.json()).then(data => {
        document.getElementById("v").innerHTML = format(data.v); 
        document.getElementById("i").innerHTML = format(data.i);
        document.getElementById("p").innerHTML = format(data.p); 
        document.getElementById("q").innerHTML = format(data.q);
        document.getElementById("s").innerHTML = format(data.s); 
        document.getElementById("f").innerHTML = format(data.f);
        document.getElementById("fdp").innerHTML = format(data.fdp); 
        document.getElementById("proy").innerHTML = format(data.proy);
        document.getElementById("h_valv").innerHTML = format(data.h_valv); 
        document.getElementById("h_vfd").innerHTML = format(data.h_vfd);
      });
    }, 1000);

    function toggleVFD() { 
      fetch('/vfd?state=' + (document.getElementById("vfd_switch").checked ? 1 : 0)); 
    }
    
    function resetData() { 
      if(confirm("¿Seguro desea borrar los históricos de consumo?")) fetch('/reset'); 
    }
  </script>
</body></html>)rawliteral";

#endif