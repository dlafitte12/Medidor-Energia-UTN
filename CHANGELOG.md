# Changelog

## [1.2.0] - 2026-2-22

### Added
- Implementación de servidor asíncrono para monitoreo en tiempo real.
- Interfaz web con visualización de parámetros eléctricos y consumos históricos.
- Lógica de captura ("Latch") de consumo proyectado al conmutar entre modos Válvula/VFD.
- Rutina de reconexión automática de red (WiFi/MQTT) para funcionamiento autónomo.

### Changed
- Migración de concatenación manual de strings a encapsulamiento JSON mediante librería ArduinoJson v7.
- Reorganización de variables y constantes para cumplir con normas de legibilidad de la cátedra.
- Separación de credenciales de red a archivo de configuración externo `config.h`.

### Fixed
- Error de bloqueo en el bucle principal ante caídas temporales de la infraestructura de red.