#include "ParserLib.h"
#include <ngl/ShaderLib.h>

//----------------------------------------------------------------------------------------------------------------------
/// @brief ctor for our ParserLib
//----------------------------------------------------------------------------------------------------------------------
parserLib::parserLib()
{

}

//----------------------------------------------------------------------------------------------------------------------
parserLib::~parserLib()
{
  std::cout<<"ParserLib destroyed"<<std::endl;
}

//----------------------------------------------------------------------------------------------------------------------
void parserLib::initializeUniformData()
{
  //create instance of a shader
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  GLuint id=shader->getProgramID("Phong");

  // extract the number of uniforms active and update class data.
  GLint nUniforms;
  glGetProgramInterfaceiv(id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &nUniforms);
  m_num=nUniforms;
  m_uniformList.resize(m_num);

  // declare some temp variables
  char uniformName[256];
  GLsizei length;
  GLint size;
  GLenum type;

  // Permanently assign data to the class
  for (GLint i=0; i<nUniforms; i++)
  {
    // craete local variables to store temp values.
    GLint tempLoc;
    // get the active uniform data...
    glGetActiveUniform(id,i, 256, &length, &size , &type , uniformName);
    // ... store the temp data
    tempLoc=glGetUniformLocation(id,uniformName);

    // set a different data type depending on typeData
    switch(type)
    {
      case GL_BOOL:
      {
        uniformDataBool* newData= new uniformDataBool(uniformName, tempLoc, type);
        // Add uniformData to the currently registeredUniforms
        m_registeredUniforms[uniformName]=newData;
        m_uniformList[i]=newData;
        break;
      }
      case GL_FLOAT:
      {
        uniformDataFloat* newData= new uniformDataFloat(uniformName, tempLoc, type);
        // Add uniformData to the currently registeredUniforms
        m_registeredUniforms[uniformName]=newData;
        m_uniformList[i]=newData;
        break;
      }
      case GL_INT:
      {
        uniformDataInt* newData= new uniformDataInt(uniformName, tempLoc, type);
        // Add uniformData to the currently registeredUniforms
        m_registeredUniforms[uniformName]=newData;
        m_uniformList[i]=newData;
        break;
      }
      case GL_FLOAT_VEC3:
      {
        uniformDataV3* newData= new uniformDataV3(uniformName, tempLoc, type);
        // Add uniformData to the currently registeredUniforms
        m_registeredUniforms[uniformName]=newData;
        m_uniformList[i]=newData;
        break;
      }
      case GL_FLOAT_VEC4:
      {
        uniformDataV4* newData= new uniformDataV4(uniformName, tempLoc, type);
        // Add uniformData to the currently registeredUniforms
        m_registeredUniforms[uniformName]=newData;
        m_uniformList[i]=newData;
        break;
      }
      case GL_FLOAT_MAT3:
      {
        uniformDataM3* newData= new uniformDataM3(uniformName, tempLoc, type);
        // Add uniformData to the currently registeredUniforms
        m_registeredUniforms[uniformName]=newData;
        m_uniformList[i]=newData;
        break;
      }
      case GL_FLOAT_MAT4:
      {
        uniformDataM4* newData= new uniformDataM4(uniformName, tempLoc, type);
        // Add uniformData to the currently registeredUniforms
        m_registeredUniforms[uniformName]=newData;
        m_uniformList[i]=newData;
        break;
      }
    default:
      std::cout<<"unrecognised data type name"<<std::endl;
    }


  }
  uniformDataTypes();
}



void parserLib::assignAllData()
{
  initializeUniformData();
  assignUniformValues();
  printUniforms(true);
}


