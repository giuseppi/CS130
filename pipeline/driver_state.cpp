#include "driver_state.h"
#include <cstring>

driver_state::driver_state()
{
}

driver_state::~driver_state()
{
    delete [] image_color;
    delete [] image_depth;
}

// This function should allocate and initialize the arrays that store color and
// depth.  This is not done during the constructor since the width and height
// are not known when this class is constructed.
void initialize_render(driver_state& state, int width, int height)
{
    state.image_width=width;
    state.image_height=height;
    state.image_color= new pixel[width * height];
    state.image_depth= new float[width * height];
    std::fill(state.image_color, state.image_color + (width * height), make_pixel(0,0,0));
    std::fill(state.image_depth, state.image_depth + (width * height), 2);
}

// This function will be called to render the data that has been stored in this class.
// Valid values of type are:
//   render_type::triangle - Each group of three vertices corresponds to a triangle.
//   render_type::indexed -  Each group of three indices in index_data corresponds
//                           to a triangle.  These numbers are indices into vertex_data.
//   render_type::fan -      The vertices are to be interpreted as a triangle fan.
//   render_type::strip -    The vertices are to be interpreted as a triangle strip.
void render(driver_state& state, render_type type)
{
	std::cout<<"TODO: implement rendering."<<std::endl;
	// 4 cases
	switch(type) {
		case render_type::triangle:
			for(int i = 0; i < state.num_vertices; i+=3){
				const data_geometry* d[3];
				for(int j = 0; j < 3; j++){
					data_geometry* g = new data_geometry();
					data_vertex v;
					g->data = state.vertex_data + (i + j) * state.floats_per_vertex;
					v.data = g->data;
					state.vertex_shader(v, *g, state.uniform_data);
					d[j] = g;
				}
				rasterize_triangle(state, d);
				delete d[2];
				delete d[1];
				delete d[0];
			}
				break;
		case render_type::indexed:
			for(int i = 0; i < 3 * state.num_triangles; i+=3){
				const data_geometry* d[3];
				for(int j = 0; j < 3; j++){
					data_geometry* g = new data_geometry();
					data_vertex v;
					g->data = state.vertex_data + (state.index_data[(i + j)] * state.floats_per_vertex);
					v.data = g->data;
					state.vertex_shader(v, *g, state.uniform_data);
					d[j] = g;
				}
				rasterize_triangle(state, d);
				delete d[2];
				delete d[1];
				delete d[0];
			}
				
			break;
		case render_type::fan:
			break;
		case render_type::strip:
			break;
		case render_type::invalid:
			break;
	}

}


// This function clips a triangle (defined by the three vertices in the "in" array).
// It will be called recursively, once for each clipping face (face=0, 1, ..., 5) to
// clip against each of the clipping faces in turn.  When face=6, clip_triangle should
// simply pass the call on to rasterize_triangle.
void clip_triangle(driver_state& state, const data_geometry* in[3],int face)
{
    if(face==6)
    {
        rasterize_triangle(state, in);
        return;
    }
    std::cout<<"TODO: implement clipping. (The current code passes the triangle through without clipping them.)"<<std::endl;
    clip_triangle(state,in,face+1);
}

// Rasterize the triangle defined by the three vertices in the "in" array.  This
// function is responsible for rasterization, interpolation of data to
// fragments, calling the fragment shader, and z-buffering.
void rasterize_triangle(driver_state& state, const data_geometry* in[3])
{
    float x[3],y[3],z[3];
    //float k0[3],k1[3],k2[3];
    //float b[3];
   

    for (int i = 0; i < 3; i++) {
        float halfw = state.image_width / 2;
        float halfh = state.image_height / 2;
        
        x[i] = halfw * ((in[i]->gl_Position[0]) / (in[i]->gl_Position[3]) + 1);
        y[i] = halfh * ((in[i]->gl_Position[1]) / (in[i]->gl_Position[3]) + 1);
        z[i] = (in[i]->gl_Position[2]) / (in[i]->gl_Position[3]);
    }
    
    float area = 0.5 * ( (x[1] * y[2] - x[2] * y[1]) -
                         (x[0] * y[2] - x[2] * y[0]) +
                         (x[0] * y[1] - x[1] * y[0]) );  

    float x_min = std::min(std::min(x[0],x[1]),x[2]);
    float x_max = std::max(std::max(x[0],x[1]),x[2]);
    float y_min = std::min(std::min(y[0],y[1]),y[2]);
    float y_max = std::max(std::max(y[0],y[1]),y[2]);
   
   /*
    k0[0] = x[1] * y[2] - x[2] * y[1];
    k1[0] = y[1] - y[2];
    k2[0] = x[2] - x[1];

    k0[1] = x[2] * y[0] - x[0] * y[2];
    k1[1] = y[2] - y[0];
    k2[1] = x[0] - x[2];

    k0[2] = x[0] * y[1] - x[1] * y[0];
    k1[2] = y[0] - y[1];  
    k2[2] = x[1] - x[0];
    
    for (int y = y_min; y < y_max + 1; y++) {
        for (int x = x_min + 1; x < x_max + 1; x++) {
            for (int v = 0; v < 3; v++) {
                b[v] = 0.5 * (k0[v] + k1[v] * x + k2[v] * y) / area;
            }
            if (b[0] >= 0 && b[1] >= 0 && b[2] >= 0) {
                state.image_color[x + y * state.image_width] = make_pixel(255,255,255);
            }
        }
    }
 */
 
    for (int i = x_min; i < x_max; i++) {
        for (int j = y_min; j < y_max; j++) {
      	    float Brea = 0.5 * ((x[0] * (j - y[2])) + (i * (y[2] - y[0])) + (x[2] * (y[0] - j)));
        	  float Grea = 0.5 * ((x[0] * (y[1] - j)) + (x[1] * (j - y[0])) + (i * (y[0] - y[1])));
            double beta = Brea / area;
            double gamma = Grea / area;
            double alpha = (1 - beta) - gamma;
            if (alpha >= -0.001 && beta >= -0.001 && gamma >= -0.001 && (alpha + beta + gamma) <= 1.001) {
                int img_index = (j * state.image_width) + i;
                float interpolation_stuff[state.floats_per_vertex];
                for (int k = 0; k < state.floats_per_vertex; k++) {
                if (state.interp_rules[k] == interp_type::flat) {
                     interpolation_stuff[k] = in[0]->data[k];
                } else if (state.interp_rules[k] == interp_type::smooth) {
                } else if (state.interp_rules[k] == interp_type::noperspective) {
                     interpolation_stuff[k] = (alpha * in[0]->data[k]) + (beta * in[1]->data[k]) + (gamma * in[2]->data[k]);
                }
             }
             float zValue = (alpha * z[0]) + (beta * z[1]) + (gamma * z[2]);
             if (zValue <= state.image_depth[img_index]) {
               data_fragment dataFrag; 
               dataFrag.data = interpolation_stuff;
               data_output dataOut;
               state.fragment_shader(dataFrag, dataOut, state.uniform_data);
               state.image_color[img_index] = make_pixel(255 * dataOut.output_color[0], 255 * dataOut.output_color[1], 255 * dataOut.output_color[2]);
               state.image_depth[img_index] = zValue;
            }
          }
        }
    } 

}
