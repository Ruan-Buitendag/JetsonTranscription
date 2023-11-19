//
// Created by ruanb on 9/4/2023.
//

#ifndef C_PIANO_TRANSCRIPTION_DICTIONARY_H
#define C_PIANO_TRANSCRIPTION_DICTIONARY_H

#include <stdio.h>
#include <stdlib.h>
//#include "hdf5.h"
#include "spectrogram.h"

typedef struct DictionaryStruct {
    int shape[3];
    double*** data;
} Dictionary;

Dictionary CreateDictionary(int shape[3]);

void LoadDictionary(Dictionary *dictionary, char *filename);
void AllocateDictionaryMemory(Dictionary* dictionary);
void PrintDictionary(Dictionary* dictionary);

Dictionary HardFilterSpectrograms(Dictionary* dictionary, unsigned int numNewRows);
void NormaliseDictionary(Dictionary* dictionary);

Spectrogram GetSpectrogramFromDictionary(Dictionary const * dictionary, unsigned int axis, unsigned int index);
Matrix GetMatrixFromDictionary(Dictionary const *dictionary, unsigned int axis, unsigned int index);

Dictionary GetDictionary(const char * piano_name);
Dictionary LoadTranscriptionDictionary();

void DestroyDictionary(Dictionary *dictionary);

void dictionaryTest();


#endif //C_PIANO_TRANSCRIPTION_DICTIONARY_H
