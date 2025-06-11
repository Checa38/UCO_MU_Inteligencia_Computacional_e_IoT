#include <iostream>
#include <exception>

//OpenCV includes
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
//#include <opencv2/calib3d.hpp> //Uncomment when it was appropiated.
//#include <opencv2/ml.hpp> //Uncomment when it was appropiated.


#include "common_code.hpp"

const char * keys =
    "{help h usage ? |      | print this message}"
    "{w              |20    | Wait time (miliseconds) between frames.}"
    "{v              |      | the input is a video file.}"
    "{c              |      | the input is a camera index.}"    
    "{@input         |<none>| input <fname|int>}"
    ;




int
main (int argc, char* const* argv)
{
  int retCode=EXIT_SUCCESS;
  
  try {    

      cv::CommandLineParser parser(argc, argv, keys);
      parser.about("Show the extremes values and their locations.");
      if (parser.has("help"))
      {
          parser.printMessage();
          return 0;
      }
      bool is_video = parser.has("v");
      bool is_camera = parser.has("c");
      int wait = parser.get<int>("w");
      cv::String input = parser.get<cv::String>("@input");
      if (!parser.check())
      {
          parser.printErrors();
          return 0;
      }

    // TODO
    std::vector<cv::uint8_t> min_v;
    std::vector<cv::uint8_t> max_v;
    std::vector<cv::Point> min_loc;
    std::vector<cv::Point> max_loc;

    if (!parser.has("v") && !parser.has("c"))
    {
      cv::Mat img = cv::imread(input, cv::IMREAD_ANYCOLOR);
      if (img.empty())
      {
        std::cerr << "Error: no he podido abrir el fichero '" << input << "'." << std::endl;
        return EXIT_FAILURE;
      }
      cv::namedWindow("IMG", cv::WINDOW_GUI_EXPANDED);
      cv::imshow("IMG", img);

      fsiv_find_min_max_loc_1(img, min_v, max_v, min_loc, max_loc);

      for (int c = 0; c < img.channels(); ++c)
      {
        std::cout << "Resultados para el canal " << c+1 << std::endl;
        std::cout << "\t- El valor mínimo de la imagen es: " << (int)min_v[c] << ". Ubicado en: " << min_loc[c] << std::endl;
        std::cout << "\t- El valor máximo de la imagen es: " << (int)max_v[c] << ". Ubicado en: " << max_loc[c] << std::endl;
      }

      std::cout << "Pulsa ESC para salir." << std::endl;
      while ((cv::waitKey(0) & 0xff) != 27); //Hasta que no se pulse la tecla ESC no salimos.

      cv::destroyWindow("IMG");
    }
    else
    {
      
      cv::VideoCapture vid;
      vid.open(input);

      if (!vid.isOpened())
      {
         std::cerr << "Error: no he podido abrir el la fuente de vídeo." << std::endl;
         return EXIT_FAILURE;
      }
      cv::namedWindow("VIDEO");
      cv::Mat frame;
      vid >> frame;

      if (frame.empty())
      {
          std::cerr << "Error: could not capture any frame from source." << std::endl;
          return EXIT_FAILURE;
      }

      std::cout << "Input size (WxH): " << frame.cols << 'x' << frame.rows << std::endl;
      std::cout << "Frame rate (fps): " << vid.get(cv::CAP_PROP_FPS) << std::endl;
      std::cout << "Num of frames   : " << vid.get(cv::CAP_PROP_FRAME_COUNT) << std::endl;

      cv::namedWindow("VIDEO");
      std::cerr << "Pulsa una tecla para continuar (ESC para salir)." << std::endl;
      int key = cv::waitKey(0) & 0xff;
      
      int cont = 2;
      bool is_paused = false;


      while (!frame.empty() && key != 27) 
      {
          std::cout << "Frame " << cont << std::endl;
          cont++;
          cv::imshow("VIDEO", frame);

          fsiv_find_min_max_loc_1(frame, min_v, max_v, min_loc, max_loc);

          for (int c = 0; c < frame.channels(); ++c)
          {
              std::cout << "    + Resultados para el canal " << c + 1 << std::endl;
              std::cout << "\t- El valor mínimo de la imagen es: " << (int)min_v[c] << ". Ubicado en: " << min_loc[c] << std::endl;
              std::cout << "\t- El valor máximo de la imagen es: " << (int)max_v[c] << ". Ubicado en: " << max_loc[c] << std::endl;
              max_v[c]=0;
              min_v[c]=255;
          }

          key= cv::waitKey(wait) & 0xFF;

          if (key== 32) 
          {
              is_paused = !is_paused;  
              std::cout << (is_paused ? "Pausado." : "Reanudado.") << std::endl;
              while (is_paused) 
              {
                  key= cv::waitKey(0) & 0xFF; 
                  if (key== 27) 
                  { 
                    return 0;
                  }
                  else
                  { 
                    is_paused = !is_paused;
                  }
                  
              }
          }

          if (!is_paused) 
          {
              vid >> frame;
          }
      }

      cv::destroyWindow("VIDEO");
    }
    //    
  }
  catch (std::exception& e)
  {
    std::cerr << "Caught exception: " << e.what() << std::endl;
    retCode = EXIT_FAILURE;
  }
  catch (...)
  {
    std::cerr << "Caught unknown exception!" << std::endl;
    retCode = EXIT_FAILURE;
  }
  return retCode;
}
