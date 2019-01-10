#pragma once

#include <functional>

struct CharLess : public binary_function<const char*,const char*,bool>
{
public:
	result_type operator()(const first_argument_type& l, 
		const second_argument_type& r) const
	{
		return(stricmp(l,r)<0?true:false);
	}
};
