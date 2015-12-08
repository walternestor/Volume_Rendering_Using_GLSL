
// TODO: Implement/improve TF

//#include <omp.h>

// Program Includes
#include "myglwidget.h"

// Qt Includes
#include <QDebug>
#include <QColor>

// C/C++ includes
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <stdio.h>

// GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using glm::mat4;
using glm::vec3;

#define GL_ERROR() checkForOpenGLError(__FILE__, __LINE__)

GLuint g_vao;
GLuint vao;
GLuint g_programHandle;
GLuint g_winWidth = 400;
GLuint g_winHeight = 400;
GLint g_angle = 0;
GLuint g_frameBuffer;
// transfer function
GLuint g_tffTexObj;
GLuint g_bfTexObj;
GLuint g_texWidth;
GLuint g_texHeight;
GLuint g_volTexObj;
GLuint g_rcVertHandle;
GLuint g_rcFragHandle;
GLuint g_bfVertHandle;
GLuint g_bfFragHandle;

GLuint g_rcGrayFragShaderHandle;
GLuint g_rcRGBAFragShaderHandle;
GLuint g_rcIsoFragHandle;
GLuint g_rcDirectSurfaceFragShaderHandle;

float g_stepSize = 0.001f;
GLuint vertexdat;

GLfloat vertices[24] = {
    0.0, 0.0, 0.0,
    0.0, 0.0, 1.0,
    0.0, 1.0, 0.0,
    0.0, 1.0, 1.0,
    1.0, 0.0, 0.0,
    1.0, 0.0, 1.0,
    1.0, 1.0, 0.0,
    1.0, 1.0, 1.0
};

// Mouse Controls
GLfloat rotationY;
GLfloat rotationZ;
QPoint  lastPos;

// Clip Plane Controls
GLint   elevationValue;
GLint   azimuthValue;
GLint   clipEnableValue;
GLfloat clipPlaneValue;

// Transfer Function
// Red
GLfloat redCenter;
GLfloat redWidth;
// Green
GLfloat greenCenter;
GLfloat greenWidth;
// Blue
GLfloat blueCenter;
GLfloat blueWidth;
// Alpha
GLfloat alphaCenter;
GLfloat alphaWidth;

// Intensity Control
GLfloat intensityMin;
GLfloat intensityMax;
GLint renderType;


QVector< double > keyGraphValue;
QVector< double > nonEqValue;
QVector< double > eqValue;
QVector< double > lutValue;

//int size;
// Ray Picking
//GLubyte *read_data = new GLubyte[size];			  // 16bit
GLubyte read_data[4];
GLfloat mpx;
GLfloat mpy;
GLubyte pickedcolor[4];
GLfloat prev_x;
GLfloat prev_y;



int checkForOpenGLError(const char* file, int line)
{
    // return 1 if an OpenGL error occured, 0 otherwise.
    GLenum glErr;
    int retCode = 0;

    glErr = glGetError();
    while(glErr != GL_NO_ERROR)
    {
        cout << "glError in file " << file
             << "@line " << line << gluErrorString(glErr) << endl;
        retCode = 1;
        exit(EXIT_FAILURE);
    }
    return retCode;
}

void initVBO();
void initShader();
void initFrameBuffer(GLuint, GLuint, GLuint);
GLuint initTFF1DTex(const char* filename);
GLuint initFace2DTex(GLuint texWidth, GLuint texHeight);
//GLuint initVol3DTex(const char* filename, GLuint width, GLuint height, GLuint depth);
void render_gl(GLenum cullFace);
void linkShader(GLuint shaderPgm, GLuint newVertHandle, GLuint newFragHandle);


// init the vertex buffer object
void initVBO()
{

    //    GLfloat vertices[24] = {
    //        0.0, 0.0, 0.0,
    //        0.0, 0.0, 1.0,
    //        0.0, 1.0, 0.0,
    //        0.0, 1.0, 1.0,
    //        1.0, 0.0, 0.0,
    //        1.0, 0.0, 1.0,
    //        1.0, 1.0, 0.0,
    //        1.0, 1.0, 1.0
    //    };

    // draw the six faces of the boundbox by drawwing triangles
    // draw it contra-clockwise
    // front: 1 5 7 3
    // back: 0 2 6 4
    // left: 0 1 3 2
    // right:7 5 4 6
    // up: 2 3 7 6
    // down: 1 0 4 5
    GLuint indices[36] = {
        1,5,7,
        7,3,1,
        0,2,6,
        6,4,0,
        0,1,3,
        3,2,0,
        7,5,4,
        4,6,7,
        2,3,7,
        7,6,2,
        1,0,4,
        4,5,1
    };

    GLuint veridxdat;
    glGenBuffers(1, &vertexdat);
    glGenBuffers(1, &veridxdat);

    glBindBuffer(GL_ARRAY_BUFFER, vertexdat);
    glBufferData(GL_ARRAY_BUFFER, 24*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    // used in glDrawElement()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, veridxdat);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36*sizeof(GLuint), indices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    // vao like a closure binding 3 buffer object: verlocdat vercoldat and veridxdat
    glBindVertexArray(vao);

    glEnableVertexAttribArray(0); // for vertexloc
    glEnableVertexAttribArray(1); // for vertexcol

    // the vertex location is the same as the vertex color
    glBindBuffer(GL_ARRAY_BUFFER, vertexdat);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLfloat *)NULL);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLfloat *)NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, veridxdat);
    // glBindVertexArray(0);
    g_vao = vao;
}

void drawBox(GLenum glFaces)
{
    glEnable(GL_CULL_FACE);
    glCullFace(glFaces);
    glBindVertexArray(g_vao);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)NULL);
    glDisable(GL_CULL_FACE);
}

