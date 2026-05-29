#include "type_registry.h"

namespace hsscomm
{
namespace type_registry
{
	namespace
	{
		factory_list_ele* g_head = 0;
	}

	factory_list_ele::factory_list_ele(abstract_factory* fact)
		: next(g_head), manufacturer(fact)
	{
		g_head = this;
	}

	void* create(const char* creation_tag)
	{
		for (factory_list_ele* cur = g_head; cur; cur = cur->next)
		{
			void* created = cur->manufacturer->create_obj(creation_tag);
			if (created)
			{
				return created;
			}
		}

		return 0;
	}

	factory_list_ele::~factory_list_ele()
	{
		for (factory_list_ele** nextp = &g_head; *nextp; nextp = &(*nextp)->next)
		{
			if (*nextp == this)
			{
				*nextp = (*nextp)->next;
				break;
			}
		}
	}
} // namespace type_registry
} // namespace hsscomm
