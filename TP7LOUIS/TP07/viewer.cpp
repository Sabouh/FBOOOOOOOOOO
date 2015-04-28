#include "viewer.h"

#include <math.h>
#include <iostream>
#include "meshLoader.h"
#include <QTime>

using namespace std;


Viewer::Viewer(char *filename,const QGLFormat &format)
  : QGLWidget(format),
    _timer(new QTimer(this)),
    _currentshader(0),
    _light(glm::vec3(0,0,1)),
    _mode(false) {

  setlocale(LC_ALL,"C");

  _mesh = new Mesh(filename);
  _cam  = new Camera(_mesh->radius,glm::vec3(_mesh->center[0],_mesh->center[1],_mesh->center[2]));

  _timer->setInterval(10);
  connect(_timer,SIGNAL(timeout()),this,SLOT(updateGL()));
}

Viewer::~Viewer() {
  delete _timer;
  delete _mesh;
  delete _cam;

  for(unsigned int i=0;i<_shaders.size();++i) {
    delete _shaders[i];
  }

  // delete all GPU objects  
  deleteTextures();
  deleteVAO(); 
  deleteFBO();
}

void Viewer::deleteTextures() {
  // delete loaded textures 
  glDeleteTextures(3,_texIds);
}

void Viewer::deleteVAO() {
  // delete your VAO here (function called in destructor)
  glDeleteBuffers(5,_buffers);
  glDeleteBuffers(1,&_quad);

  glDeleteVertexArrays(1,&_vaoObject);
  glDeleteVertexArrays(1,&_vaoQuad);
}

void Viewer::createFBO() {
  // Ids needed for the FBO
  glGenFramebuffers(1,&_fbo);
  glGenTextures(1,&_rendNormalId);
  glGenTextures(1,&_rendDepthId);
  glGenTextures(1,&_rendColorId);
}

void Viewer::initFBO() {

  // create the texture for rendering normals 
  glBindTexture(GL_TEXTURE_2D,_rendNormalId);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA32F,width(),height(),0,GL_RGBA,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // create the texture for rendering depth values
  glBindTexture(GL_TEXTURE_2D,_rendDepthId);
  glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT24,width(),height(),0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // create the texture for rendering color values
  glBindTexture(GL_TEXTURE_2D,_rendColorId);
  glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT24,width(),height(),0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // attach textures to framebuffer object 
  glBindFramebuffer(GL_FRAMEBUFFER,_fbo);
  glBindTexture(GL_TEXTURE_2D,_rendNormalId);
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,_rendNormalId,0);
  glBindTexture(GL_TEXTURE_2D,_rendDepthId);
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,_rendDepthId,0);
  glBindTexture(GL_TEXTURE_2D,_rendColorId);
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D,_rendColorId,0);
  glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void Viewer::deleteFBO() {
  // delete all FBO Ids
  glDeleteFramebuffers(1,&_fbo);
  glDeleteTextures(1,&_rendNormalId);
  glDeleteTextures(1,&_rendDepthId);
  glDeleteTextures(1,&_rendColorId);
}

void Viewer::loadTexture(GLuint id,const char *filename) {
  // load image 
  QImage image = QGLWidget::convertToGLFormat(QImage(filename));

  // activate texture 
  glBindTexture(GL_TEXTURE_2D,id);
  
  // set texture parameters 
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_MIRRORED_REPEAT);
  
  // store texture in the GPU
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,image.width(),image.height(),0,
  	       GL_RGBA,GL_UNSIGNED_BYTE,(const GLvoid *)image.bits());
  
  // generate mipmaps 
  glGenerateMipmap(GL_TEXTURE_2D);
}

void Viewer::createTextures() {
  // enables 2D textures 
  glEnable(GL_TEXTURE_2D);

  // generate 3 texture ids 
  glGenTextures(3,_texIds);

  // load all needed textures 
  loadTexture(_texIds[0],"../TP07/textures/normal01.jpg");
  loadTexture(_texIds[1],"../TP07/textures/normal02.jpg");
  loadTexture(_texIds[2],"../TP07/textures/normal03.jpg");
}

