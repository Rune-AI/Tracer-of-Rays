#include "Scene.h"
#include "Utils.h"
#include "Material.h"

namespace dae {

#pragma region Base Scene
	//Initialize Scene with Default Solid Color Material (RED)
	Scene::Scene():
		m_Materials({ new Material_SolidColor({1,0,0})})
	{
		m_SphereGeometries.reserve(32);
		m_PlaneGeometries.reserve(32);
		m_TriangleMeshGeometries.reserve(32);
		m_Lights.reserve(32);
	}

	Scene::~Scene()
	{
		for(auto& pMaterial : m_Materials)
		{
			delete pMaterial;
			pMaterial = nullptr;
		}

		m_Materials.clear();
	}

	void dae::Scene::GetClosestHit(const Ray& ray, HitRecord& closestHit) const
	{
		//todo W1
		//assert(false && "No Implemented Yet!");
		//HitRecord closestHit = 
		Ray closestRay{ ray };

		for (const Sphere& sphere : GetSphereGeometries())
		{
			if (GeometryUtils::HitTest_Sphere(sphere, closestRay, closestHit))
			{
				closestRay.max = closestHit.t;
			}
		}

		for (const Plane& plane : GetPlaneGeometries())
		{
			if (GeometryUtils::HitTest_Plane(plane, closestRay, closestHit))
			{
				closestRay.max = closestHit.t;
			}
		}

		for (const Triangle& triangle : GetTriangleGeometries())
		{
			if (GeometryUtils::HitTest_Triangle(triangle, closestRay, closestHit))
			{
				closestRay.max = closestHit.t;
			}
		}

		for (const TriangleMesh& triangleMesh : GetTriangleMeshGeometries())
		{
			if (GeometryUtils::HitTest_TriangleMesh(triangleMesh, closestRay, closestHit))
			{
				closestRay.max = closestHit.t;
			}
		}
	}

	bool Scene::DoesHit(const Ray& ray) const
	{
		//todo W3
		/*assert(false && "No Implemented Yet!");
		return false;*/

		for (const Sphere& sphere : GetSphereGeometries())
		{
			if (GeometryUtils::HitTest_Sphere(sphere, ray))
			{
				return true;
			}
		}

		/*for (const Plane& plane : GetPlaneGeometries())
		{
			if (GeometryUtils::HitTest_Plane(plane, ray))
			{
				return true;
			}
		}*/

		for (const Triangle& triangle : GetTriangleGeometries())
		{
			if (GeometryUtils::HitTest_Triangle(triangle, ray))
			{
				return true;
			}
		}

		for (const TriangleMesh& triangleMesh : GetTriangleMeshGeometries())
		{
			if (GeometryUtils::HitTest_TriangleMesh(triangleMesh, ray))
			{
				return true;
			}
		}

		return false;
	}

#pragma region Scene Helpers
	Sphere* Scene::AddSphere(const Vector3& origin, float radius, unsigned char materialIndex)
	{
		Sphere s;
		s.origin = origin;
		s.radius = radius;
		s.materialIndex = materialIndex;

		m_SphereGeometries.emplace_back(s);
		return &m_SphereGeometries.back();
	}

	Plane* Scene::AddPlane(const Vector3& origin, const Vector3& normal, unsigned char materialIndex)
	{
		Plane p;
		p.origin = origin;
		p.normal = normal;
		p.materialIndex = materialIndex;

		m_PlaneGeometries.emplace_back(p);
		return &m_PlaneGeometries.back();
	}

	TriangleMesh* Scene::AddTriangleMesh(TriangleCullMode cullMode, unsigned char materialIndex)
	{
		TriangleMesh m{};
		m.cullMode = cullMode;
		m.materialIndex = materialIndex;

		m_TriangleMeshGeometries.emplace_back(m);
		return &m_TriangleMeshGeometries.back();
	}

