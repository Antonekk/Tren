// ==========================================================================
// AGL3:  GL/GLFW init AGLWindow and AGLDrawable class definitions
//
// Ver.3  14.I.2020 (c) A. Łukaszewski
// ==========================================================================
// AGLWindow.cpp
//===========================================================================
#include <unistd.h>


#include <stdexcept>
#include <iostream>
#include <vector>

using namespace std;

#include <AGL3Window.hpp>

void AGLErrors(const char *comment) {
   GLenum er;
   while (er=glGetError())
      fprintf(stderr, "\nOpenGL ERROR: 0x%04x    =============%s===\n", er, comment);
}

AGLWindow::~AGLWindow() {
   glfwDestroyWindow(window);
   glfwTerminate();
}

float AGLWindow::Viewport(int _vtx,int _vty,int _vwd,int _vht) {
   glViewport(_vtx,_vty,_vwd,_vht);
   return float(_vwd)/float(_vht);
}

float AGLWindow::ViewportOne(int _vtx,int _vty,int _vwd,int _vht) {
   vtx=_vtx;
   vty=_vty;
   vwd=_vwd;
   vht=_vht;
   glViewport(_vtx,_vty,_vwd,_vht);
   return float(_vwd)/float(_vht);
}

void AGLWindow::GetCursorNDC(float *x, float *y) {
   double xpos, ypos;
   glfwGetCursorPos(window, &xpos, &ypos);
   *x = 2.0*(xpos   -vtx)/vwd - 1.0;
   *y = 2.0*(ht-ypos-vty)/vht - 1.0;
}

void AGLWindow::Resize(int _wd, int _ht) {
   // Ustawienie WxH na mniejsze z podanych (chemy żeby kwadrat mieścił się w okienku)
   wd = _wd;
   ht = _ht; 


   aspect = wd/ht;  

   ViewportOne(0,0, (GLsizei)wd, (GLsizei)ht);
}
void AGLWindow::KeyCB(int key, int scancode, int action, int mods) {
   if (key == GLFW_KEY_F  && action == GLFW_PRESS) {
      SetFullScreen( !IsFullScreen() );
   }
}

void AGLWindow::MouseButtonCB(int button,    int action, int mods){}

void AGLWindow::ScrollCB(double xp, double yp){}

void AGLWindow::MousePosCB(double xp, double yp){}


void AGLWindow::MainLoop ( void ) {
   while (  glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
            glfwWindowShouldClose(window) == 0 ) {

      // ..... render the scene

      glfwSwapBuffers(window);
      glfwPollEvents();
   }
}

void AGLWindow::WaitForFixedFPS(float frame_time){
   double wait_time= frame_time - (glfwGetTime()-prev_time);
   if (wait_time>0.0)
       usleep(1000000*wait_time); //before swapBuf.. and time sync 60Hz..
   prev_time=glfwGetTime();
}

//===========================================================================
// Private functions:
//===========================================================================


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
   glViewport(0, 0, width, height);
}

