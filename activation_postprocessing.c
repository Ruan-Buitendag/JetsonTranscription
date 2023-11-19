//
// Created by ruanb on 9/30/2023.
//

#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include "activation_postprocessing.h"



// Define matrix dimensions
#define ROWS_X 30
#define COLS_X 2

#define ROWS_Y 30

double xxx[ROWS_X][COLS_X] = {{0.0376389803991012, 0.001892375792694},
                            {0.0503753723508801, 0.0044388102653744},
                            {0.0806076449274108, 0.0118402074336559},
                            {0.0382854763034231, 0.0025461702682659},
                            {0.0453409629836223, 0.0025051921416984},
                            {0.0343576250549369, 0.0020110814114229},
                            {0.0433400978145086, 0.0024278940683001},
                            {0.0338838773282324, 0.0014778719653621},
                            {0.0433399657284752, 0.0024279066064473},
                            {0.0377977718297317, 0.0018283984289626},
                            {0.0400056714666979, 0.0030185134068109},
                            {0.0338464242755685, 0.0013261903264773},
                            {0.0774441395725354, 0.008190018452805},
                            {0.0420373194273847, 0.0020212836411307},
                            {0.0402859799122641, 0.002614098309641},
                            {0.0373769077765476, 0.0015498773302625},
                            {0.0557853045601628, 0.0041156320317165},
                            {0.045407046425397, 0.0024455903456566},
                            {0.0531938730048406, 0.003055987499361},
                            {0.0448582980723782, 0.0027415420724721},
                            {0.0459270934904357, 0.0031610461843619},
                            {0.0427589734469269, 0.0026777119098417},
                            {0.0590067354511753, 0.0057217458513823},
                            {0.0458622460031415, 0.0038687383107522},
                            {0.0409631236155547, 0.0025968042301171},
                            {0.0540293575884004, 0.0040789425706537},
                            {0.040315663009496, 0.0027336250992709},
                            {0.0394331149151944, 0.0022520577270207},
                            {0.044986950634853, 0.0033416143622964},
                            {0.0376107657942642, 0.0022163057014253}};


double yyy[ROWS_Y] = {0.04000,
    0.10000,
    0.14000,
    0.07000,
    0.04000,
    0.05000,
    0.04000,
    0.03000,
    0.03000,
    0.05000,
    0.04000,
    0.03000,
    0.08000,
    0.04000,
    0.01000,
    0.03000,
    0.08000,
    0.04000,
    0.05000,
    0.04000,
    0.04000,
    0.07000,
    0.08000,
    0.06000,
    0.31000,
    0.04000,
    0.06000,
    0.04000,
    0.07000,
    0.01000
};



Matrix kernel(Matrix x, Matrix y, double l2){
    Matrix result = CreateMatrix(x.rows, y.rows);

    for (int i = 0; i < x.rows; i++) {
        for (int j = 0; j < y.rows; j++) {
            result.array[i * result.cols + j] = 0;
            for (int k = 0; k < y.cols; k++) {
                result.array[i * result.cols + j] += x.array[i * x.cols + k] * x.array[i * x.cols + k] + y.array[j *y.cols + k] * y.array[j * y.cols + k] - 2 * x.array[i * x.cols + k] * y.array[j * y.cols + k];
            }
            result.array[i * result.cols + j] = exp(-0.5 * (1 / l2) * result.array[i * result.cols + j]);
        }
    }

    return result;
}

Matrix cholesky_decomposition(Matrix A) {
    Matrix L = CreateMatrix(A.rows, A.cols);

    for (int i = 0; i < A.rows; i++) {
        for (int j = 0; j <= i; j++) {
            if (i == j) {
                L.array[i *L.cols+ j] = sqrt(A.array[i *A.cols + i] - 0);
            } else {
                L.array[i *L.cols + j] = (A.array[i *A.cols+ j] - 0) / L.array[j *L.cols + j];
            }
        }
    }

    return L;
}

Matrix solve_lower_triangular(Matrix L, Matrix b) {
    Matrix x = CreateMatrix(ROWS_X, 1);

    for (int i = 0; i < ROWS_X; i++) {
        x.array[i *x.cols+ 0] = b.array[i *b.cols+ 0];
        for (int j = 0; j < i; j++) {
            x.array[i *x.cols+ 0] -= L.array[i *L.cols + j] * x.array[j *x.cols + 0];
        }
        x.array[i *x.cols + 0] /= L.array[i *L.cols + i];
    }

    return x;
}

double GPR(double Xtest[2], double l2, double noise_var) {
    Matrix X = CreateMatrix(ROWS_X, COLS_X);
    Matrix y = CreateMatrix(ROWS_Y, 1);

    for (int i = 0; i < ROWS_X; i++) {
        for (int j = 0; j < COLS_X; j++) {
            X.array[i *X.cols + j] = xxx[i][j];
        }
    }

    for (int i = 0; i < ROWS_Y; i++) {
        y.array[i *y.cols + 0] = yyy[i];
    }


    Matrix K = kernel(X, X, l2);

    for (int i = 0; i < ROWS_X; i++) {
        for (int j = 0; j < ROWS_X; j++) {
            K.array[i *K.cols + j] += noise_var * (i == j ? 1 : 0);
        }
    }

    Matrix xtest = CreateMatrix(1, COLS_X);

    for (int i = 0; i < COLS_X; i++) {
        xtest.array[0 *xtest.cols+ i] = Xtest[i];
    }

    Matrix L = cholesky_decomposition(K);

    Matrix balls = kernel(X, xtest, l2);

    Matrix Lk = solve_lower_triangular(L, balls);

    double mu = 0;


    for (int j = 0; j < ROWS_X; j++) {
        mu += Lk.array[j *Lk.cols+ 0] * y.array[j *y.cols+ 0];
    }

    DestroyMatrix(&X);
    DestroyMatrix(&y);
    DestroyMatrix(&K);
    DestroyMatrix(&xtest);
    DestroyMatrix(&L);
    DestroyMatrix(&balls);
    DestroyMatrix(&Lk);

    return mu;


}

