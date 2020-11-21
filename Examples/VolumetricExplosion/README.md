# Volumetric explosion

Animated volumetric explosion. The basic idea is to use sphere signed distance field and add noise to it, and map the displaced amount to gradient texture (the further from center, the darker it is). In addition there's an optimization done with domain shader that calculates a shell for the noised sphere.

Based on Realistic Volumetric Explosions in Games by Alex Dunn on GPU Pro 6

http://gpupro.blogspot.com/2014/12/realistic-volumetric-explosions-in-games.html

https://www.routledge.com/GPU-Pro-6-Advanced-Rendering-Techniques/Engel/p/book/9781482264616

![ve](ve.png?raw=true "ve")
