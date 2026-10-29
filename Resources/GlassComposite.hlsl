// Gaussian convolution is shared across nodes and performed before all optical
// sampling. Dispersion and scattering must not restore the unfiltered texture.
float gaussianStrength = saturate(Gaussian.x) * lerp(1, saturate(Gaussian.z), Edge);
float3 filtered = lerp(Center, FilteredCenter, gaussianStrength);
float3 sharpDiffuse = (Center * 4 + Scatter0 + Scatter1 + Scatter2 + Scatter3
                 + Scatter4 + Scatter5 + Scatter6 + Scatter7) / 12;
float3 blurredDiffuse = (FilteredCenter * 4 + FilteredScatter0 + FilteredScatter1
    + FilteredScatter2 + FilteredScatter3 + FilteredScatter4 + FilteredScatter5
    + FilteredScatter6 + FilteredScatter7) / 12;
float3 diffuse = lerp(sharpDiffuse, blurredDiffuse, gaussianStrength);
float3 transmitted = lerp(filtered, diffuse, saturate(Optics.x));

// These overlapping violet-to-red bands approximate a spectrum using RGB video.
// Normalize each channel separately so colored and neutral constants stay exact.
float3 spectrumSamples[9] = {
    Spectrum0, Spectrum1, Spectrum2, Spectrum3, Spectrum4,
    Spectrum5, Spectrum6, Spectrum7, Spectrum8
};
float3 blurredSpectrumSamples[9] = {
    FilteredSpectrum0, FilteredSpectrum1, FilteredSpectrum2, FilteredSpectrum3,
    FilteredSpectrum4, FilteredSpectrum5, FilteredSpectrum6, FilteredSpectrum7,
    FilteredSpectrum8
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
    spectrumSum += lerp(spectrumSamples[band], blurredSpectrumSamples[band], gaussianStrength)
        * spectrumWeights[band];
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