Matrix TranscribeNotesFromActivations(const Matrix *activations, double threshold, double time_step) {
//    Matrix notes = CreateMatrix(500, 3);
    Matrix notes = CreateMatrix(1000, 2);

    int note_count = 0;

    int midi_codebook[88];

    for (int i = 0; i < 88; i++) {
        midi_codebook[i] = i + 21;
//        midi_codebook[i] = i + 21 + 11;
    }

    bool note_presence = false;

//    SaveMatrixToCSV("unsmoothed_activation.csv", activations);

    double current_pitch = 0;
    double current_onset = 0;
    int sliding_window_width = 10;

//    TODO: midi file creation

    Matrix smoothed_activations = CreateMatrix(activations->rows, activations->cols);

    for (int i = 0; i < activations->cols; i++) {
        if (i - sliding_window_width < 0 || i + sliding_window_width + 1 > activations->cols) {
            int d = floor(fmin(i, activations->cols - i));

            for (int j = 0; j < activations->rows; j++) {
                double sum = 0;
                int count = 0;
                for (int l = 0; l < activations->rows; l++) {
                    for (int k = i - d; k < i + d + 1; k++) {
                        if ((k >= 0 && k < activations->cols) && (l >= 0 && l < activations->rows)) {
                            sum += activations->array[l * activations->cols +k];
                            count++;
                        }
                    }
                }
                smoothed_activations.array[j *smoothed_activations.cols + i] = sum / count;
            }
        } else {
            for (int j = 0; j < activations->rows; j++) {
                double sum = 0;
                int count = 0;
                for (int k = i - sliding_window_width; k < i + sliding_window_width + 1; k++) {
                    if (k >= 0 && k < activations->cols) {
                        sum += activations->array[j *activations->cols+  k];
                        count++;
                    }
                }

                smoothed_activations.array[j *smoothed_activations.cols+  i] = sum / count;
            }
        }
    }

    for (int note_index = 0; note_index < smoothed_activations.rows; note_index++) {
        if (note_presence)
            note_presence = false;

        for (int time_index = 0; time_index < smoothed_activations.cols; time_index++) {
            bool minimal_sustain_condition = (
                    activations->array[note_index *activations->cols +  time_index] - smoothed_activations.array[note_index *smoothed_activations.cols +  time_index] >
                    threshold);

            if(minimal_sustain_condition){
                if(!note_presence){
                    current_pitch = midi_codebook[note_index];
                    current_onset = time_index * time_step;
                    note_presence = true;
                }
            }
            else{
                if(note_presence){
//                    double current_offset = time_index * time_step;
                    notes.array[note_count *notes.cols + 0] = current_onset;
//                    notes.array[note_count][1] = current_offset;
//                    notes.array[note_count][2] = current_pitch;
                    notes.array[note_count *notes.cols+  1] = current_pitch;
                    note_count++;
//                    TODO: writing notes to midi file

                    note_presence = false;
                }
            }
        }

    }

    return notes;
}

void testTranscription() {
    Matrix X = CreateMatrix(88, 100);

    for (int i = 0; i < X.rows; i++) {
        for (int j = 0; j < X.cols; j++) {
            X.array[i *X.cols +  j] = i * j % 100;
        }
    }

    Matrix notes = TranscribeNotesFromActivations(&X, 0, 0.08);
    SaveMatrixToCSV("notes.csv", &notes);
}

double GetThreshold(const Matrix *activations) {
    //        Calculate variance of all activation entires above 0.01
    double mean = 0;
    int count = 0;
    for (int i = 0; i < activations->rows; i++) {
        for (int j = 0; j < activations->cols; j++) {
            if (activations->array[i *activations->cols +  j] > 0.01) {
                mean += activations->array[i *activations->cols+   j];
                count++;
            }
        }
    }

    mean /= count;

    double variance = 0;

    for (int i = 0; i < activations->rows; i++) {
        for (int j = 0; j < activations->cols; j++) {
            if (activations->array[i *activations->cols +   j] > 0.01) {
                variance += pow(activations->array[i *activations->cols +   j] - mean, 2);
            }
        }
    }

    variance /= count - 1;

    double l2 = 0.1;
    double noise_var = 1e-6;

    double inputs[2] = {mean, variance};

    printf("%lf %lf", mean, variance);


    double threshold = GPR(inputs, l2, noise_var);

//    double threshold = 9.8125 * variance + 0.0318;

//    threshold -= 0.01;

    printf("Threshold: %f\n", threshold);

    return threshold;
}
