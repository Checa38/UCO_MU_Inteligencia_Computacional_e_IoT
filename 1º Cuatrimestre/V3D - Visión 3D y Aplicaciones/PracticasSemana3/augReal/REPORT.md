# Lo que he hecho

Minimum requirements (60%)
- Obtener calibration.yml: He usado el código de moodle (3.1_cameracalibration) para calibrar la cámara del vídeo de augreal.mp4 mediante la imágenes dadas
- Realizar programa: 
	- 1. cv::findChessboardCorners y cv::cornerSubPix
	- 2. cv::solvePnP
	- 3. Dibujar ejes 3D

Optional requirements (40%)
- Cubos 1x1x1 en cada cuadrado negro

# Lo que no he hecho

# Ejemplos de uso

Como se indica en la descripción de la práctica:
./augReal <size> <intrisics.yml> <videofile>
./augReal 7 ../calibration.yml ../augreal.mp4 
./augReal 4 ../calibration.yml ../augreal.mp4 
./augReal 1 ../calibration.yml ../augreal.mp4 

En caso de que tuviesemos un tablero diferentes podríamos cambiar las columnas (c), filas (r) y tamaño de cuadrado (s):
./augReal 4 <intrisics.yml> <videofile> -c=5 -r=6 -s=0.4
./augReal 4 <intrisics.yml> <videofile> -c=8 -r=9 -s=1.2
