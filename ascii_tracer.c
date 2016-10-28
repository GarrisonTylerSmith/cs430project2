// polymorphism in c
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
typedef struct{
	int kind; // 0 = cylinder, 1 = sphere, 2 = teapot
	double color[3];
	union{
		struct {
			double center[3];
			double radius;
			double height;
		}cylinder;
		struct{
			double center[3];
			double radius;
		}sphere;
		struct{
			double handle_length;
		}teapot;
		struct{
			double position;
			double normal;
		}plane;
		struct{
			double position;
			double width;
			double height;
		}camera;
	};
}Object;

static inline double sqr(double v){
	return v*v;
}
static inline void normalize(double* v){
	double len = sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
	v[0] /= len;
	v[1] /= len;
	v[2] /= len;
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
	double b = (2 * (Ro[0] * Rd[0] - Rd[0] * c[0] + Ro[2] * Rd[2] - Rd[2] * c[2]))
	double c = (sqr(Ro[0]) - 2*Ro[0]*c[0] + sqr(c[0]) + sqr(Ro[2]) - 2* Ro[2]*c[2] + sqr(c[2]) - sqr(r));
	double det = sqr(b) - 4*a*c; 
	if( det < 0 ) return -1;

	det = sqrt(det);

	double t0 = (-b - det) / (2*a);
	if (t0 > 0 ) return t0;

	double t1 = (-b + det) / (2*a);
	if(t1 > 0) return t1;

	return -1;

}
double ray_plane_intersection(double* Ro, double* Rd, double* t){
	//Ray: R(t) = Ro + Rd*t
	//Plane: ax + by + cz = 0
	//solve for t 
	return -1;
}
double ray_sphere_intersection(double* Ro, double* Rd, double* t){
	// Ray: P = Ro + Rd*t
	// Sphere: (x-xc)^2 + (y-yc)^2 + (z-zc)^2 - r^2 = 0
	// Subsituting R(t) into sphere equation
	// (Xo + Xd*t - Xc)^2 + (Yo + Yd*t - Yc)^2 + (Zo + Zd*t - Zc)^2 - r^2 = 0
	// rearranging we get
	// At^2 + Bt + C = 0 where we get two solutions
	return -1;
}
void raycater(){
	double cx;
	double cy;
	double h;
	double w;
	// width in pixels
	double N;
	// height in pixels 
	double M;
	Pix_height = h/M;
	Pix_width = w/N;
	for(int i = 0; i < M; i+=1){
		for(int j = 0; j < N; j+=1){
			P_y = cy - h/2.0 + Pix_height * (i + .5);
			P_y = cx - w/2.0 + Pix_width * (j + .5);
		}
	}

	// here I must create my raycaster
}
// create my own raycasting fucntion
int main(){
// this is all hard code, use the parsing file for json and raycaster function
	Object** objects;
	objects = malloc(sizeof(Object*)*2);
	objects[0] = malloc(sizeof(Object));
	objects[0]->kind = 0;
	objects[0]->cylinder.radius = 2;
	objects[0]->cylinder.center[0] = 0;
	objects[0]->cylinder.center[1] = 0;
	objects[0]->cylinder.center[2] = 20;
	objects[1] = NULL;

	double cx = 0;
	double cy = 0;
	double h = 0.7;
	double w = 0.7;

	int M = 20;
	int N = 20;


	double pixheight = h / M;
	double pixwidth = w / N;
	for(int y = 0; y < M; y+=1){
		for(int y = 0; x < N; x+=1){
			double Ro[3] = {0,0,0};
			// Rd = normalize(P - Ro)
			double Rd[3] = {
				cx - (w/2) + pixwidth * (x + 0.5),
				cy - (h/2) + pixheight * (y + 0.5),
				1
			};
			normalize(Rd);

			double best_t = INFINITY;
			for(int i = 0; objects[i] != 0; i+=1){
				double t = 0;
				switch(objects[i]->kind){
				case 0;
					t = cylinder_intersecion(Ro,Rd,objects[i]->cylinder.center, objects[i]->cylinder.radius);


					break;
				default;
				// horrible error
				exit(1);
				}
				if(t > 0 best_t && t < best_t) best_t = t;
			}
			if(best_t > 0 && best_t != INFINITY){
				printf("#");
			}else {
				printf(".");
			}
		}
	}


	return 0;
}