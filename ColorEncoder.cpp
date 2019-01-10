#include "StdAfx.h"

#include "ColorEncoder.h"
#include "MathUtil.h"
#include "Color.h"

vector<Color> ColorEncoder::unpremult(const vector<Color>&color,const vector<float>&alpha) 
{
	vector<Color> output;
	output.resize(color.size());
	for(int i=0; i<color.size(); i++)
		output[i]=color[i].copy().mul(1.0f/alpha[i]);
	 
	return output;
}

vector<Color> ColorEncoder::unlinearize(const vector<Color>&color) 
{
    vector<Color> output;
	output.resize(color.size());
	for(int i=0; i<color.size(); i++)
		output[i]=color[i].copy().toNonLinear();

	return output;
}

vector<byte> ColorEncoder::quantizeRGB8(const vector<Color>&color)
{
	vector<byte> output;
	output.resize(color.size()*3);
	for(int i=0, index=0; i<color.size(); i++, index+=3) 
	{
		vector<float> rgb=color[i].getRGB();
		output[index+0]=(byte)MathUtil::clamp((int)(rgb[0]*255+0.5f),0,255);
		output[index+1]=(byte)MathUtil::clamp((int)(rgb[1]*255+0.5f),0,255);
		output[index+2]=(byte)MathUtil::clamp((int)(rgb[2]*255+0.5f),0,255);
	}

	return output;
}

vector<byte> ColorEncoder::quantizeRGBA8(const vector<Color>&color,const vector<float>&alpha) 
{
	vector<byte> output;
	output.resize(color.size()*4);
	for(int i=0, index=0; i<color.size(); i++, index+=4)
	{
		vector<float> rgb=color[i].getRGB();
		output[index+0]=(byte)MathUtil::clamp((int)(rgb[0]*255+0.5f),0,255);
		output[index+1]=(byte)MathUtil::clamp((int)(rgb[1]*255+0.5f),0,255);
		output[index+2]=(byte)MathUtil::clamp((int)(rgb[2]*255+0.5f),0,255);
		output[index+3]=(byte)MathUtil::clamp((int)(alpha[i]*255+0.5f),0,255);
	}

	return output;
}

vector<int> ColorEncoder::encodeRGBE(const vector<Color>&color)
{
	vector<int> output;
	output.resize(color.size());
	for(int i=0; i<color.size(); i++)
		output[i]=color[i].toRGBE();

	return output;
}