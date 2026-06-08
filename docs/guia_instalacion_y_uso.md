# Guía de instalación y uso
## OpenGlove ESP32 BNO055 Tracking

Este documento describe los pasos mínimos para conectar, instalar, ejecutar y continuar el desarrollo del prototipo de rastreo posicional de OpenGlove basado en una unidad de adquisición ESP32-WROOM-32 y dos sensores inerciales BNO055.
---

## 1. Objetivo del sistema

El sistema adquiere la orientación de dos segmentos del miembro superior mediante dos sensores BNO055, transmite los datos hacia un sistema anfitrión y los procesa en una capa de software para estimar la posición relativa de la mano mediante un modelo cinemático segmentario.

La arquitectura considera:

1. Dos sensores inerciales BNO055.
2. Una unidad de adquisición basada en ESP32-WROOM-32.
3. Firmware de adquisición y transmisión serial.
4. Servicio intermedio de comunicación.
5. Aplicación cliente o SDK para PC.
6. Módulos de calibración, cálculo posicional, visualización y exportación CSV.

---

## 2. Hardware requerido

- 1 ESP32 DevKit V1 con módulo ESP32-WROOM-32.
- 2 sensores inerciales BNO055.
- 1 protoboard.
- Cableado de conexión.
- Cable Micro USB para conexión entre ESP32 y PC.
- Computador anfitrión para recepción, procesamiento y visualización.
- Soporte físico para ubicar los sensores sobre el brazo superior y el antebrazo.

---

## 3. Conexión de sensores

El sistema utiliza dos sensores BNO055 conectados a dos buses I2C independientes del ESP32. Ambos sensores utilizan la dirección I2C `0x28`, pero no generan conflicto porque se conectan a buses distintos.

### 3.1 IMU 1 / BNO055 - brazo superior

| Función | Pin ESP32 | Pin BNO055 |
|---|---|---|
| Alimentación | 3V3 | VCC |
| Tierra | GND | GND |
| Datos I2C | GPIO 21 | SDA |
| Reloj I2C | GPIO 22 | SCL |

Configuración en firmware:

```cpp
Wire.begin(21, 22);
Adafruit_BNO055 bno1 = Adafruit_BNO055(-1, 0x28, &Wire);
```

Este sensor entrega el cuaternión:

```text
q1 = (w1, x1, y1, z1)
```

### 3.2 IMU 2 / BNO055 - antebrazo

| Función | Pin ESP32 | Pin BNO055 |
|---|---|---|
| Alimentación | 3V3 | VCC |
| Tierra | GND | GND |
| Datos I2C | GPIO 18 | SDA |
| Reloj I2C | GPIO 19 | SCL |

Configuración en firmware:

```cpp
Wire1.begin(18, 19);
Adafruit_BNO055 bno2 = Adafruit_BNO055(-1, 0x28, &Wire1);
```

Este sensor entrega el cuaternión:

```text
q2 = (w2, x2, y2, z2)
```

### 3.3 Consideraciones

- Ambos sensores se alimentan desde el pin `3V3` de la placa ESP32.
- Ambos sensores comparten tierra común con la ESP32.
- Se utiliza una protoboard como medio de interconexión.
- No se agregan resistencias externas; se utilizan únicamente las conexiones disponibles en los módulos BNO055 y la protoboard.
- El sensor ubicado en el brazo superior corresponde a la IMU 1.
- El sensor ubicado en el antebrazo corresponde a la IMU 2.

---

## 4. Firmware ESP32

El firmware se encarga de:

1. Inicializar la comunicación serial.
2. Inicializar el sensor del brazo superior mediante `Wire`.
3. Inicializar el sensor del antebrazo mediante `Wire1`.
4. Leer los cuaterniones de ambos sensores.
5. Enviar una trama serial con ocho valores numéricos.
6. Mantener una frecuencia de muestreo de 100 Hz.

### 4.1 Ubicación del firmware

```text
firmware/
```

Archivo:

```text
esp32_dual_bno055_quaternion_stream.ino
```

### 4.2 Dependencias

```cpp
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
```

Instalar las librerías correspondientes antes de compilar y cargar el firmware.

### 4.3 Parámetros principales

