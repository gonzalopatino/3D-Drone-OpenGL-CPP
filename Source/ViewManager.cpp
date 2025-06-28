///////////////////////////////////////////////////////////////////////////////
// viewmanager.h
// ============
// manage the viewing of 3D objects within the viewport
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "ViewManager.h"
#include "camera.h"

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>    


// declaration of the global variables and defines
namespace
{
	// Variables for window width and height
	const int WINDOW_WIDTH = 1000;
	const int WINDOW_HEIGHT = 800;
	const char* g_ViewName = "view";
	const char* g_ProjectionName = "projection";

	// camera object used for viewing and interacting with
	// the 3D scene
	Camera* g_pCamera = nullptr;

	// these variables are used for mouse movement processing
	float gLastX = WINDOW_WIDTH / 2.0f;
	float gLastY = WINDOW_HEIGHT / 2.0f;
	bool gFirstMouse = true;

	// time between current frame and last frame
	float gDeltaTime = 0.0f; 
	float gLastFrame = 0.0f;

	// the following variable is false when orthographic projection
	// is off and true when it is on
	bool bOrthographicProjection = false;
	float gOrthoZoom = 10.0f; // Default orthographic zoom size
	bool bProjectionChanged = false; //added
	


}

/***********************************************************
 *  ViewManager()
 *
 *  The constructor for the class
 ***********************************************************/
ViewManager::ViewManager(ShaderManager* pShaderManager)
{
	// initialize the member variables
	m_pShaderManager = pShaderManager;
	m_pWindow = NULL;

	// create and configure camera
	g_pCamera = new Camera();
	g_pCamera->Position = glm::vec3(12.0f, 8.0f, 10.0f);  // Camera position
	g_pCamera->Yaw = -135.0f;    // Look diagonally into the scene
	g_pCamera->Pitch = -25.0f;   // Look downward
	g_pCamera->Zoom = 25.0f;
	g_pCamera->updateCameraVectors(); // Finalize direction vectors
}


/***********************************************************
 *  ~ViewManager()
 *
 *  The destructor for the class
 ***********************************************************/
ViewManager::~ViewManager()
{
	// free up allocated memory
	m_pShaderManager = NULL;
	m_pWindow = NULL;
	if (NULL != g_pCamera)
	{
		delete g_pCamera;
		g_pCamera = NULL;
	}
}

/***********************************************************
 *  CreateDisplayWindow()
 *
 *  This method is used to create the main display window.
 ***********************************************************/
GLFWwindow* ViewManager::CreateDisplayWindow(const char* windowTitle)
{
	GLFWwindow* window = nullptr;

	// try to create the displayed OpenGL window
	window = glfwCreateWindow(
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		windowTitle,
		NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);

	// tell GLFW to capture all mouse events
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// this callback is used to receive mouse moving events
	glfwSetCursorPosCallback(window, &ViewManager::Mouse_Position_Callback);

	glfwSetScrollCallback(window, [](GLFWwindow*, double, double yoffset) {
		ViewManager::ProcessMouseScroll(static_cast<float>(yoffset));
		});


	// enable blending for supporting tranparent rendering
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_pWindow = window;

	return(window);
}

/***********************************************************
 *  Mouse_Position_Callback()
 *
 *  This method is automatically called from GLFW whenever
 *  the mouse is moved within the active GLFW display window.
 ***********************************************************/
void ViewManager::Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos)
{
	if (bOrthographicProjection)
		return; // Ignore mouse movement in orthographic mode

	float xpos = static_cast<float>(xMousePos);
	float ypos = static_cast<float>(yMousePos);

	if (gFirstMouse)
	{
		gLastX = xpos;
		gLastY = ypos;
		gFirstMouse = false;
	}

	float xoffset = xpos - gLastX;
	float yoffset = gLastY - ypos;

	gLastX = xpos;
	gLastY = ypos;

	if (g_pCamera)
		g_pCamera->ProcessMouseMovement(xoffset, yoffset);
}

/***********************************************************
 *  ProcessKeyboardEvents()
 *
 *  This method is called to process any keyboard events
 *  that may be waiting in the event queue.
 ***********************************************************/
