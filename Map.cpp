

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <bits/stdc++.h>

#include "Map.hpp"




void MapTile::ParseData(const std::string dir)
{

    std::ifstream file(dir, std::ios::in|std::ios::binary);

    if(!file)
    {
        std::cerr << "Error opening file: " << dir <<std::endl;
        exit(-1);
    }

    MapData.resize(HTG_DIM_SIZE, std::vector<int16_t>(HTG_DIM_SIZE, 0));

    for (int i = 0; i < HTG_DIM_SIZE; ++i) {
        for(int j = 0; j < HTG_DIM_SIZE; ++j){
            uint16_t value;
            file.read(reinterpret_cast<char*>(&value), sizeof(value));
            MapData[i][j] = static_cast<int16_t>((value >> 8) | (value << 8)); // Swap bytes
        }
    }


    auto res = std::accumulate(MapData.begin(), MapData.end(), std::make_pair(MapData[0][0], MapData[0][0]),
    [](const auto& current, const auto& v) {
        auto minmax = std::minmax_element(v.begin(), v.end());
        return std::make_pair(std::min(current.first, *minmax.first),
                              std::max(current.second, *minmax.second));
    });

    std::cout << res.first << " | " << res.second << std::endl;
    std::flush(std::cout);



    file.close();
    return;
}


void MapTile::SetShaders()
{
    
    compileShadersFromFile("Shaders/Map2d/Map2d.vert","Shaders/Map2d/Map2d.frag");
    compileShadersFromFile("Shaders/Map3d/Map3d.vert","Shaders/Map3d/Map3d.frag");
}




void MapTile::RenderDataInit(float centerLat, float centerLon, float LatCos)
{
    step = LatCos;
    verticies.clear();
    float latSize = 1.0f / HTG_DIM_SIZE * HTG_DIM_SIZE;
    float lonSize = LatCos/ HTG_DIM_SIZE * HTG_DIM_SIZE;

    for(int i = 0 ; i < HTG_DIM_SIZE; i++){
        for(int j = 0; j < HTG_DIM_SIZE; j++){
            float z = MapData[i][j];
            verticies.push_back(z);

            //std::cout<<"( " << x <<", " << y << ", " << z << ")" <<std::endl;
        }
    }

    bindBuffers();
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLshort) * verticies.size(), verticies.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 1, GL_SHORT, GL_FALSE, sizeof(GLshort), (void*)0 );
    glEnableVertexAttribArray(0);

    
}

int MapTile::RenderMap2D(glm::vec2 offset, float zoom, GLuint LODebo, int count, float aspect_ratio)
{

    bindProgram(0);
    bindVAO();

    GLuint colorLoc = glGetUniformLocation(p(0), "offset");
    glUniform2f(colorLoc, offset.x, offset.y);

    GLuint zoomLoc = glGetUniformLocation(p(0), "zoom");
    glUniform1f(zoomLoc, zoom );

    GLuint lonLoc = glGetUniformLocation(p(0), "lon");
    glUniform1f(lonLoc, lon );

    GLuint latLoc = glGetUniformLocation(p(0), "lat");
    glUniform1f(latLoc, lat );

    GLuint stepLoc = glGetUniformLocation(p(0), "step");
    glUniform1f(stepLoc, step );

    GLuint asspectLoc = glGetUniformLocation(p(0), "aspect");
    glUniform1f(asspectLoc, aspect_ratio );

    //std::cout<<offset.x << " | "<<offset.y  << "Zoom: " << zoom << "| LOD: "<< std::endl;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, LODebo);
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);

    return count/3;
}

int MapTile::RenderMap3D(glm::mat4 MVP, GLuint LODebo, int count)
{

    bindProgram(1);
    bindVAO();

    GLuint viewLoc = glGetUniformLocation(p(1), "MVP");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &MVP[0][0]);


    GLuint lonLoc = glGetUniformLocation(p(1), "lon");
    glUniform1f(lonLoc, lon );

    GLuint latLoc = glGetUniformLocation(p(1), "lat");
    glUniform1f(latLoc, lat );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, LODebo);
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
    return count /3;
}

/*

=========================================== Map ===========================================

*/


std::vector<GLuint> Map::GenerateIndices(int step, std::vector<std::vector<int16_t>> *MapData)
{
    std::vector<GLuint> indices;

    for (GLuint i = 0; i < HTG_DIM_SIZE - step; i += step) {
        for (GLuint j = 0; j < HTG_DIM_SIZE - step; j += step) {
            GLuint idx = i * HTG_DIM_SIZE + j;

            // Heights of the vertices
            float h1 = (*MapData)[i][j];                   
            float h2 = (*MapData)[i][j + step];            
            float h3 = (*MapData)[i + step][j];  
            float h4 = (*MapData)[i + step][j + step];


            if (h1 >= -5200 && h2 >= -500 && h3 >= -500) {
                indices.push_back(idx);                 
                indices.push_back(idx + step);          
                indices.push_back(idx + step * HTG_DIM_SIZE); 
            }

            // Check if all vertices of Triangle 2 are valid
            if (h2 >= -500 && h3 >= -500 && h4 >= -500) {
                indices.push_back(idx + step);          
                indices.push_back(idx + (step * HTG_DIM_SIZE) + step); 
                indices.push_back(idx + step * HTG_DIM_SIZE);         
            }
        }
    }

    return indices;
}


void Map::AddTile(MapTile* tile)
{
    Tiles.push_back(tile);
}

void Map::CalculateMiddle()
{
    if(!Tiles.empty()){
      centerLat = (latEnd + latStart) / 2.0f;
      centerLon = (lonEnd + lonStart) / 2.0f;
   }
}

void Map::SetParams(int lonS, int lonE, int latS, int latE)
{
    lonStart = lonS;
    lonEnd = lonE;
    latStart = latS;
    latEnd = latE;
}



void Map::RenderDataInit()
{

    float LatCos = std::cos(centerLat * M_PI /180.0f); // degrees * PI / 180 -> Convert degrees to radians

    for(int i = 0; i < Tiles.size(); i++){
        Tiles[i]->RenderDataInit(centerLat, centerLon, LatCos);
    }

    for(int i = 1; i <= LOD_COUNT; i++){
        std::vector<GLuint> indices = GenerateIndices(i, Tiles[i-1]->GetMapData());
        indexCounts.push_back(indices.size());

        GLuint LODebo = eboList[i - 1];
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, LODebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    }


}

int Map::RenderMap2D(const glm::vec2 offset, float zoom, int LOD, float aspect_ratio)
{
    int triangles = 0;
    for(int i = 0; i < Tiles.size(); i++){
        triangles += Tiles[i]->RenderMap2D(offset, zoom, eboList[LOD], indexCounts[LOD], aspect_ratio);
    }
    return triangles;
    
}

int Map::RenderMap3D(glm::mat4 MVP, int LOD)
{
    int triangles = 0;
    for(int i = 0; i < Tiles.size(); i++){
        triangles += Tiles[i]->RenderMap3D(MVP,eboList[LOD], indexCounts[LOD]);
    }
    return triangles;
}
