#include "TestScene.h"
#include <fstream>
#include "ShaderManager.h"
#include "Mesh.h"

TestScene::TestScene(void)
{
}

TestScene::~TestScene(void)
{
}

void TestScene::OnInitialize()
{
	//Camera
	{
		Core::Object* camObj = new Core::Object(nullptr);
		Rendering::Camera* cam = camObj->AddComponent<Rendering::Camera>();
		Core::Transform* camObjTf = camObj->GetTransform();
		camObjTf->UpdatePosition(Math::Vector3(0,0,0));
		camObjTf->UpdateDirection(Math::Vector3(0,0,1));
		_cameraMgr->Add("mainCamera", cam, false);
	}

	{
		Core::Object* testObj = new Core::Object(nullptr);
		Rendering::Mesh::Mesh* mesh = testObj->AddComponent<Rendering::Mesh::Mesh>();
		Core::Transform* tf = testObj->GetTransform();
		_test = tf;
		tf->UpdatePosition(Math::Vector3(0, 0, 10));
		_rootObjects.Add("Test", testObj);
		{
			//struct TestVertex
			//{
			//	Math::Vector3 v;
			//};

			//static TestVertex vertices[] =
			//{
			//	Math::Vector3( -1.0f, 1.0f, -1.0f ) ,
			//	Math::Vector3( 1.0f, 1.0f, -1.0f ), 
			//	Math::Vector3( 1.0f, 1.0f, 1.0f ), 
			//	Math::Vector3( -1.0f, 1.0f, 1.0f ),

			//	Math::Vector3( -1.0f, -1.0f, -1.0f ),
			//	Math::Vector3( 1.0f, -1.0f, -1.0f ), 
			//	Math::Vector3( 1.0f, -1.0f, 1.0f ), 
			//	Math::Vector3( -1.0f, -1.0f, 1.0f ),

			//	Math::Vector3( -1.0f, -1.0f, 1.0f ), 
			//	Math::Vector3( -1.0f, -1.0f, -1.0f ),
			//	Math::Vector3( -1.0f, 1.0f, -1.0f ), 
			//	Math::Vector3( -1.0f, 1.0f, 1.0f ),

			//	Math::Vector3( 1.0f, -1.0f, 1.0f ),
			//	Math::Vector3( 1.0f, -1.0f, -1.0f ),
			//	Math::Vector3( 1.0f, 1.0f, -1.0f ),
			//	Math::Vector3( 1.0f, 1.0f, 1.0f ),

			//	Math::Vector3( -1.0f, -1.0f, -1.0f ),
			//	Math::Vector3( 1.0f, -1.0f, -1.0f ),
			//	Math::Vector3( 1.0f, 1.0f, -1.0f ),
			//	Math::Vector3( -1.0f, 1.0f, -1.0f ),

			//	Math::Vector3( -1.0f, -1.0f, 1.0f ),
			//	Math::Vector3( 1.0f, -1.0f, 1.0f ),
			//	Math::Vector3( 1.0f, 1.0f, 1.0f ),
			//	Math::Vector3( -1.0f, 1.0f, 1.0f )
			//};

			//static WORD indices[] =
			//{
			//	3,1,0,
			//	2,1,3,

			//	6,4,5,
			//	7,4,6,

			//	11,9,8,
			//	10,9,11,

			//	14,12,13,
			//	15,12,14,

			//	19,17,16,
			//	18,17,19,

			//	22,20,21,
			//	23,20,22
			//};

			//Rendering::Shader::VertexShader* vs = nullptr;
			//Rendering::Shader::PixelShader* ps = nullptr;

			//std::vector<D3D11_INPUT_ELEMENT_DESC> vertexDeclations;
			//D3D11_INPUT_ELEMENT_DESC desc;
			//desc.SemanticName = "POSITION";
			//desc.SemanticIndex = 0;
			//desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			//desc.InputSlot = 0; desc.AlignedByteOffset = 0;
			//desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			//desc.InstanceDataStepRate = 0;
			//vertexDeclations.push_back(desc);

			//vs = dynamic_cast<Rendering::Shader::VertexShader*>(_shaderMgr->LoadVertexShader("./", "Test:VS", true, vertexDeclations));
			//ps = dynamic_cast<Rendering::Shader::PixelShader*>(_shaderMgr->LoadPixelShader("./", "Test:PS", false));

			//Rendering::Material::Material* material = new Rendering::Material::Material("TestMaterial", vs, ps);
			//mesh->Create(vertices, ARRAYSIZE(vertices), sizeof(TestVertex), indices, ARRAYSIZE(indices), material, false);
		}
	}
}

void TestScene::OnRenderPreview()
{
}

void TestScene::OnUpdate(float dt)
{
	//static float t = 0.0f;
	//t+=0.01f;
	//_test->UpdatePosition(Math::Vector3(0,t,0));
}

void TestScene::OnRenderPost()
{

}

void TestScene::OnDestroy()
{

}