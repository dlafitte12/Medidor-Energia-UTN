# Changelog

## [1.2.0] - 2026-02-24

### Added
- Arquitectura modular mediante separación de responsabilidades en archivos de cabecera (.h) e implementación (.cpp).
- Implementación de servidor asíncrono para monitoreo en tiempo real.
- Interfaz web con visualización de parámetros eléctricos y consumos históricos.
- Lógica de captura ("Latch") de consumo proyectado al conmutar entre modos Válvula/VFD.
- Rutina de reconexión automática de red (WiFi/MQTT) con timeout no bloqueante.
- Implementación de métodos de acceso (Getters) para asegurar el encapsulamiento de datos.

### Changed
- Migración de almacenamiento volátil a persistencia en memoria NVS (Non-Volatile Storage) mediante librería Preferences.
- Migración de concatenación manual de strings a encapsulamiento JSON mediante librería ArduinoJson v7.
- Reorganización de variables y constantes para cumplir con normas de legibilidad de la cátedra.
- Separación de credenciales de red y definiciones globales al archivo externo config.h.

### Fixed
- Error de bloqueo en el bucle principal ante caídas temporales de la infraestructura de red.
- Riesgo de desbordamiento de memoria RAM al trasladar el código HMI a la memoria Flash (PROGMEM).