// check the compilation result
GLboolean compileCheck(GLuint shader)
{
    GLint err;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &err);
    if (GL_FALSE == err)
    {
        GLint logLen;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
        if (logLen > 0)
        {
            char* log = (char *)malloc(logLen);
            GLsizei written;
            glGetShaderInfoLog(shader, logLen, &written, log);
            cerr << "Shader log: " << log << endl;
            free(log);
        }
    }
    return err;
}

// init shader object
GLuint initShaderObj(const GLchar* srcfile, GLenum shaderType)
{
    ifstream inFile(srcfile, ifstream::in);
    // use assert?
    if (!inFile)
    {
        cerr << "Error openning file: " << srcfile << endl;
        exit(EXIT_FAILURE);
    }

    const int MAX_CNT = 1000000;
    GLchar *shaderCode = (GLchar *) calloc(MAX_CNT, sizeof(GLchar));
    inFile.read(shaderCode, MAX_CNT);
    if (inFile.eof())
    {
        size_t bytecnt = inFile.gcount();
        *(shaderCode + bytecnt) = '\0';
    }
    else if(inFile.fail())
    {
        cout << srcfile << "read failed " << endl;
    }
    else
    {
        cout << srcfile << "is too large" << endl;
    }
    // create the shader Object
    GLuint shader = glCreateShader(shaderType);
    if (0 == shader)
    {
        cerr << "Error creating vertex shader." << endl;
    }
    // cout << shaderCode << endl;
    // cout << endl;
    const GLchar* codeArray[] = {shaderCode};
    glShaderSource(shader, 1, codeArray, NULL);
    free(shaderCode);

    // compile the shader
    glCompileShader(shader);
    if (GL_FALSE == compileCheck(shader))
    {
        cerr << "shader compilation failed" << endl;
    }
    return shader;
}

GLint checkShaderLinkStatus(GLuint pgmHandle)
{
    GLint status;
    glGetProgramiv(pgmHandle, GL_LINK_STATUS, &status);
    if (GL_FALSE == status)
    {
        GLint logLen;
        glGetProgramiv(pgmHandle, GL_INFO_LOG_LENGTH, &logLen);
        if (logLen > 0)
        {
            GLchar * log = (GLchar *)malloc(logLen);
            GLsizei written;
            glGetProgramInfoLog(pgmHandle, logLen, &written, log);
            cerr << "Program log: " << log << endl;
        }
    }
    return status;
}

// link shader program
GLuint createShaderPgm()
{
    // Create the shader program
    GLuint programHandle = glCreateProgram();
    if (0 == programHandle)
    {
        cerr << "Error create shader program" << endl;
        exit(EXIT_FAILURE);
    }
    return programHandle;
}

