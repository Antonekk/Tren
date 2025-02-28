// ==========================================================================
// AGL3:  GL/GLFW init AGLWindow and AGLDrawable class definitions
//
// Ver.3  14.I.2020 (c) A. Łukaszewski
// ==========================================================================
// AGLDrawable
//===========================================================================
#include <vector>
#include <iostream>
#include <fstream>



#ifndef AGLDRAWABLE_HPP
#define AGLDRAWABLE_HPP

class AGLDrawable {
public:
   AGLDrawable(GLuint _pid=0) {
      pIds.clear();
      glGenVertexArrays(1, &vaoId);
      glGenBuffers(     1, &vboId);
      glGenBuffers(     1, &eboId);
      bindVAO();
   }
   ~AGLDrawable() {    // Cleanup VBO,VBO,Prog
#ifdef DEBUG
      fprintf(stderr,"AGL: destroying Drawable\n");
#endif
      glDeleteBuffers(1, &vboId);
      glDeleteBuffers(1, &eboId);
      glDeleteVertexArrays(1, &vaoId);
      if (!pIds.empty()) {
         for(const GLuint& i : pIds){
            glDeleteProgram(i);
         } 
      }
   }
   int compileShaders(const char *vs, const char *fs, const char *gs=NULL) {
      GLuint  v = glCreateShader(GL_VERTEX_SHADER);
      GLuint  f = glCreateShader(GL_FRAGMENT_SHADER);
      GLuint  g = 0;
      if (gs) g = glCreateShader(GL_GEOMETRY_SHADER);
      glShaderSource(v, 1, &vs, NULL);   // Also read from file: next fun
      glShaderSource(f, 1, &fs, NULL);   // ...
      if (gs) glShaderSource(g, 1, &gs, NULL);   // ...

      int res = compileShaders(v,f,g);
      return res;
   }
   int compileShadersFromFile(const char *vs, const char *fs, const char *gs=NULL) {
      GLuint  v = glCreateShader(GL_VERTEX_SHADER);
      GLuint  f = glCreateShader(GL_FRAGMENT_SHADER);
      GLuint  g = 0;
      if (gs) g = glCreateShader(GL_GEOMETRY_SHADER);
      getShaderSource(v, vs);
      getShaderSource(f, fs);
      if (gs) getShaderSource(g, gs);

      int res = compileShaders(v,f,g);
      return res;
   }
   void bindVAO() {
      glBindVertexArray(vaoId);
   }
   void bindBuffers() {
      glBindVertexArray(            vaoId);
      glBindBuffer(GL_ARRAY_BUFFER, vboId);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
   }
   void bindProgram(int p) {
      glUseProgram(pIds[p]);
   }
   GLuint p(int _pid) {
      return pIds[_pid];
   }

private:
   // VAO, VBO, Prog to clean in destructor
   GLuint vaoId, vboId, eboId, pId;
   std::vector<GLuint> pIds;

   int compileShaders(GLuint v, GLuint f, GLuint g=0) {
      GLint Result = GL_FALSE;
      if (g) Result = CompileLink(g, "GS");
      if ( Result = CompileLink(v, "VS") )
         if ( CompileLink(f, "FS") ) {
            GLuint pId = glCreateProgram();
            pIds.push_back(pId);
            glAttachShader(pId,v);
            glAttachShader(pId,f);
            if (g) glAttachShader(pId,g);
            CompileLink(pId, "Linking",  3);
         }
      glDeleteShader(v);
      glDeleteShader(f);
      if (g) glDeleteShader(g);
      return Result;
   }
   GLint CompileLink(GLuint v, const char *which, int prog=0) {
      GLint Result = GL_FALSE;
      int InfoLogLength;
      if (prog) {
         glLinkProgram(v);
         glGetProgramiv(v, GL_LINK_STATUS, &Result);
         glGetProgramiv(v, GL_INFO_LOG_LENGTH, &InfoLogLength);
      } else {
         glCompileShader(v);
         glGetShaderiv(v, GL_COMPILE_STATUS, &Result);
         glGetShaderiv(v, GL_INFO_LOG_LENGTH, &InfoLogLength);
      }
      if ( InfoLogLength > 0 && !Result ) {
         std::vector<char> Message(InfoLogLength+1);
         if (prog)
            glGetProgramInfoLog(v,InfoLogLength,NULL,&Message[0]);
         else
            glGetShaderInfoLog(v,InfoLogLength,NULL,&Message[0]);
         printf("%s: %s\n", which, &Message[0]);
      }
      return Result;
   }
   void getShaderSource(GLuint sId, const char * file) {
      std::string sCode;
      std::ifstream sStream(file, std::ios::in);
      if(sStream.is_open()) {
         std::string Line = "";
         while(getline(sStream, Line))
            sCode += "\n" + Line;
         sStream.close();
      } else {
         printf("Error opening file:  %s !\n", file);
         getchar();
         return ;
      }
      char const * SourcePointer = sCode.c_str();
      glShaderSource(sId, 1, &SourcePointer , NULL);
   }
};

#endif