﻿///////////////////////////////////////////////////////////////////////////////
// shadermanager.cpp
// ============
// manage the loading and rendering of 3D scenes
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////
// Student: Gonzalo Patino

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>

// declaration of global variables
namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";
}

/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/
SceneManager::SceneManager(ShaderManager* pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
	m_pShaderManager = NULL;
	delete m_basicMeshes;
	m_basicMeshes = NULL;
}

/***********************************************************
 *  CreateGLTexture()
 *
 *  This method is used for loading textures from image files,
 *  configuring the texture mapping parameters in OpenGL,
 *  generating the mipmaps, and loading the read texture into
 *  the next available texture slot in memory.
 ***********************************************************/
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0;
	int height = 0;
	int colorChannels = 0;
	GLuint textureID = 0;

	// indicate to always flip images vertically when loaded
	stbi_set_flip_vertically_on_load(true);

	// try to parse the image data from the specified image file
	unsigned char* image = stbi_load(
		filename,
		&width,
		&height,
		&colorChannels,
		0);

	// if the image was successfully read from the image file
	if (image)
	{
		std::cout << "Successfully loaded image:" << filename << ", width:" << width << ", height:" << height << ", channels:" << colorChannels << std::endl;

		glGenTextures(1, &textureID);
		std::cout << "Generated Texture ID: " << textureID << " for file: " << filename << std::endl;

		glBindTexture(GL_TEXTURE_2D, textureID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if the loaded image is in RGB format
		if (colorChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		// if the loaded image is in RGBA format - it supports transparency
		else if (colorChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			std::cout << "Not implemented to handle image with " << colorChannels << " channels" << std::endl;
			return false;
		}

		// generate the texture mipmaps for mapping textures to lower resolutions
		glGenerateMipmap(GL_TEXTURE_2D);

		// free the image data from local memory
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		// register the loaded texture and associate it with the special tag string
		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;


		if (!glIsTexture(textureID)) {
			std::cout << "ERROR: Texture ID " << textureID << " is not valid!" << std::endl;
		}
		else {
			std::cout << "✅ Texture successfully validated and stored with tag: " << tag << std::endl;
		}



		return true;
	}

	std::cout << "Could not load image:" << filename << std::endl;

	// Error loading the image
	return false;
}

/***********************************************************
 *  BindGLTextures()
 *
 *  This method is used for binding the loaded textures to
 *  OpenGL texture memory slots.  There are up to 16 slots.
 ***********************************************************/
void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  DestroyGLTextures()
 *
 *  This method is used for freeing the memory in all the
 *  used texture memory slots.
 ***********************************************************/
void SceneManager::DestroyGLTextures()
{
	/*for (int i = 0; i < m_loadedTextures; i++)
	{
		glGenTextures(1, &m_textureIDs[i].ID);
	}*/
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glDeleteTextures(1, &m_textureIDs[i].ID);
	}

}

/***********************************************************
 *  FindTextureID()
 *
 *  This method is used for getting an ID for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureID(std::string tag)
{
	int textureID = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureID = m_textureIDs[index].ID;
			bFound = true;
		}
		else
			index++;
	}

	return(textureID);
}

/***********************************************************
 *  FindTextureSlot()
 *
 *  This method is used for getting a slot index for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureSlot(std::string tag)
{
	int textureSlot = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureSlot = index;
			bFound = true;
		}
		else
			index++;
	}

	return(textureSlot);
}

/***********************************************************
 *  FindMaterial()
 *
 *  This method is used for getting a material from the previously
 *  defined materials list that is associated with the passed in tag.
 ***********************************************************/
bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
	if (m_objectMaterials.size() == 0)
	{
		return(false);
	}

	int index = 0;
	bool bFound = false;
	while ((index < m_objectMaterials.size()) && (bFound == false))
	{
		if (m_objectMaterials[index].tag.compare(tag) == 0)
		{
			bFound = true;
			material.ambientColor = m_objectMaterials[index].ambientColor;
			material.ambientStrength = m_objectMaterials[index].ambientStrength;
			material.diffuseColor = m_objectMaterials[index].diffuseColor;
			material.specularColor = m_objectMaterials[index].specularColor;
			material.shininess = m_objectMaterials[index].shininess;
		}
		else
		{
			index++;
		}
	}

	return(true);
}

/***********************************************************
 *  SetTransformations()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/
void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	// variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// set the scale value in the transform buffer
	scale = glm::scale(scaleXYZ);
	// set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
	// set the translation value in the transform buffer
	translation = glm::translate(positionXYZ);

	modelView = translation * rotationX * rotationY * rotationZ * scale;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

/***********************************************************
 *  SetShaderColor()
 *
 *  This method is used for setting the passed in color
 *  into the shader for the next draw command
 ***********************************************************/