//----------------------------------------------------------------------------------------------------------------------
void parserLib::printUniforms(bool _printValues)
{
  std::cout<<"_________________________________________Uniform Information: Starts//"<<std::endl;
  std::cout<<"There are "<<m_num<<" Uniforms"<<std::endl;

  // print information
  for (uint i=0; i<m_num; i++)
  {
    std::cout << "Name: "<<m_uniformList[i]->getName();
    std::cout << ";  Location: "<<m_uniformList[i]->getName()<<" ("<<i<<")";
    std::cout << ";  Type: "<<m_uniformList[i]->getTypeEnum()<<"; "
                            <<m_uniformList[i]->getTypeName()<<std::endl;

    switch(m_uniformList[i]->getTypeEnum())
    {
      case GL_BOOL:
      {
        std::cout<<"m_bool: "<<m_uniformList[i]->getBool()<<std::endl;
        break;
      }
      case GL_FLOAT:
      {
        std::cout<<"m_float: "<<m_uniformList[i]->getFloat()<<std::endl;
        break;
      }
      case GL_INT:
      {
        std::cout<<"m_int: "<<m_uniformList[i]->getInt()<<std::endl;
        break;
      }
      case GL_FLOAT_VEC3:
      {
        std::cout<<"m_vec3:(x) "<<m_uniformList[i]->getVec3().m_x<<std::endl;
        std::cout<<"m_vec3:(y) "<<m_uniformList[i]->getVec3().m_y<<std::endl;
        std::cout<<"m_vec3:(z) "<<m_uniformList[i]->getVec3().m_z<<std::endl;
        break;
      }
      case GL_FLOAT_VEC4:
      {
        std::cout<<"m_vec4:(x) "<<m_uniformList[i]->getVec4().m_x<<std::endl;
        std::cout<<"m_vec4:(y) "<<m_uniformList[i]->getVec4().m_y<<std::endl;
        std::cout<<"m_vec4:(z) "<<m_uniformList[i]->getVec4().m_z<<std::endl;
        break;
      }
      case GL_FLOAT_MAT3:
      {
        std::cout<<"m_mat3:(x) "<<m_uniformList[i]->getMat3().m_00
                                <<m_uniformList[i]->getMat3().m_01
                                <<m_uniformList[i]->getMat3().m_02<<std::endl;
        std::cout<<"m_mat3:(y) "<<m_uniformList[i]->getMat3().m_10
                                <<m_uniformList[i]->getMat3().m_11
                                <<m_uniformList[i]->getMat3().m_12<<std::endl;
        std::cout<<"m_mat3:(z) "<<m_uniformList[i]->getMat3().m_20
                                <<m_uniformList[i]->getMat3().m_21
                                <<m_uniformList[i]->getMat3().m_22<<std::endl;
        break;
      }
      case GL_FLOAT_MAT4:
      {
        std::cout<<"m_mat3:(x) "<<m_uniformList[i]->getMat4().m_00
                                <<m_uniformList[i]->getMat4().m_01
                                <<m_uniformList[i]->getMat4().m_02
                                <<m_uniformList[i]->getMat4().m_03<<std::endl;
        std::cout<<"m_mat3:(y) "<<m_uniformList[i]->getMat4().m_10
                                <<m_uniformList[i]->getMat4().m_11
                                <<m_uniformList[i]->getMat4().m_12
                                <<m_uniformList[i]->getMat4().m_13<<std::endl;
        std::cout<<"m_mat3:(z) "<<m_uniformList[i]->getMat4().m_20
                                <<m_uniformList[i]->getMat4().m_21
                                <<m_uniformList[i]->getMat4().m_22
                                <<m_uniformList[i]->getMat4().m_23<<std::endl;
        std::cout<<"m_mat3:(w) "<<m_uniformList[i]->getMat4().m_30
                                <<m_uniformList[i]->getMat4().m_31
                                <<m_uniformList[i]->getMat4().m_32
                                <<m_uniformList[i]->getMat4().m_33<<std::endl;
        break;
      }
      default:
        std::cout<<"unrecognised data type name"<<std::endl;
    }
    std::cout<<"___________________________________________Uniform Information: Ends//"<<std::endl;
  }
}