// init the 2D texture for render backface 'bf', stands for backface
GLuint initFace2DTex(GLuint bfTexWidth, GLuint bfTexHeight)
{
    GLuint backFace2DTex;
    glGenTextures(1, &backFace2DTex);
    glBindTexture(GL_TEXTURE_2D, backFace2DTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, bfTexWidth, bfTexHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    return backFace2DTex;
}


QVector<double> MyGLWidget::keyValue()
{
    return keyGraphValue;
}

QVector<double> MyGLWidget::equalizedValue()
{
    return eqValue;
}

QVector<double> MyGLWidget::nonEqualizedValue()
{
    return nonEqValue;
}

QVector<double> MyGLWidget::lutEqualizedValue()
{
    return lutValue;
}

// init 3D texture to store the volume data used fo ray casting
GLuint MyGLWidget::initVol3DTex(const char* filename, GLuint w, GLuint h, GLuint d)
//GLuint MyGLWidget::initVol3DTex(const char* filename, int fileSize)
{
    FILE *fp;
    int size = w * h * d;
//    int size = fileSize;
    GLushort *data = new GLushort[size];			  // 16bit

    //    GLubyte *data = new GLubyte[size];			  // 8bit

    if (!(fp = fopen(filename, "rb")))
    {
        cout << "Error: opening .raw file failed" << endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        cout << "OK: open .raw file successed" << endl;
    }
    //    if ( fread(data, sizeof(char), size, fp)!= size)     // 8bit
    if ( fread(data, sizeof(GLushort), size, fp)!= size) // 16bit
    {
        cout << "Error: read .raw file failed" << endl;
        exit(1);
    }
    else
    {
        cout << "OK: read .raw file successed" << endl;
    }

    fclose(fp);

    // =====================================================================
    // Histogram Equalization
    // =====================================================================
    // Histogram Variables
    int histSize = 65535;
    unsigned int sum = 0;
    GLushort *eqHistogram = new GLushort[histSize];
    GLushort *histogram = new GLushort[histSize];
    GLushort *hist_eq   = new GLushort[size];			  // 16bit
    GLushort *lut       = new GLushort[size];			  // 16bit

    float MAX_INTENSITY = 65536.0f;         // max value in 16-bit image
    float scale;

    FILE *eqHistogramFile;
    FILE *histogramFile;
    FILE *dataFile;
    FILE *lutFile;

    //     initialize all intensity values to 0
    #pragma omp parallel for
    for (int i = 0; i < histSize; i++)
    {
        histogram[i] = 0;
        eqHistogram[i] = 0;
    }

    #pragma omp for ordered
    for (int i = 0; i < size; i++)
    {
//        data[i] = (data[i] >> 8) | (data[i] << 8);
        // calculate the Nº of pixels for each intensity values
        // traverse all pixels and accumulate the count of same intensity values
        histogram[data[i]]++;


        // Find max intensity that not necessarilly is 65536
        if (data[i] > MAX_INTENSITY)
        {
            MAX_INTENSITY = data[i];
        }
    }

    MAX_INTENSITY = 65536.0f;
    scale = MAX_INTENSITY / size;    // scale factor ,so the values in LUT are from 0 to MAX_VALUE

//    qDebug() << "Max Intensity" << MAX_INTENSITY;
//    qDebug() << "scale" << scale;
//    qDebug() << "fixed" << (float)65536.0f/size;
//    qDebug() << "fixed 4096" << (float)4096.0f/size;

    #pragma omp for ordered
    for (int i = 0; i < histSize; i++)
    {
        // cumulative sum is used as LUT
        sum += histogram[i];
        // build look-up table
        lut[i] += (sum * scale);

        // Used in the Histogram graphs
        keyGraphValue.append(i);
        nonEqValue.append(static_cast<double>(histogram[i]));
        lutValue.append(static_cast<double>(lut[i]));
    }

    //        dataFile = fopen("../../Volume_Rendering_Using_GLSL/dataFile.txt","wt");
    //        lutFile = fopen("../../Volume_Rendering_Using_GLSL/lutFile.txt","wt");
    //        histogramFile = fopen("../../Volume_Rendering_Using_GLSL/histogramFile.txt","wt");
    //        eqHistogramFile = fopen("../../Volume_Rendering_Using_GLSL/eqhistogramFile.txt","wt");

    #pragma omp for ordered
    for (int i = 0; i < size; i++)
    {
        hist_eq[i] = (GLushort)lut[data[i]];

        //        fprintf(dataFile,"%d : %d\n",i, data[i]);
        //        fprintf(eqHistogramFile,"%d : %d\n",i, hist_eq[i]);
    }

    // Histogram Equalized
    #pragma omp for ordered
    for (int i = 0; i < histSize; i++)
    {
        //        data[i] = (data[i] >> 8) | (data[i] << 8);
        // calculate the Nº of pixels for each intensity values
        // traverse all pixels and accumulate the count of same intensity values
        eqHistogram[hist_eq[i]]++;
    }

    #pragma omp for ordered
    for (int i = 0; i < histSize; i++)
    {
        eqValue.append(static_cast<double>(eqHistogram[i]));
    }

    //        #pragma omp for ordered
    //        for (int i = 0; i < histSize; i++) {
    //            fprintf(histogramFile,"%d : %d\n",i, histogram[i]);
    //            fprintf(lutFile,"%d : %d\n",i, lut[i]);
    //        }

    //        fclose(dataFile);
    //        fclose(eqHistogramFile);
    //        fclose(lutFile);
    //        fclose(histogramFile);

    delete [] lut;
    delete [] histogram;
    // =====================================================================


    glGenTextures(1, &g_volTexObj);
    // bind 3D texture target
    glBindTexture(GL_TEXTURE_3D, g_volTexObj);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // GL_REPEAT
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // GL_REPEAT

    // pixel transfer happens here from client to OpenGL server
//    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//    glTexImage3D(GL_TEXTURE_3D, 0, GL_INTENSITY16, w, h, d, 0, GL_RED, GL_UNSIGNED_SHORT, hist_eq);
    delete [] hist_eq;

    // Need to swap bytes due to endianness of the DICOM file
////    glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_TRUE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_INTENSITY16, w, h, d, 0, GL_RED, GL_UNSIGNED_SHORT, data);


    GL_ERROR();
    delete [] data;
    cout << "volume texture created" << endl;
    return g_volTexObj;
}

void checkFramebufferStatus()
{
    GLenum complete = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (complete != GL_FRAMEBUFFER_COMPLETE)
    {
        cout << "framebuffer is not complete" << endl;
        exit(EXIT_FAILURE);
    }
}

// init the framebuffer, the only framebuffer used in this program
void initFrameBuffer(GLuint texObj, GLuint texWidth, GLuint texHeight)
{
    // create a depth buffer for our framebuffer
    GLuint depthBuffer;
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, texWidth, texHeight);

    // attach the texture and the depth buffer to the framebuffer
    glGenFramebuffers(1, &g_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, g_frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texObj, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    checkFramebufferStatus();
    glEnable(GL_DEPTH_TEST);
}

void MyGLWidget::clipEnableUniform(bool checked)
{
    clipEnableValue = (GLint)checked;
    updateGL();
}

void MyGLWidget::elevationUniform(int value)
{
    elevationValue = value;
    updateGL();
}

void MyGLWidget::azimuthUniform(int value)
{
    azimuthValue = value;
    updateGL();
}

void MyGLWidget::clipPlaneDepthUniform(int value)
{
    clipPlaneValue = ((float)value/100.0f);
    updateGL();
}

void rcSetUinforms()
{
    // setting uniforms such as
    // ScreenSize
    // StepSize
    // TransferFunc
    // ExitPoints i.e. the backface, the backface hold the ExitPoints of ray casting
    // VolumeTex the texture that hold the volume data i.e. head256.raw
    GLint screenSizeLoc = glGetUniformLocation(g_programHandle, "ScreenSize");
    if (screenSizeLoc >= 0)
    {
        // Load data into uniform shader variable
        glUniform2f(screenSizeLoc, (float)g_winWidth, (float)g_winHeight);
    }
    else
    {
//        cout << "ScreenSize is not bind to the uniform" << endl;
    }
    GLint stepSizeLoc = glGetUniformLocation(g_programHandle, "StepSize");
    GL_ERROR();
    if (stepSizeLoc >= 0)
    {
        // Load data into uniform shader variable
        glUniform1f(stepSizeLoc, g_stepSize);
    }
    else
    {
//        cout << "StepSize is not bind to the uniform" << endl;
    }
    GL_ERROR();
    GLint transferFuncLoc = glGetUniformLocation(g_programHandle, "TransferFunc");
    if (transferFuncLoc >= 0)
    {
        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_1D, g_tffTexObj);
        // Load data into uniform shader variable
        glUniform1i(transferFuncLoc, 0);
    }
    else
    {
//        cout << "TransferFunc is not bind to the uniform" << endl;
    }
    GL_ERROR();
    GLint backFaceLoc = glGetUniformLocation(g_programHandle, "ExitPoints");
    if (backFaceLoc >= 0)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, g_bfTexObj);
        // Load data into uniform shader variable
        glUniform1i(backFaceLoc, 1);
    }
    else
    {
//        cout << "ExitPoints is not bind to the uniform" << endl;
    }
    GL_ERROR();
    GLint volumeLoc = glGetUniformLocation(g_programHandle, "VolumeTex");
    if (volumeLoc >= 0)
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_3D, g_volTexObj);
        // Load data into uniform shader variable
        glUniform1i(volumeLoc, 2);
    }
    else
    {
//        qDebug() << "VolumeTex is not bind to the uniform";

//        cout << "VolumeTex is not bind to the uniform" << endl;
    }

    GL_ERROR();
    GLint stepElevation = glGetUniformLocation(g_programHandle, "elevation");
    if (stepElevation >= 0)
    {
        // Load data into uniform shader variable
        glUniform1f(stepElevation, (float)elevationValue);
    }
    else
    {
//        qDebug() << "stepElevation is NOT bind to the uniform";

//        cout << "stepElevation is not bind to the uniform" << endl;
    }

    GL_ERROR();
    GLint stepAzimuth = glGetUniformLocation(g_programHandle, "azimuth");
    if (stepAzimuth >= 0)
    {
        // Load data into uniform shader variable
        glUniform1f(stepAzimuth, (float)azimuthValue);
    }
    else
    {
//        cout << "stepAzimuth is not bind to the uniform" << endl;
    }

    GL_ERROR();
    GLint stepClipPlaneDepth = glGetUniformLocation(g_programHandle, "clipPlaneDepth");
    if (stepClipPlaneDepth >= 0)
    {
        // Load data into uniform shader variable
        glUniform1f(stepClipPlaneDepth, clipPlaneValue);
    }
    else
    {
//        cout << "stepAzimuth is not bind to the uniform" << endl;
    }

    GL_ERROR();
    GLint stepClipEnable = glGetUniformLocation(g_programHandle, "clip");
    if (stepClipEnable >= 0)
    {
        // Load data into uniform shader variable
        glUniform1i(stepClipEnable, clipEnableValue);
    }
    else
    {
//        qDebug() << "stepAzimuth is NOT bind to the uniform";

//        cout << "stepAzimuth is not bind to the uniform" << endl;
    }

    //=========================================================================
    // Intensity Control
    //=========================================================================
    GL_ERROR();
    GLint intensityMinValue = glGetUniformLocation(g_programHandle, "intensityMin");
    if (intensityMinValue >= 0) {
        // Load data into uniform shader variable
        glUniform1f(intensityMinValue, intensityMin);
    }
    else {
//        qDebug() << "intensityMin is NOT bind to the uniform";
//        cout << "intensityMin is not bind to the uniform" << endl;
    }

    GL_ERROR();
    GLint intensityMaxValue = glGetUniformLocation(g_programHandle, "intensityMax");
    if (intensityMaxValue >= 0) {
        // Load data into uniform shader variable
        glUniform1f(intensityMaxValue, intensityMax);
    }
    else {
//        qDebug() << "intensityMax is NOT bind to the uniform";
//        cout << "intensityMax is not bind to the uniform" << endl;
    }

    //=========================================================================
    // TFF
    //=========================================================================
    // Red
    GL_ERROR();
    GLint redCenterValue = glGetUniformLocation(g_programHandle, "redCenter");
    if (redCenterValue >= 0) {
        // Load data into uniform shader variable
        glUniform1f(redCenterValue, redCenter);
    }
    else {
//        qDebug() << "redCenterValue is NOT bind to the uniform";
//        cout << "redCenterValue is not bind to the uniform" << endl;
    }

    GL_ERROR();
    GLint redWidthValue = glGetUniformLocation(g_programHandle, "redWidth");
    if (redWidthValue >= 0) {
        // Load data into uniform shader variable
        glUniform1f(redWidthValue, redWidth);
    }
    else {
//        qDebug() << "redWidth is NOT bind to the uniform";
//        cout << "redWidth is not bind to the uniform" << endl;
    }

    // Green
    GL_ERROR();
    GLint greenCenterValue = glGetUniformLocation(g_programHandle, "greenCenter");
    if (greenCenterValue >= 0) {
        // Load data into uniform shader variable
        glUniform1f(greenCenterValue, greenCenter);
    }
    else {
//        qDebug() << "greenCenter is NOT bind to the uniform";
//        cout << "greenCenter is not bind to the uniform" << endl;
    }

    GL_ERROR();
    GLint greenWidthValue = glGetUniformLocation(g_programHandle, "greenWidth");
    if (greenWidthValue >= 0) {
        // Load data into uniform shader variable
        glUniform1f(greenWidthValue, greenWidth);
    }
    else {
//        qDebug() << "greenWidth is NOT bind to the uniform";
//        cout << "greenWidth is not bind to the uniform" << endl;
    }

    // Blue
    GL_ERROR();
    GLint blueCenterValue = glGetUniformLocation(g_programHandle, "blueCenter");
    if (blueCenterValue >= 0) {
        // Load data into uniform shader variable
        glUniform1f(blueCenterValue, blueCenter);
    }
    else {
//        qDebug() << "blueCenter is NOT bind to the uniform";
//        cout << "blueCenter is not bind to the uniform" << endl;
    }

    GL_ERROR();
    GLint blueWidthValue = glGetUniformLocation(g_programHandle, "blueWidth");
    if (blueWidthValue >= 0) {
        // Load data into uniform shader variable
        glUniform1f(blueWidthValue, blueWidth);
    }
    else {
//        qDebug() << "blueWidth is NOT bind to the uniform";
//        cout << "blueWidth is not bind to the uniform" << endl;
    }

    // Alpha
    GL_ERROR();
    GLint alphaCenterValue = glGetUniformLocation(g_programHandle, "alphaCenter");
    if (alphaCenterValue >= 0) {
        // Load data into uniform shader variable
        glUniform1f(alphaCenterValue, alphaCenter);
    }
    else {
//        qDebug() << "alphaCenter is NOT bind to the uniform";
//        cout << "alphaCenter is not bind to the uniform" << endl;
    }

    GL_ERROR();
    GLint alphaWidthValue = glGetUniformLocation(g_programHandle, "alphaWidth");
    if (alphaWidthValue >= 0) {
        // Load data into uniform shader variable
        glUniform1f(alphaWidthValue, alphaWidth);
    }
    else {
//        qDebug() << "alphaWidth is NOT bind to the uniform";
//        cout << "alphaWidth is not bind to the uniform" << endl;
    }

    GL_ERROR();
    GLint renderTypeValue = glGetUniformLocation(g_programHandle, "renderType");
    if (renderTypeValue >= 0) {
        // Load data into uniform shader variable
        glUniform1i(renderTypeValue, renderType);
    }
    else {
//        qDebug() << "renderType is NOT bind to the uniform";
//        cout << "renderType is not bind to the uniform" << endl;
    }
}