	Light* Scene::AddPointLight(const Vector3& origin, float intensity, const ColorRGB& color)
	{
		Light l;
		l.origin = origin;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Point;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	Light* Scene::AddDirectionalLight(const Vector3& direction, float intensity, const ColorRGB& color)
	{
		Light l;
		l.direction = direction;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Directional;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	unsigned char Scene::AddMaterial(Material* pMaterial)
	{
		m_Materials.push_back(pMaterial);
		return static_cast<unsigned char>(m_Materials.size() - 1);
	}
#pragma endregion
#pragma endregion

#pragma region SCENE W1
	void Scene_W1::Initialize()
	{
				//default: Material id0 >> SolidColor Material (RED)
		constexpr unsigned char matId_Solid_Red = 0;
		const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });

		const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });
		const unsigned char matId_Solid_Green = AddMaterial(new Material_SolidColor{ colors::Green });
		const unsigned char matId_Solid_Magenta = AddMaterial(new Material_SolidColor{ colors::Magenta });

		//Spheres
		AddSphere({ -25.f, 0.f, 100.f }, 50.f, matId_Solid_Red);
		AddSphere({ 25.f, 0.f, 100.f }, 50.f, matId_Solid_Blue);

		//Plane
		AddPlane({ -75.f, 0.f, 0.f }, { 1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 75.f, 0.f, 0.f }, { -1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 0.f, -75.f, 0.f }, { 0.f, 1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 75.f, 0.f }, { 0.f, -1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 0.f, 125.f }, { 0.f, 0.f,-1.f }, matId_Solid_Magenta);
	}
#pragma endregion

#pragma region SCENE W2
	void Scene_W2::Initialize()
	{
		m_Camera.origin = { 0.f, 3.f, -9.0f };
		m_Camera.fovAngle = 45.f;
		//default: Material id0 >> SolidColor Material (RED)
		constexpr unsigned char matId_Solid_Red = 0;
		const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });

		const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });
		const unsigned char matId_Solid_Green = AddMaterial(new Material_SolidColor{ colors::Green });
		const unsigned char matId_Solid_Magenta = AddMaterial(new Material_SolidColor{ colors::Magenta });

		//Plane
		AddPlane({ -5.f,0.f,0.f }, { 1., 0.f, 0.f }, matId_Solid_Green);
		AddPlane({ 5.f, 0.f,0.f }, { -1., 0, 0 }, matId_Solid_Green);
		AddPlane({ 0.f, 0.f,0.f }, { 0.f, 1, 0 }, matId_Solid_Yellow);
		AddPlane({ 0.f, 10.f,0.f }, { 0.f, -1.f,  0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 0.f,10.f }, { 0.f, 0, -1.f }, matId_Solid_Magenta);
		//Spheres
		AddSphere({ -1.75f,1.f,0.f }, .75f, matId_Solid_Red);
		AddSphere({ 0.f,1.f,0.f }, .75f, matId_Solid_Blue);
		AddSphere({ 1.75f,1.f,0.f }, .75f, matId_Solid_Red);
		AddSphere({ -1.75f,3.f,0.f }, .75f, matId_Solid_Blue);
		AddSphere({ 0.f,3.f,0.f }, .75f, matId_Solid_Red);
		AddSphere({ 1.75f,3.f,0.f }, .75f, matId_Solid_Blue);

		//Light
		AddPointLight({ 0.f,5.f,-5.f }, 70.f, colors::White);
	}
#pragma endregion

#pragma region SCENE W3 TEST
	void Scene_W3_TestScene::Initialize()
	{
		m_Camera.origin = { 0.f, 1.f, -5.0f };
		m_Camera.fovAngle = 45.f;

		//default: Material id0 >> SolidColor Material (RED)
		const unsigned char matLambert_Red = AddMaterial(new Material_Lambert{ colors::Red, 1.f });
		const unsigned char matLambertPhong_Blue = AddMaterial(new Material_LambertPhong{ colors::Blue, 1.f, 1.f, 60.f });
		const unsigned char matLambert_Yellow = AddMaterial(new Material_Lambert{ colors::Yellow, 1.f });

		//Plane
		AddPlane({ 0.f,0.f,0.f }, { 0.f, 1.f, 0.f }, matLambert_Yellow);
		
		//Spheres
		AddSphere({ -.75f,1.f,0.f }, 1.f, matLambert_Red);
		AddSphere({ .75f,1.f,0.f }, 1.f, matLambertPhong_Blue);

		//Light
		AddPointLight({ 0.f,5.f,5.f }, 25.f, colors::White);
		AddPointLight({ 0.f,2.5f,-5.f }, 25.f, colors::White);
	}

	//void Scene_W3_TestScene::Initialize()
	//{
	//	m_Camera.origin = { 0.f, 1.f, -5.0f };
	//	m_Camera.fovAngle = 45.f;

	//	//default: Material id0 >> SolidColor Material (RED)
	//	const unsigned char matLambert_Red = AddMaterial(new Material_Lambert{ colors::Red, 1 });
	//	const unsigned char matLambert_Blue = AddMaterial(new Material_Lambert{ colors::Blue, 1 });
	//	const unsigned char matLambert_Yellow = AddMaterial(new Material_Lambert{ colors::Yellow, 1 });

	//	//Plane
	//	AddPlane({ 0.f,0.f,0.f }, { 0.f, 1.f, 0.f }, matLambert_Yellow);

	//	//Spheres
	//	AddSphere({ -.75f,1.f,0.f }, 1.f, matLambert_Red);
	//	AddSphere({ .75f,1.f,0.f }, 1.f, matLambert_Blue);

	//	//Light
	//	AddPointLight({ 0.f,5.f,5.f }, 25.f, colors::White);
	//	AddPointLight({ 0.f,2.5f,-5.f }, 25.f, colors::White);
	//}