void Viewer::createVAO() {
  // create VAO
  glGenVertexArrays(1,&_vaoObject);

  // create 4 associated VBOs (for positions, normals, coords and face indices)
  glGenBuffers(5,_buffers);

  // bind VAO 
  glBindVertexArray(_vaoObject);
  
  // send and enable positions 
  glBindBuffer(GL_ARRAY_BUFFER,_buffers[0]);
  glBufferData(GL_ARRAY_BUFFER,_mesh->nb_vertices*3*sizeof(float),_mesh->vertices,GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
  glEnableVertexAttribArray(0);

  // send and enable normals 
  glBindBuffer(GL_ARRAY_BUFFER,_buffers[1]);
  glBufferData(GL_ARRAY_BUFFER,_mesh->nb_vertices*3*sizeof(float),_mesh->normals,GL_STATIC_DRAW);
  glVertexAttribPointer(1,3,GL_FLOAT,GL_TRUE,0,(void *)0);
  glEnableVertexAttribArray(1);

  // send and enable tangents
  glBindBuffer(GL_ARRAY_BUFFER,_buffers[2]);
  glBufferData(GL_ARRAY_BUFFER,_mesh->nb_vertices*3*sizeof(float),_mesh->tangents,GL_STATIC_DRAW);
  glVertexAttribPointer(2,3,GL_FLOAT,GL_TRUE,0,(void *)0);
  glEnableVertexAttribArray(2);

  // send and enable coords 
  glBindBuffer(GL_ARRAY_BUFFER,_buffers[3]);
  glBufferData(GL_ARRAY_BUFFER,_mesh->nb_vertices*2*sizeof(float),_mesh->coords,GL_STATIC_DRAW);
  glVertexAttribPointer(3,2,GL_FLOAT,GL_FALSE,0,(void *)0);
  glEnableVertexAttribArray(3);

  // send faces 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_buffers[4]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,_mesh->nb_faces*3*sizeof(unsigned int),_mesh->faces,GL_STATIC_DRAW);


  // create the VBO associated with a simple quad
  static const GLfloat quadData[] = { 
    -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,
  };
  
  glGenVertexArrays(1,&_vaoQuad);
  glGenBuffers(1,&_quad);

  glBindVertexArray(_vaoQuad);

  glBindBuffer(GL_ARRAY_BUFFER,_quad);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadData),quadData,GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
  glEnableVertexAttribArray(0);

  // back to normal
  glBindVertexArray(0);
}

void Viewer::createShaders() {
  // add your own shader files here 

  // *** shader for the first pass *** 
  _vertexFilenames.push_back("../TP07/shaders/first-pass.vert");
  _fragmentFilenames.push_back("../TP07/shaders/first-pass.frag");
  // ******************************

  // *** shader for the second pass *** 
  _vertexFilenames.push_back("../TP07/shaders/second-pass.vert");
  _fragmentFilenames.push_back("../TP07/shaders/second-pass.frag");
  // ******************************
}

void Viewer::drawObject(const glm::vec3 &pos,const glm::vec3 &col,int textureType) {
 
  // send uniform (constant) variables to the shader 
  glm::mat4 mdv = glm::translate(_cam->mdvMatrix(),pos);
  glUniformMatrix4fv(glGetUniformLocation(_shaders[0]->id(),"mdvMat"),1,GL_FALSE,&(mdv[0][0]));
  glUniformMatrix4fv(glGetUniformLocation(_shaders[0]->id(),"projMat"),1,GL_FALSE,&(_cam->projMatrix()[0][0]));
  glUniformMatrix3fv(glGetUniformLocation(_shaders[0]->id(),"normalMat"),1,GL_FALSE,&(_cam->normalMatrix()[0][0]));
  glUniform3fv(glGetUniformLocation(_shaders[0]->id(),"color"),1,&(col[0]));

  // send textures
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,_texIds[textureType]);
  glUniform1i(glGetUniformLocation(_shaders[0]->id(),"normalmap"),0);


  // activate faces and draw!
  glBindVertexArray(_vaoObject);
  glDrawElements(GL_TRIANGLES,3*_mesh->nb_faces,GL_UNSIGNED_INT,(void *)0);
  glBindVertexArray(0);
}

void Viewer::paintGL() {
  // TODO: bind (activate) the created framebuffer object
    glBindFramebuffer(GL_FRAMEBUFFER,_fbo);

  // activate the shader 
  glUseProgram(_shaders[0]->id());

  // clear buffers 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // draw multiple objects 
  const float r = _mesh->radius*2.5;
  const int   v = 5;
  for(int i=-v;i<=v;++i) {
    for(int j=-v;j<=v;++j) {
      drawObject(glm::vec3(i*r,0,j*r),glm::vec3((float)(i+v)/(float)(2*v+1),0.5,(float)(j+v)/(float)(2*v+1)),(i+v)%3);
    }
  }

  // TODO: unbind (desactivate) the created framebuffer object and apply the second pass 
  glBindFramebuffer(GL_FRAMEBUFFER,0);
  glUseProgram(_shaders[1]->id());

  // clear buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D,_rendNormalId);
  glUniform1i(glGetUniformLocation(_shaders[1]->id(),"inputmap"),1);

  glUniform3fv(glGetUniformLocation(_shaders[1]->id(),"light"),1,&(_light[0]));


  glBindVertexArray(_vaoQuad);
  glDrawArrays(GL_TRIANGLES,0,6);
  glBindVertexArray(0);

  // disable shader 
  glUseProgram(0);

}

