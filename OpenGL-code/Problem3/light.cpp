//#+#+#+#+#+#+#+#+#+#+#+#++#+#+#+#+#+#+#+#+#+
    // - WASD for camera position
    // - Mouse for view direction, scroll for zoom
    // - Page UP, DOWN for camera moment in upward direction
    // - Right, Left arrow key for Roll ( kind of).
    // - 'M' to switch from Blinn Phong to Phong
    // - 'L' to switch lighting mode from directional --> spotlight --> pointlight
//#+#+#+#+#+#+#+#+#+#+#+#++#+#+#+#+#+#+#+#+#+

#include "light.hpp"
#include<iostream>
using namespace std ;
#include "../stb_image.h"
#include <vector>
#include "camera.hpp"

void cursor_callback(GLFWwindow* window, double x, double y);
void scroll_callback(GLFWwindow* w, double x, double y);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window);
unsigned int loadTexture( const char* path) ;
unsigned int loadCubemap(vector<std::string> faces);

//! The pointer to the GLFW window
GLFWwindow* window;

GLuint shaderProgram, shader_sky, shader_light;
GLuint vbo, vao, vao_sky, vbo_sky, vao_light;
unsigned int cubemapTexture ;
GLuint proMatrix, viewMatrix;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;
int blinPhong = 1;
int lighttype = 1; // 0 = directional light, 1 = point light, 2 = spotlight
int movelight = 0 ;
glm::vec3 lightPos = glm::vec3(5.0f, 0.0f, 0.0f);
glm::vec3 scale = glm::vec3(0.3,0.3,0.3);
float movlt = -0.5 ;


glm::vec3 Camera_Pos = glm::vec3 (0.0, 0.0, 8.0);
glm::vec3 Camera_Up = glm::vec3 (0.0, 1.0, 0.0) ; 

//-----------------------------------------------------------------
Camera camera(Camera_Pos, Camera_Up) ;
float currentFrame ;
//--------------------------------------------------------------

//6 faces, 2 triangles/face, 3 vertices/triangle
const int num_vertices = 36;
GLuint albedo, normal, roughness, ambient, height_map ;

//-------------------------------------------------------------------------------

//---------------------------------------------------
void initBuffersGL(void)  {
  glEnable(GL_DEPTH_TEST);
  // Load shaders and use the resulting shader program
  std::string vertex_shader_file("shaders/vshader.glsl");
  std::string fragment_shader_file("shaders/fshader.glsl");

  std::vector<GLuint> shaderList;
  shaderList.push_back(opengl::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file));
  shaderList.push_back(opengl::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file));

  shaderProgram = opengl::CreateProgramGL(shaderList);
                      /////////////////////////////////
  std::string vertex_shader("shaders/vshader_sky.glsl");
  std::string fragment_shader("shaders/fshader_sky.glsl");

  std::vector<GLuint> shaderList_sky;
  shaderList_sky.push_back(opengl::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader));
  shaderList_sky.push_back(opengl::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader));

  shader_sky = opengl::CreateProgramGL(shaderList_sky);
                      ////////////////////////////////////
  std::string vertex_shader_li("shaders/vshader_light.glsl");
  std::string fragment_shader_li("shaders/fshader_light.glsl");

  std::vector<GLuint> shaderList_light;
  shaderList_light.push_back(opengl::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_li));
  shaderList_light.push_back(opengl::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_li));

  shader_light = opengl::CreateProgramGL(shaderList_light);
                      ////////////////////////////////////
  //Cube 
  glGenVertexArrays (1, &vao);
  glGenBuffers (1, &vbo);
  glBindVertexArray (vao);
  glBindBuffer (GL_ARRAY_BUFFER, vbo);
  glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);
  // set up vertex array
  glEnableVertexAttribArray( 0 );
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 8* sizeof(float), (void*)0 );
  
  glEnableVertexAttribArray( 1 );
  glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 8* sizeof(float), (void*)(3* sizeof(float)) );

  glEnableVertexAttribArray( 2 );
  glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 8* sizeof(float), (void*)(6* sizeof(float)) );

  ////////////////////////////////////////////////////////////////
  glGenVertexArrays(1, &vao_light) ;
  glBindVertexArray(vao_light) ;
  glBindBuffer (GL_ARRAY_BUFFER, vbo) ; // Same vertices (will update position later)
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  ////////////////////////////////////////////////////////////////

  glGenVertexArrays(1, &vao_sky);
  glGenBuffers (1, &vbo_sky);
  glBindVertexArray (vao_sky );
  glBindBuffer (GL_ARRAY_BUFFER, vbo_sky );
  glBufferData (GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW );
  glEnableVertexAttribArray(0);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0 ) ;

  //=====================//====================//========================//=====================

  albedo = loadTexture ("texture2/Pebbles_017_baseColor.jpg");
  normal = loadTexture ("texture2/Pebbles_017_normal.jpg") ;
  ambient = loadTexture ("texture2/Pebbles_017_ambientOcclusion.jpg") ;
  roughness = loadTexture ( "texture2/Pebbles_017_roughness.jpg") ;
  //height_map = loadTexture("texture2/Pebbles_017_height.jpg");

  vector<std::string> faces=
         
  {
      "skybox3/right.jpg",
      "skybox3/left.jpg",
      "skybox3/top.jpg",
      "skybox3/bottom.jpg",
      "skybox3/front.jpg",
      "skybox3/back.jpg"
  };
         
 
  cubemapTexture = loadCubemap(faces);
  
   glUseProgram(shaderProgram) ;
  glUniform1i (glGetUniformLocation(shaderProgram, "material.ALBEDO"), 0);

  glUniform1i (glGetUniformLocation(shaderProgram, "material.NORMAL"), 1);

  glUniform1i (glGetUniformLocation(shaderProgram, "material.AMBIENT"), 2);

  glUniform1i (glGetUniformLocation(shaderProgram, "material.ROUGHNESS"), 3);

  glUniform1i (glGetUniformLocation(shaderProgram, "material.HT"), 4) ;
  //glUniform1i (glGetUniformLocation(shaderProgram, "texture"),0) ;



  glUseProgram( shader_sky) ;
  glUniform1i( glGetUniformLocation(shader_sky, "skybox"), 0);


}


