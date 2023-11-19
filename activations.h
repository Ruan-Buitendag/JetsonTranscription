//
// Created by ruanb on 9/10/2023.
//

#ifndef C_PIANO_TRANSCRIPTION_ACTIVATIONS_H
#define C_PIANO_TRANSCRIPTION_ACTIVATIONS_H

#include "spectrogram.h"
#include "dictionary.h"
#include "matrix.h"
#include "dynamicarray.h"
#include "stdbool.h"

double BetaDivergence(Matrix const *x, Matrix const *y, double beta);

Matrix ComputeConvolution(Dictionary const *dictionary, Matrix const *matrix2, int t);

Matrix
ComputeActivations(Spectrogram const *input, int iterations, const Dictionary *dictionary, const char * init);

Matrix GetActivationsFromFile(const char *filename, Dictionary *dictionary, double time_limit, int iterations);
Matrix GetActivationsFromArray( DynamicArray * array, Dictionary *dictionary, double time_limit, int iterations);

Matrix GetActivations(const char *filename, Dictionary* dictionary, double time_limit, int iterations, bool recompute);


//Matrix Activations(Spectrogram const *input, Dictionary *dictionary, unsigned int iterations);

void TestActivations();

#endif //C_PIANO_TRANSCRIPTION_ACTIVATIONS_H
