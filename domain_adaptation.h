#ifndef DOMAIN_ADAPTATION_H
#define DOMAIN_ADAPTATION_H

#include "dynamicarray.h"
#include "spectrogram.h"
#include "dictionary.h"

DynamicArray CalculateTemplateWeights(Spectrogram* spectrogram, int num_iterations);

Dictionary AdaptDictionary(Dictionary* dictionaries, DynamicArray* weights);

Dictionary GetBestDictionaryForRecording(const char * filename);

Dictionary GetBestDictionaryForArray(DynamicArray* array, Dictionary* dictionaries, int num_dictionaries, Spectrogram spec);


void DomainAdaptationTest();





#endif // DOMAIN_ADAPTATION_H