/// The following section was originally written by Jon Macey:-
/// Jon Macey. NCCA library NGL::ShaderProgram::printRegisteredUniforms [online]. [Accessed 01/10/16].
/// Available from: <https://nccastaff.bournemouth.ac.uk/jmacey/GraphicsLib/index.html>.
//----------------------------------------------------------------------------------------------------------------------
void parserLib::uniformDataTypes()
{
  std::cout<<"Started Uniforms" <<"\n";

  const static std::unordered_map<GLenum,const char *> types=
  {
    {GL_FLOAT,"float"},
    {GL_FLOAT_VEC2,"vec2"},
    {GL_FLOAT_VEC3,"vec3"},
    {GL_FLOAT_VEC4,"vec4"},
    {GL_DOUBLE,"double"},
    {GL_DOUBLE_VEC2,"dvec2"},
    {GL_DOUBLE_VEC3,"dvec3"},
    {GL_DOUBLE_VEC4,"dvec4"},
    {GL_INT,"int"},
    {GL_INT_VEC2,"ivec2"},
    {GL_INT_VEC3,"ivec3"},
    {GL_INT_VEC4,"ivec4"},
    {GL_UNSIGNED_INT,"unsigned int"},
    {GL_UNSIGNED_INT_VEC2,"uvec2"},
    {GL_UNSIGNED_INT_VEC3,"uvec3"},
    {GL_UNSIGNED_INT_VEC4,"uvec4"},
    {GL_BOOL,"bool"},
    {GL_BOOL_VEC2,"bvec2"},
    {GL_BOOL_VEC3,"bvec3"},
    {GL_BOOL_VEC4,"bvec4"},
    {GL_FLOAT_MAT2,"mat2"},
    {GL_FLOAT_MAT3,"mat3"},
    {GL_FLOAT_MAT4,"mat4"},
    {GL_FLOAT_MAT2x3,"mat2x3"},
    {GL_FLOAT_MAT2x4,"mat2x4"},
    {GL_FLOAT_MAT3x2,"mat3x2"},
    {GL_FLOAT_MAT3x4,"mat3x4"},
    {GL_FLOAT_MAT4x2,"mat4x2"},
    {GL_FLOAT_MAT4x3,"mat4x3"},
    {GL_DOUBLE_MAT2,"dmat2"},
    {GL_DOUBLE_MAT3,"dmat3"},
    {GL_DOUBLE_MAT4,"dmat4"},
    {GL_DOUBLE_MAT2x3,"dmat2x3"},
    {GL_DOUBLE_MAT2x4,"dmat2x4"},
    {GL_DOUBLE_MAT3x2,"dmat3x2"},
    {GL_DOUBLE_MAT3x4,"dmat3x4"},
    {GL_DOUBLE_MAT4x2,"dmat4x2"},
    {GL_DOUBLE_MAT4x3,"dmat4x3"},
    {GL_SAMPLER_1D,"sampler1D"},
    {GL_SAMPLER_2D,"sampler2D"},
    {GL_SAMPLER_3D,"sampler3D"},
    {GL_SAMPLER_CUBE,"samplerCube"},
    {GL_SAMPLER_1D_SHADOW,"sampler1DShadow"},
    {GL_SAMPLER_2D_SHADOW,"sampler2DShadow"},
    {GL_SAMPLER_1D_ARRAY,"sampler1DArray"},
    {GL_SAMPLER_2D_ARRAY,"sampler2DArray"},
    {GL_SAMPLER_1D_ARRAY_SHADOW,"sampler1DArrayShadow"},
    {GL_SAMPLER_2D_ARRAY_SHADOW,"sampler2DArrayShadow"},
    {GL_SAMPLER_2D_MULTISAMPLE,"sampler2DMS"},
    {GL_SAMPLER_2D_MULTISAMPLE_ARRAY,"sampler2DMSArray"},
    {GL_SAMPLER_CUBE_SHADOW,"samplerCubeShadow"},
    {GL_SAMPLER_BUFFER,"samplerBuffer"},
    {GL_SAMPLER_2D_RECT,"sampler2DRect"},
    {GL_SAMPLER_2D_RECT_SHADOW,"sampler2DRectShadow"},
    {GL_INT_SAMPLER_1D,"isampler1D"},
    {GL_INT_SAMPLER_2D,"isampler2D"},
    {GL_INT_SAMPLER_3D,"isampler3D"},
    {GL_INT_SAMPLER_CUBE,"isamplerCube"},
    {GL_INT_SAMPLER_1D_ARRAY,"isampler1DArray"},
    {GL_INT_SAMPLER_2D_ARRAY,"isampler2DArray"},
    {GL_INT_SAMPLER_2D_MULTISAMPLE,"isampler2DMS"},
    {GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,"isampler2DMSArray"},
    {GL_INT_SAMPLER_BUFFER,"isamplerBuffer"},
    {GL_INT_SAMPLER_2D_RECT,"isampler2DRect"},
    {GL_UNSIGNED_INT_SAMPLER_1D,"usampler1D"},
    {GL_UNSIGNED_INT_SAMPLER_2D,"usampler2D"},
    {GL_UNSIGNED_INT_SAMPLER_3D,"usampler3D"},
    {GL_UNSIGNED_INT_SAMPLER_CUBE,"usamplerCube"},
    {GL_UNSIGNED_INT_SAMPLER_1D_ARRAY,"usampler2DArray"},
    {GL_UNSIGNED_INT_SAMPLER_2D_ARRAY,"usampler2DArray"},
    {GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE,"usampler2DMS"},
    {GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,"usampler2DMSArray"},
    {GL_UNSIGNED_INT_SAMPLER_BUFFER,"usamplerBuffer"},
    {GL_UNSIGNED_INT_SAMPLER_2D_RECT,"usampler2DRect"},
    {GL_IMAGE_1D,"image1D"},
    {GL_IMAGE_2D,"image2D"},
    {GL_IMAGE_3D,"image3D"},
    {GL_IMAGE_2D_RECT,"image2DRect"},
    {GL_IMAGE_CUBE,"imageCube"},
    {GL_IMAGE_BUFFER,"imageBuffer"},
    {GL_IMAGE_1D_ARRAY,"image1DArray"},
    {GL_IMAGE_2D_ARRAY,"image2DArray"},
    {GL_IMAGE_2D_MULTISAMPLE,"image2DMS"},
    {GL_IMAGE_2D_MULTISAMPLE_ARRAY,"image2DMSArray"},
    {GL_INT_IMAGE_1D,"iimage1D"},
    {GL_INT_IMAGE_2D,"iimage2D"},
    {GL_INT_IMAGE_3D,"iimage3D"},
    {GL_INT_IMAGE_2D_RECT,"iimage2DRect"},
    {GL_INT_IMAGE_CUBE,"iimageCube"},
    {GL_INT_IMAGE_BUFFER,"iimageBuffer"},
    {GL_INT_IMAGE_1D_ARRAY,"iimage1DArray"},
    {GL_INT_IMAGE_2D_ARRAY,"iimage2DArray"},
    {GL_INT_IMAGE_2D_MULTISAMPLE,"iimage2DMS"},
    {GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY,"iimage2DMSArray"},
    {GL_UNSIGNED_INT_IMAGE_1D,"uimage1D"},
    {GL_UNSIGNED_INT_IMAGE_2D,"uimage2D"},
    {GL_UNSIGNED_INT_IMAGE_3D,"uimage3D"},
    {GL_UNSIGNED_INT_IMAGE_2D_RECT,"uimage2DRect"},
    {GL_UNSIGNED_INT_IMAGE_CUBE,"uimageCube"},
    {GL_UNSIGNED_INT_IMAGE_BUFFER,"uimageBuffer"},
    {GL_UNSIGNED_INT_IMAGE_1D_ARRAY,"uimage1DArray"},
    {GL_UNSIGNED_INT_IMAGE_2D_ARRAY,"uimage2DArray"},
    {GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE,"uimage2DMS"},
    {GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY,"uimage2DMSArray"},
    {GL_UNSIGNED_INT_ATOMIC_COUNTER,"atomic_uint"}
  };

  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  GLuint id=shader->getProgramID("Phong");

  GLint nUniforms;
  glGetProgramInterfaceiv(id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &nUniforms);


  std::cout<<"Registered Uniforms for shader "<< id <<"\n";


  for(auto d : m_registeredUniforms)
  {
    std::string type;
    auto value=types.find(d.second->getTypeEnum());
    if(value !=types.end())
    {
      type=value->second;
    }
    else
    {
      type="unknown type";
    }
    /// end of Citation
    m_uniformList[d.second->getLocation()]->setTypeName(type);
  }
  std::cout<<"End Uniforms" <<"\n";
}

