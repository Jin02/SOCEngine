#include "CSOutputBuffer.h"
#include "Director.h"

using namespace Device;
using namespace GPGPU::DirectCompute;

CSOutputBuffer::CSOutputBuffer() : BaseBuffer(), _uav(nullptr)
{

}

CSOutputBuffer::~CSOutputBuffer()
{

}