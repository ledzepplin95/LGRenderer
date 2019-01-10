#include "StdAfx.h"

#include "StrUtil.h"
#include "EXRBitmapWriter.h"
#include "ByteUtil.h"
#include "MathUtil.h"
#include "zlib.h"
#include "Color.h"

byte EXRBitmapWriter::HALF=1;
byte EXRBitmapWriter::FLOAT=2;
int EXRBitmapWriter::HALF_SIZE=2;
int EXRBitmapWriter::FLOAT_SIZE=4;
int EXRBitmapWriter::OE_MAGIC=20000630;
int EXRBitmapWriter::OE_EXR_VERSION=2;
int EXRBitmapWriter::OE_TILED_FLAG=0x00000200;
int EXRBitmapWriter::NO_COMPRESSION=0;
const int EXRBitmapWriter::RLE_COMPRESSION=1;
const int EXRBitmapWriter::ZIP_COMPRESSION=3;
int EXRBitmapWriter::RLE_MIN_RUN=3;
int EXRBitmapWriter::RLE_MAX_RUN=217;

LG_IMPLEMENT_DYNCREATE(EXRBitmapWriter,BitmapWriter)

EXRBitmapWriter::EXRBitmapWriter() 
{
	configure("compression","zip");
	configure("channeltype","half");
}

void EXRBitmapWriter::configure(CString o,CString v)
{
	if(o=="compression") 
	{
		if(v=="none")
			compression=NO_COMPRESSION;
		else if(v=="rle")
			compression=RLE_COMPRESSION;
		else if(v=="zip")
			compression=ZIP_COMPRESSION;
		else
		{
			StrUtil::PrintPrompt("EXR - 不认识的压缩类型，缺省为zip");
			compression = ZIP_COMPRESSION;
		}
	} 
	else if(o=="channeltype")
	{
		if(v=="float") 
		{
			channelType=FLOAT;
			channelSize=FLOAT_SIZE;
		} 
		else if(v=="half") 
		{
			channelType=HALF;
			channelSize=HALF_SIZE;
		} 
		else 
		{
			StrUtil::PrintPrompt("EXR - 不认识的通道类型，缺省为float");
			channelType=FLOAT;
			channelSize=FLOAT_SIZE;
		}
	}
}

void EXRBitmapWriter::openFile(CString fName)
{
	fileName=fName.IsEmpty()?"output.exr":fName;
}

void EXRBitmapWriter::writeHeader(int width,int height,int tileSize) 
{
	if( !file.Open(fileName,CFile::modeReadWrite | CFile::typeBinary) )
		return;
	if( tileSize<=0 )
	{
		StrUtil::PrintPrompt("没有bucket，不能使用OpenEXR位图写程序");
		return;
	}
	writeRGBAHeader(width,height,tileSize);
}

void EXRBitmapWriter::writeTile(int x,int y,int w,int h,
								const vector<Color>&color,const vector<float>&alpha)
{
	int tx=x/tileSize;
	int ty=y/tileSize;
	writeEXRTile(tx,ty,w,h,color,alpha);
}

void EXRBitmapWriter::closeFile() 
{
	writeTileOffsets();
	file.Close();
}

