#pragma once
#include "bitmapwriter.h"

class EXRBitmapWriter :
	public BitmapWriter
{
	LG_DECLARE_DYNCREATE(EXRBitmapWriter);

public:
	EXRBitmapWriter();

	void configure(CString o,CString v);
	void openFile(CString fName);
	void writeHeader(int width,int height,int tSize);
	void writeTile(int x,int y,int w,int h,const vector<Color>&color,const vector<float>&alpha);
	void closeFile();

private:
	static byte HALF;
	static byte FLOAT;
	static int HALF_SIZE;
	static int FLOAT_SIZE;
	static int OE_MAGIC;
	static int OE_EXR_VERSION;
	static int OE_TILED_FLAG;
	static int NO_COMPRESSION;
	static const int RLE_COMPRESSION;
	static const int ZIP_COMPRESSION;
	static int RLE_MIN_RUN;
	static int RLE_MAX_RUN;

	CString fileName;
	CFile file;
	vector<vector<long>> tileOffsets;
	long tileOffsetsPosition;
	int tilesX;
	int tilesY;
	int tileSize;
	int compression;
	byte channelType;
	int channelSize;
	vector<byte> tmpbuf;
	vector<byte> comprbuf;

	void writeRGBAHeader(int w,int h,int tileSize);
	void writeTileOffsets();
	void writeEXRTile(int tileX,int tileY,int w,int h,
		const vector<Color>&tile,const vector<float>&alpha);
	static int ebw_compress(int tp,const vector<byte>&inside,int inSize,vector<byte>&out);
	static int rleCompress(const vector<byte>&inside,int inLen,vector<byte>&out);
};
