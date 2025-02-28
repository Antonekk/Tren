#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <time.h>
#include <limits>
#include <filesystem>
#include<bits/stdc++.h> 


//==========================================GLM====================================================
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



#include <AGL3Window.hpp>
#include <AGL3Drawable.hpp>
#include "Map.hpp"
#include "utils.hpp"
#include "Sphere.hpp"



#define DEBUG
#define EARTH_RADIUS (6378.0f)



// ==========================================================================
// Window Main Loop Inits ...................................................
// ==========================================================================
class MyWin : public AGLWindow {
public:
    MyWin() {SetupMovement();};
    MyWin(int _wd, int _ht, const char *name, int vers, int fullscr=0)
        : AGLWindow(_wd, _ht, name, vers, fullscr) ,mainView(true),last_time(glfwGetTime()),delta_time(0.0f) {SetupMovement();};
    virtual void KeyCB(int key, int scancode, int action, int mods);
    void SetupCameraParams(float lon, float lat, float h);
    void CalculateDeltaTime();
    void CalculateTransformationMatrices();
    void SetupMovement();
    void MainLoop(Map Tmap);
    void UpdateFPS();


private:

   void SetPosition();
   bool mainView = true;
   double last_time;
   float delta_time;
   glm::vec3 position;
   float horizontalAngle,verticalAngle,initialFoV,speed,mouseSpeed, zoomSpeed;
   glm::mat4 ProjectionM, ViewM;

   // Current longitude, latitude and height for 3D map movement
   float current_lon, current_lat, current_height;

   /*==========================View 1=====================================*/
   float V1_zoom, V1_zoom_speed;
   glm::vec2 V1_offset;
   float V1speed;

   /*========================== LOD ======================================*/
   int LOD = 0;
   bool LOD_AUTO = true;
   int triangles = 0;

};


void MyWin::SetupCameraParams(float lon, float lat, float h){
   current_lon = lon;
   current_lat = lat;
   current_height = h;
   SetPosition();
}

void MyWin::SetPosition(){
   float cLatRad = DegreesToRadians(current_lat);
   float cLonRad = DegreesToRadians(current_lon);
   position = glm::vec3((EARTH_RADIUS + current_height) * cos(cLatRad) * cos(cLonRad),
      (EARTH_RADIUS + current_height) * cos(cLatRad) * sin(cLonRad),
      (EARTH_RADIUS + current_height) * sin(cLatRad));
}

void MyWin::UpdateFPS() {
    static double lastTime = glfwGetTime();
    static int frames = 0;

    double currentTime = glfwGetTime();
    frames++;

    if (currentTime - lastTime >= 1.0) { // One second passed
      double fps = frames / (currentTime - lastTime);

      if(LOD_AUTO == true){
         if (fps < 20 && LOD < 3) LOD++;
         else if (fps > 60 && LOD > 0) LOD--;  
      }

      std::cout << "FPS: " << fps<< " LOD: " << LOD  << " | Triangles: " << triangles << std::endl;

      lastTime = currentTime;
      triangles = 0;
      frames = 0;
    }
}

void MyWin::CalculateDeltaTime(){
   double current_time;
   current_time = glfwGetTime();
   delta_time = float(current_time - last_time);
   last_time = current_time;
}


void MyWin::SetupMovement(){
   V1_zoom = 1.0f;
   V1_zoom_speed = 0.2f;
   V1_offset = glm::vec2(0.0f, 0.0f);
   V1speed = 0.5f;

   horizontalAngle = 3.14;
   verticalAngle = 0.0f;


   initialFoV = 45.0f;
   speed = 0.5f;
   zoomSpeed = 6.0f;
   mouseSpeed = 0.0005f;
}

