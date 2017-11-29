#include <iostream>
#include <vector>
#include <string>
#include <mpi.h>

int main()
{
    std::vector<int> int_buffer;
    std::string char_buffer;
    std::vector<double> double_buffer;
    int size;

    MPI::Init();
    auto rank = MPI::COMM_WORLD.Get_rank();

    if (rank == 0) {
        int_buffer = {0, 1, 2, 3};
        char_buffer = "this is a test";
        double_buffer = {0.1, 0.2, 0.3};

        size  = MPI::INT   .Pack_size(int_buffer   .size(), MPI::COMM_WORLD);
        size += MPI::CHAR  .Pack_size(char_buffer  .size(), MPI::COMM_WORLD);
        size += MPI::DOUBLE.Pack_size(double_buffer.size(), MPI::COMM_WORLD);

        std::vector<char> out_buffer(size);
        auto position = 0;
        MPI::INT   .Pack(&int_buffer   .front(), 4,  &out_buffer.front(), size, position, MPI::COMM_WORLD);
        MPI::CHAR  .Pack(&char_buffer  .front(), 14, &out_buffer.front(), size, position, MPI::COMM_WORLD);
        MPI::DOUBLE.Pack(&double_buffer.front(), 3,  &out_buffer.front(), size, position, MPI::COMM_WORLD);

        MPI::COMM_WORLD.Send(&out_buffer.front(), size, MPI::PACKED, 1, 99);
    } else {
        MPI::Status status;
        MPI::COMM_WORLD.Probe(0, 99, status);
        auto size = status.Get_count(MPI::PACKED);

        std::vector<char> in_buffer(size);
        int_buffer   .resize(4);
        char_buffer  .resize(14);
        double_buffer.resize(3);

        MPI::COMM_WORLD.Recv(&in_buffer.front(), size, MPI::PACKED, 0, 99);
        auto position = 0;
        MPI::INT   .Unpack(&in_buffer.front(), size, &int_buffer   .front(), 4,  position, MPI::COMM_WORLD);
        MPI::CHAR  .Unpack(&in_buffer.front(), size, &char_buffer  .front(), 14, position, MPI::COMM_WORLD);
        MPI::DOUBLE.Unpack(&in_buffer.front(), size, &double_buffer.front(), 3,  position, MPI::COMM_WORLD);

        std::cout << "int buffer:";
        for (auto i : int_buffer) std::cout << " " << i;
        std::cout << "\n";

        std::cout << "char buffer: ";
        std::cout << char_buffer << "\n";

        std::cout << "double buffer:";
        for (auto i : double_buffer) std::cout << " " << i;
        std::cout << "\n";
    }

    MPI::Finalize();
}