void SceneManager::SetShaderColor(
	float redColorValue,
	float greenColorValue,
	float blueColorValue,
	float alphaValue)
{
	// variables for this method
	glm::vec4 currentColor;

	currentColor.r = redColorValue;
	currentColor.g = greenColorValue;
	currentColor.b = blueColorValue;
	currentColor.a = alphaValue;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/***********************************************************
 *  SetShaderTexture()
 *
 *  This method is used for setting the texture data
 *  associated with the passed in ID into the shader.
 ***********************************************************/
void SceneManager::SetShaderTexture(
	std::string textureTag)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, true);

		int textureID = -1;
		textureID = FindTextureSlot(textureTag);
		m_pShaderManager->setSampler2DValue(g_TextureValueName, textureID);
	}
}

/***********************************************************
 *  SetTextureUVScale()
 *
 *  This method is used for setting the texture UV scale
 *  values into the shader.
 ***********************************************************/
void SceneManager::SetTextureUVScale(float u, float v)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
	}
}

/***********************************************************
 *  SetShaderMaterial()
 *
 *  This method is used for passing the material values
 *  into the shader.
 ***********************************************************/
void SceneManager::SetShaderMaterial(
	std::string materialTag)
{
	if (m_objectMaterials.size() > 0)
	{
		OBJECT_MATERIAL material;
		bool bReturn = false;

		bReturn = FindMaterial(materialTag, material);
		if (bReturn == true)
		{
			// Send the material
			m_pShaderManager->setVec3Value("material.ambientColor", material.ambientColor);
			m_pShaderManager->setFloatValue("material.ambientStrength", material.ambientStrength);
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);

		}
	}
}

/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/*** Please refer to the code in the OpenGL sample project  ***/
/*** for assistance.                                        ***/
/**************************************************************/


/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene
 *  rendering
 ***********************************************************/
void SceneManager::PrepareScene()
{
	// only one instance of a particular mesh needs to be
	// loaded in memory no matter how many times it is drawn
	// in the rendered 3D scene

	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadBoxMesh(); //  Required for drone body
	m_basicMeshes->LoadCylinderMesh(); // required for camera lens

	//Drone Texture #1
	if (!CreateGLTexture("Resources/stainless_end.jpg", "droneTextureBlack"))
	{
		std::cerr << " Failed to load drone texture!" << std::endl;
	}
	//Drone Texture #2
	if (!CreateGLTexture("Resources/tilesf2.jpg", "droneTextureTiles"))
	{
		std::cerr << " Failed to load drone texture!" << std::endl;
	}
	//Drone Texture #3
	if (!CreateGLTexture("Resources/backdrop.jpg", "droneTextureBackDrops"))
	{
		std::cerr << " Failed to load drone texture!" << std::endl;
	}
	//Drone Texture #4
	if (!CreateGLTexture("Resources/pavers.jpg", "droneTextureStainlessEnd"))
	{
		std::cerr << " Failed to load drone texture!" << std::endl;
	}

	// Floor Texture
	if (!CreateGLTexture("Resources/rusticwood.jpg", "floorTexture"))
	{
		std::cerr << "Failed to load floor texture!" << std::endl;
	}

	// Camera Lens
	if (!CreateGLTexture("Resources/abstract.jpg", "cameraLens"))
	{
		std::cerr << "Failed to load floor texture!" << std::endl;
	}



	OBJECT_MATERIAL droneMaterial;
	droneMaterial.tag = "default";

	droneMaterial.ambientStrength = 0.2f;
	droneMaterial.ambientColor = glm::vec3(0.2f, 0.2f, 0.2f);
	droneMaterial.diffuseColor = glm::vec3(0.5f);
	droneMaterial.specularColor = glm::vec3(0.7f);
	droneMaterial.shininess = 32.0f;

	m_objectMaterials.push_back(droneMaterial);

	// Light source 0 – key light (from above front-right)
	m_pShaderManager->setVec3Value("lightSources[0].ambientColor", glm::vec3(0.2f));
	m_pShaderManager->setVec3Value("lightSources[0].diffuseColor", glm::vec3(0.6f));
	m_pShaderManager->setVec3Value("lightSources[0].specularColor", glm::vec3(0.8f));
	m_pShaderManager->setFloatValue("lightSources[0].specularIntensity", 0.8f);
	m_pShaderManager->setFloatValue("lightSources[0].focalStrength", 48.0f);





	// Light source 1 – soft fill light
	m_pShaderManager->setVec3Value("lightSources[1].position", glm::vec3(-4.0f, 3.0f, -4.0f));
	m_pShaderManager->setVec3Value("lightSources[1].ambientColor", glm::vec3(0.2f));
	m_pShaderManager->setVec3Value("lightSources[1].diffuseColor", glm::vec3(0.3f));
	m_pShaderManager->setVec3Value("lightSources[1].specularColor", glm::vec3(0.3f));
	m_pShaderManager->setFloatValue("lightSources[1].focalStrength", 16.0f);
	m_pShaderManager->setFloatValue("lightSources[1].specularIntensity", 0.5f);

	// Light source 2 – top fill light (softened)
	m_pShaderManager->setVec3Value("lightSources[2].position", glm::vec3(0.0f, 10.0f, 0.0f));
	m_pShaderManager->setVec3Value("lightSources[2].ambientColor", glm::vec3(0.1f));   // lower from 0.3
	m_pShaderManager->setVec3Value("lightSources[2].diffuseColor", glm::vec3(0.25f));  // lower from 0.6
	m_pShaderManager->setVec3Value("lightSources[2].specularColor", glm::vec3(0.3f));  // lower from 0.8
	m_pShaderManager->setFloatValue("lightSources[2].focalStrength", 32.0f);           // standard sharpness
	m_pShaderManager->setFloatValue("lightSources[2].specularIntensity", 0.5f);        // soften highlight


	// Light source 3 – subtle bounce from below
	m_pShaderManager->setVec3Value("lightSources[3].position", glm::vec3(0.0f, -2.0f, 0.0f));
	m_pShaderManager->setVec3Value("lightSources[3].ambientColor", glm::vec3(0.05f));
	m_pShaderManager->setVec3Value("lightSources[3].diffuseColor", glm::vec3(0.1f));
	m_pShaderManager->setVec3Value("lightSources[3].specularColor", glm::vec3(0.05f));
	m_pShaderManager->setFloatValue("lightSources[3].focalStrength", 16.0f);
	m_pShaderManager->setFloatValue("lightSources[3].specularIntensity", 0.1f);


}

