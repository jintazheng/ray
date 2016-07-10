// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
// +----------------------------------------------------------------------
// | * Redistribution and use of this software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer in the documentation and/or other
// |   materials provided with the distribution.
// |
// | * Neither the name of the ray team, nor the names of its
// |   contributors may be used to endorse or promote products
// |   derived from this software without specific prior
// |   written permission of the ray team.
// |
// | THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// | "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// | LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// | A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// | OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// | SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// | LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// | DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// | THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// | (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// | OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// +----------------------------------------------------------------------
#ifndef _H_ATMOSPHERE_H_
#define _H_ATMOSPHERE_H_

#include <ray/render_post_process.h>
#include <ray/geometry.h>

_NAME_BEGIN

class EXPORT Atmospheric : public RenderPostProcess
{
public:
	struct EXPORT Setting
	{
		float earthRadius;
		float earthAtmTopHeight;

		float2 particleScaleHeight;

		float4 rayleighAngularSctrCoeff;
		float4 rayleighTotalSctrCoeff;
		float4 rayleighExtinctionCoeff;
		
		float4 mieAngularSctrCoeff;
		float4 mieTotalSctrCoeff;
		float4 mieExtinctionCoeff;

		float minElevation;
		float maxElevation;

		float aerosolAbsorbtionScale;
		float aerosolPhaseFuncG;
		float aerosolDensityScale;
		float4 aerosolPhaseFuncG4;

		float planeNearZ;
		float planeFarZ;

		Setting() noexcept;
	};

public:
	Atmospheric() noexcept;
	~Atmospheric() noexcept;

	void onActivate(RenderPipeline& pipeline) noexcept;
	void onDeactivate(RenderPipeline& pipeline) noexcept;

	bool onRender(RenderPipeline& pipeline, RenderQueue queue, GraphicsFramebufferPtr& source, GraphicsFramebufferPtr swap) noexcept;

private:
	void computeRaySphereIntersection(const float3& rayOrigin, const float3& rayDirection, const float3& center, float radius, float2& intersections) noexcept;
	void computeApproximateNearFarPlaneDist(const float3& cameraPos, const float4x4& view, const float4x4& proj, const float3& center, const float3& radius, float &znear, float& zfar) noexcept;
	void computeScatteringCoefficients() noexcept;
	void computeDensityIntegralFromChapmanFunc(float heightAboveSurface, const float3& earthCentreToPointDir, const float3& rayDir, float2& densityIntegral) noexcept;
	void computeSunColor(const float3& directionOnSun, float4& sunColor, float4& ambientLight) noexcept;

private:
	bool _needUpdateNetDensityToAtmTop;

	Setting _setting;
	MaterialPtr _sat;
	MaterialTechPtr _computeNetDensityToAtmTop;
	MaterialTechPtr _computeInscatteredRadiance;

	MaterialParamPtr _lightDirection;

	MaterialParamPtr _rayleighTotalSctrCoeff;
	MaterialParamPtr _rayleighExtinctionCoeff;
	MaterialParamPtr _rayleighAngularSctrCoeff;

	MaterialParamPtr _mieTotalSctrCoeff;
	MaterialParamPtr _mieExtinctionCoeff;
	MaterialParamPtr _mieAngularSctrCoeff;

	MaterialParamPtr _earthRadius;
	MaterialParamPtr _earthAtmTopHeight;
	MaterialParamPtr _earthAtmTopRadius;

	MaterialParamPtr _extraterrestrialSunColor;
	MaterialParamPtr _aerosolPhaseFuncG4;
	MaterialParamPtr _particleScaleHeight;
	MaterialParamPtr _tex2DOccludedNetDensityToAtmTop;
	MaterialParamPtr _matProjectInverse;
	MaterialParamPtr _matViewProjectInverse;

	GraphicsTexturePtr _occludedNetDensityMap;
	GraphicsFramebufferPtr _netDensityToAtmTopView;
	GraphicsFramebufferLayoutPtr _netDensityToAtmTopLayout;
};

_NAME_END

#endif