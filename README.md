# Medidor de Energía - Electrónica II

## Integrantes
* **Cetraro, Fabricio**
* **Lafitte, Didier**

## Descripción del Proyecto
Este sistema embebido actúa como un **Medidor de Energía Inteligente** reactivo. Utiliza un ESP32 para capturar parámetros eléctricos de un motor/bomba mediante un sensor **Shelly Gen4** vía protocolo **MQTT**. 

El sistema permite comparar el consumo de una bomba bajo dos regímenes de control:
1. **Presión por Válvula:** Control mecánico tradicional.
2. **Presión por VDF (Variador de Frecuencia):** Control electrónico de eficiencia.



## Características Técnicas
* **Funcionamiento Reactivo:** Procesamiento de datos en tiempo real mediante callbacks MQTT.
* **Monitoreo Asíncrono:** Servidor web basado en `ESPAsyncWebServer` que no bloquea el lazo principal de control.
* **Encapsulamiento JSON:** Intercambio de datos bajo estándar JSON según requerimientos de la cátedra.
* **Robustez de Red:** Rutinas automáticas de reconexión para WiFi y Broker MQTT.

## Parámetros Monitoreados
* Tensión RMS (V)
* Corriente RMS (A)
* Potencia Activa (W), Reactiva (VAR) y Aparente (VA)
* Frecuencia (Hz) y Factor de Potencia (fdp)
* **Consumo Proyectado (kWh):** Estimación horaria instantánea.

## Instalación y Uso
1. Clonar el repositorio.
2. Configurar credenciales en `include/config.h`.
3. Compilar y subir mediante **PlatformIO** en VS Code.
4. Acceder a la IP del dispositivo desde cualquier navegador.

---
*UTN FRRo - Cátedra de Electrónica II* *Esp. Ing. Marcelo Castello*