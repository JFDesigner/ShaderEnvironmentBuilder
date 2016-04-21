#include "NGLScene.h"
#include "ParserLib.h"
#include "CebErrors.h"
#include <iostream>
#include <ngl/Vec3.h>
#include <ngl/Camera.h>
#include <ngl/Light.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Material.h>
#include <ngl/ShaderLib.h>
#include <iostream>
#include <typeinfo>
#include <QColorDialog>
#include <QString>

//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for x/y translation with mouse movement
//----------------------------------------------------------------------------------------------------------------------
const static float INCREMENT=0.01f;
//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for the wheel zoom
//----------------------------------------------------------------------------------------------------------------------
const static float ZOOM=0.1f;
//----------------------------------------------------------------------------------------------------------------------

bool checkCompileError(std::string _shaderProgName, QString *o_log)
{
  GLint isCompiled = 0;
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  GLuint shaderId = shader->getShaderID(_shaderProgName);

  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isCompiled);
  if(isCompiled == GL_FALSE)
  {
    GLint maxLength = 0;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);

    // The maxLength includes the NULL character
    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(shaderId, maxLength, &maxLength, &errorLog[0]);

    std::string s(errorLog.begin(), errorLog.end());

    QString errLog = QString(s.c_str());

    *o_log = errLog;

    // Provide the infolog in whatever manor you deem best.
    //throw ceb_error::openGL_list_error(_shaderProgName, errLog);
  }
  return isCompiled;
}

bool checkAllCompileError(std::vector<std::string> _shaderProgNames, QString *o_log)
{
  GLint isCompiled = GL_TRUE;
  QString temp_log;
  for (auto shaderProg: _shaderProgNames)
  {
    isCompiled &= checkCompileError(shaderProg, &temp_log);
    if (!isCompiled)
    {
      o_log->append(QString("%1:\n").arg(shaderProg.c_str()));
      o_log->append(temp_log);
    }
  }
  return isCompiled;
}


//----------------------------------------------------------------------------------------------------------------------
NGLScene::NGLScene( QWidget *_parent, parserLib *_libParent ) : QOpenGLWidget( _parent )
{
  // re-size the widget to that of the parent (in that case the GLFrame passed in on construction)
  m_rotate=false;
  // mouse rotation values set to 0
  m_spinXFace=0.0f;
  m_spinYFace=0.0f;
  m_parser= _libParent;
  // Store main window to send data from compile errors
  m_window = dynamic_cast<MainWindow*>(_parent);
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  this->resize(_parent->size());
  m_wireframe=false;
  m_fov=65.0;
  m_newJson= new Json();
  m_shaderManager = new ShaderManager();
  // set this widget to have the initial keyboard focus
  setFocus();
}

//----------------------------------------------------------------------------------------------------------------------
NGLScene::~NGLScene()
{
  delete m_newJson;
  //delete m_readFromXML;
  delete m_parser;
}

