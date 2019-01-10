#pragma once

#include "LightSample.h"
#include "IntersectionState.h"
#include "point2.h"
#include "point3.h"
#include "OrthoNormalBasis.h"
#include "Matrix4.h"

class ShadingState
{
public:
	static ShadingState* createPhotonState(const Ray&rr,IntersectionState*is,int ii,
		PhotonStore*mm,LightServer*ls);
	static ShadingState* createState(IntersectionState*is,float rrx,float rry, 
		float ttime,const Ray&rr,int ii,int dd,LightServer*ls);
	static ShadingState* createDiffuseBounceState(const ShadingState&pre,const Ray&rr,int ii);
	static ShadingState* createGlossyBounceState(const ShadingState&pre,const Ray&rr,int ii);
    static ShadingState* createReflectionBounceState(const ShadingState&pre,const Ray&rr,int ii);
    static ShadingState* createRefractionBounceState(const ShadingState&pre,const Ray&rr,int ii);
	static ShadingState* createFinalGatherState(const ShadingState&ss,const Ray&rr,int ri);

	ShadingState();
	~ShadingState();
	ShadingState(const ShadingState&pre,IntersectionState*is,
		const Ray&rr,int ii,int dd);

	double getRandom(int j,int dim)const;
	double getRandom(int j,int dim,int nn)const;	
	float getRasterX()const;
	float getRasterY()const;
	float getCosND()const;
	BOOL isBehind()const;
	IntersectionState* getIntersectionState()const;
	float getU()const;
	float getV()const;
	float getW()const;
	Instance* getInstance()const;
	int getPrimitiveID()const;	
	BOOL includeLights()const;
	BOOL includeSpecular()const;
	Shader* getShader()const;	
	Modifier* getModifier()const;	
	int getDepth()const;
	int getDiffuseDepth()const;
	int getReflectionDepth()const;
	int getRefractionDepth()const;
	Point3 getPoint() const;
	Vector3 getNormal()const;
	Point2 getUV()const;
	Vector3 getGeoNormal()const;
	OrthoNormalBasis getBasis()const;	
	Ray getRay()const;
	Matrix4 getCameraToWorld()const;
	Matrix4 getWorldToCamera()const;
	Color getResult()const;
	LightServer* getLightServer()const;
	BOOL getTrianglePoints(vector<Point3>&pp)const;
	Point3 transformObjectToWorld(const Point3&pp)const;
	Point3 transformWorldToObject(const Point3&pp)const;
	Vector3 transformNormalObjectToWorld(const Vector3&nn)const;
	Vector3 transformNormalWorldToObject(const Vector3&nn)const;
	Vector3 transformVectorObjectToWorld(const Vector3&v)const;
	Vector3 transformVectorWorldToObject(const Vector3&v)const;
	BOOL operator!=(const ShadingState&ss)const;
	BOOL operator==(const ShadingState&ss)const;
	BOOL isNull()const;

	Color getGlobalRadiance();
	Color getIrradiance(const Color&diffuseReflectance);
	Color diffuse(const Color&diff);
	void setRay(const Ray&rr);
	void init();
	Color shade();
	void correctShadingNormal();
	void faceforward();
	void setResult(const Color&c);	
	void addSample(LightSample* sample);
	void setShader(Shader*ss);
	void setBasis(const OrthoNormalBasis&onb);
	void setModifier(Modifier*mm);
	void storePhoton(const Vector3&dir,const Color&power,const Color&diff);
	void initLightSamples();
	void initCausticSamples();
	Color traceGlossy(Ray&rr,int ii);
	Color traceReflection(Ray&rr,int ii);
	Color traceRefraction(Ray&rr,int ii);
	Color traceTransparency();
	Color traceShadow(Ray&rr);	
	void traceReflectionPhoton(Ray&rr,Color&power);
	void traceRefractionPhoton(Ray&rr,Color&power);
	void traceDiffusePhoton(Ray&rr,Color&power);	
	ShadingState* traceFinalGather(Ray&rr,int ii);
	Color occlusion(int samples,float maxDist);
	Color occlusion(int samples,float maxDist,const Color&bright,const Color&dark);	
	Color specularPhong(const Color&spec,float power,int numRays);
    Point2& ss_tex();
	LightSample* ss_lightSampleHead();
	Point3& ss_point3();	
	Vector3& ss_normal();
	Vector3& ss_geoNormal();
	OrthoNormalBasis& ss_basis();
	IntersectionState& ss_is();
	Ray& ss_ray();

private:	
	LightServer* server;
	LightSample* lightSample;
	PhotonStore* ps;
	Shader* shader;
	Modifier* modifier;
	Instance* instance;
	IntersectionState* iState;
	float rx,ry,time;
	Color result;
	Point3 p;
	Vector3 n;
	Point2 tex;
	Vector3 ng;
	OrthoNormalBasis basis;
	float cosND;
	float bias;
	BOOL behind;
	float hitU,hitV,hitW;
	int primitiveID;
	Matrix4 o2w,w2o;
	Ray r;
	int d;
	int i; 
	double qmcD0I;
	double qmcD1I;
	int diffuseDepth;
	int reflectionDepth;
	int refractionDepth;
	BOOL incluLights;
	BOOL incluSpecular;		
};