void EXRBitmapWriter::writeRGBAHeader(int w,int h,int _tileSize)
{
	byte chanOut[]={0,channelType, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
		0, 0, 0};

	vector<byte> dataByte;
	dataByte=ByteUtil::get4Bytes(OE_MAGIC);
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());
	dataByte=ByteUtil::get4Bytes(OE_EXR_VERSION | OE_TILED_FLAG);
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());

	CString str;
    str="channels";
	file.Write(str,str.GetLength());
    int dataInt;
	dataInt=0;
	file.Write(&dataInt,sizeof(dataInt));
	str="chlist";
	file.Write(str,str.GetLength());
    dataInt=0;
	file.Write(&dataInt,sizeof(dataInt));
	dataByte=ByteUtil::get4Bytes(73);
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());
	str="R";
	file.Write(str,str.GetLength());
	file.Write(chanOut,sizeof(byte)*17);
	str="G";
	file.Write(str,str.GetLength());
	file.Write(chanOut,sizeof(byte)*17);
	str="B";
	file.Write(str,str.GetLength());
	file.Write(chanOut,sizeof(byte)*17);
	str="A";
	file.Write(str,str.GetLength());
	file.Write(chanOut,sizeof(byte)*17);
	dataInt=0;
	file.Write(&dataInt,sizeof(dataInt));

	str="compression";
	file.Write(str,str.GetLength());
	dataInt=0;
	file.Write(&dataInt,sizeof(dataInt));
	str="compression";
	file.Write(str,str.GetLength());
	dataInt=0;
	file.Write(&dataInt,sizeof(dataInt));
	dataInt=1;
	file.Write(&dataInt,sizeof(dataInt));
	dataByte=ByteUtil::get4BytesInv(compression);
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());

	str="dataWindow";
	file.Write(str,str.GetLength());
	dataInt=0;
	file.Write(&dataInt,sizeof(dataInt));
	str="box2i";
	file.Write(str,str.GetLength());
	dataInt=0;
	file.Write(&dataInt,sizeof(dataInt));
	dataByte=ByteUtil::get4Bytes(0x10);
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());
	dataByte=ByteUtil::get4Bytes(0);
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());
	dataByte=ByteUtil::get4Bytes(0);
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());
	dataByte=ByteUtil::get4Bytes(w-1);
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());
	dataByte=ByteUtil::get4Bytes(h-1);
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());

	str="displayWindow";
	file.Write(str,str.GetLength());
	dataInt=0;
	file.Write(&dataInt,sizeof(dataInt));
	str="box2i";
	file.Write(str,str.GetLength());
	dataInt=0;
	file.Write(&dataInt,sizeof(dataInt));
	dataByte=ByteUtil::get4Bytes(0x10);
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());
	dataByte=ByteUtil::get4Bytes(0);
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());
	dataByte=ByteUtil::get4Bytes(0);
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());
	dataByte=ByteUtil::get4Bytes(w-1);
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());
	dataByte=ByteUtil::get4Bytes(h-1);
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());

	str="lineOrder";
	file.Write(str,str.GetLength());
	dataInt=0;
	file.Write(&dataInt,sizeof(dataInt));
	str="lineOrder";
	file.Write(str,str.GetLength());
	dataInt=0;
	file.Write(&dataInt,sizeof(dataInt));
	dataInt=1;
	file.Write(&dataInt,sizeof(dataInt));
	dataByte=ByteUtil::get4BytesInv(2);
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());

	str="pixelAspectRatio";
	file.Write(str,str.GetLength());
	dataInt=0;
	file.Write(&dataInt,sizeof(dataInt));
	str="float";
	file.Write(str,str.GetLength());
	dataInt=0;
	file.Write(&dataInt,sizeof(dataInt));
	dataByte=ByteUtil::get4Bytes(4);
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());
	dataByte=ByteUtil::get4Bytes(MathUtil::floatToRawIntBits(1.0));
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());

	str="screenWindowCenter";
	file.Write(str,str.GetLength());
	dataInt=0;
	file.Write(&dataInt,sizeof(dataInt));
	str="v2f";
	file.Write(str,str.GetLength());
	dataInt=0;
	file.Write(&dataInt,sizeof(dataInt));
	dataByte=ByteUtil::get4Bytes(8);
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());
	dataByte=ByteUtil::get4Bytes(MathUtil::floatToRawIntBits(0.0));
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());
	dataByte=ByteUtil::get4Bytes(MathUtil::floatToRawIntBits(0.0));
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());

	str="screenWindowWidth";
	file.Write(str,str.GetLength());
	dataInt=0;
	file.Write(&dataInt,sizeof(dataInt));
	str="float";
	file.Write(str,str.GetLength());
	dataInt=0;
	file.Write(&dataInt,sizeof(dataInt));
	dataByte=ByteUtil::get4Bytes(4);
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());
	dataByte=ByteUtil::get4Bytes(MathUtil::floatToRawIntBits(1.0));
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());

	tileSize=_tileSize;
	tilesX=((w+_tileSize-1)/_tileSize);
	tilesY=((h+_tileSize-1)/_tileSize);

	tmpbuf.resize(tileSize*tileSize*channelSize*4);
	comprbuf.resize(tileSize*tileSize*channelSize*4*2);

	tileOffsets.resize(tilesX);
	for(int i=0; i<tileOffsets.size(); i++)
		tileOffsets[i].resize(tilesY);

	str="tiles";
	file.Write(str,str.GetLength());
	dataInt=0;
	file.Write(&dataInt,sizeof(dataInt));
	str="tiledesc";
	file.Write(str,str.GetLength());
	dataInt=0;
	file.Write(&dataInt,sizeof(dataInt));
	dataByte=ByteUtil::get4Bytes(9);
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());
    dataByte=ByteUtil::get4Bytes(tileSize);
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());
	dataByte=ByteUtil::get4Bytes(tileSize);
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());

	dataInt=0;
	file.Write(&dataInt,sizeof(dataInt));
	dataInt=0;
	file.Write(&dataInt,sizeof(dataInt));

	tileOffsetsPosition=file.GetPosition();
	writeTileOffsets();
}