| Parámetro | Valor |
|---|---:|
| Baudrate serial | 115200 |
| Frecuencia de muestreo | 100 Hz |
| Periodo de muestreo | 10 ms |
| Dirección I2C IMU 1 | 0x28 |
| Dirección I2C IMU 2 | 0x28 |
| Bus IMU 1 | Wire |
| Bus IMU 2 | Wire1 |

### 4.4 Carga del firmware

1. Conectar la ESP32 al computador mediante USB.
2. Abrir el firmware en Arduino IDE o entorno equivalente.
3. Seleccionar la placa ESP32 correspondiente.
4. Seleccionar el puerto serial asociado a la ESP32.
5. Verificar que las librerías requeridas estén instaladas.
6. Compilar el firmware.
7. Cargar el firmware en la ESP32.
8. Abrir el monitor serial a `115200` baudios y verificar la salida.

---

## 5. Formato de datos seriales

El firmware transmite una línea por muestra con el siguiente formato:

```text
w1,x1,y1,z1,w2,x2,y2,z2
```

| Campo | Descripción |
|---|---|
| `w1` | Componente w del cuaternión del brazo superior |
| `x1` | Componente x del cuaternión del brazo superior |
| `y1` | Componente y del cuaternión del brazo superior |
| `z1` | Componente z del cuaternión del brazo superior |
| `w2` | Componente w del cuaternión del antebrazo |
| `x2` | Componente x del cuaternión del antebrazo |
| `y2` | Componente y del cuaternión del antebrazo |
| `z2` | Componente z del cuaternión del antebrazo |

Cada línea termina con salto de línea:

```text
\n
```

Ejemplo:

```text
0.98,0.01,-0.04,0.15,0.95,0.03,-0.10,0.28
```

Los valores se transmiten con dos decimales de precisión.

---

## 6. Servicio intermedio de comunicación

El servicio intermedio actúa como puente entre la unidad de adquisición y la aplicación cliente o SDK para PC.

Responsabilidades:

1. Abrir el puerto serial asociado a la ESP32.
2. Leer las tramas enviadas por el firmware.
3. Validar que cada trama contenga los ocho valores esperados.
4. Descartar datos inválidos.
5. Encapsular los datos en el formato utilizado por la capa de software.
6. Difundir los datos mediante WebSocket hacia la aplicación cliente.

---

## 7. Aplicación cliente / SDK para PC

La aplicación cliente recibe los datos difundidos por el servicio intermedio y aplica el procesamiento asociado al rastreo posicional.

Responsabilidades:

1. Recibir los cuaterniones `q1` y `q2`.
2. Ejecutar el proceso de calibración.
3. Aplicar el modelo cinemático segmentario.
4. Calcular la posición relativa del codo y de la mano.
5. Mostrar la representación gráfica o coordenadas calculadas.
6. Exportar datos experimentales en formato CSV.

### 7.1 Referencia del SDK

https://github.com/Open-Glove/SDK-OpenGlove

Archivos modificados o implementados durante esta investigación:

todos los de la imagen "archivos-modificados.jpeg"

### 7.2 Ejecución general

1. Abrir el ejecutable del SDK o iniciar la ejecucion del proyecto desde Visual Studio.
2. Crear una configuracion o Abrir una ya existente (a terminos de este proyecto se dejó por defecto asi que solamente al crear deben dar a Save).
3. Dar a Connect.
4. Clickear en Profile Configuration y luego en IMU.
5. Clickear en Activate data y Test.
6. Ejecutar la calibración inicial segun el largo deseado.
7. Iniciar la visualización o registro de datos.
8. Exportar los resultados si corresponde.

## 8. Calibración

La calibración establece una referencia inicial para interpretar la orientación de los segmentos del brazo.

Procedimiento general:

1. Colocar el brazo en la pose inicial definida por el protocolo experimental (L).
2. Mantener los sensores estables durante la captura de la referencia.
3. Ejecutar la acción de calibración en la aplicación cliente.
4. Guardar las orientaciones iniciales de ambos sensores.
5. Utilizar esas orientaciones como referencia para el cálculo relativo posterior.

Durante la calibración se asocian:

```text
q1_ref = orientación inicial del brazo superior
q2_ref = orientación inicial del antebrazo
```