void Viewer::resizeGL(int width,int height) {
  _cam->initialize(width,height,false);
  glViewport(0,0,width,height);
  initFBO();
  updateGL();
}

void Viewer::mousePressEvent(QMouseEvent *me) {
  const glm::vec2 p((float)me->x(),(float)(height()-me->y()));

  if(me->button()==Qt::LeftButton) {
    _cam->initRotation(p);
    _mode = false;
  } else if(me->button()==Qt::MidButton) {
    _cam->initMoveZ(p);
    _mode = false;
  } else if(me->button()==Qt::RightButton) {
    _light[0] = (p[0]-(float)(width()/2))/((float)(width()/2));
    _light[1] = (p[1]-(float)(height()/2))/((float)(height()/2));
    _light[2] = 1.0f-std::max(fabs(_light[0]),fabs(_light[1]));
    _light = glm::normalize(_light);
    _mode = true;
  } 

  updateGL();
}

void Viewer::mouseMoveEvent(QMouseEvent *me) {
  const glm::vec2 p((float)me->x(),(float)(height()-me->y()));
 
  if(_mode) {
    // light mode
    _light[0] = (p[0]-(float)(width()/2))/((float)(width()/2));
    _light[1] = (p[1]-(float)(height()/2))/((float)(height()/2));
    _light[2] = 1.0f-std::max(fabs(_light[0]),fabs(_light[1]));
    _light = glm::normalize(_light);
  } else {
    // camera mode
    _cam->move(p);
  }

  updateGL();
}

void Viewer::keyPressEvent(QKeyEvent *ke) {
  
  // key a: play/stop animation
  if(ke->key()==Qt::Key_A) {
    if(_timer->isActive()) 
      _timer->stop();
    else 
      _timer->start();
  }

  // key i: init camera
  if(ke->key()==Qt::Key_I) {
    _cam->initialize(width(),height(),true);
  }
  
  // key f: compute FPS
  if(ke->key()==Qt::Key_F) {
    int elapsed;
    QTime timer;
    timer.start();
    unsigned int nb = 500;
    for(unsigned int i=0;i<nb;++i) {
      paintGL();
    }
    elapsed = timer.elapsed();
    double t = (double)nb/((double)elapsed);
    cout << "FPS : " << t*1000.0 << endl;
  }

  // key r: reload shaders 
  if(ke->key()==Qt::Key_R) {
    for(unsigned int i=0;i<_vertexFilenames.size();++i) {
      _shaders[i]->reload(_vertexFilenames[i].c_str(),_fragmentFilenames[i].c_str());
    }
  }

  updateGL();
}

void Viewer::initializeGL() {
  // make this window the current one
  makeCurrent();

  // init and chack glew
  if(glewInit()!=GLEW_OK) {
    cerr << "Warning: glewInit failed!" << endl;
  }

  if(!GLEW_ARB_vertex_program   ||
     !GLEW_ARB_fragment_program ||
     !GLEW_ARB_texture_float    ||
     !GLEW_ARB_draw_buffers     ||
     !GLEW_ARB_framebuffer_object) {
    cerr << "Warning: Shaders not supported!" << endl;
  }

  // init OpenGL settings
  glClearColor(0.0,0.0,0.0,1.0);
  glEnable(GL_DEPTH_TEST);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glViewport(0,0,width(),height());

  // initialize camera
  _cam->initialize(width(),height(),true);

  // load shader files
  createShaders();

  // init and load all shader files
  for(unsigned int i=0;i<_vertexFilenames.size();++i) {
    _shaders.push_back(new Shader());
    _shaders[i]->load(_vertexFilenames[i].c_str(),_fragmentFilenames[i].c_str());
  }

  // init VAO
  createVAO();
  
  // init textures 
  createTextures();
  
  // create/init FBO
  createFBO();
  initFBO();

  // starts the timer 
  _timer->start();
}

