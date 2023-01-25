#include <iostream>
#include <chrono>

#include <fpacker.hpp>

int main(int argc, char** argv)
{
    // --------- Basic Functionality ---------
    // 
    // -- Pack a directory into a file. The target file extension doesnt matter --
    // FPacker::Pack("examplefolder", "examplefile.fjpkg");
    // 
    // -- Unpack a file into a directory. --
    // FPacker::Unpack("examplefile.fjpkg", "unpackedfolder");


    // --------- Example Console App ---------
    if (argc < 2) { std::cout << "Requires argument --pack or --unpack" << std::endl; return 0; }

    if (std::string(argv[1]) == "--pack") {
        std::string targetFolder = "examplefolder";
        std::string targetFile = "examplefile.fjpkg";

        if (argc > 2) {
            targetFolder = std::string(argv[2]);
            if (argc > 3) {
                targetFile = std::string(argv[3]);
            }
        }

        std::cout << "Packing '" << targetFolder << "' to '" << targetFile << "'" << std::endl;
        auto start = std::chrono::high_resolution_clock::now();

        FPacker::Pack(targetFolder, targetFile);

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Finished packing in " << duration.count() << "ms" << std::endl;
    }
    else {
        if (std::string(argv[1]) == "--unpack") {
            std::string targetFolder = "unpackedFolder";
            std::string targetFile = "examplefile.fjpkg";

            if (argc > 2) {
                targetFile = std::string(argv[2]);
                if (argc > 3) {
                    targetFolder = std::string(argv[3]);
                }
            }

            std::cout << "Unpacking '" << targetFile << "' to '" << targetFolder << "'" << std::endl;
            auto start = std::chrono::high_resolution_clock::now();

            FPacker::Unpack(targetFile, targetFolder);

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            std::cout << "Finished unpacking in " << duration.count() << "ms" << std::endl;
        }
    }

    system("PAUSE");
}
