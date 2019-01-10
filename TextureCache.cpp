#include "StdAfx.h"

#include "TextureCache.h"
#include "strutil.h"

hash_map<CString,Texture,hash_compare<const char*,CharLess>> TextureCache::textures;

TextureCache::TextureCache(void)
{
}

Texture TextureCache::getTexture(CString fileName,BOOL isLinear)
{
	CCriticalSection cs;
	cs.Lock();
	hash_map<CString,Texture,hash_compare<const char*,CharLess>>::iterator iter;
	iter=textures.find(fileName);
	if(iter!=textures.end()) 
	{
		StrUtil::PrintPrompt("为文件\"%s\"使用缓存拷贝...",fileName);
		cs.Unlock();
		return textures[fileName];
	}
	StrUtil::PrintPrompt("使用文件\"%s\" ...",fileName);
	Texture t(fileName,isLinear);
	textures.insert(CT_PAIR(fileName,t));
	cs.Unlock();

	return t;
}

void TextureCache::flush()
{
	CCriticalSection cs;
	cs.Lock();
	StrUtil::PrintPrompt("清除纹理缓存");
	textures.clear();
	cs.Unlock();
}