//----------------------------------------------------------------------------------------------------------------------
void parserLib::exportUniforms()
{
  std::ofstream fileOut;
  fileOut.open("ParsingOutput.txt");
  if(!fileOut.is_open())    ///If it can be opened
  {
    std::cerr<<"couldn't' open file\n";
    exit(EXIT_FAILURE);
  }
  for(uint i=0;i<m_num;i++)
  {
    fileOut<<m_uniformList[i]->getName()<<"\n";
    fileOut<<m_uniformList[i]->getLocation()<<"\n";
    fileOut<<m_uniformList[i]->getTypeName()<<"\n";
  }
  fileOut.close();
  // close files
  std::cout<<"EXPORTED\n"<<std::endl;
}

////----------------------------------------------------------------------------------------------------------------------
//bool parserLib::getBool(int _loc)
//{
//    return m_uniformList[_loc].m_bool;
//}
////----------------------------------------------------------------------------------------------------------------------
//int parserLib::getInt(int _loc)
//{
//    return m_uniformList[_loc].m_int;
//}
////----------------------------------------------------------------------------------------------------------------------
//float parserLib::getFloat(int _loc)
//{
//    return m_uniformList[_loc].m_float;
//}
////----------------------------------------------------------------------------------------------------------------------
//ngl::Mat3 parserLib::getMat3(int _loc)
//{
//    return m_uniformList[_loc].m_mat3;
//}
////----------------------------------------------------------------------------------------------------------------------
//ngl::Mat4 parserLib::getMat4(int _loc)
//{
//    return m_uniformList[_loc].m_mat4;
//}
////----------------------------------------------------------------------------------------------------------------------
//ngl::Vec3 parserLib::getVec3(int _loc)
//{
//    return m_uniformList[_loc].m_vec3;
//}
////----------------------------------------------------------------------------------------------------------------------
//ngl::Vec4 parserLib::getVec4(int _loc)
//{
//    return m_uniformList[_loc].m_vec4;
//}
////----------------------------------------------------------------------------------------------------------------------
//void parserLib::setBool(int _loc, bool _value)
//{
//    m_uniformList[_loc].m_bool=_value;
//}
////----------------------------------------------------------------------------------------------------------------------
//void parserLib::setInt(int _loc, int _value)
//{
//    m_uniformList[_loc].m_int=_value;
//}
////----------------------------------------------------------------------------------------------------------------------
//void parserLib::setFloat(int _loc, float _value)
//{
//    m_uniformList[_loc].m_float=_value;
//}
////----------------------------------------------------------------------------------------------------------------------
//void parserLib::setMat3(int _loc, ngl::Mat3 _value)
//{
//    m_uniformList[_loc].m_mat3=_value;
//}
////----------------------------------------------------------------------------------------------------------------------
//void parserLib::setMat4(int _loc, ngl::Mat4 _value)
//{
//    m_uniformList[_loc].m_mat4=_value;
//}
////----------------------------------------------------------------------------------------------------------------------
//void parserLib::setVec3(int _loc, ngl::Vec3 _value)
//{
//    m_uniformList[_loc].m_vec3=_value;
//}
////----------------------------------------------------------------------------------------------------------------------
//void parserLib::setVec4(int _loc, ngl::Vec4 _value)
//{
//    m_uniformList[_loc].m_vec4=_value;
//}