#pragma endregion

#pragma region SCENE W3
	void Scene_W3::Initialize()
	{
		m_Camera.origin = { 0.f, 3.0f, -9.0f };
		m_Camera.fovAngle = 45.0f;
		//default: Materials 
		const auto matCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence({ 0.972f, 0.960f, 0.915f }, 1.0f, 1.0f));
		const auto matCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence({ 0.972f, 0.960f, 0.915f }, 1.0f, 0.6f));
		const auto matCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence({ 0.972f, 0.960f, 0.915f }, 1.0f, 0.1f));
		const auto matCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ 0.75f, 0.75f, 0.75f }, 0.0f, 1.0f));
		const auto matCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ 0.75f, 0.75f, 0.75f }, 0.0f, 0.6f));
		const auto matCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ 0.75f, 0.75f, 0.75f }, 0.0f, 0.1f));

		const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ 0.49f, 0.57f, 0.57f }, 1.0f));
		//Plane
		AddPlane({ 0.0f,0.0f,10.0f }, { 0.0f, 0.0f, -1.0f }, matLambert_GrayBlue); //BLACK
		AddPlane({ 0.0f, 0.0f,0.0f }, { 0.0f, 1.0f, 0.0f }, matLambert_GrayBlue); //BOTTOM
		AddPlane({ 0.0f, 10.0f,0.0f }, { 0.0f, -1.0f, 0.0f }, matLambert_GrayBlue); //TOP
		AddPlane({ 5.0f, 0.0f,0.0f }, { -1.0f, 0.0f,  0.0f }, matLambert_GrayBlue); //RIGHT
		AddPlane({ -5.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, matLambert_GrayBlue); //LEFT

		//TMP phong test spheres
		/*const auto matLamertPhong1 = AddMaterial(new Material_LambertPhong(colors::Blue, .5f, .5f, 3.f));
		const auto matLamertPhong2 = AddMaterial(new Material_LambertPhong(colors::Blue, .5f, .5f, 15.f));
		const auto matLamertPhong3 = AddMaterial(new Material_LambertPhong(colors::Blue, .5f, .5f, 50.f));
		AddSphere({ -1.75f,1.0f,0.0f }, .75f, matLamertPhong1);
		AddSphere({ 0.0f,1.0f,0.0f }, .75f, matLamertPhong2);
		AddSphere({ 1.75f,1.0f,0.0f }, .75f, matLamertPhong3);*/


		//Spheres
		AddSphere({ -1.75f,1.0f,0.0f }, .75f, matCT_GrayRoughMetal);
		AddSphere({ 0.0f,1.0f,0.0f }, .75f, matCT_GrayMediumMetal);
		AddSphere({ 1.75f,1.0f,0.0f }, .75f, matCT_GraySmoothMetal);
		AddSphere({ -1.75f,3.0f,0.0f }, .75f, matCT_GrayRoughPlastic);
		AddSphere({ 0.0f,3.0f,0.0f }, .75f, matCT_GrayMediumPlastic);
		AddSphere({ 1.75f,3.0f,0.0f }, .75f, matCT_GraySmoothPlastic);
		//Light
		AddPointLight({ 0.0f,5.0f,5.0f }, 50.0f, ColorRGB{ 1.0f, 0.61f, 0.45f }); //BACKLIGHT
		AddPointLight({ -2.5f, 5.0f, -5.0f }, 70.0f, ColorRGB{ 1.0f, 0.8f, 0.45f }); //Front Light Left
		AddPointLight({ 2.5f, 2.5f, -5.0f }, 50.0f, ColorRGB{ 0.34f, 0.47f, 0.68f });
	}
