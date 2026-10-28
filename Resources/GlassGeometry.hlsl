// Inputs are node-local coordinates; UV axes include graph zoom and window DPI.
float2 size = max(Size.xy, float2(1, 1));
float radius = min(Shape.x, min(size.x, size.y) * 0.5);
float2 p = (UV - 0.5) * size;
float2 q = abs(p) - (size * 0.5 - radius);
float2 outside = max(q, 0);
float distance = length(outside) + min(max(q.x, q.y), 0) - radius;
float aa = max(fwidth(distance), 0.5);
Mask = 1 - smoothstep(-aa, aa, distance);
float2 normal = length(outside) > 0.0001
    ? normalize(outside) * sign(p)
    : (q.x > q.y ? float2(sign(p.x), 0) : float2(0, sign(p.y)));
float edge = saturate(1 + distance / max(Shape.y, 1));
float bend = sin(edge * 1.5707963);
float2 offset = -normal * bend * Shape.z;
// Keep the subtle glass reflection stable; the separate outline follows the mouse.
float2 lightDirection = normalize(float2(-0.55, -0.83));
float rim = exp(-abs(distance + 1.2) * 0.7);
Highlight = rim * (0.25 + 0.75 * saturate(dot(normal, lightDirection)));
Edge = edge;
DispersionEdge = saturate(1 + distance / max(DispersionShape.x, 1));
NormalUV = (normal.x / size.x) * Axes.xy + (normal.y / size.y) * Axes.zw;
return Mapping.xy + (UV.x + offset.x / size.x) * Axes.xy
                  + (UV.y + offset.y / size.y) * Axes.zw;
