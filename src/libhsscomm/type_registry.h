#ifndef TYPE_REGISTRY_H__
#define TYPE_REGISTRY_H__

#include "dllexport.h"
#include <string.h>

namespace hsscomm
{
namespace type_registry
{
	DLLEXPORT void* create(const char* creation_tag);

	class DLLEXPORT abstract_factory
	{
	public:
		virtual ~abstract_factory() {}
		virtual void* create_obj(const char* creation_tag) = 0;
	};

	class DLLEXPORT factory_list_ele
	{
		friend DLLEXPORT void* create(const char* creation_tag);

		factory_list_ele* next;
		abstract_factory* manufacturer;

	public:
		explicit factory_list_ele(abstract_factory* fact);
		virtual ~factory_list_ele();
	};

	template <class t>
	class factory : public abstract_factory
	{
		factory_list_ele manufacturer;

	public:
		factory() : manufacturer(this) {}
		virtual ~factory() {}

		void* create_obj(const char* creation_tag) override
		{
			return t::CheckCreationTag(creation_tag) ? static_cast<void*>(new t) : 0;
		}
	};
} // namespace type_registry
} // namespace hsscomm

#endif // TYPE_REGISTRY_H__
