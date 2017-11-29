#include <iostream>
#include <fstream>
#include <vector>
#include <mpi.h>

int main(int argc, char** argv)
{
    std::ifstream fin{"mat.data"};
    int n;
    int m;
    fin >> n >> m;
    std::vector<int> mat(n * m);
    std::vector<int> trans_mat(m * n);

    for (auto i = 0; i < n; i++)
        for (auto j = 0; j < m; j++)
            fin >> mat[i * m + j];

    MPI::Init(argc, argv);
    // this 'auto' keyword automatically deduce the type of matrix_type,
    // here same as, MPI::Datatype matrix_type = ...
    auto matrix_type = MPI::INT.Create_vector(n, m, m);
    matrix_type.Commit();
    // auto -> MPI::Datatype
    auto column_type = MPI::INT.Create_vector(m, 1, n);
    column_type.Commit();
    // auto -> MPI::Datatype
    auto trans_matrix_type = column_type.Create_hvector(n, 1, sizeof(int));
    trans_matrix_type.Commit();

    MPI::COMM_WORLD.Sendrecv(
        &mat.front(), 1, matrix_type, 0, 99,
        &trans_mat.front(), 1, trans_matrix_type, 0, 99
    );

    matrix_type.Free();
    column_type.Free();
    trans_matrix_type.Free();
    MPI::Finalize();

    std::ofstream fout{"trans_mat.data"};
    fout << m << " " << n << "\n";
    for (auto i = 0; i < m; i++) {
        for (auto j = 0; j < n; j++) {
            fout << trans_mat[i * n + j] << "\t";
        }
        fout << "\n";
    }
}