// init the shader object and shader program
void initShader()
{
    // vertex shader object for first pass
    g_bfVertHandle = initShaderObj("../../Volume_Rendering_Using_GLSL/shader/backface.vert", GL_VERTEX_SHADER);
    // fragment shader object for first pass
    g_bfFragHandle = initShaderObj("../../Volume_Rendering_Using_GLSL/shader/backface.frag", GL_FRAGMENT_SHADER);

    // vertex shader object for second pass
    g_rcVertHandle = initShaderObj("../../Volume_Rendering_Using_GLSL/shader/raycasting.vert", GL_VERTEX_SHADER);


    // fragment shader object for second pass
    //g_rcFragHandle = initShaderObj("../../Volume_Rendering_Using_GLSL/shader/raycasting.frag", GL_FRAGMENT_SHADER);

    // Surface Rendering
    // fragment shader object for second pass - Iso Superficie
    g_rcIsoFragHandle = initShaderObj("../../Volume_Rendering_Using_GLSL/shader/raycasting_direct_surface_vector_color_render.frag", GL_FRAGMENT_SHADER);
    // fragment shader object for second pass - Iso Superficie
    g_rcDirectSurfaceFragShaderHandle = initShaderObj("../../Volume_Rendering_Using_GLSL/shader/raycasting_direct_surface_render.frag", GL_FRAGMENT_SHADER);

    // Volume Rendering
    // fragment shader object for second pass - Ray Casting Grayscale Volume Rendering
    g_rcGrayFragShaderHandle = initShaderObj("../../Volume_Rendering_Using_GLSL/shader/raycasting_gray.frag", GL_FRAGMENT_SHADER);
    // fragment shader object for second pass - Ray Casting RGBA Volume Rendering
    g_rcRGBAFragShaderHandle = initShaderObj("../../Volume_Rendering_Using_GLSL/shader/raycasting_rgba.frag", GL_FRAGMENT_SHADER);

    // create the shader program , use it in an appropriate time
    g_programHandle = createShaderPgm();
}

