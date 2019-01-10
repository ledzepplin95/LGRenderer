#pragma once

class ByteUtil
{
public:
	static vector<byte> get2Bytes(int i);
	static vector<byte> get4Bytes(int i);
	static vector<byte> get4BytesInv(int i);
	static vector<byte> get8Bytes(long i);
	static long toLong(const vector<byte>&ints);
	static int toInt(byte in0,byte in1,byte in2,byte in3);
	static int toInt(const vector<byte>&ints);
	static int toInt(const vector<byte>&ints,int ofs);
	static int floatToHalf(float f);
};