void MyWin::CalculateTransformationMatrices(){
   CalculateDeltaTime();

   // Get mouse position
   double mouse_xpos, mouse_ypos;
   glfwGetCursorPos(win(), &mouse_xpos, &mouse_ypos);

   //Reset mouse position to avoid exiting screen
   glfwSetCursorPos(win(), wd/2, ht/2);

   // Calculate angle change
   horizontalAngle += mouseSpeed * float(wd/2 - mouse_xpos);
   verticalAngle += mouseSpeed * float (ht/2 - mouse_ypos);


   verticalAngle = glm::clamp(verticalAngle, -glm::half_pi<float>() + 0.01f, glm::half_pi<float>() - 0.01f);

   glm::vec3 up = glm::normalize(position);

   glm::vec3 direction = glm::normalize(
      glm::vec3(
         -sin(horizontalAngle),               
         0.0f,                                
         -cos(horizontalAngle)     
      )
    );


    glm::vec3 right = glm::normalize(glm::cross(up, direction));

    direction = glm::normalize(glm::cross(right, up));



   //========================================= 3D View ========================================
	// Move forward
   if(mainView){
      if (glfwGetKey( win(), GLFW_KEY_W ) == GLFW_PRESS){
      current_lat += delta_time * speed;
      }
      // Move backward
      if (glfwGetKey( win(), GLFW_KEY_S ) == GLFW_PRESS){
         current_lat -= delta_time * speed;
      }
      // Move right
      if (glfwGetKey( win(), GLFW_KEY_D ) == GLFW_PRESS){
         current_lon += delta_time * speed;
      }
      // Move left
      if (glfwGetKey( win(), GLFW_KEY_A ) == GLFW_PRESS){
         current_lon -= delta_time * speed;
      }
      if(glfwGetKey( win(), GLFW_KEY_SPACE ) == GLFW_PRESS){
         current_height +=delta_time * zoomSpeed;
      }
      else if(glfwGetKey( win(), GLFW_KEY_LEFT_SHIFT ) == GLFW_PRESS){
         current_height -=delta_time * zoomSpeed;
      }
      SetPosition();
   }
   else{
      /*====================== 2D View ========================s*/
      if (glfwGetKey( win(), GLFW_KEY_UP ) == GLFW_PRESS){
         V1_offset.y += delta_time * V1speed;
      }
      if (glfwGetKey( win(), GLFW_KEY_DOWN ) == GLFW_PRESS){
         V1_offset.y -= delta_time * V1speed;
      }
      if (glfwGetKey( win(), GLFW_KEY_RIGHT ) == GLFW_PRESS){
         V1_offset.x += delta_time * V1speed;
      }
      if (glfwGetKey( win(), GLFW_KEY_LEFT ) == GLFW_PRESS){
         V1_offset.x -= delta_time * V1speed;
      }
      if(glfwGetKey( win(), GLFW_KEY_KP_ADD) == GLFW_PRESS){
         V1_zoom *= 1.0f + (delta_time * V1_zoom_speed);
      }
      if (glfwGetKey( win(), GLFW_KEY_KP_SUBTRACT ) == GLFW_PRESS){
         V1_zoom /= 1.0f + (delta_time * V1_zoom_speed);
      }
   }


   /*====================== LOD ======================*/
   if (glfwGetKey( win(), GLFW_KEY_0 ) == GLFW_PRESS){
      LOD_AUTO = true;
   }
   else if (glfwGetKey( win(), GLFW_KEY_1 ) == GLFW_PRESS){
      LOD_AUTO = false;
      LOD = 0;
   }
   else if (glfwGetKey( win(), GLFW_KEY_2 ) == GLFW_PRESS){
      LOD_AUTO = false;
      LOD = 1;
   }
   else if (glfwGetKey( win(), GLFW_KEY_3 ) == GLFW_PRESS){
      LOD_AUTO = false;
      LOD = 2;
   }
   else if (glfwGetKey( win(), GLFW_KEY_4 ) == GLFW_PRESS){
      LOD_AUTO = false;
      LOD = 3;
   }
   else if (glfwGetKey( win(), GLFW_KEY_5 ) == GLFW_PRESS){
      LOD_AUTO = false;
      LOD = 4;
   }

   /*====================== View Switch ====================== */

   if (glfwGetKey( win(), GLFW_KEY_LEFT_BRACKET ) == GLFW_PRESS){
      mainView = true;
   }
   else if (glfwGetKey( win(), GLFW_KEY_RIGHT_BRACKET ) == GLFW_PRESS){
      mainView = false;
   }

   ProjectionM = glm::perspective(glm::radians(initialFoV), (float) wd / (float) ht, 0.1f, 1000.0f);

	ViewM       = glm::lookAt(
								position,           // Camera is here
								position+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );
}



// ==========================================================================
void MyWin::KeyCB(int key, int scancode, int action, int mods) {
    AGLWindow::KeyCB(key,scancode, action, mods); // f-key full screen switch
}