#pragma endregion


#pragma region SCENE W4 TEST
	void Scene_W4_TestScene::Initialize()
	{
		m_Camera.origin = { 0.f, 1.0f, -5.0f };
		
		/*m_Camera.origin = { 0.f, 1.0f, 4.0f };
		m_Camera.totalYaw = { 180.f };*/

		m_Camera.fovAngle = 45.0f;
		//default: Materials 
		const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ 0.49f, 0.57f, 0.57f }, 1.f));
		const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));


		
		//Plane
		AddPlane({ 0.0f,0.0f,10.0f }, { 0.0f, 0.0f, -1.0f }, matLambert_GrayBlue); //BLACK
		AddPlane({ 0.0f, 0.0f,0.0f }, { 0.0f, 1.0f, 0.0f }, matLambert_GrayBlue); //BOTTOM
		AddPlane({ 0.0f, 10.0f,0.0f }, { 0.0f, -1.0f, 0.0f }, matLambert_GrayBlue); //TOP
		AddPlane({ 5.0f, 0.0f,0.0f }, { -1.0f, 0.0f,  0.0f }, matLambert_GrayBlue); //RIGHT
		AddPlane({ -5.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, matLambert_GrayBlue); //LEFT

		//Triangle
		/*auto triangle = Triangle{ {-.75f, .5f, .0f}, {-.75f, 2.f, .0f}, {.75f, .5f, .0f} };
		triangle.cullMode = TriangleCullMode::FrontFaceCulling;
		triangle.materialIndex = matLambert_White;
		
		m_Triangles.emplace_back(triangle);*/

		//weird thing
		//pMesh = AddTriangleMesh(TriangleCullMode::NoCulling, matLambert_White);
		//pMesh->positions = { {-.75f, -1.f, .0f}, {-.75f, 1.f, .0f}, {.75f, 1.f, 1.f}, {.75f, -1.f, 0.f} };
		//pMesh->indices = {
		//	0,1,2, //triangle 1
		//	0,2,3 //triangle 2
		//};

		//pMesh->CalculateNormals();

		//pMesh->Translate({ 0.f, 1.5f, 0.f });
		//pMesh->RotateY(45);

		//pMesh->UpdateTransforms();

		//CUBE
		pMesh = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
		Utils::ParseOBJ("Resources/lowpoly_bunny2.obj",
			pMesh->positions, 
			pMesh->normals, 
			pMesh->indices);
		
		pMesh->Scale({ 2.f, 2.f, 2.f });
		//pMesh->Translate({ .0f, 1.f, 0.f });

		//No need to calc normals, they are calculated in ParseOBJ
		pMesh->UpdateAABB();
		pMesh->UpdateTransforms();
		
		//Light
		AddPointLight({ 0.0f,5.0f,5.0f }, 50.0f, ColorRGB{ 1.0f, 0.61f, 0.45f }); //BACKLIGHT
		AddPointLight({ -2.5f, 5.0f, -5.0f }, 70.0f, ColorRGB{ 1.0f, 0.8f, 0.45f }); //Front Light Left
		AddPointLight({ 2.5f, 2.5f, -5.0f }, 50.0f, ColorRGB{ 0.34f, 0.47f, 0.68f });
	}
	void Scene_W4_TestScene::Update(Timer* pTimer)
	{
		Scene::Update(pTimer);

		pMesh->RotateY(PI_DIV_2 * pTimer->GetTotal());
		pMesh->UpdateTransforms();
	}
#pragma endregion

