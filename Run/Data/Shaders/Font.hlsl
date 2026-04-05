//------------------------------------------------------------------------------------------------
// Font effects shader for Tier 5 rendering
//
// Requires Vertex_Font vertex data (Position, Color, UVs, GlyphPosition, TextPosition,
// CharacterIndex, EffectParam).
// Combines SDF threshold rendering with vertex-driven effects (wave, rainbow).
// Caller must set sampler to BILINEAR_CLAMP for smooth distance field interpolation.
//------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------
struct VertexInput
{
	float3	a_position			: VERTEX_POSITION;
	float4	a_color				: VERTEX_COLOR;
	float2	a_uvTexCoords		: VERTEX_UVTEXCOORDS;
	float2	a_glyphPosition		: GLYPH_POSITION;
	float2	a_textPosition		: TEXT_POSITION;
	int		a_characterIndex	: CHARACTER_INDEX;
	float	a_effectParam		: EFFECT_PARAM;
};


//------------------------------------------------------------------------------------------------
struct VertexOutPixelIn
{
	float4 v_position			: SV_Position;
	float4 v_color				: SURFACE_COLOR;
	float2 v_uvTexCoords		: SURFACE_UVTEXCOORDS;
	float2 v_glyphPosition		: GLYPH_POSITION;
	float2 v_textPosition		: TEXT_POSITION;
	int    v_characterIndex		: CHARACTER_INDEX;
	float  v_effectParam		: EFFECT_PARAM;
};


//------------------------------------------------------------------------------------------------
cbuffer PerFrameConstants : register(b1)
{
	float	c_time;
	int		c_debugInt;
	float	c_debugFloat;
	float	c_perFramePadding;
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
cbuffer FontConstants : register(b6)
{
	float	g_threshold;        // SDF alpha threshold (default 0.5)
	float	g_effectIntensity;  // Effect strength multiplier
	float	g_fontPadding0;
	float	g_fontPadding1;
};


//------------------------------------------------------------------------------------------------
Texture2D<float4>	t_diffuseTexture : register(t0);
SamplerState		s_diffuseSampler : register(s0);


//------------------------------------------------------------------------------------------------
// HSV to RGB conversion helper
//------------------------------------------------------------------------------------------------
float3 HSVtoRGB( float h, float s, float v )
{
	float c = v * s;
	float x = c * ( 1.0 - abs( fmod( h * 6.0, 2.0 ) - 1.0 ) );
	float m = v - c;

	float3 rgb;
	if      ( h < 1.0 / 6.0 ) rgb = float3( c, x, 0 );
	else if ( h < 2.0 / 6.0 ) rgb = float3( x, c, 0 );
	else if ( h < 3.0 / 6.0 ) rgb = float3( 0, c, x );
	else if ( h < 4.0 / 6.0 ) rgb = float3( 0, x, c );
	else if ( h < 5.0 / 6.0 ) rgb = float3( x, 0, c );
	else                       rgb = float3( c, 0, x );

	return rgb + float3( m, m, m );
}


//------------------------------------------------------------------------------------------------
// VERTEX SHADER — MVP transform + wave effect
//------------------------------------------------------------------------------------------------
VertexOutPixelIn VertexMain( VertexInput input )
{
	VertexOutPixelIn output;

	float4 modelPos		= float4( input.a_position, 1.0 );
	float4 worldPos		= mul( c_modelToWorld, modelPos );
	float4 cameraPos	= mul( c_worldToCamera, worldPos );
	float4 renderPos	= mul( c_cameraToRender, cameraPos );
	float4 clipPos		= mul( c_renderToClip, renderPos );

	// Wave effect: offset Y based on character index and time
	float waveOffset = sin( c_time * 3.0 + float(input.a_characterIndex) * 0.5 ) * g_effectIntensity;
	clipPos.y += waveOffset * 0.02; // Scale to clip space

	output.v_position			= clipPos;
	output.v_color				= input.a_color;
	output.v_uvTexCoords		= input.a_uvTexCoords;
	output.v_glyphPosition		= input.a_glyphPosition;
	output.v_textPosition		= input.a_textPosition;
	output.v_characterIndex		= input.a_characterIndex;
	output.v_effectParam		= input.a_effectParam;

	return output;
}


//------------------------------------------------------------------------------------------------
// PIXEL SHADER — SDF threshold + rainbow color cycling
//------------------------------------------------------------------------------------------------
float4 PixelMain( VertexOutPixelIn input ) : SV_Target0
{
	float4 texel    = t_diffuseTexture.Sample( s_diffuseSampler, input.v_uvTexCoords );
	float  distance = texel.a;

	// SDF threshold with smoothstep
	float smoothing = 0.05;
	float alpha     = smoothstep( g_threshold - smoothing, g_threshold + smoothing, distance );

	if( alpha < 0.01 )
	{
		discard;
	}

	// Rainbow color cycling based on text position and time
	float hue = frac( input.v_textPosition.x + c_time * 0.2 );
	float3 rainbowColor = HSVtoRGB( hue, 0.8, 1.0 );

	// Blend between vertex color and rainbow based on effect intensity
	float3 baseColor = texel.rgb * input.v_color.rgb * c_modelTint.rgb;
	float  rainbowBlend = saturate( g_effectIntensity * 0.5 );
	float3 finalColor = lerp( baseColor, rainbowColor, rainbowBlend );

	return float4( finalColor, alpha * input.v_color.a * c_modelTint.a );
}
