#include "StdAfx.h"

#include "SunSkyLight.h"
#include "float.h"
#include "LGdef.h"
#include "ShadingState.h"
#include "MathUtil.h"
#include "ChromaticitySpectrum.h"
#include "ConstantSpectralCurve.h"
#include "parameterlist.h"
#include "xyzcolor.h"
#include "boundingbox.h"
#include "instance.h"

float SunSkyLight::solAmplitudes[]={ 165.5f, 162.3f, 211.2f,
258.8f, 258.2f, 242.3f, 267.6f, 296.6f, 305.4f, 300.6f, 306.6f,
288.3f, 287.1f, 278.2f, 271.0f, 272.3f, 263.6f, 255.0f, 250.6f,
253.1f, 253.5f, 251.3f, 246.3f, 241.7f, 236.8f, 232.1f, 228.2f,
223.4f, 219.7f, 215.3f, 211.0f, 207.3f, 202.4f, 198.7f, 194.3f,
190.7f, 186.3f, 182.6f ,FLT_MAX};

RegularSpectralCurve SunSkyLight::solCurve(solAmplitudes,380,750);

float SunSkyLight::k_oWavelengths[]={ 300, 305, 310, 315, 320,
325, 330, 335, 340, 345, 350, 355, 445, 450, 455, 460, 465, 470,
475, 480, 485, 490, 495, 500, 505, 510, 515, 520, 525, 530, 535,
540, 545, 550, 555, 560, 565, 570, 575, 580, 585, 590, 595, 600,
605, 610, 620, 630, 640, 650, 660, 670, 680, 690, 700, 710, 720,
730, 740, 750, 760, 770, 780, 790, FLT_MAX};

float SunSkyLight::k_oAmplitudes[]={ 10.0f, 4.8f, 2.7f, 1.35f,
.8f, .380f, .160f, .075f, .04f, .019f, .007f, .0f, .003f, .003f,
.004f, .006f, .008f, .009f, .012f, .014f, .017f, .021f, .025f,
.03f, .035f, .04f, .045f, .048f, .057f, .063f, .07f, .075f, .08f,
.085f, .095f, .103f, .110f, .12f, .122f, .12f, .118f, .115f, .12f,
.125f, .130f, .12f, .105f, .09f, .079f, .067f, .057f, .048f, .036f,
.028f, .023f, .018f, .014f, .011f, .010f, .009f, .007f, .004f, .0f,
.0f ,FLT_MAX};

float SunSkyLight::k_gWavelengths[]={ 759, 760, 770, 771,FLT_MAX };
float SunSkyLight::k_gAmplitudes []={ 0, 3.0f, 0.210f, 0 ,FLT_MAX};
float SunSkyLight::k_waWavelengths[]={ 689, 690, 700, 710, 720,
730, 740, 750, 760, 770, 780, 790, 800,FLT_MAX };
float SunSkyLight::k_waAmplitudes[]={ 0, 0.160e-1, 0.240e-1,
0.125e-1, 0.100e+1, 0.870, 0.610e-1, 0.100e-2, 0.100e-4,
0.100e-4, 0.600e-3, 0.175e-1, 0.360e-1,FLT_MAX};

IrregularSpectralCurve SunSkyLight::k_oCurve(k_oWavelengths, k_oAmplitudes);
IrregularSpectralCurve SunSkyLight::k_gCurve(k_gWavelengths,k_gAmplitudes);
IrregularSpectralCurve SunSkyLight::k_waCurve(k_waWavelengths,k_waAmplitudes);

LG_IMPLEMENT_DYNCREATE(SunSkyLight,LightSource)

SunSkyLight::SunSkyLight()
{
	numSkySamples=64;
	sunDirWorld.set(1,1,1);
	turbidity=6.0f;
	basis=OrthoNormalBasis::makeFromWV(Vector3(0,0,1),Vector3(0,1,0));
	groundExtendSky=FALSE;
	groundColor=Color::BLACK;
	initSunSky();
}

