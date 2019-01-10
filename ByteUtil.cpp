#include "StdAfx.h"
#include "ByteUtil.h"
#include "MathUtil.h"

vector<byte> ByteUtil::get2Bytes(int i) 
{
	vector<byte> b;
	b.resize(2);

	b[0]=(byte)(i&0xFF);
	b[1]=(byte)((i>>8)&0xFF);

	return b;
}

vector<byte> ByteUtil::get4Bytes(int i) 
{
	vector<byte> b;
	b.resize(4);

	b[0]=(byte)(i&0xFF);
	b[1]=(byte)((i>>8)&0xFF);
	b[2]=(byte)((i>>16)&0xFF);
	b[3]=(byte)((i>>24)&0xFF);

	return b;
}

vector<byte> ByteUtil::get4BytesInv(int i) 
{
	vector<byte> b;
	b.resize(4);

	b[3]=(byte)(i&0xFF);
	b[2]=(byte)((i>>8)&0xFF);
	b[1]=(byte)((i>>16)&0xFF);
	b[0]=(byte)((i>>24)&0xFF);

	return b;
}

vector<byte> ByteUtil::get8Bytes(long i)
{
	vector<byte> b;
	b.resize(8);

	b[0]=(byte)(i&0xFF);
	b[1]=(byte)((i>>8)&0xFF);
	b[2]=(byte)((i>>16)&0xFF);
	b[3]=(byte)((i>>24)&0xFF);

	b[4]=(byte)((i>>32)&0xFF);
	b[5]=(byte)((i>>40)&0xFF);
	b[6]=(byte)((i>>48)&0xFF);
	b[7]=(byte)((i>>56)&0xFF);

	return b;
}

long ByteUtil::toLong(const vector<byte>&ints)
{
	return (((toInt(ints[0],ints[1],ints[2],ints[3]))) 
		| ((long)(toInt(ints[4],ints[5],ints[6],ints[7]))<<(long)32));
}

int ByteUtil::toInt(byte in0,byte in1,byte in2,byte in3) 
{
	return (in0&0xFF) | ( (in1&0xFF)<<8) | ((in2&0xFF)<<16) 
		| ((in3&0xFF)<<24 );
}

int ByteUtil::toInt(const vector<byte>&ints) 
{
	return toInt(ints[0],ints[1],ints[2],ints[3]);
}

int ByteUtil::toInt(const vector<byte>&ints,int ofs)
{
	return toInt(ints[ofs+0],ints[ofs+1],ints[ofs+2],ints[ofs+3]);
}

int ByteUtil::floatToHalf(float f) 
{
	int i=MathUtil::floatToRawIntBits(f);
	int s=(i>>16)&0x00008000;
	int e=((i>>23)&0x000000ff)-(127-15);
	int m=i&0x007fffff;
	if(e<=0)
	{
		if(e<-10)				
			return 0;
		
		m=(m|0x00800000)>>(1-e);	
		if((m&0x00001000)==0x00001000)
			m+=0x00002000;

		return s | (m>>13);
	}
	else if(e==0xff-(127-15)) 
	{
		if(m==0)
		{		
			return s | 0x7c00;
		}
		else 
		{			
			m>>=13;

			return s | 0x7c00 | m | ((m==0)?0:1);
		}
	} 
	else 
	{		
		if((m&0x00001000)==0x00001000)
		{
			m+=0x00002000;
			if((m&0x00800000)==0x00800000) 
			{
				m=0;
				e+=1;
			}
		}		
		if(e>30)
		{			
			return s | 0x7c00; 
		} 	

		return s | (e<<10) | (m>>13);
	}
}