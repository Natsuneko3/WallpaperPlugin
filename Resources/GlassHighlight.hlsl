// Draw over an expanded quad without changing the node's layout or wallpaper UVs.
float2 size = max(Size.xy, float2(1, 1));
float2 position = UV * (size + 2 * Size.z) - Size.z;
float radius = clamp(Size.w, 0, min(size.x, size.y) * 0.5);
float2 q = abs(position - size * 0.5) - (size * 0.5 - radius);
float distance = length(max(q, 0)) + min(max(q.x, q.y), 0) - radius;

float width = max(Rim.x, 0.1);
float aa = max(fwidth(distance), 0.5);
float rimBand = 1 - smoothstep(width * 0.5 - aa * 0.5,
    width * 0.5 + aa * 0.5, abs(distance + width * 0.5));

// Local Slate units keep the hotspot circular on both wide and tall nodes.
// Do not clamp the mouse to the node: distance outside the node must still fade.
float2 delta = (position - Mouse.xy) / max(Rim.z, 1);
float proximity = exp(-3 * dot(delta, delta));
float illumination = lerp(saturate(Rim.w), 1, proximity);

float glowRadius = max(Glow.x, 0.001);
float glowDistance = abs(distance) / glowRadius;
float halo = exp(-4 * glowDistance * glowDistance)
    * (1 - smoothstep(0.75, 1, glowDistance));
float energy = max(Rim.y, 0) * illumination * (rimBand + halo * max(Glow.y, 0));
// Additive Slate blending leaves the underlying glass and the node center intact.
return float4(max(HighlightColor.rgb, 0) * energy, saturate(HighlightColor.a));
