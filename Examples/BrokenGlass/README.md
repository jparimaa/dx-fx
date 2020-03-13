# Broken glass

An example that shows one way to implement "broken glass" effect. First the opaque object is rendered to a texture. That texture is used as an input for the translucent object. Instead of blending directly, translucent object samples from the opaque texture with a small offset that comes from "light refraction through broken glass". Refraction is calculated using a normal map, HLSL has a ready made refract-function for that. The refracted vector is calculated in world space and then transformed to screen space for sampling.

![broken_glass](broken_glass.png?raw=true "broken_glass")