//----------------------------------------------------------------------------


void renderGL(void)
{
      glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
      glm::vec3 lightDirection(-1.0f, 0.0f, 0.0f);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(shaderProgram);

  // pass projection matrix to shader (note that in this case it could change every frame)
  glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "proMatrix"), 1, GL_FALSE, glm::value_ptr(projection));

  glm:: mat4 model = glm:: mat4(1.0f);
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model)) ;
  // camera/view transformation
  glm::mat4 view = glm::lookAt(camera.cameraPos, camera.cameraPos + camera.cameraFront, camera.cameraUp);
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(view));

  //normal matrix
  glm:: mat4 normalmatrix = glm::transpose(glm::inverse(model)); //used if plain texture?
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "normalmatrix"), 1, GL_FALSE, glm::value_ptr(normalmatrix)) ;
  glUniform3fv(glGetUniformLocation(shaderProgram, "viewLoc"),1, glm::value_ptr( camera.cameraPos));
  glUniform3fv(glGetUniformLocation(shaderProgram, "camfront"),1, glm::value_ptr( camera.cameraFront));

  glUniform3f(glGetUniformLocation(shaderProgram, "globalAmbient"), 0.1f, 0.1f, 0.1f) ;
  glUniform3f(glGetUniformLocation(shaderProgram, "light.diffuse"), 0.5f, 0.5f, 0.5f) ;
  glUniform3f(glGetUniformLocation(shaderProgram, "light.specular"), 1.0f, 1.0f, 1.0f);
  glUniform3fv(glGetUniformLocation(shaderProgram, "light.position"),1, glm::value_ptr (lightPos));
  glUniform3fv(glGetUniformLocation(shaderProgram, "light.direction"),1, glm::value_ptr(lightDirection));
  glUniform1i(glGetUniformLocation(shaderProgram, "light.type"), lighttype);
  glUniform1f(glGetUniformLocation(shaderProgram, "light.constant"), 1.0f);                     // |
  glUniform1f(glGetUniformLocation(shaderProgram, "light.linear"), 0.04f);                      //  } For ATTENUATION  
  glUniform1f(glGetUniformLocation(shaderProgram, "light.quadratic"), 0.012f);                   // |
  glUniform1f(glGetUniformLocation(shaderProgram, "light.cutoff"), glm::cos(glm::radians(7.5f)));        // FOR SPOTLIGHT
  glUniform1f(glGetUniformLocation(shaderProgram, "light.outerCutoff"), glm::cos(glm::radians(10.5f)));   //
  glUniform1i (glGetUniformLocation(shaderProgram, "blinnPhong"), blinPhong);
 
  glUniform3f (glGetUniformLocation(shaderProgram, "material.specular"), 0.5f, 0.5f, 0.5f);  
  glUniform1f (glGetUniformLocation(shaderProgram, "material.shininess"), 32.0f);
  
 
  // Draw cube
  glBindVertexArray (vao);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, albedo );
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, normal);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, ambient);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, roughness); 

  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, height_map); 
  //glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
  glDrawArrays(GL_TRIANGLES, 0, 36);  
  glBindVertexArray(0) ;


  // Draw light source
  glUseProgram(shader_light);
  glUniformMatrix4fv(glGetUniformLocation(shader_light, "proMatrix"), 1, GL_FALSE, glm::value_ptr(projection));
  glUniformMatrix4fv(glGetUniformLocation(shader_light, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(view));  
  //model = glm::translate(glm::mat4(1.0f), lightPos);
  model = glm::scale(glm::mat4(1.0f), scale);
  model = glm::translate(model, lightPos) ;

  glUniformMatrix4fv(glGetUniformLocation(shader_light, "model"), 1, GL_FALSE, glm::value_ptr(model));
  glUniform3fv(glGetUniformLocation( shader_light, "light"),1, glm::value_ptr (lightColor)) ;
  glBindVertexArray (vao_light);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0) ;


  //Skybox cube prep
  glm::mat4 skyMapView = glm::mat4(glm::mat3(view));  // VIP ;)

  glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
  glUseProgram(shader_sky) ;
  glUniformMatrix4fv(glGetUniformLocation(shader_sky, "proMatrix"), 1, GL_FALSE, glm::value_ptr(projection));

  glUniformMatrix4fv(glGetUniformLocation(shader_sky, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(skyMapView));
  glBindTexture( GL_TEXTURE_CUBE_MAP, cubemapTexture) ;
  glBindVertexArray(vao_sky);
  glDrawArrays(GL_TRIANGLES, 0, 36) ;
  glBindVertexArray(0);
  glDepthFunc(GL_LESS); 

}