#pragma region SCENE W4
	void Scene_W4_ReferenceScene::Initialize()
	{
		sceneName = "Reference Scene";
		m_Camera.origin = { 0.f, 3.0f, -9.0f };
		m_Camera.fovAngle = 45.0f;

		const auto matCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence({.972f, .960f, .915f}, 1.f, 1.f ));
		const auto matCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, .6f));
		const auto matCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, .1f));
		const auto matCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, .0f, 1.f));
		const auto matCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, .0f, 0.6f));
		const auto matCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, .0f, .1f));

		const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ 0.49f, 0.57f, 0.57f }, 1.f));
		const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

		//Plane
		AddPlane({ 0.0f,0.0f,10.0f }, { 0.0f, 0.0f, -1.0f }, matLambert_GrayBlue); //BLACK
		AddPlane({ 0.0f, 0.0f,0.0f }, { 0.0f, 1.0f, 0.0f }, matLambert_GrayBlue); //BOTTOM
		AddPlane({ 0.0f, 10.0f,0.0f }, { 0.0f, -1.0f, 0.0f }, matLambert_GrayBlue); //TOP
		AddPlane({ 5.0f, 0.0f,0.0f }, { -1.0f, 0.0f,  0.0f }, matLambert_GrayBlue); //RIGHT
		AddPlane({ -5.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, matLambert_GrayBlue); //LEFT

		//Balls
		AddSphere(Vector3{ -1.75f, 1.f, 0.f }, .75f, matCT_GrayRoughMetal);
		AddSphere(Vector3{ 0.f, 1.f, 0.f }, .75f, matCT_GrayMediumMetal);
		AddSphere(Vector3{ 1.75f, 1.f, 0.f }, .75f, matCT_GraySmoothMetal);
		AddSphere(Vector3{ -1.75f, 3.f, 0.f }, .75f, matCT_GrayRoughPlastic);
		AddSphere(Vector3{ 0.f, 3.f, 0.f }, .75f, matCT_GrayMediumPlastic);
		AddSphere(Vector3{ 1.75f, 3.f, 0.f }, .75f, matCT_GraySmoothPlastic);

		//Triangle
		const Triangle baseTriangle = { Vector3{-.75f, 1.5f, 0.f}, Vector3{.75f, 0.f, 0.f}, Vector3{-.75f, 0.f, 0.f} };

		//m_Meshes.push_back(AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White));

		m_Meshes[0] = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
		m_Meshes[0]->AppendTriangle(baseTriangle, true);
		m_Meshes[0]->Translate({ -1.75f, 4.5f, 0.f });
		m_Meshes[0]->UpdateAABB();
		m_Meshes[0]->UpdateTransforms();

		m_Meshes[1] = AddTriangleMesh(TriangleCullMode::FrontFaceCulling, matLambert_White);
		m_Meshes[1]->AppendTriangle(baseTriangle, true);
		m_Meshes[1]->Translate({ 0.f, 4.5f, 0.f });
		m_Meshes[1]->UpdateAABB();
		m_Meshes[1]->UpdateTransforms();

		m_Meshes[2] = AddTriangleMesh(TriangleCullMode::NoCulling, matLambert_White);
		m_Meshes[2]->AppendTriangle(baseTriangle, true);
		m_Meshes[2]->Translate({ 1.75f, 4.5f, 0.f });
		m_Meshes[2]->UpdateAABB();
		m_Meshes[2]->UpdateTransforms();


		//Light
		AddPointLight({ 0.0f,5.0f,5.0f }, 50.0f, ColorRGB{ 1.0f, 0.61f, 0.45f }); //BACKLIGHT
		AddPointLight({ -2.5f, 5.0f, -5.0f }, 70.0f, ColorRGB{ 1.0f, 0.8f, 0.45f }); //Front Light Left
		AddPointLight({ 2.5f, 2.5f, -5.0f }, 50.0f, ColorRGB{ 0.34f, 0.47f, 0.68f });
	}
	void Scene_W4_ReferenceScene::Update(Timer* pTimer)
	{
		Scene::Update(pTimer);

		const auto yawAngle = (cos(pTimer->GetTotal()) + 1.f) / 2.f * PI_2;
		for (const auto m : m_Meshes)
		{
			m->RotateY(yawAngle);
			m->UpdateTransforms();
		}
	}
#pragma endregion