// link the shader objects using the shader program
void linkShader(GLuint shaderPgm, GLuint newVertHandle, GLuint newFragHandle)
{
    const GLsizei maxCount = 2;
    GLsizei count;
    GLuint shaders[maxCount];
    glGetAttachedShaders(shaderPgm, maxCount, &count, shaders);
    // cout << "get VertHandle: " << shaders[0] << endl;
    // cout << "get FragHandle: " << shaders[1] << endl;
    GL_ERROR();
    for (int i = 0; i < count; i++) {
        glDetachShader(shaderPgm, shaders[i]);
    }
    // Bind index 0 to the shader input variable "VerPos"
    glBindAttribLocation(shaderPgm, 0, "VerPos");
    // Bind index 1 to the shader input variable "VerClr"
    glBindAttribLocation(shaderPgm, 1, "VerClr");
    GL_ERROR();
    glAttachShader(shaderPgm,newVertHandle);
    glAttachShader(shaderPgm,newFragHandle);
    GL_ERROR();
    glLinkProgram(shaderPgm);
    if (GL_FALSE == checkShaderLinkStatus(shaderPgm))
    {
        cerr << "Failed to relink shader program!" << endl;
        exit(EXIT_FAILURE);
    }
    GL_ERROR();
}


// both of the two pass use the "render() function"
// the first pass render the backface of the boundbox
// the second pass render the frontface of the boundbox
// together with the frontface, use the backface as a 2D texture in the second pass
// to calculate the entry point and the exit point of the ray in and out the box.
void render_gl(GLenum cullFace)
{
    GL_ERROR();
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //  transform the box
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)g_winWidth/g_winHeight, 1.0f, 100.f);
    //    glm::mat4 projection = glm::perspective(15.0f, (GLfloat)g_winWidth/(GLfloat)g_winHeight, 1.0f, 100.f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f),
                                 glm::vec3(0.0f, 0.0f, 0.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 model = mat4(1.0f);
    //    model *= glm::rotate((float)g_angle, glm::vec3(0.0f, 1.0f, 0.0f));
    model *= glm::rotate((float)rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
    model *= glm::rotate((float)rotationZ, glm::vec3(1.0f, 0.0f, 0.0f));

    model *= glm::rotate(180.0f, vec3(1.0f, 0.0f, 0.0f));

    // Add zoom option with it or change de FOV value, best approach?
    //    model *= glm::scale(glm::mat4(1.0f),glm::vec3(1.2f, 1.2f, 1.2f));

    model *= glm::translate(glm::vec3(-0.5f, -0.5f, -0.5f));

    // notice the multiplication order: reverse order of transform
    glm::mat4 mvp = projection * view * model;

    GLuint mvpIdx = glGetUniformLocation(g_programHandle, "MVP");
    if (mvpIdx >= 0)
    {
        // Load data into uniform shader variable
        // Loads 4x4 matrices
        // 1 - Location of the uniform variable (retrieved after shader compilation using glGetUniformLocation())
        // 2 - Number of matrices we are updating
        // 3 - Indicates whether the matrix is supplied in row-major or column-major order.
        // 4 - Starting address of the matrix in memory

        // Every frame we generate a translation matrix
        // Shader multiplies the position of every vertex by that matrix which results in the combined object moving
        glUniformMatrix4fv(mvpIdx, 1, GL_FALSE, &mvp[0][0]);
    }
    else
    {
        cerr << "can't get the MVP" << endl;
    }
    GL_ERROR();
    drawBox(cullFace);
    GL_ERROR();
}

// ============================================================================
// Transfer Function
// ============================================================================
// Red
void MyGLWidget::redWidthUniform(int value)
{
    redWidth = (GLfloat) (value / 65536.0f) * 2.0f;
//    qDebug() << "redWidth" << redWidth;
    updateGL();
}

void MyGLWidget::redCenterUniform(int value)
{
    redCenter = (GLfloat) (value / 65536.0f);
//    qDebug() << "redCenter" << redCenter;
    updateGL();
}

// Green
void MyGLWidget::greenWidthUniform(int value)
{
    greenWidth = (GLfloat) (value / 65536.0f) * 2.0f;
//    qDebug() << "greenWidth" << greenWidth;
    updateGL();
}

void MyGLWidget::greenCenterUniform(int value)
{
    greenCenter = (GLfloat) (value / 65536.0f);
//    qDebug() << "greenCenter" << greenCenter;
    updateGL();
}

// Blue
void MyGLWidget::blueWidthUniform(int value)
{
    blueWidth = (GLfloat) (value / 65536.0f) * 2.0f;
//    qDebug() << "blueWidth" << blueWidth;
    updateGL();
}

void MyGLWidget::blueCenterUniform(int value)
{
    blueCenter = (GLfloat) (value / 65536.0f);
//    qDebug() << "blueCenter" << blueCenter;
    updateGL();
}

// Alpha
void MyGLWidget::alphaWidthUniform(int value)
{
    alphaWidth = (GLfloat) (value / 65536.0f) * 2.0f;
//    qDebug() << "alphaWidth" << alphaWidth;
    updateGL();
}

void MyGLWidget::alphaCenterUniform(int value)
{
    alphaCenter = (GLfloat) (value / 65536.0f);
//    qDebug() << "alphaCenter" << alphaCenter;
    updateGL();
}

// ============================================================================
// Intesity Control
// ============================================================================
void MyGLWidget::intensityMaxSliderUniform(int value)
{
    intensityMax = (GLfloat) (value / 65536.0f);
//    qDebug() << "intensityMax" << intensityMax;
    updateGL();
}

void MyGLWidget::intensityMinSliderUniform(int value)
{
    intensityMin = (GLfloat) (value / 65536.0f);
//    qDebug() << "intensityMin" << intensityMin;
    updateGL();
}

// init the 1 dimentional texture for transfer function
GLuint initTFF1DTex(const char* filename)
{
    //     read in the user defined data of transfer function
    ifstream inFile(filename, ifstream::in);
    if (!inFile) {
        cerr << "Error openning file: " << filename << endl;
        exit(EXIT_FAILURE);
    }

    const int MAX_CNT = 100000;
    GLubyte *tff = (GLubyte *) calloc(MAX_CNT, sizeof(GLubyte));
    inFile.read(reinterpret_cast<char *>(tff), MAX_CNT);
    if (inFile.eof()) {
        size_t bytecnt = inFile.gcount();
        *(tff + bytecnt) = '\0'; // --> Null Terminator
        cout << "bytecnt " << bytecnt << endl;
    }
    else if(inFile.fail()) {
        cout << filename << "read failed " << endl;
    }
    else {
        cout << filename << "is too large" << endl;
    }

    glGenTextures(1, &g_tffTexObj);
    glBindTexture(GL_TEXTURE_1D, g_tffTexObj);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT); // GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //GL_LINEAR
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, tff);
    free(tff);
    return g_tffTexObj;
}

