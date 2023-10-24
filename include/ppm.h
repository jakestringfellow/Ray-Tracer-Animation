#ifndef PPM_H
#define PPM_H

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>

// Define a pixel 
struct Pixel {
    int red = 0;
    int green = 0;
    int blue = 0;
};

class PPM {

    std::string type;
    int width;
    int height;
    int pixelRange;
    std::vector<int> pixelData;

    public: 
        PPM(const std::string& type, int width, int height, int pixelRange, const std::vector<int>& pixelData) 
            : type(type), width(width), height(height), pixelRange(pixelRange), pixelData(pixelData) {}

        // Save the ppm
        void save(const std::string& filepath) {
            // Create the header with the main ppm attributes
            std::string header = type + "\n" + std::to_string(width) + " " +std::to_string(height) + "\n" + std::to_string(pixelRange) + "\n";
            std::vector<char> fileData;

            if (type == "P3") {
                // Create the pixel lines by mapping each pixel's components to a new line
                // Use "toString()" to make the numbers able to be written
                std::string pixelLines;
                for (const auto& component : pixelData) {
                    pixelLines += std::to_string(component) + "\n";
                }
                // add the pixel info beneath the header 
                // New line added to allow previewing in mac
                std::string data = header + pixelLines + "\n";
                fileData.assign(data.begin(), data.end());            
                }
            else if (type == "P6") {
                fileData.assign(header.begin(), header.end());
                fileData.insert(fileData.end(), pixelData.begin(), pixelData.end());
            }

            // Write this content to a new file
            std::ofstream file(filepath, std::ios::binary);
            if (file.is_open()) {
                file.write(fileData.data(), fileData.size());
                file.close();
                std::cout << "File written.\n";
            }
            else {
                std::cerr << "Error writing file.\n";
            }
        }

        // Darken the ppm 
        void darken(int multiplier) {
            for (auto& pixel : pixelData) {
                pixel /= multiplier;
            }
        }

        // Brighten the ppm 
        void brighten(int multiplier) {
            // For each pixel
            for (auto& pixel : pixelData) {

                pixel *= multiplier;
            
                if (pixel > pixelRange) {
                    pixel = pixelRange;
                }
            

            }
        }

        // Set a ppm's pixel
        void setPixel(int x, int y, int r, int g, int b) {
            const int index = (width * y) - width + x;

            pixelData[index] = r;
            pixelData[index+1] = g;
            pixelData[index+2] = b;
        }

        void clearImage() {
            std::fill(pixelData.begin(), pixelData.end(), 0);
        }

        void drawCircle() {
            int centerX = width / 2;
            int centerY = height / 2;
            int radius = width / 4;

            // Loop through the pixels
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    // Get the index of the pixel
                    int index = (y * width + x) * 3;   // Remember to account for 3 components

                    // Check if the pixel is within the circle perimeter
                    int dx = x - centerX;
                    int dy = y - centerY;
                    int distance = std::sqrt(dx * dx + dy * dy);
                    if (distance <= radius) {
                        // Set the color of the circle's pixels to green
                        pixelData[index] = 0;          // Red
                        pixelData[index + 1] = 200;    // Green
                        pixelData[index + 2] = 0;      // Blue

                    }
                }
            }
        }

        // Convert a P6 format to P3 format
        void convertToP3() {
            type = "P3";
        }

        // Convert a P3 format to P6 format
        // Create a new pixel array with bytes instead of uints
        void convertToP6() {

            type = "P6";

            std::vector<uint8_t> pixelBytes(pixelData.begin(), pixelData.end());
            pixelData.assign(pixelBytes.begin(), pixelBytes.end());

        }

    };

PPM parsePPM(const std::string& filepath) {
    std::ifstream file(filepath);


    std::stringstream filteredStream;

    std::string line;
    while (std::getline(file, line)) {
        // Only add lines that don't start with '#' to the filtered stream
        if (line.empty() || line[0] != '#') {
            filteredStream << line << '\n';
        }
    }

    // Read the PPM type
    std::getline(filteredStream, line);
    std::string ppmType = line;

    // Skip comments
    do {
        std::getline(filteredStream, line);
    } while (line[0] == '#');

    // Read width and height
    int ppmWidth, ppmHeight;
    std::istringstream dimensions(line);
    dimensions >> ppmWidth >> ppmHeight;

    // Read pixel range
    std::getline(filteredStream, line);
    int ppmRange = std::stoi(line);

    // Read pixels
    std::vector<int> pixels;
    while (std::getline(filteredStream, line)) {
        std::istringstream iss(line);
        int component;
        while (iss >> component) {
            component = std::max(0, std::min(ppmRange, component));
            pixels.push_back(component);
        }
    }

    std::vector<int> organizedPixels;
    for (size_t i = 0; i < pixels.size(); i += 3) {
        organizedPixels.push_back(pixels[i]);
        organizedPixels.push_back(pixels[i + 1]);
        organizedPixels.push_back(pixels[i + 2]);
    }

    return PPM(ppmType, ppmWidth, ppmHeight, ppmRange, organizedPixels);
}

#endif