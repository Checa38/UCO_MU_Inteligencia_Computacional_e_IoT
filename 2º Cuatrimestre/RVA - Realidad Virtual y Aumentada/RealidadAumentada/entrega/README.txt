El programa tiene las siguientes opciones de ejecución:
./armuseum <model> <escena> [<patch>] [<video|index-cam>]

Ejemplos:
> Descripción de la imagen:
./armuseum ../../data/modelo.jpg ../../data/escena2.jpg
> Aplicando patch:
./armuseum ../../data/modelo.jpg ../../data/escena2.jpg --patch=../../data/opencv3_model.jpg 
> Usando un vídeo:
./armuseum ../../data/modelo.jpg ../../data/escena2.jpg --video=../../data/video.mp4 