#pragma region SCENE W4 Bunny
	void Scene_W4_BunnyScene::Initialize()
	{
		sceneName = "Bunny Scene";
		m_Camera.origin = { 0.f, 3.0f, -9.0f };
		m_Camera.fovAngle = 45.0f;

		//default: Materials 
		const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ 0.49f, 0.57f, 0.57f }, 1.f));
		const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

		//Plane
		AddPlane({ 0.0f,0.0f,10.0f }, { 0.0f, 0.0f, -1.0f }, matLambert_GrayBlue); //BLACK
		AddPlane({ 0.0f, 0.0f,0.0f }, { 0.0f, 1.0f, 0.0f }, matLambert_GrayBlue); //BOTTOM
		AddPlane({ 0.0f, 10.0f,0.0f }, { 0.0f, -1.0f, 0.0f }, matLambert_GrayBlue); //TOP
		AddPlane({ 5.0f, 0.0f,0.0f }, { -1.0f, 0.0f,  0.0f }, matLambert_GrayBlue); //RIGHT
		AddPlane({ -5.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, matLambert_GrayBlue); //LEFT

		pMesh = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
		Utils::ParseOBJ("Resources/lowpoly_bunny2.obj",
			pMesh->positions,
			pMesh->normals,
			pMesh->indices);

		pMesh->Scale({ 2.f, 2.f, 2.f });

		//No need to calc normals, they are calculated in ParseOBJ
		
		pMesh->UpdateAABB();
		pMesh->UpdateTransforms();

		//Light
		AddPointLight({ 0.0f,5.0f,5.0f }, 50.0f, ColorRGB{ 1.0f, 0.61f, 0.45f }); //BACKLIGHT
		AddPointLight({ -2.5f, 5.0f, -5.0f }, 70.0f, ColorRGB{ 1.0f, 0.8f, 0.45f }); //Front Light Left
		AddPointLight({ 2.5f, 2.5f, -5.0f }, 50.0f, ColorRGB{ 0.34f, 0.47f, 0.68f });
	}
	void Scene_W4_BunnyScene::Update(Timer* pTimer)
	{
		Scene::Update(pTimer);

		const auto yawAngle = (cos(pTimer->GetTotal()) + 1.f) / 2.f * PI_2;
		pMesh->RotateY(yawAngle);
		pMesh->UpdateTransforms();
	}
#pragma endregion

