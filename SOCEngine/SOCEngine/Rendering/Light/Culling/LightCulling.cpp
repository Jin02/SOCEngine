#include "LightCulling.h"
#include "Director.h"

using namespace Device;
using namespace Core;
using namespace Rendering;
using namespace Rendering::Light;
using namespace GPGPU::DirectCompute;

LightCulling::LightCulling(const ComputeShader::ThreadGroup& threadGroupInfo) :
	_computeShader(nullptr), _threadGroupInfo(threadGroupInfo)
{

}

LightCulling::~LightCulling()
{

}

void LightCulling::Init(const std::string& folderPath, const std::string& fileName)
{
	Scene* scene = Director::GetInstance()->GetCurrentScene();
	auto shaderMgr = scene->GetShaderManager();

	ID3DBlob* blob = shaderMgr->CreateBlob(folderPath, fileName, "cs", "CS", false);
	_computeShader = new ComputeShader(_threadGroupInfo, blob);
	assert(_computeShader->Create());

	ID3D11DeviceContext* context = Director::GetInstance()->GetDirectX()->GetContext();
	std::vector<ComputeShader::InputBuffer> inputBuffer;
	std::vector<ComputeShader::OutputBuffer> outputBuffer;

	struct TestBuff
	{
		Math::Vector3 v1;
		Math::Vector2 v2;
	};
	std::vector<TestBuff> d;
	for(int i=0; i<100; ++i)
	{
		TestBuff t;
		t.v1 = Math::Vector3(i, i, i);
		t.v2 = Math::Vector2(-i, -i);
		d.push_back(t);
	}


	CSInputBuffer csInput;
	csInput.Create(sizeof(TestBuff), 100, d.data());
	ComputeShader::InputBuffer i;
	i.idx = 0;
	i.buffer = &csInput;
	inputBuffer.push_back(i);

	CSOutputBuffer csOb;
	csOb.Create(sizeof(TestBuff), 100);
	ComputeShader::OutputBuffer o;
	o.idx = 0;
	o.buffer = &csOb;
	outputBuffer.push_back(o);

	_computeShader->SetInputBuffers(inputBuffer);
	_computeShader->SetOutputBuffers(outputBuffer);
	_computeShader->Dispatch(context);

	Buffer::CPUReadConstBuffer readCB;
	readCB.Create(sizeof(TestBuff), 100);
	auto what = [&](const void* data)
	{
		std::ofstream fout("result.txt");
		const TestBuff* dataView = reinterpret_cast<const TestBuff*>(data);
		for(int i=0; i<100; ++i)
		{
			fout << "(" << dataView[i].v1.x << ", " << dataView[i].v1.y << ", " << dataView[i].v1.z <<
			", " << dataView[i].v2.x << ", " << dataView[i].v2.y << ")" << std::endl;
		}
		fout.close();
	};
	readCB.Read(context, &csOb, what);
}

void LightCulling::Run()
{
}