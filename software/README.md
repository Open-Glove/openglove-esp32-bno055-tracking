# Referencias de software OpenGlove

El SDK y los componentes base de OpenGlove se encuentran disponibles en el siguiente repositorio:

- OpenGlove SDK: https://github.com/Open-Glove/SDK-OpenGlove

Este proyecto utiliza dicho software como base para la integración del módulo de rastreo posicional. Las modificaciones realizadas durante esta tesis se documentan en el informe y en la guía técnica del repositorio.

Componentes intervenidos (sobre el SDK base en [SDK-OpenGlove](https://github.com/Open-Glove/SDK-OpenGlove)):

### OpenGloveService
- `OpenGloveService.csproj` — ajustes de compilación del servicio Windows que hospeda la API WCF entre el puerto serial y la aplicación cliente.

### OpenGlove API C Sharp HL
- `dataReceiver.cs` — cliente WebSocket del SDK: añade el evento `dualQuaternionFunction` y el caso `Q` para recibir los ocho valores `w1,x1,y1,z1,w2,x2,y2,z2` difundidos por el servicio.

### OpenGlove-API-C-Sharp
- `Communication.cs` — lectura del puerto serial ESP32 (`w1,x1,y1,z1,w2,x2,y2,z2` por línea), validación de tramas y difusión vía WebSocket con prefijo `Q,` hacia la aplicación cliente.

### OpenGlovePrototype2
- `ArmTrackingMath.cs` *(nuevo)* — operaciones de cuaterniones y vectores 3D para el modelo segmentario (conjugado, multiplicación, rotación, suma/resta).
- `ConfigManager.cs` — persistencia del atributo `imuModel` (`BNO055`) al guardar y cargar perfiles de configuración.
- `Boards.xml` — incorpora la placa `ESP32 DevKit V1` (34 pines digitales, 8 analógicos).
- `Greeter.xaml` — ajuste menor de márgenes en la pantalla principal.
- `IMUConfiguration.xaml` / `IMUConfiguration.xaml.cs` — ventana de IMU: visualización de cuaterniones de brazo y antebrazo (absolutos y relativos), longitudes `L1`/`L2`, calibración de referencia, cálculo de posición de mano (`pMano`) y exportación de filas a CSV.
- `PinsConfiguration.xaml` / `PinsConfiguration.xaml.cs` — selección de placa ESP32, listado de pines GPIO, baudrate por defecto `115200` y diagrama de pinout.
- `OpenGlovePrototype2.csproj` — incluye `ArmTrackingMath.cs` y el recurso `ESP32-Pinout.jpg`.
- `ESP32-Pinout.jpg` *(nuevo)* — imagen de referencia del pinout ESP32 mostrada en la configuración de pines.
- `OpenGlove_IMU_export.csv` *(nuevo)* — archivo de salida con filas exportadas (cuaterniones, posición calculada y etiqueta de prueba).