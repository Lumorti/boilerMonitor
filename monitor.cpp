#include <ctime>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <raspicam/raspicam.h>

// Where to grab the pixels
std::vector<std::vector<int>> grabLocs = {{100, 100}, 
										  {100, 200}, 
										  {100, 300}, 
										  {100, 400}, 
										  {100, 500}};

// Minimum colour to be active
std::vector<int> minCol = {255, 0, 0};

// Current boiler level
int level = -1;

// C++ entry point
int main (int argc, char **argv){

	// Init the camera
    raspicam::RaspiCam Camera;
	std::cout << "Opening Camera..." << std::endl;
    if (!Camera.open()){
		std::cerr << "Error opening camera" << std::endl;
		return -1;
	}

    // Wait until the camera stabilises
    usleep(3*1000000);

    // Capture a photo
    Camera.grab();

    // Allocate memory
    unsigned char *data = new unsigned char[Camera.getImageTypeSize(raspicam::RASPICAM_FORMAT_RGB)];

    // Extract the image in rgb format
    Camera.retrieve(data, raspicam::RASPICAM_FORMAT_RGB);

	// Draw the locations of the grab points

    // Save the initial image to file
    std::ofstream outFile("initial.ppm", std::ios::binary);
    outFile << "P6\n" << Camera.getWidth() << " " << Camera.getHeight() << " 255\n";
    outFile.write((char*) data, Camera.getImageTypeSize(raspicam::RASPICAM_FORMAT_RGB));

    // Clean up
    delete data;
    return 0;

}
//
