#pragma once
#include<iostream>
#include<map>
#include<array>
#include<base/glsl_program.h>
#include<base/model.h>
#include<base/texture2d.h>
#include<base/camera.h>
#include<base/light.h>
#include<base/framebuffer.h>
#include<base/texture_cubemap.h>
#include<scene.h>
#include<StorageManager.h>
#include<IntegrateModel.h>
namespace Game
{
	class NormalScene :public scene
	{
	public:
		std::vector<IntegrateModel> IntegrateModelList;
		std::shared_ptr<PerspectiveCamera> _camera;
		std::shared_ptr<DirectionalLight> _light;
		std::shared_ptr<PointLight> _pointLight;
		std::unique_ptr<GLSLProgram> modelShader;
		std::unique_ptr<GLSLProgram> lightShader;
		std::shared_ptr<Model> lightCube;
		NormalScene(StorageManager* storageManger);

		std::unique_ptr<GLSLProgram> _directionalDepthShader;
		std::unique_ptr<Framebuffer> _depthFbo;
		std::unique_ptr<Texture2D> _depthTexture;
		glm::mat4 _directionalLightSpaceMatrix;

		std::unique_ptr<GLSLProgram> _omnidirectionalDepthShader;
		std::array<std::unique_ptr<Framebuffer>, 6> _depthCubeFbos;
		std::unique_ptr<TextureCubemap> _depthCubeTexture;
		std::array<glm::mat4, 6> _pointLightSpaceMatrices;
		float _pointLightZfar = 100.0f;
		bool _enableOmnidirectionalPCF = true;
		int _directionalFilterRadius = 0;
		bool enableShadow = true;


		void draw() override;
		void updateDirectionalLightSpaceMatrix();
		void updatePointLightSpaceMatrices();
		void renderShadowMaps();
		void renderDirectionalLightShadowMap();
		void renderSceneFromLight(const GLSLProgram& shader);
		void renderPointLightShadowMap();


	};


}

