<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://github.com/othneildrew/Best-README-Template">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a>

  <h3 align="center">SDL CAN Dashboard</h3>

<!-- ABOUT THE PROJECT -->
## About The Project
![Dashboard](https://github.com/dbbert7729/SDL_Dashboard_CAN/blob/master/s1.jpg)
![Home Screen](https://github.com/dbbert7729/SDL_Dashboard_CAN/blob/master/s2.jpg)
![Graph Widget](https://github.com/dbbert7729/SDL_Dashboard_CAN/blob/master/s3.jpg)


### Built With
Designed for Linux(CMAKE build system), with an emphasis on the Raspberry PI 4.
###
Libraries used:
SDL2 | SDL2_ttf | SDL2_image | SDL2_gfx
###
Simply use the CMakeLists file to create the necessary Makefile; and run make.

<!-- GETTING STARTED -->
## Getting Started
This project is intended to be used with a Holley EFI engine management setup paired with a canbus adapter. Simply connect the Raspberry PI or Linux computer to the CAN network and run the program.
One of the main goals of this project is to give the user the ability to create a custom dashboard layout while maintating a clear/concise telemetry interface. 
The main.cpp layout file is the one I personally use, and therefore serves as an example for users to follow.
Additionaly, Sensor rules(alarms) can be created for ensuring engine components stay within safe operating regions. 

<!-- ROADMAP -->
## Roadmap

<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE` for more information.
