// ==========================================================================
// AGL3:  GL/GLFW init AGLWindow and AGLDrawable class definitions
//
// Ver.3  14.I.2020 (c) A. Łukaszewski
// ==========================================================================
// AGLWindow
//===========================================================================
#include <epoxy/gl.h>
#include <epoxy/glx.h>
#include <GLFW/glfw3.h>

#include <array>

void AGLErrors(const char *comment); // Prints  Comment and error

#ifndef AGL3WINDOW_HPP
#define AGL3WINDOW_HPP

class AGLWindow {
public:
   AGLWindow(){};
   AGLWindow(int _wd, int _ht, const char *name, int fullscr=0, int vers=33)
   {  Init(_wd,_ht,name,vers,fullscr);  };          // fullscr=1/2 vers=33
   void Init(int _wd, int _ht, const char *name, int fullscr=0, int vers=33);
   ~AGLWindow();
   GLFWwindow* win() {  return window;  };

   int   wd,ht;                                       // Window dimensions
   float aspect;                                      // Window aspect ration

   float Viewport(   int _vtx,int _vty,int _vwd,int _vht);//return aspect ratio
   float ViewportOne(int _vtx,int _vty,int _vwd,int _vht);//store vp, return aspect
   void  GetCursorNDC(float *x, float *y);            // mouse position VpOne

   // Handles key 'f' for switching fullscreen mode:
   virtual void Resize( int _wd, int _ht ); // Sets Win:wd,ht,aspec +glViewp
   virtual void KeyCB(int key, int scancode, int action, int mods);
   virtual void MouseButtonCB(int button,    int action, int mods);
   virtual void ScrollCB(double xp, double yp);
   virtual void MousePosCB(double xp, double yp);

   void MainLoop ( void );
   
   void WaitForFixedFPS(float frame_time=1./60.); // Use before glfwSwapBuffers()
//===========================================================================

private:
   bool IsFullScreen(  void );
   void SetFullScreen( bool fullscreen );

   int vtx, vty, vwd, vht; // GtCursorNDC() and ViewportOne()

   // vers=33 for GLSL 3.3, fullscr=0 for window or 1/2 for FullScr 1/2 monitor
   GLFWwindow  *        window         = nullptr;
   GLFWmonitor *        monitor        = nullptr;
   std::array< int, 2 > winPos         = {0, 0};
   std::array< int, 2 > winSize        = {0, 0};

   std::array< int, 2 > vwpSize        = {0, 0};
   bool                 updateViewport = true;
   std::array<GLFWmonitor *,2> monitors= {nullptr, nullptr}; // first and last m.

   double prev_time=0.0; // for fixed FPS
   
   static void CallbackResize(GLFWwindow* window, int cx, int cy);
   static void CallbackKey(GLFWwindow* window, int key, int scancode, int action, int mods);
   static void CallbackMouseButton(GLFWwindow* window, int button, int action, int mods);
   static void CallbackScroll(GLFWwindow* window, double xp, double yp);
   static void CallbackMousePos(GLFWwindow* window, double xp, double yp);
};

#endif