SpectralCurve* SunSkyLight::computeAttenuatedSunlight(float theta,float turbidity) 
{
	float data[91];
	
	double alpha=1.3;
	double lozone=0.35;
	double w=2.0;
	double beta=0.04608365822050 *turbidity-0.04586025928522;	
	double m=1.0/(cos(theta)+0.000940*pow(1.6386-theta,-1.253));
	for(int i=0, lambda=350; lambda<=800; i++,lambda+=5) 
	{		
		double tauR=exp(-m*0.008735*pow(lambda/1000.0,-4.08));		
		double tauA=exp(-m*beta*pow(lambda/1000.0,-alpha));		
		double tauO=exp(-m*k_oCurve.sample(lambda)*lozone);		
		double tauG=exp(-1.41*k_gCurve.sample(lambda)*m/pow(1.0 
			+118.93*k_gCurve.sample(lambda)*m,0.45));
	
		double tauWA=exp(-0.2385*k_waCurve.sample(lambda)*w 
			*m/pow(1.0+20.07*k_waCurve.sample(lambda)*w*m,0.45));
		
		double amp=solCurve.sample(lambda)*tauR*tauA*tauO*tauG*tauWA;
		data[i]=(float)amp;
	}	
	SpectralCurve*p=new RegularSpectralCurve(data,350,800);
	LGMemoryAllocator::lg_mem_pointer(p);

	return p;
}

double SunSkyLight::perezFunction(const vector<double>&lam, 
								  double theta,double gamma,double lvz)const 
{
	double den=((1.0+lam[0]*exp(lam[1]))*(1.0+lam[2]*exp(lam[3]*sunTheta)
		+lam[4]*cos(sunTheta)*cos(sunTheta)));
	double num=((1.0+lam[0]*exp(lam[1]/cos(theta))) 
		*(1.0+lam[2]*exp(lam[3]*gamma)+lam[4]
	    *cos(gamma)*cos(gamma)));

	return lvz*num/den;
}

void SunSkyLight::initSunSky() 
{	
	sunDirWorld.normalize();
	sunDir=basis.untransform(sunDirWorld,Vector3());
	sunDir.normalize();
	sunTheta=(float)acos(MathUtil::clamp(sunDir.z,(float)-1.0,(float)1.0));
	if(sunDir.z>0.0f) 
	{
		sunSpectralRadiance=computeAttenuatedSunlight(sunTheta,turbidity);		
		RGBSpace::SRGB.convertXYZtoRGB(sunSpectralRadiance->toXYZ().mul(1e-4f),sunColor);
		sunColor.constrainRGB();
	} 
	else 
	{
		void*lgp=LGMemoryAllocator::lg_malloc(sizeof(ConstantSpectralCurve));
		sunSpectralRadiance=new (lgp) ConstantSpectralCurve(0);		
	}	
	float theta2=sunTheta*sunTheta;
	float theta3=sunTheta*theta2;
	float T=turbidity;
	float T2=turbidity*turbidity;
	double chi=(4.0/9.0-T/120.0)*(LG_PI-2.0*sunTheta);
	zenithY=(4.0453*T-4.9710)*tan(chi)-0.2155*T+2.4192;
	zenithY*=1000; 
	zenithx=(0.00165*theta3-0.00374*theta2 
		+0.00208 * sunTheta+0)*T2 
		+(-0.02902*theta3+0.06377*theta2
		-0.03202*sunTheta+0.00394)*T 
		+(0.11693*theta3-0.21196*theta2+0.06052*sunTheta+0.25885);
	zenithy=(0.00275*theta3-0.00610*theta2
		+0.00316*sunTheta+0)*T2 
		+(-0.04212*theta3+0.08970*theta2-0.04153*sunTheta+0.00515)* T 
		+(0.15346*theta3-0.26756*theta2+0.06669*sunTheta+0.26688);

	perezY.resize(5); 
	perezy.resize(5);
	perezx.resize(5);
	perezY[0]=0.17872*T-1.46303;
	perezY[1]=-0.35540*T+0.42749;
	perezY[2]=-0.02266*T+5.32505;
	perezY[3]=0.12064*T-2.57705;
	perezY[4]=-0.06696*T+0.37027;

	perezx[0]=-0.01925*T-0.25922;
	perezx[1]=-0.06651*T+0.00081;
	perezx[2]=-0.00041*T+0.21247;
	perezx[3]=-0.06409*T-0.89887;
	perezx[4]=-0.00325*T+0.04517;

	perezy[0]=-0.01669*T-0.26078;
	perezy[1]=-0.09495*T+0.00921;
	perezy[2]=-0.00792*T+0.21023;
	perezy[3]=-0.04405*T-1.65369;
	perezy[4]=-0.01092*T+0.05291;

	int w=32,h=32;
	imageHistogram.resize(w);
	for(int i=0; i<w; i++)
		imageHistogram[i].resize(h);
	colHistogram.resize(w);
	float du=1.0f/w;
	float dv=1.0f/h;
	for(int x=0; x<w; x++)
	{
		for(int y=0; y<h; y++)
		{
			float u=(x+0.5f)*du;
			float v=(y+0.5f)*dv;
			Color c=getSkyRGB(getDirection(u, v));
			imageHistogram[x][y]=c.getLuminance()*(float)sin(LG_PI*v);
			if(y>0)
				imageHistogram[x][y]+=imageHistogram[x][y-1];
		}
		colHistogram[x]=imageHistogram[x][h-1];
		if(x>0)
			colHistogram[x]+=colHistogram[x-1];
		for(int y=0; y<h; y++)
			imageHistogram[x][y]/=imageHistogram[x][h-1];
	}
	for(int x=0; x<w; x++)
		colHistogram[x]/=colHistogram[w-1];
	jacobian=(2.0f*LG_PI*LG_PI)/(w*h);
}

