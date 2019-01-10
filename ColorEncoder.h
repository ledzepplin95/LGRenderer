#pragma once

class ColorEncoder
{
public:
    static vector<Color> unpremult(const vector<Color>&color,const vector<float>&alpha);
	static vector<Color> unlinearize(const vector<Color>&color);
	static vector<byte> quantizeRGB8(const vector<Color>&color);
	static vector<byte> quantizeRGBA8(const vector<Color>&color,const vector<float>&alpha);
	static vector<int> encodeRGBE(const vector<Color>&color);
};
