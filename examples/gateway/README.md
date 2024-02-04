<div align="center">
  <h1>
    <i>loconet-gateway-esp32</i>
</h1>
<h3>A LocoNet gateway on ESP32 hardware.</h3>
<p>
  <img alt="Build" src="https://github.com/benmusson/loconet-gateway-esp32/actions/workflows/build.yml/badge.svg"/>
  <img alt="Dev Build" src="https://github.com/benmusson/loconet-gateway-esp32/actions/workflows/buildDev.yml/badge.svg"/>
  <a href="https://github.com/benmusson/loconet-gateway-esp32/blob/main/LICENSE">
    <img src="https://img.shields.io/badge/License-GPL-yellow.svg" />
  </a>
</p>
</div>

## Hardware
<p align="center">
  <img src="./docs/Pictures/ESP32_LocoNet_RS485_Board_v1_1.jpg?raw=true" align="center" width="800px"/>
</p>


## Downloading to the Controller
> [!IMPORTANT]
> Dip switch 8 must be on in order to download to the board.
> GPIO2 (the input used for dip-switch 8) is a **strapping** pin, and on boot up is used to determine if the ESP32 is able to be downloaded to.