BOOL SunSkyLight::update(ParameterList&pl,LGAPI&api) 
{
	PrimitiveList::update(pl,api);
	LightSource::update(pl,api);
	Shader::update(pl,api);
	Vector3 up=pl.getVector("up",Vector3());
	Vector3 east=pl.getVector("east",Vector3());
	basis=OrthoNormalBasis::makeFromWV(up,east);	
	numSkySamples=pl.getInt("samples",numSkySamples);
	sunDirWorld=pl.getVector("sundir",sunDirWorld);
	turbidity=pl.getFloat("turbidity",turbidity);
	groundExtendSky=pl.getBoolean("ground.extendsky",groundExtendSky);
	groundColor=pl.getColor("ground.color",groundColor);	
	initSunSky();

	return TRUE;
}

Color SunSkyLight::getSkyRGB(Vector3&dir)const
{
	if( dir.z<0.0f && !groundExtendSky )
		return groundColor;
	if( dir.z<0.001f ) dir.z=0.001f;
	dir.normalize();
	double theta=acos(MathUtil::clamp(dir.z,-1.0f,1.0f));
	double gamma=acos(MathUtil::clamp(Vector3::dot(dir, sunDir),-1.0f,1.0f));
	double x=perezFunction(perezx,theta,gamma,zenithx);
	double y=perezFunction(perezy,theta,gamma,zenithy);
	double Y=perezFunction(perezY,theta,gamma,zenithY)*1e-4;
	XYZColor c=ChromaticitySpectrum::get((float)x,(float)y);	
	float X=c.getX()*Y/c.getY();
	float Z=c.getZ()*Y/c.getY();
	Color cc;
	RGBSpace::SRGB.convertXYZtoRGB(X,Y,Z,cc);

	return cc;
}

int SunSkyLight::getNumSamples() const
{
	return 1+numSkySamples;
}

void SunSkyLight::getPhoton(double randX1,double randY1,double randX2,double randY2,
			   Point3&p,Vector3&dir,Color&power)const
{	
}

float SunSkyLight::getPower()const
{
	return 0.0f;
}

