#ifndef VIEWER_H
#define VIEWER_H

// GLEW lib: needs to be included first!!
#include <GL/glew.h> 

// OpenGL library 
#include <GL/gl.h>

// OpenGL Utility library
#include <GL/glu.h>

// OpenGL Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <QGLFormat>
#include <QGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include <stack>

#include "camera.h"
#include "meshLoader.h"
#include "shader.h"

class Viewer : public QGLWidget {
 public:
  Viewer(char *filename,
	 const QGLFormat &format=QGLFormat::defaultFormat());
  ~Viewer();
  
 protected :
  virtual void paintGL();
  virtual void initializeGL();
  virtual void resizeGL(int width,int height);
  virtual void keyPressEvent(QKeyEvent *ke);
  virtual void mousePressEvent(QMouseEvent *me);
  virtual void mouseMoveEvent(QMouseEvent *me);

 private:
  void loadTexture(GLuint id,const char *filename);
  void createShaders();

  void createVAO();
  void deleteVAO();

  void createTextures();
  void deleteTextures();

  void createFBO();
  void deleteFBO();
  void initFBO();

  void drawObject(const glm::vec3 &pos,const glm::vec3 &col,int textureType);

  QTimer        *_timer;    // timer that controls the animation
  unsigned int   _currentshader; // current shader index

  Mesh   *_mesh;   // the mesh
  Camera *_cam;    // the camera

  glm::vec3 _light; // light direction
  bool      _mode;  // camera motion or light motion

  std::vector<std::string> _vertexFilenames;   // all vertex filenames
  std::vector<std::string> _fragmentFilenames; // all fragment filenames
  std::vector<Shader *>    _shaders;           // all the shaders 

  // vbo ids 
  GLuint _vaoObject;
  GLuint _vaoQuad;
  GLuint _buffers[5];
  GLuint _quad;

  // texture ids 
  GLuint _texIds[3];

  // render texture ids 
  GLuint _rendNormalId;
  GLuint _rendDepthId;
  GLuint _rendColorId;

  // fbo id
  GLuint _fbo;
};

#endif // VIEWER_H
