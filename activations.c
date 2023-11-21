//
// Created by ruanb on 9/10/2023.
//

#include "activations.h"
#include "math.h"
#include "stdio.h"
#include "wav.h"
#include "stft.h"

#include "string.h"

#include "time.h"

//const char * H_persisted_dir = "C:\\Users\\ruanb\\OneDrive\\Desktop\\Piano Transcripton\\Piano Transcription (C)\\data_persisted\\activations\\";

//double BetaDivergence(const Matrix *x, const Matrix *y, double beta) {
//    Matrix d = CreateMatrix(x->rows, x->cols);

////    printf("X dimensions: %d x %d\n", x->rows, x->cols);
////    printf("Y dimensions: %d x %d\n", y->rows, y->cols);
//    fflush(stdout);

//    double sum = 0;

//    for (int i = 0; i < x->rows; i++) {
//        for (int j = 0; j < x->cols; j++) {
////            printf("x: %f, y: %f\n", x->array[i][j], y->array[i][j]);



////            printf("y->array[i][j]: %f\n", y->array[i][j]);
////            printf("x->array[i][j]: %f\n", x->array[i][j]);

//            fflush(stdout);


//            if (y->array[i][j] < 1e-8) {
//                y->array[i][j] = 1e-8;
//            }
//            if (x->array[i][j] < 1e-8) {
//                x->array[i][j] = 1e-8;
//            }

////            printf("aaaasaasss");

//            fflush(stdout);



//            d.array[i][j] = x->array[i][j] * log(x->array[i][j] / y->array[i][j]) - x->array[i][j] + y->array[i][j];


//            if (isnan(d.array[i][j])) {
////                printf("x: %f, y: %f\n", x->array[i][j], y->array[i][j]);
////                fflush(stdout);
//            }

//            sum += d.array[i][j];

//        }
//    }

//    DestroyMatrix(&d);

//    return sum;

//}

double Gaussian(double x, double y){
    double exponent = -(pow(x, 2)/(2) + pow(y, 2)/(2));
    return exp(exponent);
}

