//
// Created by ruanb on 9/4/2023.
//

#include "dictionary.h"


void LoadDictionary(Dictionary *dictionary, char *filename) {
//    printf("in LoadDictionary\n");
//    fflush(stdout);

//    int dims[3];

//    FILE *file = fopen(filename, "r");

//    if (file == NULL) {
//        perror("Error opening file");

//        printf(filename);
//        fflush(stdout);

//        exit(1);
//    }


//    double value;
//    int shape;


//    for(int i = 0; i < 3; i++){
//        fscanf(file, "%d", &shape);
//        int ss = shape;
//        dictionary->shape[i] = ss;
//            printf("dict shape: %d\n", ss);
//    }

//    AllocateDictionaryMemory(dictionary);

//    for(int i = 0; i < dictionary->shape[0]; i++){
//        for(int j = 0; j < dictionary->shape[1]; j++){
//            for(int k = 0; k < dictionary->shape[2]; k++){
//                fscanf(file, "%lf", &value);
//                dictionary->data[i][j][k] = value;
//            }
//        }
//    }

//    fclose(file);


    // Open the binary file for reading
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    int shape[3];

    fread(shape, sizeof(int), 3, file);

//    int num_floats = shape[0] * shape[1] * shape[2];

//    Dictionary aaaaaaaa;

    dictionary->shape[0] = shape[0];
    dictionary->shape[1] = shape[1];
    dictionary->shape[2] = shape[2];

    AllocateDictionaryMemory(dictionary);

    printf("Allocated sictionary of size %d %d %d \n", shape[0], shape[1], shape[2]);
    fflush(stdout);

    for(int a = 0; a < shape[0]; a++){
        for(int b = 0; b < shape[1]; b++){
            fread(dictionary->data[a][b], sizeof(double), shape[2], file);

//            for(int c = 0; c < shape[2]; c++){
//                fread(&aaaaaaaa.data[a][b][c], sizeof(double), 1, file);
//            }
        }
    }
    fclose(file);


//    Spectrogram aggggggggg = GetSpectrogramFromDictionary(&aaaaaaaa, 2, 0);
//    SaveMatrixToCSV("ssssssssssssssssssssssssddddddddddd.csv", &aggggggggg.matrix);
//    DestroySpectrogram(&aggggggggg);


    printf("loaded dictionary\n");
    fflush(stdout);
}



void AllocateDictionaryMemory(Dictionary *dictionary) {
    dictionary->data = (double ***) malloc(dictionary->shape[0] * sizeof(double **));
    for (int i = 0; i < dictionary->shape[0]; i++) {
        dictionary->data[i] = (double **) malloc(dictionary->shape[1] * sizeof(double *));
        for (int j = 0; j < dictionary->shape[1]; j++) {
            dictionary->data[i][j] = (double *) malloc(dictionary->shape[2] * sizeof(double));
        }
    }
}