//----------------------------------------------------------------------------------------------------------------------
// This virtual function is called once before the first call to paintGL() or resizeGL(),
// and then once whenever the widget has been assigned a new QGLContext.
// This function should set up any required OpenGL context rendering flags, defining display lists, etc.
//----------------------------------------------------------------------------------------------------------------------
void NGLScene::initializeGL()
{
  ngl::NGLInit::instance();

  clearAllGlErrors();

  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);

  // create our camera
  ngl::Vec3 eye(0,1,1);
  ngl::Vec3 look(0,0,0);
  ngl::Vec3 up(0,1,0);
  m_cam.set(eye,look,up);
  setCamShape();
  // now to load the shader and set the values
  // grab an instance of shader manager
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  m_shaderManager->initialize(m_cam);
  if(!m_shaderManager->compileStatus())
  {
    m_window->updateTerminalText(m_shaderManager->getErrorLog());
  }
  if(m_shaderManager->isInit())
  {
    //now create our light this is done after the camera so we can pass the
    //transpose of the projection matrix to the light to do correct eye space
    //transformations
    ngl::Light light(ngl::Vec3(2,2,2),ngl::Colour(1,1,1,1),ngl::Colour(1,1,1,1),ngl::LightModes::POINTLIGHT);

    ngl::Mat4 iv=m_cam.getViewMatrix();
    iv.transpose();

    light.setTransform(iv);
    light.setAttenuation(1,0,0);
    light.enable();

    //load these values to the shader as well
    light.loadToShader("light");

    m_readFromXML->shaderData("WhyHelloThere", "PhongVertex", "shaders/PhongVertex.glsl", "PhongFragment", "shaders/PhongFragment.glsl");
    m_parser->assignAllData();
    std::cerr<<"Find number of active uniforms: "<<m_parser->m_num<<std::endl;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::exportUniforms()
{
  std::ofstream fileOut;
  fileOut.open("./tempFiles/ParsingOutput.txt");
  if(!fileOut.is_open())    ///If it can be opened
  {
    std::cerr<<"couldn't' open file\n";
    exit(EXIT_FAILURE);
  }
  for(uint i=0;i<m_parser->m_num;i++)
  {
    fileOut<<m_parser->m_uniformList[i]->getName()<<"\n";
    fileOut<<m_parser->m_uniformList[i]->getLocation()<<"\n";
    fileOut<<m_parser->m_uniformList[i]->getTypeName()<<"\n";
  }
  fileOut.close();
  // close files
  std::cout<<"EXPORTED\n"<<std::endl;
}



//----------------------------------------------------------------------------------------------------------------------
//This virtual function is called whenever the widget needs to be painted.
// this is our main drawing routine
//----------------------------------------------------------------------------------------------------------------------
void NGLScene::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if(m_wireframe == true)
  {
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  }
  else
  {
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  }

  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  (*shader)["Phong"]->use();

  // Rotation based on the mouse position for our global transform
  ngl::Mat4 rotX;
  ngl::Mat4 rotY;
  // create the rotation matrices
  rotX.rotateX(m_spinXFace);
  rotY.rotateY(m_spinYFace);
  // multiply the rotations
  m_mouseGlobalTX=rotY*rotX;
  // add the translations
  m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
  m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
  m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;

  m_cam.setShape(m_fov, m_aspect, 0.5f, 150.0f);

  loadMatricesToShader();
  ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
  prim->draw("teapot");


  m_text.reset(new ngl::Text(QFont ("Arial",18)));
  m_text->setScreenSize(width(),height());
  m_text->setColour(ngl::Colour (0.82,0.2,0.2));


  //m_text->renderText(10,18,"Error!");
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::resizeGL(QResizeEvent *_event)
{
  setCamShape();
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::resizeGL(int _w, int _h)
{
  setCamShape();
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::loadMatricesToShader()
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  (*shader)["Phong"]->use();

  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;
  ngl::Mat4 M;

  M=m_mouseGlobalTX;
  MV=  M*m_cam.getViewMatrix();
  MVP= M*m_cam.getVPMatrix();
  normalMatrix=MV;
  normalMatrix.inverse();


  m_parser->sendUniformsToShader(shader);
  shader->setShaderParamFromMat4("MV",MV);
  shader->setShaderParamFromMat4("MVP",MVP);
  shader->setShaderParamFromMat3("normalMatrix",normalMatrix);
  shader->setShaderParamFromMat4("M",M);
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::setCamShape()
{
  m_aspect=(float)width()/height();
  m_cam.setShape(m_fov, m_aspect, 0.5f, 150.0f);


}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // that method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quit
  //case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  // turn on wirframe rendering
  case Qt::Key_W : m_wireframe=true; break;
  // turn off wire frame
  case Qt::Key_S : m_wireframe=false; break;
  // show full screen
  case Qt::Key_F : showFullScreen(); break;
  // show windowed
  case Qt::Key_N : showNormal(); break;
  case Qt::Key_Space: m_parser->assignUniformValues();
  case Qt::Key_R : resetObjPos(); break;

  /*case Qt::Key_1 : m_parser->m_uniformList[0].m_vec4.m_x+=0.1;
  std::cout<<m_parser->m_uniformList[0].m_name<<":(x)  "<<m_parser->m_uniformList[0].m_vec4.m_x<<std::endl; break;

  case Qt::Key_2 : m_parser->m_uniformList[0].m_vec4.m_y+=0.1;
  std::cout<<m_parser->m_uniformList[0].m_name<<":(y)  "<<m_parser->m_uniformList[0].m_vec4.m_y<<std::endl; break;

  case Qt::Key_3 : m_parser->m_uniformList[0].m_vec4.m_z+=0.1;
  std::cout<<m_parser->m_uniformList[0].m_name<<":(z)  "<<m_parser->m_uniformList[0].m_vec4.m_z<<std::endl; break;

  case Qt::Key_4 : m_parser->m_uniformList[0].m_vec4.m_x-=0.1;
  std::cout<<m_parser->m_uniformList[0].m_name<<":(x)  "<<m_parser->m_uniformList[0].m_vec4.m_x<<std::endl; break;

  case Qt::Key_5 : m_parser->m_uniformList[0].m_vec4.m_y-=0.1;
  std::cout<<m_parser->m_uniformList[0].m_name<<":(y)  "<<m_parser->m_uniformList[0].m_vec4.m_y<<std::endl; break;

  case Qt::Key_6 : m_parser->m_uniformList[0].m_vec4.m_z-=0.1;
  std::cout<<m_parser->m_uniformList[0].m_name<<":(z)  "<<m_parser->m_uniformList[0].m_vec4.m_z<<std::endl; break;

  case Qt::Key_G : m_parser->m_uniformList[5].m_vec4.m_x+=0.1;
  std::cout<<m_parser->m_uniformList[5].m_name<<":(z)  "<<m_parser->m_uniformList[5].m_vec4.m_x<<std::endl; break;

  case Qt::Key_B : m_parser->m_uniformList[7].m_vec4.m_x+=0.1;
  std::cout<<m_parser->m_uniformList[7].m_name<<":(x)  "<<m_parser->m_uniformList[7].m_vec4.m_x<<std::endl; break;

  case Qt::Key_H : m_parser->m_uniformList[12].m_vec4.m_x+=0.1;
  std::cout<<m_parser->m_uniformList[12].m_name<<":(x)  "<<m_parser->m_uniformList[12].m_vec4.m_x<<std::endl; break;

  case Qt::Key_J : m_parser->m_uniformList[5].m_vec4.m_y+=0.1;
  std::cout<<m_parser->m_uniformList[5].m_name<<":(y)  "<<m_parser->m_uniformList[5].m_vec4.m_y<<std::endl; break;

  case Qt::Key_K : m_parser->m_uniformList[7].m_vec4.m_y+=0.1;
  std::cout<<m_parser->m_uniformList[7].m_name<<":(y)  "<<m_parser->m_uniformList[7].m_vec4.m_y<<std::endl; break;

  case Qt::Key_L : m_parser->m_uniformList[12].m_vec4.m_y+=0.1;
  std::cout<<m_parser->m_uniformList[12].m_name<<":(y)  "<<m_parser->m_uniformList[12].m_vec4.m_y<<std::endl; break;
  default : break;*/
  }
    update();
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent ( QMouseEvent * _event )
{
  if(m_rotate && _event->buttons() == Qt::LeftButton)
  {
    int diffx=_event->x()-m_origX;
    int diffy=_event->y()-m_origY;
    m_spinXFace += (float) 0.5f * diffy;
    m_spinYFace += (float) 0.5f * diffx;
    m_origX = _event->x();
    m_origY = _event->y();
    update();

  }
  // right mouse translate code
  else if(m_translate && _event->buttons() == Qt::RightButton)
  {
    int diffX = (int)(_event->x() - m_origXPos);
    int diffY = (int)(_event->y() - m_origYPos);
    m_origXPos=_event->x();
    m_origYPos=_event->y();
    m_modelPos.m_x += INCREMENT * diffX;
    m_modelPos.m_y -= INCREMENT * diffY;
    update();

   }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent ( QMouseEvent * _event )
{
  // that method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  if(_event->button() == Qt::LeftButton)
  {
    ngl::Vec4 _tempVec=m_parser->m_uniformList[12]->getVec4();
    std::cout<<"\nVal:\nx: "<<_tempVec.m_x<<"\ny: "<<_tempVec.m_y<<"\nz:"<<_tempVec.m_z<<std::endl;
    m_origX = _event->x();
    m_origY = _event->y();
    m_rotate =true;
  }
  // right mouse translate mode
  else if(_event->button() == Qt::RightButton)
  {
    m_origXPos = _event->x();
    m_origYPos = _event->y();
    m_translate=true;
  }
  setFocus();
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent ( QMouseEvent * _event )
{
  // that event is called when the mouse button is released
  // we then set Rotate to false
  if (_event->button() == Qt::LeftButton)
  {
    m_rotate=false;
  }
        // right mouse translate mode
  if (_event->button() == Qt::RightButton)
  {
    m_translate=false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent ( QWheelEvent * _event )
{

  // check the diff of the wheel position (0 means no change)
  if(_event->delta() > 0)
  {
    m_modelPos.m_z+=ZOOM;
  }
  else if(_event->delta() < 0)
  {
    m_modelPos.m_z-=ZOOM;
  }
  update();
}
//----------------------------------------------------------------------------------------------------------------------
void NGLScene::compileShader(QString vertSource, QString fragSource)
{
  m_shaderManager->compileShader(m_cam, vertSource, fragSource);
  if(!m_shaderManager->compileStatus())
  {
    m_window->updateTerminalText(m_shaderManager->getErrorLog());
  }
  ngl::Light light(ngl::Vec3(2,2,2),ngl::Colour(1,1,1,1),ngl::Colour(1,1,1,1),ngl::LightModes::POINTLIGHT);
  // now create our light this is done after the camera so we can pass the
  // transpose of the projection matrix to the light to do correct eye space
  // transformations
  ngl::Mat4 iv=m_cam.getViewMatrix();
  iv.transpose();

  light.setTransform(iv);
  light.setAttenuation(1,0,0);
  light.enable();
  // load these values to the shader as well
  light.loadToShader("light");

  update();
}


void NGLScene::resetObjPos()
{
  //reset object position back to default
  m_modelPos.m_x = 0;
  m_modelPos.m_y = 0;
  m_modelPos.m_z = 0;
  m_spinXFace = 0;
  m_spinYFace = 0;
}
