#include <ctime>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <math.h>
#include <raspicam/raspicam.h>

// Where to grab the pixels
std::vector<std::vector<int>> grabLocs = {{859, 393}, 
										  {782, 446}, 
										  {708, 490}, 
										  {633, 527}, 
										  {557, 561}};

// Minimum brightness to be active
int minBright = 240;

// Current boiler level
int level = -1;

// C++ entry point
int main (int argc, char **argv){

	// Init the camera
    raspicam::RaspiCam Camera;
	std::cout << "Opening camera..." << std::endl;
    if (!Camera.open()){
		std::cerr << "Error opening camera" << std::endl;
		return -1;
	}

    // Wait until the camera stabilises
	std::cout << "Waiting for camera stabilisation..." << std::endl;
    usleep(3*1000000);

    // Capture a photo
	std::cout << "Taking initial photo..." << std::endl;
    Camera.grab();

    // Allocate memory
    unsigned char *data = new unsigned char[Camera.getImageTypeSize(raspicam::RASPICAM_FORMAT_RGB)];
	int imageWidth = Camera.getWidth();
	int imageHeight = Camera.getHeight();

    // Extract the image in rgb format
    Camera.retrieve(data);

	// Draw the locations of the grab points
	for (unsigned int i=0; i<grabLocs.size(); i++){
		int pos = 3*(grabLocs[i][1]*imageWidth + grabLocs[i][0]);
		data[pos] = char(0);
		data[pos+1] = char(255);
		data[pos+2] = char(0);
	}

    // Save the initial image to file
	std::ofstream outFile("initial.ppm", std::ios::binary);
	outFile << "P6\n" << Camera.getWidth() << " " << Camera.getHeight() << " 255\n";
	outFile.write((char*) data, Camera.getImageTypeSize(raspicam::RASPICAM_FORMAT_RGB));

	// Open the data file to write to
	std::ofstream dataFile;
	dataFile.open("data.txt", std::ios_base::app);

	// Loop forever
	int pos = 0;
	int bright = 0;
	int temp = 0;
	while (true){

		// Keep checking because of the flashing
		level = 5;
		for (int j=0; j<10; j++){

			// Take an image
			Camera.grab();
			Camera.retrieve(data);
			if (argc > 1){
				std::cout << "-------" << std::endl;
			}

			// Get the level
			temp = 0;
			for (int i=0; i<grabLocs.size(); i++){
				pos = 3*(grabLocs[i][1]*imageWidth + grabLocs[i][0]);
				bright = data[pos+2];
				if (argc > 1){
					std::cout << i+1 << " " << bright << " " << minBright << std::endl;
				}
				if (bright > minBright && i+1 > temp){
					temp = i+1;
				}
			}

			// Is this level the lowest at the moment
			if (temp < level){
				level = temp;
			}

			// Wait between captures
			usleep(0.2e6);

		}

		// Get the time
		std::time_t result = std::time(nullptr);
		if (argc > 1){
			std::cout << "-------" << std::endl;
			std::cout << result << " " << level << std::endl;
		}
		dataFile << result << " " << level << std::endl;

		// Wait between captures
		usleep(60e6);

	}

    // Clean up
    delete data;
    return 0;

}