void EXRBitmapWriter::writeTileOffsets()
{
	file.Seek(tileOffsetsPosition,CFile::current);
	for(int ty=0; ty<tilesY; ty++)
		for(int tx=0; tx<tilesX; tx++)
		{
			vector<byte> bytes=ByteUtil::get8Bytes(tileOffsets[tx][ty]);
			file.Write(&bytes[0],sizeof(byte)*bytes.size());
		}
}

void EXRBitmapWriter::writeEXRTile(int tileX,int tileY,int w,int h,
								   const vector<Color>&tile,const vector<float>&alpha)
{
	CCriticalSection cs;
	cs.Lock();
	vector<byte> rgb;
	
	int pixptr=0,writeSize=0,comprSize=INT_MAX;
	int tileRangeX=(tileSize<w)?tileSize:w;
	int tileRangeY=(tileSize<h)?tileSize:h;
	int channelBase=tileRangeX*channelSize;

	if( (tileSize!=tileRangeX) && (tileX==0) )
		StrUtil::PrintPrompt("错误的X对齐");
	if( (tileSize!=tileRangeY) && (tileY==0) )
		StrUtil::PrintPrompt("错误的Y对齐");

	tileOffsets[tileX][tileY]=file.GetPosition();

	vector<byte> dataByte;
	dataByte=ByteUtil::get4Bytes(tileX);
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());
	dataByte=ByteUtil::get4Bytes(tileY);
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());
	dataByte=ByteUtil::get4Bytes(0);
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());
	dataByte=ByteUtil::get4Bytes(0);
	file.Write(&dataByte[0],sizeof(byte)*dataByte.size());

	for(int i=0; i<tmpbuf.size(); i++)
		tmpbuf[i]=0;
	for(int ty=0; ty<tileRangeY; ty++)
	{
		for(int tx=0; tx<tileRangeX; tx++)
		{
			vector<float> rgbf=tile[tx+ty*tileRangeX].getRGB();
			if(channelType==FLOAT) 
			{
				rgb=ByteUtil::get4Bytes(MathUtil::floatToRawIntBits(alpha[tx+ty*tileRangeX]));
				tmpbuf[pixptr+0]=rgb[0];
				tmpbuf[pixptr+1]=rgb[1];
				tmpbuf[pixptr+2]=rgb[2];
				tmpbuf[pixptr+3]=rgb[3];
			} 
			else if(channelType==HALF)
			{
				rgb=ByteUtil::get2Bytes(ByteUtil::floatToHalf(alpha[tx+ty*tileRangeX]));
				tmpbuf[pixptr+0]=rgb[0];
				tmpbuf[pixptr+1]=rgb[1];
			}
			for(int component=1; component<=3; component++) 
			{
				if(channelType==FLOAT)
				{
					rgb=ByteUtil::get4Bytes(MathUtil::floatToRawIntBits(rgbf[3-component]));
					tmpbuf[(channelBase*component)+pixptr+0]=rgb[0];
					tmpbuf[(channelBase*component)+pixptr+1]=rgb[1];
					tmpbuf[(channelBase*component)+pixptr+2]=rgb[2];
					tmpbuf[(channelBase*component)+pixptr+3]=rgb[3];
				}
				else if(channelType==HALF) 
				{
					rgb=ByteUtil::get2Bytes(ByteUtil::floatToHalf(rgbf[3-component]));
					tmpbuf[(channelBase*component)+pixptr+0]=rgb[0];
					tmpbuf[(channelBase*component)+pixptr+1]=rgb[1];
				}
			}
			pixptr+=channelSize;
		}
		pixptr+=(tileRangeX*channelSize*3);
	}

	writeSize=tileRangeX*tileRangeY*channelSize*4;

	if(compression!=NO_COMPRESSION)
		comprSize=ebw_compress(compression,tmpbuf,writeSize,comprbuf);

	if(comprSize<writeSize)
	{
		dataByte=ByteUtil::get4Bytes(comprSize);
		file.Write(&dataByte[0],sizeof(byte)*dataByte.size());
		file.Write(&comprbuf[0],sizeof(byte)*comprbuf.size());
	}
	else 
	{
		dataByte=ByteUtil::get4Bytes(writeSize);
		file.Write(&dataByte[0],sizeof(byte)*dataByte.size());
		file.Write(&comprbuf[0],sizeof(byte)*comprbuf.size());
	}
	cs.Unlock();
}

