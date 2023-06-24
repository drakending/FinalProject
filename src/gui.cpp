#include "gui.h"

gui::gui(Game::scene* sce)
{
	this->scene_ = sce;
}

void gui::newFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	if (!ImGui::Begin("model Panel", nullptr)) {
		ImGui::End();
	}
	else {
		static glm::vec3 euler;
		static int ini = 0;
		for (auto& t : scene_->IntegrateModelList)
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
			ImGui::ShowDemoWindow();
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
				cameraManager->camera.get()->transform.lookAt(t.transform.position);
			}
			if (ImGui::Button("rotate around this"))
			{
				cameraManager->camera.get()->transform.lookAt(t.transform.position);
				cameraManager->rotateCenter = t.transform.position;
				cameraManager->rotateLength = glm::length(t.transform.position - cameraManager->camera.get()->transform.position);
				cameraManager->rotateMode = true;
				cameraManager->rotateAngle = 0;
				cameraManager->rotateMode = true;

			}
			ImGui::Separator();

		}
		ImGui::End();
	}

	if (!ImGui::Begin("camera panel", nullptr)) {
		ImGui::End();
	}
	else
	{
		ImGui::Text("Position:");
		ImGui::Text("%f %f %f", cameraManager->camera.get()->transform.position.x, cameraManager->camera.get()->transform.position.y, cameraManager->camera.get()->transform.position.z);
		ImGui::Checkbox("wander mode", &cameraManager->wanderMode);
		if (cameraManager->rotateMode)
			ImGui::Checkbox("rotate mode", &cameraManager->rotateMode);
		ImGui::End();
	}

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
		auto t2 = lightManager->pointLight.get();
		ImGui::InputFloat("position_x", &t2->transform.position[0], 0.2, 1.0);
		ImGui::InputFloat("position_y", &t2->transform.position[1], 0.2, 1.0);
		ImGui::InputFloat("position_z", &t2->transform.position[2], 0.2, 1.0);
		ImGui::InputFloat("constant", &t2->kc);
		ImGui::InputFloat("linear", &t2->kl);
		ImGui::InputFloat("quadratic", &t2->kq);
		ImGui::ColorEdit3("point light color", (float*)&t2->color);
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}