void SunSkyLight::getSamples(ShadingState&state)const
{
	if( Vector3::dot(sunDirWorld,state.getGeoNormal())>0.0f 
		&& Vector3::dot(sunDirWorld,state.getNormal())>0.0f )
	{
		LightSample *dest=new LightSample();
		dest->setShadowRay(Ray(state.getPoint(),sunDirWorld));
		dest->getShadowRay().setMax(FLT_MAX);
		dest->setRadiance(sunColor,sunColor);
		dest->traceShadow(state);
		state.addSample(dest);
	}
	int n=state.getDiffuseDepth()>0?1:numSkySamples;
	for(int i=0; i<n; i++)
	{		
		double randX=state.getRandom(i,0,n);
		double randY=state.getRandom(i,1,n);

		int x=0;
		while( randX>=colHistogram[x] && x<colHistogram.size()-1 )
			x++;
		vector<float>rowHistogram=imageHistogram[x];
		int y=0;
		while(randY>=rowHistogram[y] && y<rowHistogram.size()-1 )
			y++;		
		float u=(float)( (x==0)?(randX/colHistogram[0])
			:((randX-colHistogram[x-1])/(colHistogram[x]-colHistogram[x-1])) );
		float v=(float)( (y==0)?(randY/rowHistogram[0])
			:((randY-rowHistogram[y-1])/(rowHistogram[y]-rowHistogram[y-1])) );

		float px=((x==0)?colHistogram[0]:(colHistogram[x]-colHistogram[x-1]));
		float py=((y==0)?rowHistogram[0]:(rowHistogram[y]-rowHistogram[y-1]));

		float su=(x+u)/colHistogram.size();
		float sv=(y+v)/rowHistogram.size();
		float invP=(float)sin(sv*LG_PI)*jacobian/(n*px*py);
		Vector3 localDir=getDirection(su,sv);
		Vector3 dir=basis.transform(localDir,Vector3());
		if( Vector3::dot(dir,state.getGeoNormal())>0.0f
			&& Vector3::dot(dir,state.getNormal())>0.0f ) 
		{
			LightSample* dest=new LightSample();
			dest->setShadowRay(Ray(state.getPoint(),dir));
			dest->getShadowRay().setMax(FLT_MAX);
			Color radiance=getSkyRGB(localDir);
			dest->setRadiance(radiance,radiance);
			dest->getDiffuseRadiance().mul(invP);
			dest->getSpecularRadiance().mul(invP);
			dest->traceShadow(state);
			state.addSample(dest);
		}
	}
}

PrimitiveList* SunSkyLight::getBakingPrimitives()const
{
	return NULL;
}

int SunSkyLight::getNumPrimitives() const
{
	return 1;
}

float SunSkyLight::getPrimitiveBound(int primID,int i) const
{
	return 0.0f;
}

BoundingBox SunSkyLight::getWorldBounds(const Matrix4&o2w)const
{
	return BoundingBox(0.0f);
}

void SunSkyLight::intersectPrimitive(Ray&r,int primID,IntersectionState&state)const
{
	if(r.getMax()==FLT_MAX)
		state.setIntersection(0);
}

void SunSkyLight::prepareShadingState(ShadingState&state) 
{
	if(state.includeLights())
		state.setShader(this);
}

Color SunSkyLight::getRadiance(ShadingState&state)const 
{
	return getSkyRGB(basis.untransform(state.getRay().getDirection())).constrainRGB();
}

void SunSkyLight::scatterPhoton(ShadingState&state,Color&power) const
{	
}

Vector3 SunSkyLight::getDirection(float u,float v)const
{
	Vector3 dest;
	double phi=0.0,theta=0.0;
	theta=u*2.0*LG_PI;
	phi=v*LG_PI;
	double sin_phi=sin(phi);
	dest.x=(float)(-sin_phi*cos(theta));
	dest.y=(float)cos(phi);
	dest.z=(float)(sin_phi*sin(theta));

	return dest;
}

Instance* SunSkyLight::createInstance()
{
	return Instance::createTemporary(*this,Matrix4(),this);	
}