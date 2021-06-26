# Image rendering using backward ray tracing

This is a simple c++ program that renders spheres of various surface properties using the principles of backward ray tracing and Fresnal equations. 


# sphere class

This class represents the spherical bodies that are to be rendered 
every sphere has properties such as position in 3-d space , radius ,
transparency , reflectivity , surface color and emission color(if it acts
as a light source)
there's a boolean valued member function which return  true if the ray intersects the sphere surface , the equation is solved in such a way to minimize the imperfections in image due to precision errors

## Vec class

This class stores a 3 dimensional vector which is used throughout the program to represent direction vectors of rays and RGB colors of surfaces. The class supports necessary vector algebra operations such 
as scaler multiplication , addition of 2 vectors ,dot product etc.

## Trace function
This is where the magic happens . The ray origin ,ray direction, spheres 
are taken as input and then the ray is traced based on it's interaction with the spheres that the ray strikes , this a recursive function and to avoid the case of infinite reflections and refractions a recursive depth is defined. if the surface that the ray strikes is reflective or transparent or both then the ray is further traced from the point of refection or refraction else if it is diffuse object, the surface color is returned.
if there are no objects in the path of the ray then the background color is returned. the direction of ray after reflection from a surface is calculated using equation of reflection and similarly the direction after
refraction is calculated using refraction equations .
Here the spheres are assumed to be made of glass and outside world contains air 
The contribution of reflection and refraction in determination of color is
determined by the Fresnal equations 

## img_render

In here we define the size of the pixel grid and store the value of color obtained from the trace function for every pixel of the grid
by default the aspect ratio is 480p and the field of view is 60 degrees
after all the processing has been completed the data is stored in a ppm file named render_result.ppm 

## main
Here the data of spheres is provided to the program and stored the
std::vector 
the data is hardcoded in the program for the sole purpose of showcasing the functioning of the program however it can be taken at runtime if required with minor tweaks in the code
A light source is taken input with emission color and is placed in the back of all the spheres for the purpose of illumination 
A very big sphere taken input which forms the base in the image