int EXRBitmapWriter::ebw_compress(int tp,const vector<byte>&inside,int inSize,vector<byte>&out) 
{
	if(inSize==0)
		return 0;

	int t1=0, t2=(inSize+1)/2;
	int inPtr=0,ret;
	vector<byte> tmp;
	tmp.resize(inSize);

	if( (tp==ZIP_COMPRESSION) || (tp==RLE_COMPRESSION) )
	{		
		while(TRUE)
		{
			if(inPtr<inSize)
				tmp[t1++]=inside[inPtr++];
			else
				break;

			if(inPtr<inSize)
				tmp[t2++]=inside[inPtr++];
			else
				break;
		}
		
		t1=1;
		int p=tmp[t1-1];
		while(t1<inSize) 
		{
			int d=tmp[t1]-p+(128+256);
			p=tmp[t1];
			tmp[t1]=(byte)d;
			t1++;
		}
	}	

	switch(tp) 
	{
	case ZIP_COMPRESSION:
		{		
			unsigned long tmpLen=sizeof(byte)*tmp.size(); 
			unsigned char *buf=new unsigned char[tmpLen+1];
			memset(buf,0,tmpLen+1);
			unsigned long bufLen=tmpLen;
			ret=compress(buf,&bufLen,&tmp[0],tmpLen);
            for(int i=0; i<bufLen; i++)
				out.push_back(buf[i]);
			delete [] buf;

			return ret;
		}
	case RLE_COMPRESSION:
		return rleCompress(tmp,inSize,out);
	default:
		return -1;
	}
}

int EXRBitmapWriter::rleCompress(const vector<byte>&inside,int inLen,vector<byte>&out)
{
	int runStart=0,runEnd=1,outWrite=0;
	while(runStart<inLen) 
	{
		while( runEnd<inLen
			&& inside[runStart]==inside[runEnd] 
		    && (runEnd-runStart-1)<RLE_MAX_RUN )
			runEnd++;
		if( runEnd-runStart>=RLE_MIN_RUN ) 
		{			
			out[outWrite++]=(byte)((runEnd-runStart)-1);
			out[outWrite++]=inside[runStart];
			runStart=runEnd;
		} 
		else 
		{			
			while( runEnd<inLen && ( ( (runEnd+1)>=inLen 
				|| inside[runEnd]!=inside[runEnd+1]) 
				|| ((runEnd+2)>=inLen 
				|| inside[runEnd+1]!=inside[runEnd+2]) ) 
				&& (runEnd - runStart)<RLE_MAX_RUN )
				runEnd++;
			out[outWrite++]=(byte)(runStart-runEnd);
			while(runStart<runEnd)
				out[outWrite++]=inside[runStart++];
		}
		runEnd++;
	}

	return outWrite;
}