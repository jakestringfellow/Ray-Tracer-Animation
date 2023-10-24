#ifndef OBJModel_HPP
#define OBJModel_HPP

#include <iostream>
#include <vector>
#include <string> //std::string
#include <fstream> //File I/O operations
#include <sstream> //sstream

// //Constructor loads a filename with .obj extension
class OBJModel {
public:
    OBJModel(std::string fileName) {
        //Load and parse an obj file to get its vertices data, normal,
        //and face values
        parseOBJFile(fileName);
    }

    std::vector<float> getVertices() const { return vertices; }
    std::vector<uint32_t> getIndices() const { return indices; }

private:
    // obj model parser will work strictly with triangulated meshes for now
    static constexpr int VERTICES_PER_FACE = 3;

    // Store, vector, normal, and face values
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> vertex_textures;
    //PPM file path
    std::string m_ppm_filePath;
    std::vector<uint32_t> indices;
    std::vector<uint32_t> vertex_texcoord_indices;
    std::vector<uint32_t> vertex_normal_indices;

    void parseOBJFile(const std::string& fileName) {
        std::stringstream ss;
        std::ifstream inputFile(fileName);
        std::string line = "";
        std::string prefix = "";

        if(inputFile.is_open()){
            //Read one line at a time
            while(std::getline(inputFile, line)) {
                ss.clear();
                ss.str(line);
                
                //Get the prefix of the line
                ss >> prefix;

                // Ignore comments
                if (prefix == "#") continue;

                // Intake of mtl file
                else if (prefix == "mtllib") parseMTLLib(ss);

                // Intake of vertex position (x,y,z) info from "v" prefixed lines
                else if (prefix == "v") parseVertex(ss);

                // Intake of vertex texture info "s" and "t" from "vt" prefixed lines
                else if (prefix == "vt") parseVertexTexture(ss);

                // Intake of vertex normal info from "vn" prefixed lines
                else if (prefix == "vn") parseVertexNormal(ss);

                // Intake of face info from "f" prefixed faces
                else if (prefix == "f") parseFace(ss);
            }
            inputFile.close();
        } else {
            std::cerr << "Failed to open .obj file: " << fileName << std::endl;
        }
    }

    void parseMTLLib(std::stringstream& ss) {
        std::ifstream mtl_file;
        std::stringstream mtl_ss;
        std::string mtl_line = "";
        std::string mtl_prefix = "";
        std::string mtl_fileName;
        std::string ppm_fileName;

        // Get the prefix of the mtl file line, then get the name of the file
        std::string mtl_name;
        ss >> mtl_name;
        mtl_fileName = m_ppm_filePath + mtl_name;

        // Open the file
        mtl_file.open(mtl_fileName);

        if (mtl_file.is_open()) {
            //Parse the file looking for the ppm image filepath
            while(std::getline(mtl_file, mtl_line)) {
                mtl_ss.clear();
                mtl_ss.str(mtl_line);
                mtl_ss >> mtl_prefix;

                // Store the ppm image name under texture filepath to be used later
                if (mtl_prefix == "map_Kd") {
                    mtl_ss >> ppm_fileName;
                    // Save the file path
                    m_ppm_filePath += ppm_fileName;
                }
            }
            mtl_file.close();
        }
    }

    void parseVertex(std::stringstream& ss) {
        float temp_float1 = 0, temp_float2 = 0, temp_float3 = 0;
        ss >> temp_float1 >> temp_float2 >> temp_float3;
        vertices.push_back(temp_float1);
        vertices.push_back(temp_float2);
        vertices.push_back(temp_float3);
    }

    void parseVertexTexture(std::stringstream& ss) {
        float temp_float1 = 0, temp_float2 = 0;
        ss >> temp_float1 >> temp_float2;
        vertex_textures.push_back(temp_float1);
        vertex_textures.push_back(temp_float2);
    }

    void parseVertexNormal(std::stringstream& ss) {
        float temp_float1 = 0, temp_float2 = 0, temp_float3 = 0;
        ss >> temp_float1 >> temp_float2 >> temp_float3;
        normals.push_back(temp_float1);
        normals.push_back(temp_float2);
        normals.push_back(temp_float3);
    }

    void parseFace(std::stringstream& ss) {
        uint32_t temp_uint = 0;

        //stream for each individual string #//# etc
        for (int i = 0; i < VERTICES_PER_FACE; i++) {
            std::string str;
            ss >> str;

            //Store the first number from the stream into temp_gluint
            std::stringstream face(str);
            face >> temp_uint;

            temp_uint = temp_uint - 1;
            indices.push_back(temp_uint);
        }
    }
};

#endif
    

// //Constructor loads a filename with .obj extension
// class OBJModel {
//     public: 
    
//     // OBJ requirement
//     OBJModel(std::string fileName) {
//     //Load and parse an obj file to get its vertices data, normal,
//     //and face values

