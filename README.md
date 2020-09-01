<!--
*** Thanks for checking out this README Template. If you have a suggestion that would
*** make this better, please fork the repo and create a pull request or simply open
*** an issue with the tag "enhancement".
*** Thanks again! Now go create something AMAZING! :D
-->





<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->



<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://github.com/othneildrew/Best-README-Template">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a>

  <h3 align="center">SDL CAN Dashboard</h3>

<!-- ABOUT THE PROJECT -->
## About The Project
Screenshots coming soon!


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
