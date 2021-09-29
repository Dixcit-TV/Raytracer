# Raytracer

Right-Handed Software Raytracer

## Introduction
Although it is a school project, it was a great experience to have a more guided and in-depth take on developing my own Raytracer. It also grew my interest in Graphics programming (as did all the projects I worked on that subject later on).

On a side note, over the summer between my first and second year at Digital Art and Entertainment, I decided to work on my own Raytracer as a side project. I knew my first graphics programming was coming up and working with a raytracer was something that was thrown at me by comapnies during job applications. It wasn't glorious back then (although my lack of C++ knowledge was also an issue) so I wanted to take my revenge.

So it technically is my second raytracer but this one is more full and interesting so it should get the most credits.

## What does it do?
The raytracer has some basic features you would expect:

- Sphere & Plane intersection.
- OBJ file loading (removes duplicate vertices).
- Triangle meshes intersection.
- Point & Directional lights.
- Hard shadows
- Lambert, Phong BRDFs
- Cook-Torrance PBR BRDF
- Front-, Back- & No-Culling modes
- Dynamic right-handed camera (UE4-like controls)
- Quaternion-based camera rotations

## The Result

Rendered on the image below:
- 6 spheres, all with different material settings: The 3 top spheres are assigned metalic cook-torrance materials with different metal fresnel (Aluminium, Gold, Iron) and different roughness. Then the 3 bottom spheres are assigned non-metallic cook-torrance materials.
- 3 rotating triangles, all with different culling modes: from left to right, Frontface-, Backface- & No-Culling.
- 2 Point Lights
- 1 Directional Light

![Raytracer_Scene1_1](https://user-images.githubusercontent.com/76394390/135266665-99590558-2eea-4991-8ceb-1e0fb307c4cd.png)


Rendered on the image below:
- 1 metallic golden Stanford Bunny.
- 2 Point Lights
- 1 Directional Light

![Raytracer_Scene1_2](https://user-images.githubusercontent.com/76394390/135266305-9378fada-c77a-4270-a608-b873955b6ced.png)