/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by
 *  transforming and drawing the basic 3D shapes
 ***********************************************************/
void SceneManager::RenderScene()
{
	// Set the view position for lighting calculations
	m_pShaderManager->setVec3Value("viewPosition", glm::vec3(0.0f, 6.0f, 5.0f));

	// ----------------------------
	// DRAW FLOOR (Textured Plane)
	// ----------------------------
	glm::vec3 scaleXYZ = glm::vec3(20.0f, 1.0f, 10.0f);
	glm::vec3 positionXYZ = glm::vec3(0.0f, 1.1f, 0.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);

	// Enable lighting and texture
	m_pShaderManager->setIntValue("bUseLighting", true);
	m_pShaderManager->setBoolValue("bUseTexture", true);

	// Bind the correct texture for the floor
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, FindTextureID("floorTexture"));

	m_pShaderManager->setIntValue("objectTexture", 0);

	SetTextureUVScale(4.0f, 4.0f);           // Tile wood texture
	SetShaderMaterial("default");            // Phong lighting material

	// Draw floor plane
	m_basicMeshes->DrawPlaneMesh();

	// Reset texture binding (optional but clean)
	glBindTexture(GL_TEXTURE_2D, 0);
	m_pShaderManager->setBoolValue("bUseTexture", false);

	// ----------------------------
	// DRAW DRONE (Textured Meshes)
	// ----------------------------
	RenderDrone();  // Handles its own texture binding per part
}


