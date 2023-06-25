#include "gui.h"

gui::gui(Game::scene* sce)
{
	this->scene_ = sce;
}

void gui::cameraPanel()
{
	if (!ImGui::Begin("camera panel", nullptr)) {
		ImGui::End();
	}
	else
	{
		ImGui::Text("Position:");
		ImGui::Text("%f %f %f", cameraManager->camera->transform.position.x, cameraManager->camera->transform.position.y, cameraManager->camera->transform.position.z);
		ImGui::Checkbox("wander mode", &cameraManager->wanderMode);
		if (cameraManager->rotateMode)
			ImGui::Checkbox("rotate mode", &cameraManager->rotateMode);
		ImGui::End();
	}

}

void gui::lightPanel()
{
	if (!ImGui::Begin("light panel", nullptr)) {
		ImGui::End();
	}
	else
	{
		auto t = lightManager->directionalLight;
		static glm::vec3 direction;
		static int init = 0;
		if (!init) direction = t->transform.getFront();
		ImGui::Text("Directional Light");
		ImGui::InputFloat("direction_x", &direction.x, 0.2, 1.0);
		ImGui::InputFloat("direction_y", &direction.y, 0.2, 1.0);
		ImGui::InputFloat("direction_z", &direction.z, 0.2, 1.0);
		ImGui::ColorEdit3("directional light color", (float*)&t->color);
		t->transform.lookAt(t->transform.position + direction);
		ImGui::Separator();
		ImGui::Text("Point Light");
		auto t2 = lightManager->pointLight;
		ImGui::InputFloat("position_x", &t2->transform.position[0], 0.2, 1.0);
		ImGui::InputFloat("position_y", &t2->transform.position[1], 0.2, 1.0);
		ImGui::InputFloat("position_z", &t2->transform.position[2], 0.2, 1.0);
		ImGui::InputFloat("constant", &t2->kc);
		ImGui::InputFloat("linear", &t2->kl);
		ImGui::InputFloat("quadratic", &t2->kq);
		ImGui::ColorEdit3("point light color", (float*)&t2->color);
		ImGui::End();
	}
}

