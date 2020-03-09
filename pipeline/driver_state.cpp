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
				clip_triangle(state, d, 0);
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
				clip_triangle(state, d, 0);
				delete d[2];
				delete d[1];
				delete d[0];
			}
			break;
		case render_type::fan:
      for (int i = 0; i < state.num_vertices; i++) {
            const data_geometry* d[3];
            for (int j = 0; j < 3; j++) {
              int ind = ((i + j) * state.floats_per_vertex);
              if (j == 0) {
                ind = 0;
              }
              data_geometry* dg = new data_geometry();
              dg->data = state.vertex_data + ind;
              data_vertex dv;
              dv.data = dg->data;
              state.vertex_shader(dv, *dg, state.uniform_data);
              d[j] = dg;
            }
            clip_triangle(state, d, 0);
            delete d[2];
				    delete d[1];
				    delete d[0];
          }
			break;
		case render_type::strip:
      for (int i = 0; i < (state.num_vertices - 2); i++) {
            const data_geometry* d[3];
            for (int j = 0; j < 3; j++) {
              int ind = ((i + j) * state.floats_per_vertex);
              data_geometry* dg = new data_geometry();
              dg->data = state.vertex_data + ind;
              data_vertex dv;
              dv.data = dg->data;
              state.vertex_shader(dv, *dg, state.uniform_data);
              d[j] = dg;
            }
            clip_triangle(state, d, 0);
            delete d[2];
				    delete d[1];
				    delete d[0];
      }
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
      float count = 0;
      float index = 0;
      float val = 1;
    
      ivec3 vec1 = {0, 0, 0};
      if (face == 0) { 		// x = 1
        index = 0;
        val = 1;
      } else if (face == 1) { // x = -1
        index = 0;
        val = -1;
      } else if (face == 2) { // y = 1
        index = 1;
        val = 1;
      } else if (face == 3) { // y = -1
        index = 1;
        val = -1;
      } else if (face == 4) { // z = 1
        index = 2;
        val = 1;
      } else if (face == 5) { // z = -1
        index = 2;
        val = -1;
      }
    
      for (int i = 0; i < 3; i++) {
        if (val < 0 && in[i]->gl_Position[index] >= (val * in[i]->gl_Position[3])) {
          count++;
          vec1[i] = 1;
        } else if (val > 0 && in[i]->gl_Position[index] <= (val * in[i]->gl_Position[3])) {
          count++;
          vec1[i] = 1;
        }
      }
    
      if (count == 3) {
        clip_triangle(state,in,face+1);
        return;
      } else if (count == 0) {
        return;
      }
    
      int chk = 0;
      if (count == 1) {
        chk = 1;
      } else if (count == 2) {
        chk = 0;
      }
      const data_geometry* A = 0;
      const data_geometry* B = 0;
      const data_geometry* C = 0;
    
      for (int i = 0; i < 3; i++) {
        if (vec1[i] == chk) {
          A = in[i];
          B = in[(i + 1) % 3];
          C = in[(i + 2) % 3];
          break;
        }
      }
    
      //Calculate barycentric values to calculate AB and AC
      float wA = A->gl_Position[3], wB = B->gl_Position[3], wC = C->gl_Position[3];
      float aV = A->gl_Position[index], bV = B->gl_Position[index], cV = C->gl_Position[index];
      float alphaB = ((val * wB) - bV) / ((aV - (val * wA)) + ((val * wB) - bV));
      float alphaC = ((val * wC) - cV) / ((aV - (val * wA)) + ((val * wC) - cV));
    
      data_geometry* AB = new data_geometry();
      data_geometry* AC = new data_geometry();
      AB->data = new float[state.floats_per_vertex];
      AC->data = new float[state.floats_per_vertex];
      for (int i = 0; i < state.floats_per_vertex; i++) {
        if (state.interp_rules[i] == interp_type::flat) {
          AB->data[i] = A->data[i];
          AC->data[i] = A->data[i];
        } else if (state.interp_rules[i] == interp_type::smooth) {
          AB->data[i] = (alphaB * A->data[i]) + ((1 - alphaB) * B->data[i]);
          AC->data[i] = (alphaC * A->data[i]) + ((1 - alphaC) * C->data[i]);
        } else if (state.interp_rules[i] == interp_type::noperspective) {
          float k = (alphaB * A->gl_Position[3]) + ((1 - alphaB) * B->gl_Position[3]);
          float alpha = (alphaB * A->gl_Position[3]) / k;
          AB->data[i] = (alpha * A->data[i]) + ((1 - alpha) * B->data[i]);
          k = (alphaC * A->gl_Position[3]) + ((1 - alphaC) * C->gl_Position[3]);
          alpha = (alphaC * A->gl_Position[3]) / k;
          AC->data[i] = (alpha * A->data[i]) + ((1 - alpha) * C->data[i]);
        }
      }
      for (int i = 0; i < 4; i++) {
        AB->gl_Position[i] = (alphaB * A->gl_Position[i]) + ((1 - alphaB) * B->gl_Position[i]);
        AC->gl_Position[i] = (alphaC * A->gl_Position[i]) + ((1 - alphaC) * C->gl_Position[i]);
      }
    
      const data_geometry* pass[3];
      if (count == 1) {	
        pass[0] = A;
        pass[1] = AB;
        pass[2] = AC;
        clip_triangle(state,pass,face+1);
        delete AB;
        delete AC;
        return;
      } else if (count == 2) {
        pass[0] = AB;
        pass[1] = B;
        pass[2] = C;
        clip_triangle(state,pass,face+1);
        pass[0] = C;
        pass[1] = AC;
        pass[2] = AB;
        clip_triangle(state,pass,face+1);
        delete AB;
        delete AC;
        return;
      }
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
        
        x[i] = (halfw * ((in[i]->gl_Position[0]) / (in[i]->gl_Position[3]) + 1)) - 0.5;
        y[i] = (halfh * ((in[i]->gl_Position[1]) / (in[i]->gl_Position[3]) + 1)) - 0.5;
        z[i] = (in[i]->gl_Position[2]) / (in[i]->gl_Position[3]);
    }
    
    float area = ( (x[1] * y[2] - x[2] * y[1]) -
                   (x[0] * y[2] - x[2] * y[0]) +
                   (x[0] * y[1] - x[1] * y[0]) );  

    float x_min = std::min(std::min(x[0],x[1]),x[2]);
    float x_max = std::max(std::max(x[0],x[1]),x[2]);
    float y_min = std::min(std::min(y[0],y[1]),y[2]);
    float y_max = std::max(std::max(y[0],y[1]),y[2]);
 
    for (int i = x_min; i < x_max; i++) {
        for (int j = y_min; j < y_max; j++) {
      	    float Brea = ((x[0] * (j - y[2])) + (i * (y[2] - y[0])) + (x[2] * (y[0] - j)));
        	  float Grea = ((x[0] * (y[1] - j)) + (x[1] * (j - y[0])) + (i * (y[0] - y[1])));
            double beta = Brea / area;
            double gamma = Grea / area;
            double alpha = (1 - beta) - gamma;
            if (alpha >= 0 && beta >= 0 && gamma >= 0) {
                int img_index = (j * state.image_width) + i;
                float interpolation_stuff[state.floats_per_vertex];
                for (int k = 0; k < state.floats_per_vertex; k++) {
                if (state.interp_rules[k] == interp_type::flat) {
                     interpolation_stuff[k] = in[0]->data[k];
                } else if (state.interp_rules[k] == interp_type::smooth) {
                     float aP = 1.0 / in[0]->gl_Position[3], bP = 1.0 / in[1]->gl_Position[3], gP = 1.0 / in[2]->gl_Position[3];
                     aP = alpha * aP;
                     bP = beta * bP;
                     gP = gamma * gP;
                     interpolation_stuff[k] = ((aP * in[0]->data[k]) + (bP * in[1]->data[k]) + (gP * in[2]->data[k])) / (aP + bP + gP);
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