void SceneManager::RenderDrone()
{
	// === DRONE BODY ===
	m_pShaderManager->setBoolValue("bUseTexture", true); // Enable texture use in the shader
	glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
	glBindTexture(GL_TEXTURE_2D, FindTextureID("droneTextureBlack")); // Bind the drone texture
	m_pShaderManager->setIntValue("objectTexture", 0); // Let the shader know to sample from texture unit 0

	SetTransformations(
		glm::vec3(3.0f, 1.0f, 2.0f),  // bigger scale
		0.0f, 0.0f, 0.0f,
		glm::vec3(0.0f, 2.0f, 0.0f)   // higher Y position
	);
	SetShaderMaterial("default"); // << ADD THIS LINE
	m_pShaderManager->setIntValue("bUseLighting", true);
	m_basicMeshes->DrawBoxMesh(); // draw the body
	m_pShaderManager->setBoolValue("bUseTexture", false); // Disable texture for non-textured parts

	// === CAMERA BOX ===
	m_pShaderManager->setBoolValue("bUseTexture", true);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, FindTextureID("cameraLens"));
	m_pShaderManager->setIntValue("objectTexture", 0);

	SetTransformations(
		glm::vec3(0.8f, 0.6f, 0.3f),  // smaller box
		0.0f, 0.0f, 0.0f,
		glm::vec3(0.0f, 1.6f, 0.9f)   // in front of the body
	);
	SetTextureUVScale(2.0f, 2.0f); // call this before drawing textured parts
	SetShaderMaterial("default"); // << ADD THIS LINE
	m_pShaderManager->setIntValue("bUseLighting", true);
	m_basicMeshes->DrawBoxMesh();
	m_pShaderManager->setBoolValue("bUseTexture", false);

	// === CAMERA LENS ===
	m_pShaderManager->setBoolValue("bUseTexture", false);
	SetShaderColor(0.0f, 0.0f, 0.0f, 1.0f);  // RGBA → solid black

	SetTransformations(
		glm::vec3(0.3f, 0.3f, 0.4f),
		90.0f, 0.0f, 0.0f,
		glm::vec3(0.0f, 1.5f, 0.8f)
	);
	SetTextureUVScale(2.0f, 2.0f); // call this before drawing textured parts
	SetShaderMaterial("default");
	m_pShaderManager->setIntValue("bUseLighting", true);
	m_basicMeshes->DrawCylinderMesh();
	m_pShaderManager->setBoolValue("bUseTexture", false);

	// === FRONT LEFT ARM ===
	m_pShaderManager->setBoolValue("bUseTexture", false);
	SetShaderColor(0.2f, 0.2f, 0.2f, 1.0f);  // Dark grey RGBA


	SetTransformations(
		glm::vec3(2.25f, 0.2f, 0.5f),
		0.0f, 30.0f, 0.0f,
		glm::vec3(-2.0f, 2.35f, 1.5f)
	);
	SetTextureUVScale(2.0f, 2.0f); // call this before drawing textured parts
	SetShaderMaterial("default");
	m_pShaderManager->setIntValue("bUseLighting", true);
	m_basicMeshes->DrawBoxMesh();
	m_pShaderManager->setBoolValue("bUseTexture", false);

	// === FRONT RIGHT ARM ===
	m_pShaderManager->setBoolValue("bUseTexture", false);
	SetShaderColor(0.2f, 0.2f, 0.2f, 1.0f);  // Dark grey RGBA


	SetTransformations(
		glm::vec3(2.25f, 0.2f, 0.5f),
		0.0f, -30.0f, 0.0f,
		glm::vec3(2.0f, 2.35f, 1.5f)
	);
	SetTextureUVScale(2.0f, 2.0f); // call this before drawing textured parts
	SetShaderMaterial("default");
	m_pShaderManager->setIntValue("bUseLighting", true);
	m_basicMeshes->DrawBoxMesh();
	m_pShaderManager->setBoolValue("bUseTexture", false);

	// === REAR LEFT ARM ===
	m_pShaderManager->setBoolValue("bUseTexture", false);
	SetShaderColor(0.2f, 0.2f, 0.2f, 1.0f);  // Dark grey RGBA


	SetTransformations(
		glm::vec3(2.25f, 0.2f, 0.5f),
		0.0f, -30.0f, 0.0f,
		glm::vec3(-2.0f, 2.35f, -1.5f)
	);
	SetTextureUVScale(2.0f, 2.0f); // call this before drawing textured parts
	SetShaderMaterial("default");
	m_pShaderManager->setIntValue("bUseLighting", true);
	m_basicMeshes->DrawBoxMesh();
	m_pShaderManager->setBoolValue("bUseTexture", false);

	// === REAR RIGHT ARM ===
	m_pShaderManager->setBoolValue("bUseTexture", false);
	SetShaderColor(0.2f, 0.2f, 0.2f, 1.0f);  // Dark grey RGBA


	SetTransformations(
		glm::vec3(2.25f, 0.2f, 0.5f),
		0.0f, 30.0f, 0.0f,
		glm::vec3(2.0f, 2.35f, -1.5f)
	);
	SetTextureUVScale(2.0f, 2.0f); // call this before drawing textured parts
	SetShaderMaterial("default");
	m_pShaderManager->setIntValue("bUseLighting", true);
	m_basicMeshes->DrawBoxMesh();
	m_pShaderManager->setBoolValue("bUseTexture", false);
}




