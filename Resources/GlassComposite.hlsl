// A normalized discrete Gaussian keeps a uniform background unchanged. The grid
// covers [-1, 1] in both directions; BlurStep supplies its radius in video UV.
float3 gaussianSamples[25] = {
    Gaussian0, Gaussian1, Gaussian2, Gaussian3, Gaussian4,
    Gaussian5, Gaussian6, Gaussian7, Gaussian8, Gaussian9,
    Gaussian10, Gaussian11, Center, Gaussian13, Gaussian14,
    Gaussian15, Gaussian16, Gaussian17, Gaussian18, Gaussian19,
    Gaussian20, Gaussian21, Gaussian22, Gaussian23, Gaussian24
};
float sigma = clamp(Gaussian.y, 0.15, 1.5);
// A 2D Gaussian factors into two 1D weights. Reuse the two noncentral
// exponentials instead of evaluating an exponential separately for every tap.
float nearWeight = exp(-0.125 / (sigma * sigma));
float farWeight = exp(-0.5 / (sigma * sigma));
float axisWeights[5] = { farWeight, nearWeight, 1, nearWeight, farWeight };
float3 gaussianSum = 0;
float axisWeightSum = 1 + 2 * (nearWeight + farWeight);
[unroll]
for (int gaussianIndex = 0; gaussianIndex < 25; ++gaussianIndex)
{
    float weight = axisWeights[gaussianIndex % 5] * axisWeights[gaussianIndex / 5];
    gaussianSum += gaussianSamples[gaussianIndex] * weight;
}
float3 gaussian = gaussianSum / (axisWeightSum * axisWeightSum);
float gaussianStrength = saturate(Gaussian.x) * lerp(1, saturate(Gaussian.z), Edge);
float3 filtered = lerp(Center, gaussian, gaussianStrength);
float3 diffuse = (Center * 4 + Scatter0 + Scatter1 + Scatter2 + Scatter3
                 + Scatter4 + Scatter5 + Scatter6 + Scatter7) / 12;
float3 transmitted = lerp(filtered, diffuse, saturate(Optics.x));

// These overlapping violet-to-red bands approximate a spectrum using RGB video.
// Normalize each channel separately so colored and neutral constants stay exact.
float3 spectrumSamples[9] = {
    Spectrum0, Spectrum1, Spectrum2, Spectrum3, Spectrum4,
    Spectrum5, Spectrum6, Spectrum7, Spectrum8
};
float3 spectrumWeights[9] = {
    float3(0.35, 0, 1), float3(0.05, 0.1, 1), float3(0, 0.45, 1),
    float3(0, 1, 0.75), float3(0.1, 1, 0.1), float3(0.75, 1, 0),
    float3(1, 0.55, 0), float3(1, 0.15, 0), float3(1, 0, 0)
};
float3 spectrumSum = 0;
float3 spectrumWeightSum = 0;
[unroll]
for (int band = 0; band < 9; ++band)
{
    spectrumSum += spectrumSamples[band] * spectrumWeights[band];
    spectrumWeightSum += spectrumWeights[band];
}
float3 dispersed = spectrumSum / spectrumWeightSum;
float dispersionWeight = saturate(Spectrum.y)
    * pow(saturate(DispersionEdge), max(Spectrum.z, 0.01));
// Radius zero must preserve the filtered image, rather than replace it with the
// unfiltered center. Apply dispersion last so scattering cannot suppress it.
dispersionWeight *= Spectrum.x > 0 ? 1 : 0;
transmitted = lerp(transmitted, dispersed, dispersionWeight);

// Uniform neutral attenuation preserves tonal relationships. Do not clamp every
// pixel to a fixed luminance or mix in an arbitrary blue/white fog color.
float3 color = max(transmitted, 0) * saturate(Absorption.rgb) * saturate(Optics.y);
color *= 1 - Edge * 0.04 * Finish.z;
color += Highlight * Finish.x;
return float4(color, Mask * saturate(Finish.y));
