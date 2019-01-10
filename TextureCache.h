#pragma once

#include <hash_map>
#include "CharLess.h"
#include "Texture.h"

class TextureCache
{
	TextureCache(void);

public:
	static Texture getTexture(CString fileName,BOOL isLinear);
	static void flush();

private:
	static hash_map<CString,Texture,
		hash_compare<const char*,CharLess>> textures;
	typedef pair<CString,Texture> CT_PAIR;	
};