void ViewManager::ProcessKeyboardEvents()
{
	static bool pKeyPressed = false;
	static bool oKeyPressed = false;

	// Handle window close on ESC
	if (glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(m_pWindow, true);

	// Move camera in six directions
	if (glfwGetKey(m_pWindow, GLFW_KEY_W) == GLFW_PRESS)
		g_pCamera->ProcessKeyboard(FORWARD, gDeltaTime);
	if (glfwGetKey(m_pWindow, GLFW_KEY_S) == GLFW_PRESS)
		g_pCamera->ProcessKeyboard(BACKWARD, gDeltaTime);
	if (glfwGetKey(m_pWindow, GLFW_KEY_A) == GLFW_PRESS)
		g_pCamera->ProcessKeyboard(LEFT, gDeltaTime);
	if (glfwGetKey(m_pWindow, GLFW_KEY_D) == GLFW_PRESS)
		g_pCamera->ProcessKeyboard(RIGHT, gDeltaTime);
	if (glfwGetKey(m_pWindow, GLFW_KEY_E) == GLFW_PRESS)
		g_pCamera->ProcessKeyboard(UP, gDeltaTime);
	if (glfwGetKey(m_pWindow, GLFW_KEY_Q) == GLFW_PRESS)
		g_pCamera->ProcessKeyboard(DOWN, gDeltaTime);

	//Reset
	if (glfwGetKey(m_pWindow, GLFW_KEY_R) == GLFW_PRESS)
	{
		g_pCamera->Position = glm::vec3(0.0f, 4.0f, 5.0f);
		g_pCamera->Yaw = -90.0f;
		g_pCamera->Pitch = -30.0f;
		g_pCamera->Zoom = 45.0f; // optional reset
		g_pCamera->updateCameraVectors();
	}

	// Toggle projection mode
	// Toggle Perspective Projection with P (on key press only)
	if (glfwGetKey(m_pWindow, GLFW_KEY_P) == GLFW_PRESS && !pKeyPressed)
	{
		bOrthographicProjection = false;
		bProjectionChanged = true; // 
		std::cout << "Switched to PERSPECTIVE projection" << std::endl;
		pKeyPressed = true;
	}
	if (glfwGetKey(m_pWindow, GLFW_KEY_P) == GLFW_RELEASE)
	{
		pKeyPressed = false;
	}

	// Toggle Orthographic Projection with O (on key press only)
	if (glfwGetKey(m_pWindow, GLFW_KEY_O) == GLFW_PRESS && !oKeyPressed)
	{
		bOrthographicProjection = true;
		bProjectionChanged = true; // 
		std::cout << "Switched to ORTHOGRAPHIC projection" << std::endl;
		g_pCamera->Position = glm::vec3(0.0f, 4.0f, 10.0f);   // Flat front view
		g_pCamera->Yaw = -90.0f;  // Facing negative Z axis
		g_pCamera->Pitch = 0.0f;  // No tilt
		g_pCamera->updateCameraVectors();





		oKeyPressed = true;
	}
	if (glfwGetKey(m_pWindow, GLFW_KEY_O) == GLFW_RELEASE)
	{
		oKeyPressed = false;
	}
}



void ViewManager::ProcessMouseScroll(float yoffset)
{
	if (bOrthographicProjection)
	{
		gOrthoZoom -= yoffset * 0.5f;  // Smooth zooming
		if (gOrthoZoom < 1.0f) gOrthoZoom = 1.0f;
		if (gOrthoZoom > 50.0f) gOrthoZoom = 50.0f;
	}
	else if (g_pCamera)
	{
		g_pCamera->ProcessMouseScroll(yoffset);
	}
}




/***********************************************************
 *  PrepareSceneView()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene 
 *  rendering
 ***********************************************************/
void ViewManager::PrepareSceneView()
{
	glm::mat4 view;
	glm::mat4 projection;

	// per-frame timing
	float currentFrame = glfwGetTime();
	gDeltaTime = currentFrame - gLastFrame;
	gLastFrame = currentFrame;

	// process any keyboard events that may be waiting in the 
	// event queue
	ProcessKeyboardEvents();

	// get the current view matrix from the camera
	view = g_pCamera->GetViewMatrix();

	// define the current projection matrix
	//projection = glm::perspective(glm::radians(g_pCamera->Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
	if (bOrthographicProjection)
	{
		// Orthographic projection with dynamic zoom
		projection = glm::ortho(
			-gOrthoZoom, gOrthoZoom,
			-gOrthoZoom * (float)WINDOW_HEIGHT / (float)WINDOW_WIDTH,
			gOrthoZoom * (float)WINDOW_HEIGHT / (float)WINDOW_WIDTH,
			0.1f, 100.0f);
	}
	else
	{
		// Perspective projection — realistic 3D
		projection = glm::perspective(glm::radians(g_pCamera->Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
	}

	// if the shader manager object is valid
	if (NULL != m_pShaderManager)
	{
		// set the view matrix into the shader for proper rendering
		m_pShaderManager->setMat4Value(g_ViewName, view);
		// set the view matrix into the shader for proper rendering
		m_pShaderManager->setMat4Value(g_ProjectionName, projection);
		// set the view position of the camera into the shader for proper rendering
		m_pShaderManager->setVec3Value("viewPosition", g_pCamera->Position);
	}
	bProjectionChanged = false; // ? Reset it for next frame

}