// Init now not private:
void AGLWindow::Init(int width, int height, const char *name, int fullscr, int vers) {
   if(  !glfwInit() ) {
      glfwTerminate();
      throw std::runtime_error( "Error initializing glfw   !" );
   }
   glfwWindowHint(GLFW_SAMPLES, 4);
   glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);
   if (vers>32) {
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, vers / 10);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, vers % 10);
      glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
   }
   GLFWmonitor  ** mons;
   int count;
   mons = glfwGetMonitors(&count);
   monitors[0]=mons[0];
   monitors[1]=mons[count-1];
   monitor=(fullscr ? monitors[fullscr-1] : monitors[0] );

   if (width==0 || height==0 || fullscr) {
      // if 0 or fullscr=1 or 2 than WINDOWLESS FULL SCREEN :
      const GLFWvidmode* mode = glfwGetVideoMode(monitor);
      /*    glfwWindowHint(GLFW_RED_BITS,     mode->redBits);
       *    glfwWindowHint(GLFW_GREEN_BITS,   mode->greenBits);
       *    glfwWindowHint(GLFW_BLUE_BITS,    mode->blueBits);
       *    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
       */
      window = glfwCreateWindow(mode->width, mode->height, name, monitor, nullptr);
      winSize[0]=width, winSize[1]=height;
   } else {
      // Open a window and create its OpenGL context
      window = glfwCreateWindow( width, height, name, nullptr, nullptr);
      glfwGetWindowSize( window, &winSize[0], &winSize[1] );
   }
   if ( window == nullptr ) {
      glfwTerminate();
      throw std::runtime_error( "Error initializing window !" );
   }
   glfwMakeContextCurrent( window );

   glfwSetWindowUserPointer ( window, this );
   glfwSetWindowSizeCallback( window, AGLWindow::CallbackResize );
   glfwSetKeyCallback(        window, AGLWindow::CallbackKey );
   glfwSetMouseButtonCallback(window, AGLWindow::CallbackMouseButton );
   glfwSetScrollCallback(     window, AGLWindow::CallbackScroll );
   glfwSetCursorPosCallback(  window, AGLWindow::CallbackMousePos );
   glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

   glfwGetWindowPos(  window, &winPos[0],  &winPos[1] );
   // Resize() begining:
   wd=width;
   ht=height;
   aspect=float(wd)/ht;
   // Ensure we can capture the escape key being pressed below
   //glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
   glfwSwapInterval(1);  // 60 fps ?
   glEnable(GL_MULTISAMPLE);
}

void AGLWindow::CallbackResize(GLFWwindow* window, int cx, int cy) {
   void *ptr = glfwGetWindowUserPointer( window );
   if ( AGLWindow *winPtr = static_cast<AGLWindow*>( ptr ) ){
      winPtr->Resize( cx, cy );
   }
      
}

void AGLWindow::CallbackKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
   void *ptr = glfwGetWindowUserPointer( window );
   if ( AGLWindow *winPtr = static_cast<AGLWindow*>( ptr ) )
      winPtr->KeyCB( key, scancode, action, mods );
}

void AGLWindow::CallbackMouseButton(GLFWwindow* window, int button, int action, int mods) {
   void *ptr = glfwGetWindowUserPointer( window );
   if ( AGLWindow *winPtr = static_cast<AGLWindow*>( ptr ) )
      winPtr->MouseButtonCB( button, action, mods );
}

void AGLWindow::CallbackScroll(GLFWwindow* window, double xp, double yp) {
   void *ptr = glfwGetWindowUserPointer( window );
   if ( AGLWindow *winPtr = static_cast<AGLWindow*>( ptr ) )
      winPtr->ScrollCB( xp, yp );
}

void AGLWindow::CallbackMousePos(GLFWwindow* window, double xp, double yp) {
   void *ptr = glfwGetWindowUserPointer( window );
   if ( AGLWindow *winPtr = static_cast<AGLWindow*>( ptr ) )
      winPtr->MousePosCB( xp, yp );
}

bool AGLWindow::IsFullScreen( void ) {
   return glfwGetWindowMonitor( window ) != nullptr;
}

// Proacownia 7:  problem with old glfw -> no function glfwSetWindowMointor
#define PRACOWNIA
void AGLWindow::SetFullScreen( bool fullscreen ) {
   if ( IsFullScreen() == fullscreen )
      return;
   if ( fullscreen ) {
      glfwGetWindowPos(  window, &winPos[0],  &winPos[1] );
      glfwGetWindowSize( window, &winSize[0], &winSize[1] );

      const GLFWvidmode * mode = glfwGetVideoMode(monitor);
#ifndef PRACOWNIA   
      glfwSetWindowMonitor( window, monitor, 0, 0, mode->width, mode->height, 0 );
#endif   
   } else {
#ifndef PRACOWNIA   
      glfwSetWindowMonitor( window, nullptr,  winPos[0], winPos[1], winSize[0], winSize[1], 0 );
#endif   
      glfwShowWindow(window);
   }
   glfwGetFramebufferSize( window, &wd, &ht );
   Resize(wd, ht);
}
