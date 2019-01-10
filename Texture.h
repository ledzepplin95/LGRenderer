#pragma once

class Bitmap;
class Texture
{
public:	
    ~Texture();
	Texture();
	Texture(CString fName,BOOL isL);

	void load();	

	Bitmap* getBitmap()const;
	Color getPixel(float x,float y)const;
	Vector3 getNormal(float x,float y,const OrthoNormalBasis&basis)const;
	Vector3 getBump(float x,float y,const OrthoNormalBasis&basis,float scale)const;
	
private:
	CString fileName;
	BOOL isLinear;
    class Bitmap* bitmap;
	int loaded;		
};
