//#+#+#+#+#+#+#+#+#+#+#+#++#+#+#+#+#+#+#+#+#+
    // - WASD for camera position
    // - Mouse for view direction, scroll for zoom
    // - Page UP, DOWN for camera moment in upward direction
    // - Right, Left arrow key for Roll ( kind of).
//#+#+#+#+#+#+#+#+#+#+#+#++#+#+#+#+#+#+#+#+#+

#include "skybox.hpp"
#include<iostream>
using namespace std ;
#include "../stb_image.h"
#include <vector>

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture( const char* path) ;
unsigned int loadCubemap(vector<std::string> faces);



GLuint shaderProgram, shader_sky;
GLuint vbo, vao, vao_sky, vbo_sky;
unsigned int texture, cubemapTexture ;
GLuint proMatrix, viewMatrix;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);


bool firstMouse = true;
float yaw   = -90.0f; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  800.0 / 2.0;
float fov   =  45.0f;

//-----------------------------------------------------------------
// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;
//--------------------------------------------------------------

//6 faces, 2 triangles/face, 3 vertices/triangle
const int num_vertices = 36;


float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,

     0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

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
  //Cube 
  glGenVertexArrays (1, &vao);
  glGenBuffers (1, &vbo);
  glBindVertexArray (vao);
  glBindBuffer (GL_ARRAY_BUFFER, vbo);
  glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);
  // set up vertex array
  glEnableVertexAttribArray( 0 );
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5* sizeof(float), (void*)0 );
  
  glEnableVertexAttribArray( 1 );
  glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 5* sizeof(float), (void*)(3* sizeof(float)) );

  ////////////////////////////////////////////////////////////////

  glGenVertexArrays(1, &vao_sky);
  glGenBuffers (1, &vbo_sky);
  glBindVertexArray (vao_sky );
  glBindBuffer (GL_ARRAY_BUFFER, vbo_sky );
  glBufferData (GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW );
  glEnableVertexAttribArray(0);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0 ) ;

  //=====================//====================//========================//=====================

  texture = loadTexture( "escher.jpg");
  vector<std::string> faces=
         
  {
      "skybox/right.jpg",
      "skybox/left.jpg",
      "skybox/top.jpg",
      "skybox/bottom.jpg",
      "skybox/front.jpg",
      "skybox/back.jpg"
  };
         
          /*
  {
      "skybox/Ely35_px.jpg",
      "skybox/Ely35_nx.jpg",
      "skybox/Ely35_py.jpg",
      "skybox/Ely35_ny.jpg",
      "skybox/Ely35_pz.jpg",
      "skybox/Ely35_nz.jpg"
  };
          */
  cubemapTexture = loadCubemap(faces);

  glUseProgram(shaderProgram);
  glUniform1i( glGetUniformLocation(shaderProgram, "texture"), 0 ) ;

  glUseProgram( shader_sky) ;
  glUniform1i( glGetUniformLocation(shader_sky, "skybox"), 0);

}


//----------------------------------------------------------------------------


void renderGL(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(shaderProgram);

  // pass projection matrix to shader (note that in this case it could change every frame)
  glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "proMatrix"), 1, GL_FALSE, glm::value_ptr(projection));

  glm:: mat4 model = glm:: mat4(1.0f);

  // camera/view transformation
  glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(view));

  // Draw cube
  glBindVertexArray (vao);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);

  glm::mat4 skyMapView = glm::mat4(glm::mat3(view));  
  //Skybox cube
  glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
  glUseProgram(shader_sky) ;
  glUniformMatrix4fv(glGetUniformLocation(shader_sky, "proMatrix"), 1, GL_FALSE, glm::value_ptr(projection));

  glUniformMatrix4fv(glGetUniformLocation(shader_sky, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(skyMapView));

  glBindVertexArray(vao_sky);
  glBindTexture( GL_TEXTURE_CUBE_MAP, cubemapTexture) ;
  glDrawArrays(GL_TRIANGLES, 0, 36) ;
  glBindVertexArray(0);
  glDepthFunc(GL_LESS); 

}

//#################################################################################
int main(int argc, char** argv)  {
      //! The pointer to the GLFW window
      GLFWwindow* window;

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
      window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "texture cube + camera + skybox", NULL, NULL);
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

      //glfwSetKeyCallback(window, opengl::key_callback);
      glfwSetFramebufferSizeCallback(window, opengl::framebuffer_size_callback);
      glfwSetCursorPosCallback(window, mouse_callback);
      glfwSetScrollCallback(window, scroll_callback);


      //glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

      //Initialize GL state
      opengl::initGL();
      initBuffersGL();

      // Loop until the user closes the window
      while (glfwWindowShouldClose(window) == 0)  {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;    

        processInput(window);  

        // Render here
        renderGL();
        // Swap front and back buffers
        glfwSwapBuffers(window);      
        // Poll for and process events
        glfwPollEvents();
      }
      glfwTerminate();
      return 0;
}

//####################################################################################

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5 * deltaTime;
    float cameraSpeed2 = 1.5 * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
        cameraPos += cameraUp * cameraSpeed ;
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
        cameraPos -= cameraUp * cameraSpeed ;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) ==GLFW_PRESS ){
        cameraUp = glm::normalize(cameraUp - glm::cross(cameraFront, cameraUp) * cameraSpeed2  );
    }    
    if (glfwGetKey(window, GLFW_KEY_LEFT) ==GLFW_PRESS ){
        cameraUp = glm::normalize(cameraUp + glm::cross(cameraFront, cameraUp) * cameraSpeed2  );
    }

}

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
//-------------------------------------------------------------------------