Matrix ComputeActivations(const Spectrogram *X, int iterations,
                          const Dictionary *dictionary, const char * init) {

//    printf("Super big balls\n");
//    fflush(stdout);

    clock_t begin = clock();

    int r = dictionary->shape[2];
    int ncol = X->matrix.cols;
    int T = dictionary->shape[0];

    double beta = 1;

    Matrix activations = CreateMatrix(r, ncol);

//    printf("Super big balls\n");
//    fflush(stdout);

    if(strcmp(init, "gauss") == 0){
        for (int ii = 0; ii < activations.rows; ii++) {
            for (int jj = 0; jj < activations.cols; jj++) {
    //            activations.array[ii][jj] = (double) rand() / (double) RAND_MAX;
                double x = (double)ii /activations.rows * 14 - 7;
    //            double x = 0;
                double y = (double)jj/activations.cols * 14 - 7;
    //            double y = 0;
                activations.array[ii *activations.cols +  jj] = Gaussian(x, y);
            }
        }
    }
    else if (strcmp(init, "constant") == 0){
        for(int i = 0; i < activations.rows; i++){
            for(int j = 0; j < activations.cols; j++){
                activations.array[i *activations.cols +   j] = 0.5;
            }
        }
    }

//    printf("Right before conv\n");
//    fflush(stdout);

    Matrix conved = ComputeConvolution(dictionary, &activations, T);

    printf("After conv\n");
    fflush(stdout);

//    double error_int = BetaDivergence(&X->matrix, &conved, 1);

//    printf("ComputeActivations: Initial error = %f\n", error_int);
    fflush(stdout);

    Matrix convolutions[T];

    Matrix ones = CreateMatrix(dictionary->shape[1], ncol);
    FillMatrix(&ones, 1);

    for (int t = 0; t < T; t++) {
        Spectrogram tspec = GetSpectrogramFromDictionary(dictionary, 0, t);

        Matrix tspec_transposed = Transpose(&tspec.matrix);

        convolutions[t] = MatrixMultiply(&tspec_transposed, &ones);

        DestroySpectrogram(&tspec);
        DestroyMatrix(&tspec_transposed);
    }

    DestroyMatrix(&ones);

    Matrix denom_all_col = SumMatrices(convolutions, T);  

    for (int i = 0; i < T; i++) {
        DestroyMatrix(&convolutions[i]);
    }

    Matrix denoms_cropped_for_end = CreateMatrix(T, r);


    for (int j = 1; j < T + 1; j++) {

        Matrix temp = CreateMatrix(T, r);

        for (int a = 0; a < j; a++) {
            Spectrogram a_spec = GetSpectrogramFromDictionary(dictionary, 0, a);
            for (int cc = 0; cc < a_spec.matrix.cols; cc++) {
                for (int rr = 0; rr < a_spec.matrix.rows; rr++) {
                    temp.array[a *temp.cols +   cc] += a_spec.matrix.array[rr *a_spec.matrix.cols + cc];
                }
            }

            DestroySpectrogram(&a_spec);

        }

        for (int cc = 0; cc < r; cc++) {
            for (int rr = 0; rr < j; rr++) {
                denoms_cropped_for_end.array[(j - 1) *denoms_cropped_for_end.cols +   cc] += temp.array[rr *temp.cols +  cc];
            }
        }

        DestroyMatrix(&temp);
    }


//    SaveMatrixToCSV("denomend.csv", &denoms_cropped_for_end);
//    SaveMatrixToCSV("denomcols.csv", &denom_all_col);

    int iteration = 0;
//    double obj, obj_prev = error_int;

    while (iteration < iterations) {
        Matrix A = ComputeConvolution(dictionary, &activations, T);

//        SaveMatrixToCSV("A.csv", &A);

      // TODO: optimize the padding


#pragma omp parallel for collapse(2)

        for (int i = 0; i < A.rows; i++) {
            for (int j = 0; j < A.cols; j++) {

                if (A.array[i *A.cols +  j] < 1e-8) {
                    A.array[i *A.cols +  j] = 1e-8;
                }

                A.array[i *A.cols +  j] = X->matrix.array[i *X->matrix.cols +   j] * 1 / A.array[i * A.cols + j];

            }
        }
        Matrix X_hadamard_A_padded = CreateMatrix(A.rows, A.cols + T);

#pragma omp parallel for collapse(2)

        for (int i = 0; i < A.rows; i++) {
            for (int j = 0; j < A.cols; j++) {
                X_hadamard_A_padded.array[i *X_hadamard_A_padded.cols +  j] = A.array[i *A.cols +  j];
            }
        }

//        SaveMatrixToCSV("X_hadamard_A_padded.csv", &X_hadamard_A_padded);


        Matrix num = CreateMatrix(r, A.cols);

        DestroyMatrix(&A);

        for (int t = 0; t < T; t++) {
            Matrix W_at_t = GetMatrixFromDictionary(dictionary, 0, t);
            Matrix transposed = Transpose(&W_at_t);

            DestroyMatrix(&W_at_t);

            Matrix X_hadamard_A_windowed = CreateMatrix(X_hadamard_A_padded.rows, ncol);
#pragma omp parallel for collapse(2)

            for (int i = 0; i < X_hadamard_A_padded.rows; i++) {
                for (int j = 0; j < ncol; j++) {
                    X_hadamard_A_windowed.array[i *X_hadamard_A_windowed.cols +   j] = X_hadamard_A_padded.array[i *X_hadamard_A_padded.cols +  (j + t)];
                }
            }

            Matrix multiplied = MatrixMultiply(&transposed, &X_hadamard_A_windowed);

            DestroyMatrix(&X_hadamard_A_windowed);
            DestroyMatrix(&transposed);
#pragma omp parallel for collapse(2)

            for (int i = 0; i < multiplied.rows; i++) {
                for (int j = 0; j < multiplied.cols; j++) {
                    num.array[i *num.cols +    j] += multiplied.array[i *multiplied.cols +    j];
                }
            }
        }

//        SaveMatrixToCSV("num.csv", &num);

        DestroyMatrix(&X_hadamard_A_padded);

#pragma omp parallel for collapse(2)

        for (int row = 0; row < activations.rows; row++) {
            for (int c = 0; c < ncol - T; c++) {
                if (denom_all_col.array[row *denom_all_col.cols +   c] < 1e-8) {
                    denom_all_col.array[row *denom_all_col.cols +  c] = 1e-8;
                }
                activations.array[row *activations.cols +  c] *= (num.array[row *num.cols +  c] / denom_all_col.array[row *denom_all_col.cols +  c]);
            }
        }

#pragma omp parallel for collapse(2)

        for (int c = (int) (ncol - T); c < ncol; c++) {
            for (int row = 0; row < activations.rows; row++) {
                activations.array[row *activations.cols +  c] *=
                        (num.array[row *num.cols +   c] / denoms_cropped_for_end.array[(ncol - c - 1) *denoms_cropped_for_end.cols +  row]);
            }
        }

//        SaveMatrixToCSV("activations.csv", &activations);

        DestroyMatrix(&num);
        DestroyMatrix(&conved);

//        conved = ComputeConvolution(dictionary, &activations, T);

//        SaveMatrixToCSV("final.csv", &conved);

        printf("Iteration done\n");
        fflush(stdout);

//        obj = BetaDivergence(&X->matrix, &conved, beta);

//        printf("ComputeActivations: Iteration %d, obj = %f\n", iteration, obj);
//        fflush(stdout);

//        if ((fabs(obj - obj_prev) / error_int) < maximum_error) {
//            printf("ComputeActivations: Converged sufficiently\n");
////            break;
//        }

//        if (fabs(obj > obj_prev)) {
//            printf("ComputeActivations: Diverged\n");
//            break;
//        }

//        obj_prev = obj;
        iteration++;

        clock_t end = clock();
//        printf("Time: %lf\n", (double)(end-begin)/CLOCKS_PER_SEC);
//        fflush(stdout);

    }

    DestroyMatrix(&denom_all_col);
    DestroyMatrix(&denoms_cropped_for_end);

    return activations;
}

Matrix ComputeConvolution(const Dictionary *dictionary, const Matrix *matrix2, int t) {
    Matrix convolutions[t/2];

    for (int i = 0; i < t/2; i++) {
        Matrix tspec = GetMatrixFromDictionary(dictionary, 0, i*2);

//        printf("Got spec from dict\n");
//        fflush(stdout);

        Matrix shifted = ShiftMatrix(matrix2, i);

//        printf("Shifted\n");
//        fflush(stdout);

        convolutions[i] = MatrixMultiply(&tspec, &shifted);


//        printf("Multipled\n");
//        fflush(stdout);

        DestroyMatrix(&tspec);
        DestroyMatrix(&shifted);
    }

    Matrix conv_sum = SumMatricesAlongAxis(convolutions, t/2, 0);

//    printf("Summed\n");
//    fflush(stdout);

    for (int i = 0; i < t/2; i++) {
        DestroyMatrix(&convolutions[i]);
    }

    return conv_sum;



}


void TestActivations() {

}



