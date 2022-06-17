#include <iostream>
#include <chrono>

#include <fpacker.h>

int main(int argc, char** argv)
{
    // --------- Basic Functionality ---------
    // 
    // -- Load the target directory into a Package object --
    // auto Package = FJoiner::Package::LoadDir("examplefolder");
    // 
    // -- Pack this package into a file. The file extension doesnt matter. --
    // Package.Pack("examplefile.fjpkg");
    // 
    // 
    // -- Load the package file into a new Package object --
    // Package = FJoiner::Package::LoadPackageFile("examplefile.fjpkg");
    // 
    // -- Unpack this package into a directory. --
    // Package.Unpack("unpackedFolder");



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

        auto Package = FPacker::Package::LoadDir(targetFolder);
        Package.Pack(targetFile);

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

            auto Package = FPacker::Package::LoadPackageFile(targetFile);
            Package.Unpack(targetFolder);

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            std::cout << "Finished unpacking in " << duration.count() << "ms" << std::endl;
        }
    }

    system("PAUSE");
}
