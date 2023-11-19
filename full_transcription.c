#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dynamicarray.h"
#include "stft.h"
#include "math.h"
#include "evaluation.h"


#include "wav.h"
#include "activations.h"
#include "activation_postprocessing.h"

#include "full_transcription.h"

#include <time.h>

#include "omp.h"

#include "domain_adaptation.h"


void test() {


    printf("Max threads: %d\n", omp_get_max_threads());

    #pragma omp parallel
    {

        printf("%d : ", omp_get_num_threads());

        printf("%d\n", omp_get_thread_num());

    }
}

Matrix transcribe(Spectrogram* songSpectrogram, Dictionary *dictionary, int transcriptionIterations)
{
    clock_t start_time, end_time;

    start_time = clock();

    Matrix activations = ComputeActivations(songSpectrogram, transcriptionIterations,  dictionary, "gauss");

    end_time = clock();

    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Elapsed time for activations: %.2f seconds\n", elapsed_time);

    double threshold = GetThreshold(&activations);

    Matrix notes = TranscribeNotesFromActivations(&activations, threshold, 0.02);

    DestroyMatrix(&activations);

    int finalIndex = 0;

    while (true) {
        if (notes.array[finalIndex *notes.cols +1] == 0) {
            break;
        }
        finalIndex++;
    }

    Matrix finalNotes = CreateMatrix(finalIndex, 2);

    double est_min = 99;

    for (int i = 0; i < finalIndex; i++) {
        if (notes.array[i *notes.cols +0] < est_min) {
            est_min = notes.array[i *notes.cols +0];
        }

        finalNotes.array[i *finalNotes.cols +0] = notes.array[i *notes.cols +0];
        finalNotes.array[i *finalNotes.cols +1] = notes.array[i *notes.cols + 1];
    }

    DestroyMatrix(&notes);


    for (int i = 0; i < finalIndex; i++) {
        finalNotes.array[i *finalNotes.cols +0] -= est_min;
    }

//    SaveMatrixToCSV("finalnotes.csv", &finalNotes);

    return finalNotes;
}

Matrix transcribe_array(DynamicArray *array, const char * transcriptionFile, int transcriptionIterations, Dictionary* dictionaries, int num_dictionaries)
{

    double time_limit = 5;
//    double time_limit = array->size/44100;


    printf("Doing STFT\n");
    fflush(stdout);

    Spectrogram spec = STFT(array, 4096, 882, 8192, time_limit, 44100);

    printf("Filtering STFT\n");
    fflush(stdout);


    Spectrogram filtered = HardFilterSpectrogram(&spec, 1500);


//    SaveSpectrogramToCSV("spectrogram.csv", &filtered);


    DynamicArray arrayCopy = CreateDynamicArray(array->size);
    CopyDynamicArray(&arrayCopy, array);

    printf("Doing dictionary adjustment\n");
    fflush(stdout);

    Dictionary dictionary = GetBestDictionaryForArray(&arrayCopy, dictionaries, num_dictionaries, filtered);

    DestroySpectrogram(&spec);

    DestroyDynamicArray(&arrayCopy);

    dictionary = HardFilterSpectrograms(&dictionary, 1500);
    NormaliseDictionary(&dictionary);

    double delay = GetDelay(&filtered, 0.2);

    printf("delay: %f\n", delay);

    printf("Transcribing\n");
    fflush(stdout);


    Matrix estimated_notes = transcribe(&filtered, &dictionary, transcriptionIterations);

    DestroySpectrogram(&filtered);
    DestroyDictionary(&dictionary);

    if(strcmp(transcriptionFile, "") != 0)
    {
        Matrix reference_notes = LoadRefsFromFile(transcriptionFile, time_limit - delay);

        EvaluateTranscription(&reference_notes, &estimated_notes);

        DestroyMatrix(&reference_notes);
    }

    return estimated_notes;
}

Matrix transcribe_wav(const char *wavFile, const char *transcriptionFile, int transcriptionIterations, Dictionary* dictionaries, int num_dictionaries)
{

    printf("Reading wave\n");
    fflush(stdout);

    WavFile wav = ReadWav(wavFile);

    printf("Converting to mono\n");
    fflush(stdout);

    DynamicArray mono = StereoToMono(&wav, "average");
//    NormaliseChannel(&mono);

//    SaveArrayToCSV("wavfile.csv", &mono);


    Matrix estimated_notes = transcribe_array(&mono, transcriptionFile, transcriptionIterations, dictionaries, num_dictionaries);

    DestroyDynamicArray(&mono);

    return estimated_notes;
}
