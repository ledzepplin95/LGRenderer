#include "StdAfx.h"

#include "ThinLens.h"
#include "mathutil.h"
#include "ParameterList.h"
#include "lgapi.h"
#include "ray.h"

LG_IMPLEMENT_DYNCREATE(ThinLens,CameraLens)

ThinLens::ThinLens() 
{
	focusDistance=1.0f;
	lensRadius=0.0f;
	fov=90.0f;
	aspect=1.0f;
	lensSides=0; 
	lensRotation=lensRotationRadians=0.0f; 
}

BOOL ThinLens::update(ParameterList&pl,LGAPI&api)
{	
	fov=pl.getFloat("fov",fov);
	aspect=pl.getFloat("aspect",aspect);
	shiftX=pl.getFloat("shift.x",shiftX);
	shiftY=pl.getFloat("shift.y",shiftY);
	focusDistance=pl.getFloat("focus.distance",focusDistance);
	lensRadius=pl.getFloat("lens.radius",lensRadius);
	lensSides=pl.getInt("lens.sides",lensSides);
	lensRotation=pl.getFloat("lens.rotation",lensRotation);
	update();

	return TRUE;
}

void ThinLens::update() 
{
	au=(float)tan(DegToRad(fov*0.5f))*focusDistance;
	av=au/aspect;
	lensRotationRadians=(float)DegToRad(lensRotation);
}

Ray ThinLens::getRay(float x,float y,int imageWidth,int imageHeight,
				  double lensX,double lensY,double time) const
{
	float du=shiftX*focusDistance-au+((2.0f*au*x)/(imageWidth-1.0f));
	float dv=shiftY*focusDistance-av+((2.0f*av*y)/(imageHeight-1.0f));

	float eyeX,eyeY;
	if(lensSides<3) 
	{
		double angle,r;		
		double r1=2.0*lensX-1.0;
		double r2=2.0*lensY-1.0;
		if(r1>-r2)
		{
			if(r1>r2) 
			{
				r=r1;
				angle=0.25*LG_PI*r2/r1;
			} 
			else
			{
				r=r2;
				angle=0.25*LG_PI*(2.0-r1/r2);
			}
		} 
		else
		{
			if(r1<r2)
			{
				r=-r1;
				angle=0.25*LG_PI*(4+r2/r1);
			} 
			else
			{
				r=-r2;
				if(r2!=0.0)
					angle=0.25*LG_PI*(6-r1/r2);
				else
					angle=0.0;
			}
		}
		r*=lensRadius;		
		eyeX=(float)(cos(angle)*r);
		eyeY=(float)(sin(angle)*r);
	} 
	else 
	{	
		lensY*=lensSides;
		float side=(int)lensY;
		float offs=(float)lensY-side;
		float dist=(float)sqrt(lensX);
		float a0=side*LG_PI*2.0f/lensSides+lensRotationRadians;
		float a1=(float)((side+1.0f)*LG_PI*2.0f/lensSides+lensRotationRadians);
		eyeX=(float)((cos(a0)*(1.0f-offs)+cos(a1)*offs)*dist);
		eyeY=(float)((sin(a0)*(1.0f-offs)+sin(a1)*offs)*dist);
		eyeX*=lensRadius;
		eyeY*=lensRadius;
	}
	float eyeZ=0.0f;	
	float dirX=du;
	float dirY=dv;
	float dirZ=-focusDistance;

	return Ray(eyeX,eyeY,eyeZ,dirX-eyeX,dirY-eyeY,dirZ-eyeZ);
}