//#################################################################################
int main(int argc, char** argv)  {



      //! Setting up the GLFW Error callback
      glfwSetErrorCallback(opengl::error_callback);

      //! Initialize GLFW
      if (!glfwInit())
        return -1;
      //We want OpenGL 4.0
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); 
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
      //This is for MacOSX - can be omitted otherwise
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
      //We don't want the old OpenGL 
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
      //! Create a windowed mode window and its OpenGL context
      window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "texture cube + camera + skybox + lighting", NULL, NULL);
      if (!window)   {
        glfwTerminate();
        return -1;
      }
      
      //! Make the window's context current 
      glfwMakeContextCurrent(window);

      //Initialize GLEW
      //Turn this on to get Shader based OpenGL
      glewExperimental = GL_TRUE;
      GLenum err = glewInit();
      if (GLEW_OK != err)  {
        //Problem: glewInit failed, something is seriously wrong.
        std::cerr<<"GLEW Init Failed : %s"<<std::endl;
      }

      //Print and see what context got enabled
      std::cout<<"Vendor: "<<glGetString (GL_VENDOR)<<std::endl;
      std::cout<<"Renderer: "<<glGetString (GL_RENDERER)<<std::endl;
      std::cout<<"Version: "<<glGetString (GL_VERSION)<<std::endl;
      std::cout<<"GLSL Version: "<<glGetString (GL_SHADING_LANGUAGE_VERSION)<<std::endl;

      glfwSetKeyCallback(window, key_callback);
      glfwSetFramebufferSizeCallback(window, opengl::framebuffer_size_callback);
      glfwSetCursorPosCallback(window, cursor_callback);
      glfwSetScrollCallback(window, scroll_callback);


      //glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

      //Initialize GL state
      opengl::initGL();
      initBuffersGL();

      // Loop until the user closes the window
      while (glfwWindowShouldClose(window) == 0)  {
        currentFrame = glfwGetTime();
        camera.deltaTime = currentFrame - camera.lastFrame;
        camera.lastFrame = currentFrame;    

        camera.processInput(window);  

        // Render here
        renderGL();
        //activate textures
               
        // Swap front and back buffers
        glfwSwapBuffers(window);      
        // Poll for and process events
        glfwPollEvents();
      }
      glfwTerminate();
      return 0;
}

//####################################################################################



unsigned int loadTexture( const char * path){
   unsigned int texture ;         
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_2D, texture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   int width, height, nrChannels;
   stbi_set_flip_vertically_on_load(true) ;
   unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
   if (data)
   {
       glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
       glGenerateMipmap(GL_TEXTURE_2D);
   }
   else
   {
       std::cout << "Failed to load texture" << std::endl;
   }
   stbi_image_free(data);
   return texture;
}

unsigned int loadCubemap( vector<std:: string> faces){
    unsigned int texture ;         
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true) ;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

   return texture;
}    

void cursor_callback(GLFWwindow* window, double x, double y){
    camera.mouse_callback(window, x, y);

}

void scroll_callback(GLFWwindow* w, double x, double y){
    camera.scroll_callback(window,x,y);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
  if (key == GLFW_KEY_M && action==GLFW_PRESS)
    blinPhong = (blinPhong + 1)%2 ;
  if (key == GLFW_KEY_L && action==GLFW_PRESS)
    lighttype = (lighttype + 1)%3 ;
  if (key == GLFW_KEY_C && action==GLFW_PRESS){
    movelight = (movelight+1)%2 ;
    if(movelight==1) movlt = -0.5 ;
    else movlt = 0.5 ;
  }
  if (key ==GLFW_KEY_Y && action==GLFW_PRESS)
    lightPos.y += movlt ;
  if (key ==GLFW_KEY_X && action==GLFW_PRESS)
    lightPos.x += movlt ;
  if (key ==GLFW_KEY_Z && action==GLFW_PRESS)
    lightPos.z += movlt ;            
}

//-------------------------------------------------------------------------

