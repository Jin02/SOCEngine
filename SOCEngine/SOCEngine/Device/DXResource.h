#pragma once

#include <memory>
#include "Common.h"
#include <functional>

class DXReleaseResource
{
protected:
	static void Release(IUnknown* resource)
	{
		SAFE_RELEASE(resource);
	}
};

template<typename Resource>
class DXUniqueResource final : private DXReleaseResource
{
public:
	DXUniqueResource()								: _resource(nullptr, nullptr)	{ }
	explicit DXUniqueResource(Resource* resource)	: _resource(resource, Release)	{ }

	inline const Resource* GetRaw() const		{ return _resource.get();		}
	inline Resource* GetRaw()					{ return _resource.get();		}
	inline Resource* operator->()				{ return _resource.get();		}
	inline const Resource* operator->() const	{ return _resource.get();		}

	inline bool IsCanUse() const				{ return _resource != nullptr;	}

	inline operator Resource*()					{ return _resource.get();		}
	inline operator const Resource* const()		{ return _resource.get();		}
	inline void Destroy()						{ _resource.reset();			}

private:
	std::unique_ptr<Resource, std::function<void(IUnknown*)>> _resource;
};

template <typename Resource>
class DXSharedResource final : private DXReleaseResource
{
public:
	DXSharedResource()								: _resource(nullptr)			{ }
	explicit DXSharedResource(Resource* resource)	: _resource(resource, Release)	{ }

	inline const Resource* GetRaw() const				{ return _resource.get();		}
	inline Resource* const GetRaw()						{ return _resource.get();		}
	inline Resource* const operator->()					{ return _resource.get(); }
	inline const Resource* operator->() const			{ return _resource.get(); }

	inline const std::shared_ptr<Resource>& Get() const	{ return _resource;				}
	inline std::shared_ptr<Resource>& Get()				{ return _resource;				}

	inline bool IsCanUse() const						{ return _resource != nullptr;	}
	void Destroy()										{ _resource.reset();			}

private:
	std::shared_ptr<Resource> _resource;
};