---

## 9. Modelo de cálculo posicional

El modelo representa el miembro superior como una cadena cinemática de dos segmentos rígidos:

- `L1`: brazo superior.
- `L2`: antebrazo.

Los sensores entregan la orientación de cada segmento mediante cuaterniones:

```text
q1 = orientación del brazo superior
q2 = orientación del antebrazo
```

A partir de estas orientaciones y de las longitudes segmentarias, el sistema calcula:

```text
p_codo
p_mano
```

---

## 10. Exportación CSV

La aplicación cliente permite exportar datos experimentales para análisis posterior.

El archivo CSV puede incluir, según la implementación final:

- tiempo o índice de muestra;
- cuaternión del sensor del brazo superior;
- cuaternión del sensor del antebrazo;
- posición calculada del codo;
- posición calculada de la mano;
- estado de calibración;
- identificador de prueba o movimiento.

Ubicación sugerida de ejemplos:

```text
data/ejemplos-csv/
```

> TODO: agregar archivos CSV de ejemplo y describir brevemente a qué prueba corresponde cada uno.

---

## 11. Flujo completo de ejecución

1. Conectar los sensores BNO055 a la ESP32 según el esquemático.
2. Conectar la ESP32 al computador mediante USB.
3. Cargar el firmware en la ESP32.
4. Verificar la salida serial en formato `w1,x1,y1,z1,w2,x2,y2,z2`.
5. Ejecutar el servicio intermedio de comunicación.
6. Verificar que el servicio reciba datos válidos desde el puerto serial.
7. Ejecutar la aplicación cliente o SDK para PC.
8. Realizar la calibración inicial.
9. Ejecutar la prueba o movimiento definido.
10. Visualizar la posición calculada.
11. Exportar los datos experimentales en CSV.
12. Analizar los resultados obtenidos.

---

## 12. Resolución de problemas

### No aparecen datos en el monitor serial

Revisar:

- puerto serial seleccionado;
- baudrate configurado en `115200`;
- conexión USB;
- carga correcta del firmware;
- alimentación de sensores desde `3V3`;
- conexión de `GND` común.

### El firmware imprime `ERR_BNO1`

Revisar el sensor del brazo superior:

- SDA en GPIO 21;
- SCL en GPIO 22;
- alimentación en 3V3;
- tierra común;
- conexión al bus `Wire`.

### El firmware imprime `ERR_BNO2`

Revisar el sensor del antebrazo:

- SDA en GPIO 18;
- SCL en GPIO 19;
- alimentación en 3V3;
- tierra común;
- conexión al bus `Wire1`.

### El servicio no recibe datos

Revisar:

- puerto serial configurado;
- baudrate;
- que no haya otro programa usando el puerto;
- que la ESP32 esté enviando datos;
- formato de trama esperado.

### La aplicación cliente no recibe datos

Revisar:

- que el servicio intermedio esté ejecutándose;
- configuración del WebSocket;
- formato del mensaje difundido;
- conexión entre aplicación cliente y servicio.

---

## 13. Continuidad del proyecto

Para continuar el desarrollo se recomienda:

1. Mantener versionado el firmware ESP32.
2. Documentar cambios en pines, frecuencia de muestreo o formato de datos.
3. Registrar cambios en el servicio intermedio y aplicación cliente.
4. Mantener actualizados los diagramas de arquitectura y esquemáticos.
5. Agregar ejemplos CSV representativos de nuevas pruebas.
6. Crear releases o etiquetas para diferenciar versiones evaluadas.

Versión asociada al informe de investigación:

```text
v1.0-esp32-bno055
```

---

## 14. Repositorios relacionados

Repositorio técnico de esta investigación:

```text
https://github.com/Open-Glove/openglove-esp32-bno055-tracking
```

Repositorio del SDK OpenGlove o software base:

```text
https://github.com/Open-Glove/SDK-OpenGlove
```

---

## 15. Licencia

Este repositorio debe incluir un archivo `LICENSE` con la licencia definida para el proyecto.

Licencia seleccionada:

```text
Este proyecto se distribuye bajo licencia MIT. Consultar el archivo LICENSE para más información.
```