#pragma region SCENE EXTRA
	void Scene_Extra::Initialize()
	{
		sceneName = "Bunny Scene";
		m_Camera.origin = { 0.f, 2.0f, -8.0f };
		m_Camera.fovAngle = 45.0f;

		//default: Materials 
		const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ 0.49f, 0.57f, 0.57f }, 1.f));
		const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

		const auto matCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, 1.f));
		const auto matCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, .6f));
		const auto matCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, .1f));
		const auto matCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, .0f, 1.f));
		const auto matCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, .0f, 0.6f));
		const auto matCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, .0f, .1f));

		const auto matCT_Mirror = AddMaterial(new Material_Mirror({ .75f, .75f, .75f }, .0f, 1.f));

		////Plane
		//AddPlane({ 0.0f,0.0f,10.0f }, { 0.0f, 0.0f, -1.0f }, matLambert_GrayBlue); //BLACK
		
		//AddPlane({ 0.0f, 10.0f,0.0f }, { 0.0f, -1.0f, 0.0f }, matLambert_GrayBlue); //TOP
		//AddPlane({ 5.0f, 0.0f,0.0f }, { -1.0f, 0.0f,  0.0f }, matLambert_GrayBlue); //RIGHT
		//AddPlane({ -5.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, matLambert_GrayBlue); //LEFT


		const auto matCT_BlueRoughPlastic = AddMaterial(new Material_CookTorrence({ 67 / 255.f, 60 / 255.f, 149 / 255.f }, .0f, 1.f));
		const auto matCT_GroundRoughPlastic = AddMaterial(new Material_CookTorrence({ 165/ 255.f, 183 / 255.f, 215 / 255.f }, .0f, 1.f));
		const auto matCT_TealRoughPlastic = AddMaterial(new Material_CookTorrence({ 146 / 255.f, 253 / 255.f, 253 / 255.f }, .0f, 1.f));
		const auto matCT_YellowRoughPlastic = AddMaterial(new Material_CookTorrence({ 255 / 255.f, 255 / 255.f, 27 / 255.f }, .0f, 1.f));
		
		AddPlane({ 0.0f, 0.0f,0.0f }, { 0.0f, 1.0f, 0.0f }, matCT_GroundRoughPlastic); //BOTTOM
		
		std::string path = "Resources/";
		
		std::string files[10]{};
		files[0] = "lowpoly_cat_body.obj";
		files[1] = "lowpoly_cat_mustashe1.obj";
		files[2] = "lowpoly_cat_mustashe2.obj";
		files[3] = "lowpoly_cat_mustashe3.obj";
		files[4] = "lowpoly_cat_mustashe4.obj";
		files[5] = "lowpoly_cat_tail.obj";
		files[6] = "lowpoly_cat_eyeL.obj";
		files[7] = "lowpoly_cat_eyeR.obj";
		files[8] = "lowpoly_fish.obj";
		files[9] = "lowpoly_background.obj";
		
		for (size_t i = 0; i < 9; i++)
		{
			m_Meshes[i] = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matCT_GrayRoughPlastic);
			Utils::ParseOBJ_Test(path + files[i],
				m_Meshes[i]->positions,
				m_Meshes[i]->normals,
				m_Meshes[i]->indices);
		}
		/*m_Meshes[6] = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matCT_YellowRoughPlastic);
		Utils::ParseOBJ_Test(path + files[6],
			m_Meshes[6]->positions,
			m_Meshes[6]->normals,
			m_Meshes[6]->indices);

		m_Meshes[7] = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matCT_YellowRoughPlastic);
		Utils::ParseOBJ_Test(path + files[6],
			m_Meshes[7]->positions,
			m_Meshes[6]->normals,
			m_Meshes[6]->indices);

		m_Meshes[8] = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matCT_TealRoughPlastic);
		Utils::ParseOBJ_Test(path + files[8],
			m_Meshes[8]->positions,
			m_Meshes[8]->normals,
			m_Meshes[8]->indices);*/
		
		m_Meshes[9] = AddTriangleMesh(TriangleCullMode::NoCulling, matCT_Mirror);
		Utils::ParseOBJ_Test(path + files[9],
			m_Meshes[9]->positions,
			m_Meshes[9]->normals,
			m_Meshes[9]->indices);

		m_Meshes[9]->Translate({ 0, 0, -1.5f });

		//No need to calc normals, they are calculated in ParseOBJ
		for (const auto m : m_Meshes)
		{
			m->RotateY(M_PI);
			m->UpdateAABB();
			m->UpdateTransforms();
		}

		AddSphere(Vector3{ -3.f, 3.f, -0.5f }, .75f, matCT_Mirror);
		AddSphere(Vector3{ 3.f, 3.f, -0.5f }, .75f, matCT_Mirror);
		

		//AddPointLight({ 0.f, 2.f, -3.0f }, 10.0f, ColorRGB{ 1.0f, 1.0f, 1.0f });//Front
		//AddPointLight({ 0.0f,5.0f,10.0f }, 80.0f, ColorRGB{ 1.0f, 1.0f, 1.0f });//Back
		
		AddPointLight({ 0.f, 2.f, -3.0f }, 10.0f, ColorRGB{ 1.0f, 0.61f, 0.45f });//Front
		AddPointLight({ 0.0f,5.0f,10.0f }, 80.0f, ColorRGB{ 0.34f, 0.47f, 0.68f });//Back
	}
	void Scene_Extra::Update(Timer* pTimer)
	{
		Scene::Update(pTimer);

		float A = 0.1f;
		float f = 0.2f;
		float t = pTimer->GetTotal();
		float shift = 0;
		float y = A * sin(2 * PI * f * t + shift);
		float x = 1 * sin(2 * PI * (f - 0.1f) * t + shift);
		float z = 0.3 * sin(2 * PI * (f - 0.1f) * t + 3);

		y = y + 1;
		for (size_t i = 0; i < 8; i++)
		{
			m_Meshes[i]->Scale({ y, y, y });
			m_Meshes[i]->RotateY(z * M_PI/2 + M_PI);
		}
		m_Meshes[8]->Translate({ x, 0, 0 });
		
		for (const auto m : m_Meshes)
		{
			m->UpdateTransforms();
		}
		
	}
#pragma endregion
}