// ==========================================================================
void MyWin::MainLoop(Map Tmap) {
   ViewportOne(0,0,wd,ht);

   // Set Mouse Imput Options
   glfwSetInputMode(win(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
   glfwPollEvents();
   glfwSetCursorPos(win(),wd/2 ,ht/2);

   V1_offset = glm::vec2(Tmap.Get_centerLon(), Tmap.Get_centerLat());


   Sphere Earth;
   Earth.setScale(glm::vec3(EARTH_RADIUS));
   Earth.setColor(glm::vec3(0.5f, 0.0f,0.0f));
   
   
   float aspect_ratio;


   //glDisable(GL_DEPTH_TEST);
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LESS); 
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


   do {
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
      AGLErrors("main-loopbegin");


      // Calculate aspect ratio
      aspect_ratio = static_cast<float>(ht) / wd;


      //printf("Pos : (%f,%f,%f)\n", position[0], position[1], position[2]);
      //====================================== MVP Calculation =================================================

      CalculateTransformationMatrices();

	   glm::mat4 model      = glm::mat4(1.0f);
      glm::mat4 MVP = ProjectionM * ViewM * model;

      if(mainView){
         triangles += Tmap.RenderMap3D(MVP, LOD);
         Earth.draw(MVP);
      }else{
         triangles += Tmap.RenderMap2D(V1_offset, V1_zoom, LOD, aspect_ratio);
      }

      //================================================= Determine If Game Is Finished ========================================
      if(False){
         printf("Over\n");
         glfwSetWindowShouldClose(win(), GLFW_TRUE);
      }

      UpdateFPS();

      
      glfwPollEvents();
      glfwSwapBuffers(win()); // =============================   Swap buffers
      //glfwWaitEvents();   


      
   } while( glfwGetKey(win(), GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
            glfwWindowShouldClose(win()) == 0 );
}

int main(int argc, char *argv[]) {


   /*
      Parse program data (args, then .hgt files into terrainMap object)
   */

   if(argc < 2) {
      std::cerr<< "Usage : [dir] | -lon [start longitude] [end longitude] | -lat [start latitude] [end latitude]" << std::endl;
      return -1;
   }

   std::string dir = argv[1]; 
   
   int lonMax, lonMin, latMin, latMax;
   lonMax = latMax = INT_MIN;
   lonMin = latMin = INT_MAX;

   int lonStart, lonEnd, latStart, latEnd;
   lonStart = latStart = INT_MIN;
   lonEnd = latEnd = INT_MAX;


   int lonCamera, latCamera;
   int heightCamera = 5;
   bool lonlatTrigg = false;

   for (int i = 2; i < argc; ++i) {
      if (std::string(argv[i]) == "-lon") {
         lonStart = std::stoi(argv[++i]);
         lonEnd = std::stoi(argv[++i]);
      } else if (std::string(argv[i]) == "-lat") {
         latStart = std::stoi(argv[++i]);
         latEnd = std::stoi(argv[++i]);
      }
      else if(std::string(argv[i]) == "-start"){
         lonCamera = std::stoi(argv[++i]);
         latCamera = std::stoi(argv[++i]);
         heightCamera = std::stoi(argv[++i]);
         lonlatTrigg = true;
      }
   }



   MyWin win;
   win.Init(800,800,"Tren",0,33);

   Map TMap(lonStart,lonEnd,latStart,latEnd);
   for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.path().extension() == ".hgt") {     
            std::string fileName = entry.path().filename().string();
            int lat = std::stoi(fileName.substr(1, 2)) * (fileName[0] == 'N' ? 1 : -1);
            int lon = std::stoi(fileName.substr(4, 3)) * (fileName[3] == 'E' ? 1 : -1);

            lonMax = std::max(lonMax, lon);
            lonMin = std::min(lonMin, lon);

            latMax = std::max(latMax, lat);
            latMin = std::min(latMin, lat);

            if (lon >= lonStart && lon <= lonEnd && lat >= latStart && lat <= latEnd) {
               //std::cout << lonStart << " | " << lonEnd << " | " << latStart << " | " << latEnd << std::endl;
               MapTile* tile (new MapTile(lon, lat));
               tile->ParseData(entry.path().string());
               TMap.AddTile(tile);
            }
        }
   }

   TMap.SetParams(lonMin, lonMax, latMin, latMax);
   TMap.CalculateMiddle();
   TMap.RenderDataInit();


   if(!lonlatTrigg){
      lonCamera = TMap.Get_centerLon();
      latCamera = TMap.Get_centerLat();
   }
   win.SetupCameraParams((float)lonCamera, (float)latCamera, (float)heightCamera);

   win.MainLoop(TMap);


   return 0;
}
