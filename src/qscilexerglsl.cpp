#include <qscilexerglsl.h>
#include <Qsci/qsciscintilla.h>

#include <iostream>
#include <sstream>
#include "glslLexer.h"

QsciLexerGLSL::QsciLexerGLSL(QObject *parent) : QsciLexerCustom(parent)
{
    //all GLSL keywords
    keywordsList <<"attribute"<<"const"<<"uniform"<<"varying"<<"layout"<<"centroid"<<"flat"<<"smooth"<<"noperspective"<<"patch"<<"sample"<<"break"<<"continue"<<"do"<<
                   "for"<<"while"<<"switch"<<"case"<<"default"<<"if"<<"else"<<"subroutine"<<"in"<<"out"<<"inout"<<"float"<<"double"<<"int"<<"void"<<"bool"<<"true"<<
                   "false"<<"invariant"<<"discard"<<"return"<<"mat2"<<"mat3"<<"mat4"<<"dmat2"<<"dmat3"<<"dmat4"<<"mat2x2"<<"mat2x3"<<"mat2x4"<<"dmat2x2"<<"dmat2x3"<<
                   "dmat2x4"<<"mat3x2"<<"mat3x3"<<"mat3x4"<<"dmat3x2"<<"dmat3x3"<<"dmat3x4"<<"mat4x2"<<"mat4x3"<<"mat4x4"<<"dmat4x2"<<"dmat4x3"<<"dmat4x4"<<"vec2"<<"vec3"<<
                   "vec4"<<"ivec2"<<"ivec3"<<"ivec4"<<"bvec2"<<"bvec3"<<"bvec4"<<"dvec2"<<"dvec3"<<"dvec4"<<"uint"<<"uvec2"<<"uvec3"<<"uvec4"<<"lowp"<<"mediump"<<"highp"<<
                   "precision"<<"sampler1D" << "sampler2D" << "sampler3D" << "samplerCube" << "sampler1DShadow" << "sampler2DShadow" << "samplerCubeShadow" << "sampler1DArray" <<
                   "sampler2DArray" << "sampler1DArrayShadow" << "sampler2DArrayShadow" << "isampler1D" << "isampler2D" << "isampler3D" << "isamplerCube" << "isampler1DArray" <<
                   "isampler2DArray" << "usampler1D" << "usampler2D" << "usampler3D" << "usamplerCube" << "usampler1DArray" << "usampler2DArray" << "sampler2DRect" <<
                   "sampler2DRectShadow" << "isampler2DRect" << "usampler2DRect" << "samplerBuffer" << "isamplerBuffer" << "usamplerBuffer" << "sampler2DMS" << "isampler2DMS" <<
                   "usampler2DMS" << "sampler2DMSArray" << "isampler2DMSArray" << "usampler2DMSArray" << "samplerCubeArray" << "samplerCubeArrayShadow" << "isamplerCubeArray" <<
                   "usamplerCubeArray" << "struct";
}

QsciLexerGLSL::~QsciLexerGLSL()
{
    return;
}

void QsciLexerGLSL::styleText(int start, int end)
{
    //return if no QsciScintilla editor
    if(!editor())
        return;

    char * data = new char[end - start +1];

    //get text to be styled
    editor()->SendScintilla(QsciScintilla::SCI_GETTEXTRANGE, start, end, data);
    QString source(data);
    delete [] data;

    //return if no text to be styled
    if(source.isEmpty())
        return;

    //style keywords
    highlightKeywords(source, start);
}

void QsciLexerGLSL::highlightKeywords(const QString &source, int start)
{
    FlexLexer* lexer = new yyFlexLexer;
    int tok;
    int loc=0;
    std::istringstream istr (source.toStdString());

    std::vector<int> tokens;

    while ((tok = lexer->yylex(&istr))>0)
    {
        tokens.push_back(tok);
        tokens.push_back(loc);
        tokens.push_back(lexer->YYLeng());
        loc = loc + lexer->YYLeng();
        if (tok!=StyleType::WHITESPACE) loc++;
    }

    for (int i = 0; i<tokens.size(); i=i+3)
    {
        switch(tokens[i])
        {
            case StyleType::DEFAULT:
                startStyling(start+tokens[i+1]);
                setStyling(tokens[i+2],StyleType::DEFAULT);
            break;
            case StyleType::KEYWORD:
                startStyling(start+tokens[i+1]);
                setStyling(tokens[i+2],StyleType::KEYWORD);
            break;
        }
    }

    //check for keywords in text
//    for (int i = 0; i<keywordsList.size(); i++)
//    {
//        QString word = keywordsList.at(i);
//        if (source.contains(word))
//        {
//            int count = source.count(word);
//            int index = 0;
//            //for each instance of the keyword, set styling
//            for (int j = 0; j<count; j++)
//            {
//                int wordStart = source.indexOf(word, index);
//                index = wordStart+1;

//                startStyling(start + wordStart);
//                printf("started styling\n");
//                setStyling(word.length(), StyleType::KEYWORD);
//            }
//        }
//    }
}

QColor QsciLexerGLSL::defaultColor(int style) const
{
    switch(style)
    {
        case StyleType::DEFAULT:
            return QColor(247,247,241);
        case StyleType::KEYWORD:
            return QColor(102,216,238);
    }
    return QColor(247,247,241);
}

QColor QsciLexerGLSL::defaultPaper(int style) const
{
    return QColor(39,40,34);
}

QFont QsciLexerGLSL::defaultFont(int style) const
{
    int weight=50;
    int size=12;
    switch(style)
    {
        case StyleType::DEFAULT:
            weight = 50;
            size = 12;
        break;
        case StyleType::KEYWORD:
            weight = 75;
        break;
    }

    return QFont("Monospace", size,weight);
}

QString QsciLexerGLSL::description(int style) const
{
    switch(style)
    {
        case StyleType::DEFAULT:
            return "Default";
        case StyleType::KEYWORD:
            return "Keyword";
    }
    return QString(style);
}

const char *QsciLexerGLSL::language() const
{
    return "GLSL";
}