void PrintDictionary(Dictionary *dictionary) {
    for (int i = 0; i < dictionary->shape[0]; i++) {
        for (int j = 0; j < dictionary->shape[1]; j++) {
            for (int k = 0; k < dictionary->shape[2]; k++) {
                printf("%f ", dictionary->data[i][j][k]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

void dictionaryTest() {
//    const char *dictionary_directory = "C:\\Users\\ruanb\\OneDrive\\Desktop\\Piano Transcripton\\Piano Transcription (C)\\data_persisted\\dictionaries\\";
//    const char *piano_W[3] = {"AkPnBsdf", "AkPnStgb", "AkPnBcht"};

//    Dictionary dictionaries[3];

//    // Iterate through piano_W
//    for (int i = 0; i < 3; i++) {
//        char W_persisted_name[256]; // Adjust the size as needed

        // Create the formatted string
//        snprintf(W_persisted_name, sizeof(W_persisted_name),
//                 "%sconv_dict_piano_%s.h5",
//                 dictionary_directory,
//                 piano_W[i]);

//        LoadDictionary(&dictionaries[i], W_persisted_name);
//    }

//    Spectrogram aa = GetSpectrogramFromDictionary(&dictionaries[0], 0);


//    SaveSpectrogramToCSV("spectest.csv", &aa);

}

Dictionary HardFilterSpectrograms(Dictionary *dictionary, unsigned int numNewRows) {
    Dictionary filtered;

    filtered.shape[0] = dictionary->shape[0];
    filtered.shape[1] = numNewRows;
    filtered.shape[2] = dictionary->shape[2];

    AllocateDictionaryMemory(&filtered);

    for (int i = 0; i < filtered.shape[0]; i++) {
        for (int j = 0; j < filtered.shape[1]; j++) {
            for (int k = 0; k < filtered.shape[2]; k++) {
                filtered.data[i][j][k] = dictionary->data[i][j][k];
            }
        }
    }

    return filtered;
}

Spectrogram GetSpectrogramFromDictionary(Dictionary const *dictionary, unsigned int axis, unsigned int index) {
    Spectrogram noteSpectrogram;

    if (axis == 0) {
        noteSpectrogram = CreateSpectrogram(dictionary->shape[1], dictionary->shape[2]);

        for (int r = 0; r < noteSpectrogram.matrix.rows; r++) {
            for (int c = 0; c < noteSpectrogram.matrix.cols; c++) {
                noteSpectrogram.matrix.array[r *noteSpectrogram.matrix.cols + c] = dictionary->data[index][r][c];
            }
        }
    } else if (axis == 1) {
//        Spectrogram noteSpectrogram = CreateSpectrogram(dictionary->shape[1], dictionary->shape[0]);
        fprintf(stderr, "GetSpectrogramFromDictionary: axis == 1 not implemented yet\n");
        exit(1);
    } else if (axis == 2) {
        noteSpectrogram = CreateSpectrogram(dictionary->shape[1], dictionary->shape[0]);

        for (int r = 0; r < noteSpectrogram.matrix.rows; r++) {
            for (int c = 0; c < noteSpectrogram.matrix.cols; c++) {
                noteSpectrogram.matrix.array[r *noteSpectrogram.matrix.cols +  c] = dictionary->data[c][r][index];
            }
        }
    } else {
        fprintf(stderr, "GetSpectrogramFromDictionary: axis must be 0, 1 or 2\n");
        exit(1);
    }


    return noteSpectrogram;
}


Matrix GetMatrixFromDictionary(const Dictionary *dictionary, unsigned int axis, unsigned int index) {
    Matrix slice;

    if (axis == 0) {
        slice = CreateMatrix(dictionary->shape[1], dictionary->shape[2]);

        for (int r = 0; r < slice.rows; r++) {
            for (int c = 0; c < slice.cols; c++) {
                slice.array[r *slice.cols +  c] = dictionary->data[index][r][c];
            }
        }
    } else if (axis == 1) {
//        Spectrogram noteSpectrogram = CreateSpectrogram(dictionary->shape[1], dictionary->shape[0]);
        fprintf(stderr, "GetSpectrogramFromDictionary: axis == 1 not implemented yet\n");
        exit(1);
    } else if (axis == 2) {
        slice = CreateMatrix(dictionary->shape[1], dictionary->shape[0]);

        for (int r = 0; r < slice.rows; r++) {
            for (int c = 0; c < slice.cols; c++) {
                slice.array[r *slice.cols +   c] = dictionary->data[c][r][index];
            }
        }
    } else {
        fprintf(stderr, "GetSpectrogramFromDictionary: axis must be 0, 1 or 2\n");
        exit(1);
    }


    return slice;
}

void NormaliseDictionary(Dictionary *dictionary) {
    double max = 0;

    for (int i = 0; i < dictionary->shape[0]; i++) {
        for (int j = 0; j < dictionary->shape[1]; j++) {
            for (int k = 0; k < dictionary->shape[2]; k++) {
                if (dictionary->data[i][j][k] > max) {
                    max = dictionary->data[i][j][k];
                }
            }
        }
    }

    for (int i = 0; i < dictionary->shape[0]; i++) {
        for (int j = 0; j < dictionary->shape[1]; j++) {
            for (int k = 0; k < dictionary->shape[2]; k++) {
                dictionary->data[i][j][k] /= max;
            }
        }
    }
}

Dictionary GetDictionary(const char *piano_name) {

    Dictionary dictionary;

    // Iterate through piano_W
    char W_persisted_name[256]; // Adjust the size as needed

//    const char *file_path = "C:/Users/ruanb/OneDrive/Desktop/Piano Transcripton/Piano Transcription (C)/data_persisted/dictionaries/bin/conv_dict_piano_AkPnBcht.bin";



    snprintf(W_persisted_name, sizeof(W_persisted_name),
             "%sconv_dict_piano_%s.bin",
             "/media/ruan/KINGSTON/data_persisted/dictionaries/bin/",
             piano_name);

    LoadDictionary(&dictionary, W_persisted_name);

    return dictionary;
}


Dictionary LoadTranscriptionDictionary()
{
    Dictionary dict = GetDictionary("AkPnBcht");

    Dictionary filtered = HardFilterSpectrograms(&dict, 1500);

    DestroyDictionary(&dict);

    NormaliseDictionary(&filtered);

    return filtered;
}

void DestroyDictionary(Dictionary *dictionary)
{
    for(int i = 0; i < dictionary->shape[0]; i++)
    {
        for(int j = 0; j < dictionary->shape[1]; j++)
        {
            free(dictionary->data[i][j]);
        }
        free(dictionary->data[i]);
    }
}

