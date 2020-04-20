#include "GL_framework/gl_framework.hpp"
#include "shaders/shader_util.hpp"
#include "../glm/vec3.hpp"
#include "../glm/vec4.hpp"
#include "../glm/mat4x4.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/glm.hpp"

class Camera {
public:
	glm::vec3 cameraPos   ;
	glm::vec3 cameraFront  ;
	glm::vec3 cameraUp    ;
	glm:: vec3 cameraRight ;
	// defaults
	float yaw   = -90.0f; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
	float pitch =  0.0f;
	float lastX =  800.0f / 2.0;
	float lastY =  800.0 / 2.0;
	float fov   =  45.0f;	
    float deltaTime = 0.0f; // time between current frame and last frame
	float lastFrame = 0.0f; 
	bool firstMouse = true;


	Camera(glm:: vec3 initialPos, glm:: vec3 UpDirec, float width=800, float height=800){
		cameraPos = initialPos ;
		cameraUp = UpDirec ;
		cameraRight = glm:: normalize(glm:: cross(cameraFront, cameraUp)) ;
		lastX = width/2;
		lastY = height/2;
	}

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

};

