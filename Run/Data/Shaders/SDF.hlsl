//------------------------------------------------------------------------------------------------
// SDF (Signed Distance Field) shader for font rendering
//
// Requires Vertex_PCU vertex data (Position, Color, UVs).
// Uses alpha channel as distance field — applies smoothstep threshold for crisp edges at any scale.
// Caller must set sampler to BILINEAR_CLAMP for smooth distance field interpolation.
//------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------
struct VertexInput
{
	float3	a_position		: VERTEX_POSITION;
	float4	a_color			: VERTEX_COLOR;
	float2	a_uvTexCoords	: VERTEX_UVTEXCOORDS;
};


//------------------------------------------------------------------------------------------------
struct VertexOutPixelIn
{
	float4 v_position		: SV_Position;
	float4 v_color			: SURFACE_COLOR;
	float2 v_uvTexCoords	: SURFACE_UVTEXCOORDS;
};


//------------------------------------------------------------------------------------------------
cbuffer CameraConstants : register(b3)
{
	float4x4	c_worldToCamera;
	float4x4	c_cameraToRender;
	float4x4	c_renderToClip;
};


//------------------------------------------------------------------------------------------------
cbuffer ModelConstants : register(b4)
{
	float4x4	c_modelToWorld;
	float4		c_modelTint;
};


//------------------------------------------------------------------------------------------------
Texture2D<float4>	t_diffuseTexture : register(t0);
SamplerState		s_diffuseSampler : register(s0);


//------------------------------------------------------------------------------------------------
// VERTEX SHADER — identical to Default.hlsl
//------------------------------------------------------------------------------------------------
VertexOutPixelIn VertexMain( VertexInput input )
{
	VertexOutPixelIn output;

	float4 modelPos		= float4( input.a_position, 1.0 );
	float4 worldPos		= mul( c_modelToWorld, modelPos );
	float4 cameraPos	= mul( c_worldToCamera, worldPos );
	float4 renderPos	= mul( c_cameraToRender, cameraPos );
	float4 clipPos		= mul( c_renderToClip, renderPos );

	output.v_position		= clipPos;
	output.v_color			= input.a_color;
	output.v_uvTexCoords	= input.a_uvTexCoords;

	return output;
}


//------------------------------------------------------------------------------------------------
// PIXEL SHADER — SDF threshold with smoothstep anti-aliasing
//------------------------------------------------------------------------------------------------
float4 PixelMain( VertexOutPixelIn input ) : SV_Target0
{
	float4 texel    = t_diffuseTexture.Sample( s_diffuseSampler, input.v_uvTexCoords );
	float  distance = texel.a;

	// Smoothstep around 0.5 threshold for anti-aliased edges
	float threshold = 0.5;
	float smoothing = 0.05;
	float alpha     = smoothstep( threshold - smoothing, threshold + smoothing, distance );

	if( alpha < 0.01 )
	{
		discard;
	}

	float3 color = texel.rgb * input.v_color.rgb * c_modelTint.rgb;
	return float4( color, alpha * input.v_color.a * c_modelTint.a );
}