void gui::newFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	if (!ImGui::Begin("scene panel", nullptr))
	{
		ImGui::End();
	}
	else
	{
		struct Funcs {
			static bool ItemGetter(void* data, int n, const char** out_str) {
				std::vector<Game::scene*>* list = (std::vector<Game::scene*>*)data;
				*out_str = (*list)[n]->sceneName.c_str();
				return true;
			}
		};
		static int item_current = 0;
		if (ImGui::Combo("choose your scene", &item_current, &Funcs::ItemGetter, &sceneList, sceneList.size()))
		{
			scene_ = sceneList[item_current];
			if (scene_->sceneName == "basic scene")
			{
				auto scene = dynamic_cast<Game::BasicScene*>(scene_);
				cameraManager->camera = scene->_camera.get();
				lightManager->directionalLight = scene->_light.get();
				lightManager->pointLight = scene->_pointLight.get();
				clothManager->managerEnable = false;
			}
			else if (scene_->sceneName == "default scene")
			{
				auto scene = dynamic_cast<Game::NormalScene*>(scene_);
				cameraManager->camera = scene->_camera.get();
				lightManager->directionalLight = scene->_light.get();
				lightManager->pointLight = scene->_pointLight.get();
				clothManager->managerEnable = false;
			}
			else if (scene_->sceneName == "cloth scene")
			{
				auto scene = dynamic_cast<Game::ClothScene*>(scene_);
				cameraManager->camera = scene->_camera.get();
				lightManager->directionalLight = scene->_light.get();
				lightManager->pointLight = scene->_pointLight.get();
				clothManager->registerCloth(&scene->cloth);

			}
		}

		ImGui::End();
	}
	if (scene_->sceneName == "default scene")
	{
		auto defaultScene_ = dynamic_cast<Game::NormalScene*> (scene_);
		if (!ImGui::Begin("model Panel", nullptr)) {
			ImGui::End();
		}
		else {
			static glm::vec3 euler;
			static int ini = 0;
			for (auto& t : defaultScene_->IntegrateModelList)
			{
				if (!ini) {
					euler = glm::eulerAngles(t.transform.rotation);
					ini = 1;
				}
				ImGui::Text(t.name.c_str());
				ImGui::DragFloat("scale_x", &t.transform.scale[0], 0.2, 0, 40);
				ImGui::DragFloat("scale_y", &t.transform.scale[1], 0.2, 0, 40);
				ImGui::DragFloat("scale_z", &t.transform.scale[2], 0.2, 0, 40);
				ImGui::InputFloat("position_x", &t.transform.position[0], 0.2, 1.0);
				ImGui::InputFloat("position_y", &t.transform.position[1], 0.2, 1.0);
				ImGui::InputFloat("position_z", &t.transform.position[2], 0.2, 1.0);
				t.transform.rotation = glm::qua<float>(glm::radians(euler));
				ImGui::DragFloat("rotate_x", &euler[0], 5, 0, 360);
				ImGui::DragFloat("rotate_y", &euler[1], 5, 0, 360);
				ImGui::DragFloat("rotate_z", &euler[2], 5, 0, 360);
				int meshInd = 0;
				auto& t2 = t.subModel.front();
				char mesh[100];
				sprintf(mesh, "mesh%d texture", meshInd++);
				ImGui::Text(mesh);
				for (auto& t3 : t2.TextureTypeMap)
				{
					char texType[100];
					if (t3.first == t2.DIFFUSEMAP)
						sprintf(texType, "diffuse map");
					else if (t3.first == t2.SPECULARMAP)
						sprintf(texType, "specular map");
					if (ImGui::InputText(texType, t2.TextureNameMap[t3.first], 100))
						t2.changeTex();
				}
				if (ImGui::Button("look at this"))
				{
					glm::vec3 min = t2._model->getBoundingBox().min;
					glm::vec3 max = t2._model->getBoundingBox().max;
					GLfloat diagonal = sqrtf(powf(max.x - min.x, 2) +powf(max.y - min.y, 2) + powf(max.z - min.z, 2));
					cameraManager->camera->transform.position = glm::vec3(t.transform.position.x, t.transform.position.y,
						(min.z+max.z)/2 + diagonal * 1.5);
					cameraManager->camera->transform.lookAt(t.transform.position);
				}
				if (ImGui::Button("rotate around this"))
				{
					cameraManager->camera->transform.lookAt(t.transform.position);
					cameraManager->rotateCenter = t.transform.position;
					cameraManager->rotateLength = glm::length(t.transform.position - cameraManager->camera->transform.position);
					cameraManager->rotateMode = true;
					//感觉继续rotate的时候还是不要重新开始的好
					//cameraManager->rotateAngle = 0;
					//cameraManager->rotateMode = true;

				}
				ImGui::Separator();

			}
			ImGui::End();
		}

		cameraPanel();
		lightPanel();
		if (!ImGui::Begin("shadow panel", nullptr))
		{
			ImGui::End();
		}
		else
		{
			ImGui::Checkbox("shadow", &defaultScene_->enableShadow);
			ImGui::Text("directional light");
			ImGui::Separator();
			ImGui::SliderInt("pcf radius", &defaultScene_->_directionalFilterRadius, 0, 3);
			ImGui::Text("point light");
			ImGui::Separator();
			ImGui::Checkbox("enable pcf", &defaultScene_->_enableOmnidirectionalPCF);

			ImGui::End();
		}
	}
	if (scene_->sceneName == "basic scene")
	{
		auto scene = dynamic_cast<Game::BasicScene*> (scene_);

		cameraPanel();
		lightPanel();
		if (!ImGui::Begin("basic model panel", nullptr))
		{
			ImGui::End();
		}
		else
		{
			struct Funcs {
				static bool ItemGetter(void* data, int n, const char** out_str) {
					std::vector<std::string>* list = (std::vector<std::string>*)data;
					*out_str = (*list)[n].c_str();
					return true;
				}
			};
			static int item_current = 0;
			if (ImGui::Combo("choose your scene", &item_current, &Funcs::ItemGetter, &scene->modelName, scene->modelName.size()))
			{
				scene->currentModel = scene->modelList[scene->modelName[item_current]];
			}
			static glm::vec3 euler;
			static int ini = 0;
			if (!ini) {
				euler = glm::eulerAngles(scene->transform.rotation);
				ini = 1;
			}
			ImGui::ColorEdit3("ambient", (float*)&scene->ambientTex.x);
			ImGui::ColorEdit3("diffuse", (float*)&scene->diffuseTex.x);
			ImGui::ColorEdit3("specular", (float*)&scene->specularTex.x);
			ImGui::InputFloat("position_x", &scene->transform.position[0], 0.2, 1.0);
			ImGui::InputFloat("position_y", &scene->transform.position[1], 0.2, 1.0);
			ImGui::InputFloat("position_z", &scene->transform.position[2], 0.2, 1.0);
			scene->transform.rotation = glm::qua<float>(glm::radians(euler));
			ImGui::DragFloat("rotate_x", &euler[0], 5, 0, 360);
			ImGui::DragFloat("rotate_y", &euler[1], 5, 0, 360);
			ImGui::DragFloat("rotate_z", &euler[2], 5, 0, 360);
			if (scene->currentModel == scene->modelList["cone"])
			{
				ImGui::DragFloat("cone_radius", &scene->parameter["cone_radius"], 0.1, 0, 10);
				ImGui::DragFloat("cone_height", &scene->parameter["cone_height"], 0.1, 0, 10);
				scene->transform.scale = glm::vec3(scene->parameter["cone_radius"], scene->parameter["cone_height"], scene->parameter["cone_radius"]);
			}
			else if (scene->currentModel == scene->modelList["sphere"])
			{
				ImGui::DragFloat("radius", &scene->parameter["radius"], 0.1, 0, 10);
				scene->transform.scale = glm::vec3(scene->parameter["radius"]);
			}
			else if (scene->currentModel == scene->modelList["cube"])
			{
				ImGui::DragFloat("length", &scene->parameter["length"], 0.1, 0, 10);
				ImGui::DragFloat("width", &scene->parameter["width"], 0.1, 0, 10);
				ImGui::DragFloat("height", &scene->parameter["height"], 0.1, 0, 10);
				scene->transform.scale = glm::vec3(scene->parameter["length"],
					scene->parameter["height"],
					scene->parameter["width"]);
			}
			else if (scene->currentModel == scene->modelList["cylinder"])
			{
				ImGui::DragFloat("cone_radius", &scene->parameter["cylinder_radius"], 0.1, 0, 10);
				ImGui::DragFloat("cone_height", &scene->parameter["cylinder_height"], 0.1, 0, 10);
				scene->transform.scale = glm::vec3(scene->parameter["cylinder_radius"],
					scene->parameter["cylinder_height"],
					scene->parameter["cylinder_radius"]);
			}
			else if (scene->currentModel == scene->modelList["prism"])
			{
				ImGui::DragFloat("cone_radius", &scene->parameter["prism_radius"], 0.1, 0, 10);
				ImGui::DragFloat("cone_height", &scene->parameter["prism_height"], 0.1, 0, 10);
				scene->transform.scale = glm::vec3(scene->parameter["prism_radius"],
					scene->parameter["prism_height"],
					scene->parameter["prism_radius"]);
			}

			ImGui::End();
		}
	}
	if (scene_->sceneName == "cloth scene")
	{
		auto scene = dynamic_cast<Game::ClothScene*> (scene_);
		cameraPanel();
		lightPanel();
		if (!ImGui::Begin("cloth panel", nullptr))
		{
			ImGui::End();
		}
		else
		{
			ImGui::InputFloat("position_x", &scene->cloth.transform.position[0], 0.2, 1.0);
			ImGui::InputFloat("position_y", &scene->cloth.transform.position[1], 0.2, 1.0);
			ImGui::InputFloat("position_z", &scene->cloth.transform.position[2], 0.2, 1.0);
			ImGui::End();
		}

	}
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}
