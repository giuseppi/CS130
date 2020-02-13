#include <algorithm>
#include "hierarchy.h"

// Reorder the entries vector so that adjacent entries tend to be nearby.
// You may want to implement box.cpp first.
void Hierarchy::Reorder_Entries()
{
    if(!entries.size()) return;
    
    double min_dist = 99999999999.0;
    int index = 0;
    for (size_t i = 0; i < entries.size(); i++) {
    	vec3 p = entries[i].box.hi - entries[i].box.lo;
    	double dist = p.magnitude();
    	if (dist < min_dist) {
    		min_dist = dist;
    		index = i;
    	}
    }

    Entry temp = entries[0];
    entries[0] = entries[index];
    entries[index] = temp;

    for (size_t i = 0; i < entries.size(); i++) {
	    for (size_t j = i + 1; j < entries.size(); j++) {
	    	vec3 p = (entries[i].box.hi - entries[i].box.lo) - (entries[j].box.hi - entries[j].box.lo);
	    	double dist = p.magnitude();
	    	if (dist < min_dist) {
	    		min_dist = dist;
	    		index = i;
	    	}
	    }
	    Entry temp = entries[i];
	    entries[i] = entries[index];
	    entries[index] = temp;
	    min_dist = 99999999999.0;
    	    index = 0;
    }  	 
}

// Populate tree from entries.
void Hierarchy::Build_Tree()
{
    if(!entries.size()) return;
    
    int parents = 1;
    while (parents < (int)entries.size()) {
    	parents *= 2;
    }

    int p = ((2 * parents) - 1) - parents;
    
    if (p == 0) {
        tree.push_back(entries[0].box);
        return;
    }

    for (int i = 0; i < p; i++) {
  	Box b;
    	b.lo = {-99999, -99999, -99999};
    	b.hi = {-99999, -99999, -99999};
    	tree.push_back(b);
    }

    for (unsigned i = 0; i < entries.size(); i++) {
         tree.push_back(entries[i].box);
    }
   
    for (int i = ((2 * parents) - 2); i > 1; i = i - 2) {
        int par = (i / 2) - 1;
        if (i >= (int)tree.size()) {
            if (i == (int)tree.size()) {
                tree[par] = tree[i-1];
            }
        }
	else {
		if (tree[i].hi[0] != -99999) {
                	if (tree[i-1].hi[0] != -99999) {
                    		tree[par] = tree[i].Union(tree[i-1]);
                	}
			else {
                    		tree[par] = tree[i];
                	}
            	}
		else {
                	tree[par] = tree[i-1];
            }
        }
    } 
}

// Return a list of candidates (indices into the entries list) whose
// bounding boxes intersect the ray.
void Hierarchy::Intersection_Candidates(const Ray& ray, std::vector<int>& candidates) const
{
    int i = 0;
    std::vector<int> v;
    v.push_back(0);
    int cutoff = ((int)(tree.size() - entries.size()));
    
    while(i < cutoff) {
	int sz = (int)v.size();
    	for (int j = 0; j < sz; j++) {
    		if (tree[v[0]].Intersection(ray)) {
    			v.push_back((2*v[0]) + 1);
			v.push_back((2*v[0]) + 2);
   		}
        	v.erase(v.begin());
   	}

	if (v.size() == 0) {
        	candidates.clear();
        	return;
	}

	i = v[0];
    }

    if (v.size() == 0) {
        candidates.clear();
        return;
    }

    for (unsigned i = 0; i < v.size(); ++i) {
        v[i] = v[i] - cutoff;
    }

    candidates = v;
    return;
}