//         uint32_t temp_uint = 0;
//         float temp_float1 = 0, temp_float2 = 0, temp_float3 = 0;

//         // Save the file path
//         std::string ppmFilePath = fileName.substr(0, fileName.find_last_of("/"));
//         ppmFilePath.append("/");
//         std::string mtlFilePath = ppmFilePath;
//         std::cout << ppmFilePath << std::endl;
    
//         //If no file name is given, end the program
//         std::stringstream ss;
//         std::ifstream inputFile;
//         std::string line = "";
//         std::string prefix = "";
//         inputFile.open(fileName);
//         if(inputFile.is_open()){

//             //Read one line at a time
//             while(std::getline(inputFile, line)) {

//                 //Get the prefix of the line
//                 ss.clear();
//                 ss.str(line);
//                 ss >> prefix;

//                 // Ignore comments
//                 if (prefix == "#") {
//                     continue;
//                 }

//                 // Intake of mtl file
//                 else if (prefix == "mtllib") {
//                     std::ifstream mtl_file;
//                     std::stringstream mtl_ss;
//                     std::string mtl_line = "";

//                     // Get the prefix of the mtl file line, then get the name of the file
//                     std::string mtl_prefix = "";
//                     std::string mtl_fileName;
//                     std::string ppm_fileName;

//                     // Open the file
//                     std::string mtl_name;
//                     ss >> mtl_name;
//                     mtl_fileName = mtlFilePath.append(mtl_name);
//                     mtl_file.open(mtl_fileName);

//                     if (mtl_file.is_open()) {
//                         //Parse the file looking for the ppm image filepath
//                         while(std::getline(mtl_file, mtl_line)) {
//                             mtl_ss.clear();
//                             mtl_ss.str(mtl_line);
//                             mtl_ss >> mtl_prefix;

//                             // Store the ppm image name under texture filepath to be used later
//                             if (mtl_prefix == "map_Kd") {
//                                 mtl_ss >> ppm_fileName;
//                                 ppmFilePath.append(ppm_fileName);
//                                 std::cout << ppmFilePath << std::endl;
//                                 m_ppm_filePath = ppmFilePath;
//                                 std::cout << m_ppm_filePath << std::endl;
//                             }
//                         }
//                     }
//                     mtl_file.close();
//                 }
	
//                 // Intake of vertex position (x,y,z) info from "v" prefixed lines
//                 else if (prefix == "v") {
//                     ss >> temp_float1 >> temp_float2 >> temp_float3;
//                     vertices.push_back(temp_float1);
//                     vertices.push_back(temp_float2);
//                     vertices.push_back(temp_float3);
//                 }    

//                 // Intake of vertex texture info "s" and "t" from "vt" prefixed lines
//                 else if (prefix == "vt") {
//                     ss >> temp_float1 >> temp_float2;
//                     vertex_textures.push_back(temp_float1);
//                     vertex_textures.push_back(temp_float2);
//                 }

//                 // Intake of vertex normal info from "vn" prefixed lines
//                 else if (prefix == "vn") {
//                     ss >> temp_float1 >> temp_float2 >> temp_float3;
//                     normals.push_back(temp_float1);
//                     normals.push_back(temp_float2);
//                     normals.push_back(temp_float3);
//                 }
	    
// 	            //If the line begins with "f" it is a face
// 	            else if (prefix == "f") {

// 		            //stream for each individual string #//# etc
// 		            std::stringstream face;


// 		            //Changed from while to for loop to avoid unnecessary fourth number (unclear why this happened)
// 		            for (int i=0; i<VERTICES_PER_FACE; i++) {

//                         //stream the string in from the line
//                         std::string str;
//                         ss >> str;
                    
//                         face.clear();
//                         face.str(str); 

//                         //Store the first number from the stream into temp_gluint
//                         face >> temp_uint;  

//                         temp_uint = temp_uint -1;
//                         indices.push_back(temp_uint);
// 		            }

	
// 	            }

//             }
    
//         }
//         inputFile.close();

//         else {
//             std::cerr << "Failed to open .obj file: " << fileName << std::endl;
//         }

//     }

//     //Returns vertex vector
// 	std::vector<float> getVertices() const { return vertices; }
// 	//Returns face triangle vector
// 	std::vector<uint32_t> getIndices() const { return indices; }


//     private:

//         // obj model parser will work strictly with triangulated meshes for now
//         static constexpr int VERTICES_PER_FACE = 3;

//         // Store, vector, normal, and face values
//         std::vector<float> vertices;
//         std::vector<float> normals;
//         std::vector<float> vertex_textures;
//         //PPM file path
//         std::string m_ppm_filePath;
//         std::vector<uint32_t> indices;
//         std::vector<uint32_t> vertex_texcoord_indices;
//         std::vector<uint32_t> vertex_normal_indices;
        

// };

// #endif
