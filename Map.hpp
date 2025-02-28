#ifndef MAP_HPP
#define MAP_HPP

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <limits>
#include <cmath>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "AGL3Window.hpp"
#include "AGL3Drawable.hpp"


#define LOD_COUNT 5
#define HTG_DIM_SIZE 1201




class MapTile : public AGLDrawable {
    public: 
        MapTile(int lon, int lat, GLuint programId = 0) 
            : AGLDrawable(programId), lon(lon), lat(lat){ 
                SetShaders();}

        // Parse data from file to MapData vector
        void ParseData(const std::string dir);

        void SetShaders();

        // Call only after you've completed reading and seting data with functions above
        void RenderDataInit(float centerLat, float centerLon, float LatCos);

        int RenderMap2D(glm::vec2 offset, float zoom, GLuint LODebo, int count, float aspect_ratio);
        int RenderMap3D(glm::mat4 MVP, GLuint LODebo, int count);

        inline int lonGet() const {return lon;}
        inline int latGet() const {return lat;}

        std::vector<std::vector<int16_t>>* GetMapData() { return &MapData; }
    private: 


        float step;
        int lon, lat;
        std::vector<std::vector<int16_t>> MapData;

        // Rendering
        std::vector<GLshort> verticies;
};


class Map{

    public:
        Map(int lonS, int lonE, int latS, int latE) 
            : lonStart(lonS), lonEnd(lonE), latStart(latS), latEnd(latE) {
                Tiles.clear();
                for(int i = 1; i <= LOD_COUNT; i++){
                    GLuint LODebo;
                    glGenBuffers(1, &LODebo);
                    eboList.push_back(LODebo);
                }
            }

        // Add Tile to the map
        void AddTile(MapTile* tile);

        // Calculate middle points for lon and lat
        void CalculateMiddle();

        // Set all lon/lat paremeters
        void SetParams(int lonS, int lonE, int latS, int latE);

        inline float Get_centerLon() const {return centerLon;}
        inline float Get_centerLat() const {return centerLat;}


        // Call only after you've completed reading and seting data with functions above
        void RenderDataInit();

        int RenderMap2D(glm::vec2 offset, float zoom, int LOD, float aspect_ratio);
        int RenderMap3D(glm::mat4 MVP, int LOD);

    private: 
        std::vector<MapTile*> Tiles;
        std::vector<GLuint> eboList;
        std::vector<int> indexCounts;

        std::vector<GLuint> GenerateIndices(int step, std::vector<std::vector<int16_t>> *MapData);

        
        int lonStart, lonEnd;
        int latStart, latEnd;
        float centerLon, centerLat;

};



#endif