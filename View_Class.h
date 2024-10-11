//CLASSI AGGIUNTIVE
#include "Shader_Class.h"
#include "Camera_Class.h"
#include "Mesh_Class.h"
#include "Model_Class.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void renderModel(Shader_Class& ourModelShader, Model_Class& ourModel, float scale, float angle, Vec3F rotatore, float xTrasl, float yTrasl, float zTrasl);

// Impostazioni
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
bool shadows = true;
bool shadowsKeyPressed = false;

// camera
Camera_Class camera(Vec3F(0.0f, 1.5f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;	//Intervallo tra il frame corrente e l'ultimo
float lastFrame = 0.0f; //Tempo dell'ultimo frame

//Illuminazione
//rappresenta la posizione della sorgente luminosa nelle coordinate del world space
Vec3F lightPos(5.0f, 2.0f, 0.0f);

//rappresentano le scale degli oggetti di scena
const float scala1 = 0.008f;
const float scala2 = 0.01f;
const float scala3 = 0.4f;
const float scala4 = 0.03f;

//rappresenta il centro del world space per gli oggetti di scena 
const Vec3F traslatore = Vec3F(0, 0, 0);

//rappresentano le rotazioni nei vari assi per gli oggetti di scena 
const Vec3F rotatore1 = Vec3F(1, 0, 0);
const Vec3F rotatore2 = Vec3F(0, 1, 0);
const Vec3F rotatore3 = Vec3F(0, 0, 1);

class View_Class
{
public:

    View_Class(GLFWwindow* window)
    {
		Vec4F coloreSfondo = { 0.1f,0.1f,0.1f,1.0f };
		camera.VelocitaMovimento = 5.f;
		
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

		unique_ptr<Shader_Class> ourModelShader(new Shader_Class("Model_VS.vs", "Model_FS.fs"));
		unique_ptr<Shader_Class> depthShader(new Shader_Class("point_shadow_depth.vs", "point_shadow_depth.fs", "point_shadow_depth.gs"));

		unique_ptr<Model_Class> ourModel(new Model_Class("C:/Modelli/Map/MuseoMod.obj"));
		unique_ptr<Model_Class> alienModel(new Model_Class("C:/Modelli/Map2/Alien/Reptile Alien Creature.obj"));
		unique_ptr<Model_Class> saturn(new Model_Class("C:/Modelli/Map2/Saturn/Stylized_Planets.obj"));
		unique_ptr<Model_Class> dog(new Model_Class("C:/Modelli/Map2/Dog/Doguinho.obj"));
		unique_ptr<Model_Class> skull(new Model_Class("C:/Modelli/Map2/Skull/animal_skull_statuete.obj"));

		// Configurazione del FBO della mappa delle ombre 
	// -------------------------------------------------------------------------------------------------------------------------
		const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
		unsigned int depthMapFBO;
		
		//glCullFace(GL_FRONT);

		glGenFramebuffers(1, &depthMapFBO);
		// Crea la texture di profondita' della cubemap
		unsigned int depthCubemap;
		glGenTextures(1, &depthCubemap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		for (unsigned int i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		// Collega la texture di profondita' come il Buffer di profondita' FBO
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//glCullFace(GL_BACK);

			// Configurazione Matrici
	// -------------------------------------------------------------------------------------------------------------------------
		unique_ptr<Mat4F> matriceConti(new Mat4F());
		Mat4F projectionCamera = Mat4F();
		Mat4F view = Mat4F();

			// Ciclo di rendering
	// -------------------------------------------------------------------------------------------------------------------------
		while (!glfwWindowShouldClose(window))
		{
			// Per farlo girare alla stessa velocit� su ogni pc
			float currentFrame = static_cast<float> (glfwGetTime());
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			// Input
	// -------------------------------------------------------------------------------------------------------------------------
			processInput(window);

			// Rendering
	// -------------------------------------------------------------------------------------------------------------------------
			glClearColor(coloreSfondo.x, coloreSfondo.y, coloreSfondo.z, coloreSfondo.w);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Spostamento della luce lungo l'asse X tramite funzione Sin(teta)
	//--------------------------------------------------------------------------------------------------------------------------
			lightPos.x = static_cast<float>(sin(glfwGetTime() * 0.7) * 9.0);

			// Crea la Shadow CubeMap tramite la matrice di trasformazione
	// -------------------------------------------------------------------------------------------------------------------------
			float far_plane = 400.0f;
			Mat4F shadowProj = shadowProj.projectionMat4F(90.0f, (float)SHADOW_WIDTH / SHADOW_HEIGHT, far_plane);
			vector<Mat4F> shadowTransforms;

			shadowTransforms.push_back(shadowProj * matriceConti->lookat(move(lightPos), lightPos + Vec3F(1.0f, 0.0f, 0.0f), Vec3F(0.0f, -1.0f, 0.0f)));
			shadowTransforms.push_back(shadowProj * matriceConti->lookat(move(lightPos), lightPos + Vec3F(-1.0f, 0.0f, 0.0f), Vec3F(0.0f, -1.0f, 0.0f)));
			shadowTransforms.push_back(shadowProj * matriceConti->lookat(move(lightPos), lightPos + Vec3F(0.0f, 1.0f, 0.0f), Vec3F(0.0f, 0.0f, 1.0f)));
			shadowTransforms.push_back(shadowProj * matriceConti->lookat(move(lightPos), lightPos + Vec3F(0.0f, -1.0f, 0.0f), Vec3F(0.0f, 0.0f, -1.0f)));
			shadowTransforms.push_back(shadowProj * matriceConti->lookat(move(lightPos), lightPos + Vec3F(0.0f, 0.0f, 1.0f), Vec3F(0.0f, -1.0f, 0.0f)));
			shadowTransforms.push_back(shadowProj * matriceConti->lookat(move(lightPos), lightPos + Vec3F(0.0f, 0.0f, -1.0f), Vec3F(0.0f, -1.0f, 0.0f)));

			// Renderizza la scena della CubeMap
	// -------------------------------------------------------------------------------------------------------------------------
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);

			depthShader->use();

			for (unsigned int i = 0; i < 6; ++i)
				depthShader->setMat4("shadowMatrices[" + to_string(i) + "]", shadowTransforms[i]);

			shadowTransforms.clear();

			depthShader->setFloat("far_plane", far_plane);
			depthShader->setVec3("lightPos", lightPos);

			//glCullFace(GL_FRONT);

			renderModel(*depthShader, *ourModel, scala1, 0, rotatore1, traslatore.x + 0 / scala1, traslatore.y + 0 / scala1, traslatore.z + 0 / scala1);
			renderModel(*depthShader, *alienModel, scala2, 180, rotatore2, traslatore.x + (-4 / scala2), traslatore.y + (0 / scala2), traslatore.z + (7.5 / scala2));
			renderModel(*depthShader, *saturn, scala3, 16, rotatore3, traslatore.x + (-4.45 / scala3), traslatore.y + (1.1 / scala3), traslatore.z + (-1.25 / scala3));
			renderModel(*depthShader, *dog, scala3, 0, rotatore1, traslatore.x + (8 / scala3), traslatore.y + (0 / scala3), traslatore.z + (-8 / scala3));
			renderModel(*depthShader, *skull, scala4, -90, rotatore2, traslatore.x + (8.95 / scala4), traslatore.y + (0.7 / scala4), traslatore.z + (-1.2 / scala4));

			//glCullFace(GL_BACK);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// Rendering della scena originale 
	// -------------------------------------------------------------------------------------------------------------------------
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			ourModelShader->use();

			projectionCamera = projectionCamera.projectionMat4F(camera.Zoom, (float)SCR_WIDTH / SCR_HEIGHT, 100.0f);
			Mat4F view = camera.GetMatriceVisualizzazione();

			ourModelShader->setMat4("projection", projectionCamera);
			ourModelShader->setMat4("view", view);

			// Settiamo la luce
			ourModelShader->setFloat("far_plane", far_plane);
			ourModelShader->setVec3("lightPos", lightPos);
			ourModelShader->setVec3("viewPos", camera.Posizione);
			ourModelShader->setBool("shadows", shadows); // Abilitiamo/Disabilitiamo le ombre 

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
			
			renderModel(*ourModelShader, *ourModel, scala1, 0, rotatore1, traslatore.x + 0 / scala1, traslatore.y + 0 / scala1, traslatore.z + 0 / scala1);
			renderModel(*ourModelShader, *alienModel, scala2, 180, rotatore2, traslatore.x + (-4 / scala2), traslatore.y + (0 / scala2), traslatore.z + (7.5 / scala2));
			renderModel(*ourModelShader, *saturn, scala3, 16, rotatore3, traslatore.x + (-4.45 / scala3), traslatore.y + (1.1 / scala3), traslatore.z + (-1.25 / scala3));
			renderModel(*ourModelShader, *dog, scala3, 0, rotatore1, traslatore.x + (8 / scala3), traslatore.y + (0 / scala3), traslatore.z + (-8 / scala3));
			renderModel(*ourModelShader, *skull, scala4, -90, rotatore2, traslatore.x + (8.95 / scala4), traslatore.y + (0.7 / scala4), traslatore.z + (-1.2 / scala4));

			// scambia i buffer e gestisce i vari eventi di input 
	// -------------------------------------------------------------------------------------------------------------------------
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
		glDeleteFramebuffers(1, &depthMapFBO);
    };
};

// Processa tutti gli input
// ---------------------------------------------------------------------------------------------------------
inline void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(AVANTI, deltaTime);
		cout << camera.Posizione << endl;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(INDIETRO, deltaTime);
		cout << camera.Posizione << endl;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(SINISTRA, deltaTime);
		cout << camera.Posizione << endl;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(DESTRA, deltaTime);
		cout << camera.Posizione << endl;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !shadowsKeyPressed)
	{
		shadows = !shadows;
		shadowsKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		shadowsKeyPressed = false;
	}

}

// Ogni volta che la dimensione della finestra cambia richiama la funzione 
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// Assicura che il viewport corrisponda alle nuove dimensioni della finestra
	glViewport(0, 0, width, height);
}

// Ogni volta che muoviamo il mouse viene richiamata la funzione
//----------------------------------------------------------------------------------------------
inline void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // Inverte l'asse delle Y relativo a Top/Bottom

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// Ogni volta che usiamo la rotella del mouse viene richiamata la funzione
//----------------------------------------------------------------------------------------------
inline void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// Funzione che renderizza i modelli di scena con i vari shader
//----------------------------------------------------------------------------------------------
inline void renderModel(Shader_Class& ourModelShader, Model_Class& ourModel, float scale, float angle, Vec3F rotatore, float xTrasl, float yTrasl, float zTrasl)
{
	Mat4F modelObject = Mat4F(1.0f);

	modelObject = modelObject.scaling(move(modelObject), scale);
	modelObject = modelObject.translation(move(modelObject), xTrasl, yTrasl, zTrasl);
	modelObject = modelObject.rotateAllAxis(move(modelObject), angle, move(rotatore));

	ourModelShader.setMat4("model", modelObject);

	ourModelShader.setVec3("viewPos", camera.Posizione);

	//propriet� dei materiali
	ourModelShader.setFloat("material.shininess", 32.0f);

	ourModelShader.setVec3("pointLights.ambient", 0.5f, 0.5f, 0.5f);
	ourModelShader.setVec3("pointLights.diffuse", 0.8f, 0.8f, 0.8f);
	ourModelShader.setVec3("pointLights.specular", 1.0f, 1.0f, 1.0f);
	ourModelShader.setFloat("pointLights.constant", 1.0f);
	ourModelShader.setFloat("pointLights.linear", 0.04f);
	ourModelShader.setFloat("pointLights.quadratic", 0.02f);
	

	ourModel.Draw(ourModelShader);
}

