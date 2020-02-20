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
    for (int i = 0; i < width * height; i++) {
	state.image_color[i] = make_pixel(0,0,0);
    }
    std::cout<<"TODO: allocate and initialize state.image_color and state.image_depth."<<std::endl;
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
	const data_geometry* dg[3];
	// 4 cases
	switch(type) {
		case render_type::triangle:
			for(int i = 0; i < state.num_vertices; i+=3){
				for(int j = 0; j < 3; j++){
					data_geometry* g = new data_geometry();
					data_vertex v;
					g->data = state.vertex_data + (i + (j * state.floats_per_vertex));
					v.data = g->data;
					state.vertex_shader(v, *g, state.uniform_data);
					dg[j] = g;
				}
			}
			rasterize_triangle(state, dg);
			delete dg[0];
			delete dg[1];
			delete dg[2];
			break;
		case render_type::indexed:
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
    data_geometry* out = new data_geometry[3];
    data_vertex* dv = new data_vertex[3];
    for(int k = 0; k < 3; k++){
        dv[k].data = in[k]->data;
        state.vertex_shader((const data_vertex)dv[k], out[k], state.uniform_data);
        float w = out[k].gl_Position[3];
        float x = out[k].gl_Position[0]/w;
        float y = out[k].gl_Position[1]/w;
        float z = out[k].gl_Position[2]/w;

        float i = (x * state.image_width / 2) + ((state.image_width - 1) / 2);
        float j = (y * state.image_height / 2) + ((state.image_height - 1) / 2);

        dv[k].data[0] = i;
        dv[k].data[1] = j;
        dv[k].data[2] = z;
        
	state.image_color[(state.image_width*(int)j) + (int)i] = make_pixel(255,255,255);
    }

    float* a = new float[3];
    float* b = new float[3];
    float* c = new float[3];
    
    a[0] = dv[0].data[0];
    a[1] = dv[0].data[1];
    a[2] = dv[0].data[2];
    b[0] = dv[1].data[0];
    b[1] = dv[1].data[1];
    b[2] = dv[1].data[2];
    c[0] = dv[2].data[0];
    c[1] = dv[2].data[1];
    c[2] = dv[2].data[2];

    float abc = (0.5)*((b[0]*c[1] - b[1]*c[0]) - (a[0]*c[1] - a[1]*c[0]) + (a[0]*b[1] - a[1]*b[0]));
    
    for(int i = 0; i < state.image_width; i++){
        for(int j = 0; j < state.image_height; j++){
		float pbc, apc, abp, alp, bet, gam, eye, jay;
		eye = i;
		jay = j;
		
		pbc = (.5)*((b[0] * c[1] - c[0] * b[1]) - (eye * c[1] - c[0] * jay) + (eye * b[1] - b[0] * jay));
		apc = (.5)*((eye * c[1] - c[0] * jay) -(a[0] * c[1] - c[0] * a[1]) +(a[0] * jay - eye * a[1]));
		abp = (.5)*((b[0] * jay - eye * b[1]) -(a[0] * jay - eye * a[1]) +(a[0] * b[1] - b[0] * a[1]));

		alp = pbc / abc;
		bet = apc / abc;
		gam = abp / abc;       

            if(alp >= 0 && bet >= 0 && gam >= 0){
                state.image_color[(state.image_width*j) + i] = make_pixel(255,255,255);
            }
        }
    }

    delete[] a;
    delete[] b;
    delete[] c;
    delete[] dv;
    delete[] out;

    std::cout<<"TODO: implement rasterization"<<std::endl;
}