// ===================================
// Boundung Box Clipping Planes
// ===================================
void MyGLWidget::cutBBLeft(int value)
{
    std::cout << value << endl;

    float lstep = 0.00f;
    lstep -= (value * 0.01f);

    vertices[0] = 0.0;
    vertices[3] = 0.0;
    vertices[6] = 0.0;
    vertices[9] = 0.0;

    vertices[0] -= lstep;
    vertices[3] -= lstep;
    vertices[6] -= lstep;
    vertices[9] -= lstep;

    glBindBuffer(GL_ARRAY_BUFFER, vertexdat);
    glBufferData(GL_ARRAY_BUFFER, 24*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    updateGL();
}

void MyGLWidget::cutBBRight(int value)
{
    float rstep = 0.00f;
    rstep -= (value * 0.01f);

    vertices[12] = 0.0;
    vertices[15] = 0.0;
    vertices[18] = 0.0;
    vertices[21] = 0.0;

    vertices[12] -= rstep;
    vertices[15] -= rstep;
    vertices[18] -= rstep;
    vertices[21] -= rstep;

    glBindBuffer(GL_ARRAY_BUFFER, vertexdat);
    glBufferData(GL_ARRAY_BUFFER, 24*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    updateGL();
}

void MyGLWidget::cutBBTop(int value)
{
    float tstep = 0.00f;
    tstep -= (value * 0.01f);

    vertices[5] = 0.0;
    vertices[11] = 0.0;
    vertices[17] = 0.0;
    vertices[23] = 0.0;

    vertices[5] -= tstep;
    vertices[11] -= tstep;
    vertices[17] -= tstep;
    vertices[23] -= tstep;

    glBindBuffer(GL_ARRAY_BUFFER, vertexdat);
    glBufferData(GL_ARRAY_BUFFER, 24*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    updateGL();
}

void MyGLWidget::cutBBBottom(int value)
{
    float dstep = 0.00f;
    dstep -= (value * 0.01f);

    vertices[2] = 0.0;
    vertices[8] = 0.0;
    vertices[14] = 0.0;
    vertices[20] = 0.0;

    vertices[2] -= dstep;
    vertices[8] -= dstep;
    vertices[14] -= dstep;
    vertices[20] -= dstep;

    glBindBuffer(GL_ARRAY_BUFFER, vertexdat);
    glBufferData(GL_ARRAY_BUFFER, 24*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    updateGL();
}

void MyGLWidget::cutBBFront(int value)
{
    float fstep = 0.00f;
    fstep -= (value * 0.01f);

    vertices[1] = 0.0;
    vertices[4] = 0.0;
    vertices[13] = 0.0;
    vertices[16] = 0.0;

    vertices[1] -= fstep;
    vertices[4] -= fstep;
    vertices[13] -= fstep;
    vertices[16] -= fstep;

    glBindBuffer(GL_ARRAY_BUFFER, vertexdat);
    glBufferData(GL_ARRAY_BUFFER, 24*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    updateGL();
}

void MyGLWidget::cutBBBack(int value)
{
    float bstep = 0.00f;
    bstep -= (value * 0.01f);

    vertices[7] = 0.0;
    vertices[10] = 0.0;
    vertices[19] = 0.0;
    vertices[22] = 0.0;

    vertices[7] -= bstep;
    vertices[10] -= bstep;
    vertices[19] -= bstep;
    vertices[22] -= bstep;

    glBindBuffer(GL_ARRAY_BUFFER, vertexdat);
    glBufferData(GL_ARRAY_BUFFER, 24*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    updateGL();
}


// ============================================================================
// Render Options
// ============================================================================
void MyGLWidget::keyPressed(int key)
{
    qDebug() << "Key" << key;
    switch (key)
    {
        case 1: renderType = 1; break;
        qDebug() << "Direct Volume Rendering - Grayscale" << endl;

        case 2: renderType = 2; break;
        qDebug() << "Direct Volume Rendering - RGBA" << endl;

        case 3: renderType = 3; break;
        qDebug() << "3 - Direct Surface Rendering" << endl;

        case 4: renderType = 4; break;
        qDebug() << "4 - Normal Vector Coloring" << endl;

        case 5: renderType = 5; break;
        qDebug() << "5 - Normal Vector Coloring (Absolute)" << endl;
    }
    updateGL();
}

void MyGLWidget::initializeGL()
{
    // Initialize the opengl extension entry points
    glewExperimental = GL_TRUE;

    GLenum glewError = glewInit();

    if (glewError != GLEW_OK)
    {
        std::cout << "Glew is not ok" << std::endl;
        exit(EXIT_FAILURE);
    }

    g_texWidth = g_winWidth;
    g_texHeight = g_winHeight;
    initVBO();
    initShader();
//    g_tffTexObj = initTFF1DTex("../../Volume_Rendering_Using_GLSL/tff.dat");
    g_bfTexObj = initFace2DTex(g_texWidth, g_texHeight);

    //    g_volTexObj = initVol3DTex("../../Volume_Rendering_Using_GLSL/model/head256.raw", 256, 256, 225);
//        g_volTexObj = initVol3DTex("../../Volume_Rendering_Using_GLSL/model/brainix.raw", 512, 512, 22);
//            g_volTexObj = initVol3DTex("../../Volume_Rendering_Using_GLSL/model/model2.raw", 512, 512, 58);
//        g_volTexObj = initVol3DTex("../../Volume_Rendering_Using_GLSL/model/phenix_COU_IV.raw", 512, 512, 361);

    //g_volTexObj = initVol3DTex("../../Volume_Rendering_Using_GLSL/model/phenix_os.raw", 512, 512, 361);

    GL_ERROR();
    initFrameBuffer(g_bfTexObj, g_texWidth, g_texHeight);
    GL_ERROR();
}

void MyGLWidget::paintGL()
{
    // the color of the vertex in the back face is also the location
    // of the vertex
    // save the back face to the user defined framebuffer bound
    // with a 2D texture named `g_bfTexObj`
    // draw the front face of the box
    // in the rendering process, i.e. the ray marching process
    // loading the volume `g_volTexObj` as well as the `g_bfTexObj`
    // after vertex shader processing we got the color as well as the location of
    // the vertex (in the object coordinates, before transformation).
    // and the vertex assemblied into primitives before entering
    // fragment shader processing stage.
    // in fragment shader processing stage. we got `g_bfTexObj`
    // (correspond to 'VolumeTex' in glsl)and `g_volTexObj`(correspond to 'ExitPoints')
    // as well as the location of primitives.
    // the most important is that we got the GLSL to exec the logic. Here we go!
    // draw the back face of the box
    glEnable(GL_DEPTH_TEST);
    // test the gl_error
    GL_ERROR();
    // render to texture
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, g_frameBuffer);
    glViewport(0, 0, g_winWidth, g_winHeight);
    linkShader(g_programHandle, g_bfVertHandle, g_bfFragHandle);
    glUseProgram(g_programHandle);
    // cull front face
    render_gl(GL_FRONT);
    glUseProgram(0);
    GL_ERROR();

    if (renderType == 1)
    {
        cout << "1 - Direct Volume Rendering (Grayscale) Selected" << endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, g_winWidth, g_winHeight);
        linkShader(g_programHandle, g_rcVertHandle, g_rcGrayFragShaderHandle);
        GL_ERROR();
        glUseProgram(g_programHandle);
    }
    else if (renderType == 2)
    {
        cout << "2 - Direct Volume Rendering (RGBA) Selected" << endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, g_winWidth, g_winHeight);
        linkShader(g_programHandle, g_rcVertHandle, g_rcRGBAFragShaderHandle);
        GL_ERROR();
        glUseProgram(g_programHandle);
    }


    //=========================================================================
    //
    //=========================================================================
    //glReadPixels(mpx, mpy, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, read_data);
    //GL_ERROR();

//        qDebug() << "Picked Color[0] - RED   - " << read_data[0];
//        qDebug() << "Picked Color[1] - GREEN - " << read_data[1];
//        qDebug() << "Picked Color[2] - BLUE  - " << read_data[2];
//        qDebug() << "Picked Color[3] - ALPHA - " << read_data[3];

    //=========================================================================
    //
    //=========================================================================


    if (renderType == 3)
    {
        cout << "2 - 3 - Direct Surface Rendering Selected" << endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, g_winWidth, g_winHeight);
        linkShader(g_programHandle, g_rcVertHandle, g_rcDirectSurfaceFragShaderHandle);
        GL_ERROR();
        glUseProgram(g_programHandle);
    }

    if (renderType == 4 || renderType == 5)
    {
//        qDebug() << "4 - Normal Vector Coloring";
//        qDebug() << "5 - Normal Vector Coloring (Absolute)";

        glReadPixels(mpx, mpy, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, read_data);
        GL_ERROR();

        qDebug() << "Picked Color[0] - RED   - " << read_data[0];
        qDebug() << "Picked Color[1] - GREEN - " << read_data[1];
        qDebug() << "Picked Color[2] - BLUE  - " << read_data[2];
        qDebug() << "Picked Color[3] - ALPHA - " << read_data[3];

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, g_winWidth, g_winHeight);
        linkShader(g_programHandle, g_rcVertHandle, g_rcIsoFragHandle);
        GL_ERROR();
        glUseProgram(g_programHandle);
    }


    rcSetUinforms();
    GL_ERROR();
    // cull back face
    render_gl(GL_BACK);
    glUseProgram(0);
    GL_ERROR();
}

void MyGLWidget::resizeGL(int w, int h)
{
    // Do NOT resize 3D Volume
    w = 400;
    h = 400;

    g_winWidth = w;
    g_winHeight = h;
    g_texWidth = w;
    g_texHeight = h;
    glViewport(0, 0, g_winWidth, g_winHeight);
}

// ============================================
//
// ============================================
MyGLWidget::MyGLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    QGLFormat glFormat;

    glFormat.setVersion( 3, 3 );
    glFormat.setProfile( QGLFormat::CoreProfile );
    glFormat.setSampleBuffers(true);
    glFormat.setAlpha(true);
    glFormat.setAccum(true);
    glFormat.setDepth(true);
    glFormat.setDoubleBuffer(true);
    glFormat.setRgba(true);

    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    // Mouse Rotation
    rotationY = 0;
    rotationZ = 0;

    // Clip Plane Sliders
    elevationValue = 90;
    azimuthValue = 0;
    clipPlaneValue = 50.0f;

    // Transfer Function Sliders
    redCenter = 32768.0f;
    redWidth = 16384.0f;
    greenCenter = 32768.0f;
    greenWidth = 16384.0f;
    blueCenter = 32768.0f;
    blueWidth = 16384.0f;

    // Intensity Control
    intensityMin = 0.0f;
    intensityMax = 65536.0f;

    renderType = 1;
}

MyGLWidget::~MyGLWidget()
{}

QSize MyGLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize MyGLWidget::sizeHint() const
{
    return QSize(800, 800);
}

// Records the position of the mouse when a button is initially pressed
void MyGLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    float dx = (float) (event->x() - lastPos.x());
    float dy = (float) (event->y() - lastPos.y());

    if (event->buttons() & Qt::LeftButton)
    {
        rotationY += 0.01f * dx;
        rotationZ += 0.01f * dy;
        update();
    }
    if (event->buttons() & Qt::RightButton)
    {
        QTransform t;
        t.scale(1, -1);
        t.translate(0, -height()+1);
        QPoint pos = event->pos() * t;

        mpx = pos.x();
        mpy = pos.y();

//        qDebug() << "x" << pos.x();
//        qDebug() << "y" << pos.y();

        updateGL();
    }

    lastPos = event->pos();
}
