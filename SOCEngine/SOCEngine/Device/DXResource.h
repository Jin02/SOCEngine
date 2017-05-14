#pragma once

#include <memory>
#include "Common.h"

template <typename Resource>
class DXResource final
{
public:
	static void Release(IUnknown* resource)	{	resource->Release();	};

	DXResource() : _resource(nullptr) { }
	explicit DXResource(Resource* resource) : _resource(resource, Release) { }

	inline const Resource* GetRaw() const				{ return _resource.get();		}
	inline Resource* GetRaw() 							{ return _resource.get();		}
	inline const std::shared_ptr<Resource>& Get() const	{ return _resource;				}
	inline std::shared_ptr<Resource>& Get()				{ return _resource;				}
	Resource* operator->()								{ return _resource.get();		}
	const Resource* operator->() const					{ return _resource.get();		}
	inline bool IsCanUse() const						{ return _resource != nullptr;	}
	void Destroy()										{ _resource.reset();			}

private:
	std::shared_ptr<Resource> _resource;
};
