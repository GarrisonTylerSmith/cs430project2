// polymorphism in c
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "Imagebuffer.c"
#include "Parsingjson.c"

static inline double sqr(double v){
	return v*v;
}
static inline void normalize(double* v){
	double len = sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
	v[0] /= len;
	v[1] /= len;
	v[2] /= len;
}
double dot(double v[], double u[], int n){
	double result = 0;
	for(int i=0; i < n; i++){
		result += v[i]*u[i];
	}
	return result;
}

double cylinder_intersecion(double* Ro, double* Rd, double* c, double r){
	// Step 1 . Find the equation for the object 
	// we are interested in (this case a cylinder)
	// this is essential for the camera view point
	// x^2 +z^2 = r^2
	// Step 2. Parameterize the equation with a center point (if needeed)
	// (x-Cx)^2 + (z-Cz)^2 = r^2
	// step 3. Substitute the eq for a ray into our object eq.
	//
	//(RoX + RDX*t - Cx)^2 + ( Roz + t*Rdz - Cz)^2 - r^2 = 0
	//
	// Step 4. Solve for t .
	//
	//Step 4.a Rewrite the equation(flatten).
	//
	// -r^2 + t^2 * Rdx^2 + t^2 * Rdz^2 + 2*t * Rox * Rdx - 2*t * Rdx * Cx + 2*t * Roz * Rdz - 2*t * Rdz *cz + Rox^2 - 2*Rox*cx + cx^2 + Roz^2 - 2* Roz* Cz + Cz^2 = 0
	//
	// Step 4.b Rewrite the equation in terms of t.
	//
	// t^2 * (Rdx^2 + Rdz^2) + 
	// t* (2 * (Rox * Rdx - Rdx * cx + Roz * Rdz - Rdz * Cz)) + 
	// Rox^2 - 2*Rox*cx + cx^2 + Roz^2 - 2* Roz* Cz + Cz^2 - r^2 = 0
	// Use quadratic equation to solvew for t..
	//
	double a = (sqr(Rd[0]) + sqr(Rd[2]));
	double b = (2 * (Ro[0] * Rd[0] - Rd[0] * c[0] + Ro[2] * Rd[2] - Rd[2] * c[2]));
	double d = (sqr(Ro[0]) - 2*Ro[0]*c[0] + sqr(c[0]) + sqr(Ro[2]) - 2* Ro[2]*c[2] + sqr(c[2]) - sqr(r));
	double det = sqr(b) - 4*a*d; 
	if( det < 0 ) return -1;

	det = sqrt(det);

	double t0 = (-b - det) / (2*a);
	if (t0 > 0 ) return t0;

	double t1 = (-b + det) / (2*a);
	if(t1 > 0) return t1;

	return -1;

}
double ray_plane_intersection(double a, double b, double c, double d, double* r0, double* rd){
	return (a*r0[0] + b*r0[1] + c*r0[2] + d) / (a*rd[0] + b*rd[1] + c*rd[2]);
	//Ray: R(t) = Ro + Rd*t
	//Plane: ax + by + cz + d = 0
	//solve for t
	//n is the normal plane
	// D is the distance from the origin
	// do subsituting we get 
	// t = -(n*Ro + d)/ (n * Rd) 

	// double a = -(n*Ro + d)
	// double b = (n*Rd)
	// double det = (a/b)
	// if(det < 0) return -1;
}
double ray_sphere_intersection(double* c, double r, double* Ro, double* Rd){
	// Ray: P = Ro + Rd*t
	// Sphere: (x-xc)^2 + (y-yc)^2 + (z-zc)^2 - r^2 = 0
	// Subsituting R(t) into sphere equation
	// (Xo + Xd*t - Xc)^2 + (Yo + Yd*t - Yc)^2 + (Zo + Zd*t - Zc)^2 - r^2 = 0
	// rearranging we get
	// At^2 + Bt + C = 0 where we get two solutions
	// this is a vector from p to c
	double a = (sqr(Rd[0]) + sqr(Rd[1]) + sqr(Rd[2]));
	double b = ((2*Rd[0]*(Ro[0]-c[0])) + (2*Rd[1]*(Ro[1]-c[1])) + (2*Rd[2]*(Ro[2]-c[2])));
	double d = ((sqr(Ro[0]-c[0])+sqr(Ro[1]-c[1])+sqr(Ro[2]-c[2])) - sqr(2));
	double det = sqr(b) - 4*a*d; 
	if( det < 0 ) return -1;

	det = sqrt(det);

	double t0 = (-b - det) / (2*a);
	if (t0 > 0 ) return t0;

	double t1 = (-b + det) / (2*a);
	if(t1 > 0) return t1;

	return -1;



}
void raycast(Scene scene, char* outfile, PPMmeta fileinfo){
	PPMImage* data = malloc(sizeof(PPMImage) * fileinfo.width * fileinfo.height);
	
	// raycasting here
	
	int N = fileinfo.width;
	int M = fileinfo.height;
	double w = scene.camera_width;
	double h = scene.camera_height;
	
	double pixel_height = h / M;
	double pixel_width = w / N;
	
	double p_z = 0;
	
	double c_x = scene.camera_position[0];
	double c_y = scene.camera_position[1];
	double c_z = scene.camera_position[2];
	
	double r0[3];
	r0[0] = c_x;
	r0[1] = c_y;
	r0[2] = c_z;
	
	double rd[3];
	rd[2] = 1.0;
	
	int i;
	int j;
	int k;
	
	for(i = 0; i < M; i ++)
	{
		rd[1] = c_y + h/2.0 - pixel_height * (i + 0.5);
		
		for(j = 0; j < N; j ++)
		{
			rd[0] = c_x - w/2.0 + pixel_width * (j + 0.5);
			
			double best_t = INFINITY;
			Object closest;

			for(k = 0; k < scene.num_objects; k ++)
			{
				double t = -1;
				Object o = scene.objects[k];
				
				if(o.kind == T_SPHERE)
				{
					double c[3];
					c[0] = o.a;
					c[1] = o.b;
					c[2] = o.c;
					t = ray_sphere_intersection(c, o.d, r0, rd);
				}
				else if(o.kind == T_PLANE)
				{
					t = ray_plane_intersection(o.a,o.b,o.c,o.d,r0,rd);
				}
				
				if(t > 0 && t < best_t)
				{
					best_t = t;
					closest = o;
				}
			}
			
			// write to Pixel* data
			
			PPMImage pixel = data[i * N + j];
			if(best_t < INFINITY && closest.kind >= 0)
			{
				// makes the colors for the obejcts 
				pixel.red = (char) (closest.color[0] * 255);
				pixel.green = (char) (closest.color[1] * 255);
				pixel.blue = (char) (closest.color[2] * 255);
			}
			else
			{
				// creates shadow effect
				pixel.red = (char) (255* scene.background_color[0]);
				pixel.green = (char) (255* scene.background_color[1]);
				pixel.blue = (char) (255* scene.background_color[2]);
			}

			data[i * N + j] = pixel;
			
		}
		
	}
	
	writePPM( outfile, data, fileinfo);
}

// create my own raycasting fucntion
int main(int argc, char** argv){
// this is all hard code, use the parsing file for json and raycaster function
	if(argc < 5){
		fprintf(stderr, "Usage: width height inout.json output.ppm\n");
		exit(1);
	}
	PPMmeta fileinfo;
	fileinfo.width = atoi(argv[1]);
	fileinfo.height = atoi(argv[2]);
	fileinfo.max = 255;
	fileinfo.type = 6;

	Scene scene;
	scene = read_scene(argv[3]); 
	// Scene scene = read_scene(argv[3]);


	printf("Read in %d objects\n", scene.num_objects);

	scene.background_color[0] = 0.5;
	scene.background_color[1] = 0.51;
	scene.background_color[2] = 0.6;

	raycast(scene,argv[4], fileinfo);

	return 0;
}

