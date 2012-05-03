/* 
 *  Copyright (c) 2008-2010  Noah Snavely (snavely (at) cs.cornell.edu)
 *    and the University of Washington
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

/* KeyMatchFull.cpp */
/* Read in keys, match, write results to a file */

#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>

#include "keys2a.h"

#ifdef _OPENMP
#include <omp.h>
#endif

#include <map>
#include <vector>

struct PairIdx
{
    PairIdx(int im1, int im2)
    {
	i1 = im1;
	i2 = im2;
    }
    int i1,i2;

    bool operator<(const PairIdx & other) const
    {
	if (i2 < other.i2)
	    return true;
	if ( i2 == other.i2 && i1 < other.i1)
	    return true;
	return false;
    }
};

typedef std::map<PairIdx, std::vector<KeypointMatch> > KeyMapType;

int main(int argc, char **argv) {
    char *list_in;
    char *file_out;
    double ratio;
    int nprev = 0;
    char * prev_matches_file;
    int n_prev_matches = 0;
    std::vector<std::vector<int> > prev_matches;
    
    if (!(argc == 3 || argc == 4)) {
	printf("Usage: %s <list.txt> <outfile> [tracklen]\n", argv[0]);
	return -1;
    }
    
    list_in = argv[1];
    ratio = 0.6;
    file_out = argv[2];

    if (argc == 4) {
	errno = 0;
	nprev = strtol(argv[3],NULL,10);
	if (errno == 0) {
	    nprev = -1;
	    prev_matches_file = argv[3];
	}
    }

    
    if (nprev == -1) {
	// load previous matching results...
	FILE * f = fopen(prev_matches_file,"r");
	if (f == NULL) {
	    printf("Error opening file %s for reading\n", prev_matches_file);
	    return 1;
	}
	fscanf(f,"%d",&n_prev_matches);
	prev_matches.resize(n_prev_matches);
	for (int i=0; i <  n_prev_matches ; ++i) {
	    std::vector<int> imgs;
	    for (int j=0; j <  n_prev_matches; ++j) {
		int img=0;
		fscanf(f,"%d", &img );
		if (img>0) {
		    prev_matches[j].push_back(i);
		}
	    }
	}
	printf("Loaded previous match matrix from %s, %d images\n", prev_matches_file, n_prev_matches);
    }

    clock_t start = clock();

    unsigned char **keys;
    int *num_keys;

    /* Read the list of files */
    std::vector<std::string> key_files;
    
    FILE *f = fopen(list_in, "r");
    if (f == NULL) {
        printf("Error opening file %s for reading\n", list_in);
        return 1;
    }

    char buf[512];
    while (fgets(buf, 512, f)) {
        /* Remove trailing newline */
        if (buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = 0;
        
        key_files.push_back(std::string(buf));
    }

    fclose(f);

    f = fopen(file_out, "w");
    assert(f != NULL);

    int num_images = (int) key_files.size();

    keys = new unsigned char *[num_images];
    num_keys = new int[num_images];

    /* Read all keys */
#ifdef _OPENMP
    #pragma omp parallel for
#endif
    for (int i = 0; i < num_images; i++) {
        keys[i] = NULL;
        num_keys[i] = ReadKeyFile(key_files[i].c_str(), keys+i);
    }

    clock_t end = clock();    
    printf("[KeyMatchFull] Reading keys took %0.3fs\n", 
           (end - start) / ((double) CLOCKS_PER_SEC));
    

    KeyMapType all_matches;
#ifdef _OPENMP
    #pragma omp parallel for schedule(dynamic, 1)
#endif
    for (int i = 0; i < num_images; i++) {
        if (num_keys[i] == 0)
            continue;

        //printf("[KeyMatchFull] Matching to image %d:", i);
	//fflush(stdout);

        start = clock();

        /* Create a tree from the keys */
        ANNkd_tree *tree = CreateSearchTree(num_keys[i], keys[i]);

	int jstart = 0;
	if (nprev > 0) {
	    jstart = i < nprev ? 0 : i - nprev;
	}

	if (nprev == -1) {
	    int kmax = prev_matches[i].size();
	    for (int k = 0; k < kmax; k++) {
		int j = prev_matches[i][k];
		if (num_keys[j] == 0)
		    continue;
		
		std::vector<KeypointMatch> matches = 
		    MatchKeys(num_keys[j], keys[j], tree, ratio);
            
		int num_matches = (int) matches.size();
		if (num_matches >= 16) {
#ifdef _OPENMP
                    #pragma omp critical
#endif
		    {
			all_matches.insert(std::make_pair(PairIdx(j,i), matches));
			/*
			printf(" %d", j);
			fflush(stdout);
			*/
		    }
		}
	    }
	} else {
            for (int j = jstart; j < i; j++) {
                if (num_keys[j] == 0)
                    continue;
    
                /* Compute likely matches between two sets of keypoints */
                std::vector<KeypointMatch> matches = 
                    MatchKeys(num_keys[j], keys[j], tree, ratio);
                
                int num_matches = (int) matches.size();
                if (num_matches >= 16) {
#ifdef _OPENMP
                    #pragma omp critical
#endif
    		    {
    		        all_matches.insert(std::make_pair(PairIdx(j,i), matches));
			/*
    		        printf(" %d", j);
    		        fflush(stdout);
			*/
    		    }
		    //	    printf("thread %d: img %d finished\n", id, j);
		}
	    }
	}
    	printf("\n");

        end = clock();    
        printf("[KeyMatchFull] Matching image %d took %0.3fs\n", 
               i, (end - start) / ((double) CLOCKS_PER_SEC));
        fflush(stdout);

        // annDeallocPts(tree->pts);
        delete tree;
    }

    /* Write keypoints
    for (int i = 0; i < num_images; i++) {
        for (int j = 0; j < i; j++) {
	    all_matches.find(PairIdx(j,i)
*/

    for (KeyMapType::const_iterator it = all_matches.begin(); it != all_matches.end(); ++it) {
	/* Write the pair */
	int j = it->first.i1;
	int i = it->first.i2;

	fprintf(f, "%d %d\n", j, i);

	int num_matches = (int) it->second.size();

	/* Write the number of matches */
	fprintf(f, "%d\n", num_matches);

	for (int i = 0; i < num_matches; i++) {
	    fprintf(f, "%d %d\n", 
		    (it->second)[i].m_idx1, (it->second)[i].m_idx2);
	}
    }
    
    /* Free keypoints */
    for (int i = 0; i < num_images; i++) {
        if (keys[i] != NULL)
            delete [] keys[i];
    }
    delete [] keys;
    delete [] num_keys;
    
    fclose(f);
    return 0;
}