//----------------------------------------------------------------------------------------------------------------------
void parserLib::assignUniformValues()
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();

  // sets default values depending on it's data type
  for (uint i=0;i<m_num;i++)
  {
    std::cout<<"assignUniforms "<< i<<std::endl;
    switch(m_uniformList[i]->getTypeEnum())
    {
      case GL_BOOL:
      {
        m_uniformList[i]->setBool(0);
        break;
      }
      case GL_FLOAT:
      {
        m_uniformList[i]->setFloat(0.5);
        break;
      }
      case GL_INT:
      {
        m_uniformList[i]->setInt(0);
        break;
      }
      case GL_FLOAT_VEC3:
      {
        m_uniformList[i]->setVec3(0.5);
        break;
      }
      case GL_FLOAT_VEC4:
      {
        m_uniformList[i]->setVec4(0.5);
        break;
      }
      case GL_FLOAT_MAT3:
      {
        m_uniformList[i]->setMat3(shader->getUniformBlockIndex(m_uniformList[i]->getName()) );
        break;
      }
      case GL_FLOAT_MAT4:
      {
        m_uniformList[i]->setMat4( shader->getUniformBlockIndex(m_uniformList[i]->getName()) );
        break;
      }
    default:
      std::cout<<"unrecognised data type name"<<std::endl;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void parserLib::sendUniformsToShader(ngl::ShaderLib *shader)
{
  for (uint i=0;i<m_num;i++)
  {
    switch(m_uniformList[i]->getTypeEnum())
    {
      case GL_BOOL:
      {
      shader->setShaderParam1i(m_uniformList[i]->getName(),m_uniformList[i]->getBool());
        break;
      }
      case GL_FLOAT:
      {
      shader->setShaderParam1f(m_uniformList[i]->getName(),m_uniformList[i]->getFloat());
        break;
      }
      case GL_INT:
      {
      shader->setShaderParam1i(m_uniformList[i]->getName(),m_uniformList[i]->getInt());
        break;
      }
      case GL_FLOAT_VEC3:
      {
        ngl::Vec4 newVec3;
        newVec3.m_x=m_uniformList[i]->getVec3().m_x;
        newVec3.m_y=m_uniformList[i]->getVec3().m_y;
        newVec3.m_z=m_uniformList[i]->getVec3().m_z;
        newVec3.m_w=1;
        shader->setShaderParamFromVec4(m_uniformList[i]->getName(), newVec3);
        break;
      }
      case GL_FLOAT_VEC4:
      {
        shader->setShaderParamFromVec4(m_uniformList[i]->getName(), m_uniformList[i]->getVec4());
        break;
      }
      case GL_FLOAT_MAT3:
      {
        shader->setShaderParamFromMat3(m_uniformList[i]->getName(), m_uniformList[i]->getMat3());
        break;
      }
      case GL_FLOAT_MAT4:
      {
        shader->setShaderParamFromMat4(m_uniformList[i]->getName(),  m_uniformList[i]->getMat4());
        break;
      }
    default:
      std::cout<<"unrecognised data type name"<<std::endl;
    }

  }
}