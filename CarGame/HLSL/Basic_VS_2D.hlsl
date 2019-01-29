#include "Basic.hlsli"

// 2D¶¥µã×ÅÉ«Æ÷
VertexPosHTex VS_2D(VertexPosTex vIn) {
	VertexPosHTex vOut;
	vOut.PosH = float4(vIn.PosL, 1.0f);
	vOut.Tex = vIn.Tex;
	return vOut;
}