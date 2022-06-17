<h1 align="center"> FPacker </h2>
<p align="center">
    <a href="#">
        <img src="https://img.shields.io/github/repo-size/oxi-dev0/fpacker" alt="Repo Size">
    </a>
    <a href="https://github.com/oxi-dev0/fpacker/releases/tag/v1.0">
        <img src="https://img.shields.io/github/downloads/oxi-dev0/fpacker/total" alt="Downloads">
    </a>
    <a href="#">
        <img src="https://img.shields.io/github/stars/oxi-dev0/fpacker" alt="Stars">
    </a>
</p>

<h3 align="center"> A small cross-platform c++ library for packing a directory into a single file </h3>
<h6 align="center"> Please Note: This will not compress the files, it just organises them within a binary file. </h6>
<br>
<h2> Documentation </h2>
<h3> Usage </h3>

First, download the static libraries from the [Release](https://github.com/oxi-dev0/fpacker/releases/) page. Inside your project, include the header from `include/`, and depending on what configuration you are building your project in, you should link either `x64/Release/fpacker.lib` for release mode, or `x64/Debug/fpacker-d.lib` for debug mode.

See [example/src/main.cpp](https://github.com/oxi-dev0/fpacker/blob/main/example/src/main.cpp) for a demonstration on how to use the library.

<h3> Building from source </h3>

> Visual Studio 2022 is required to build from source.


Clone the repo (Use github desktop, or run `git clone https://github.com/oxi-dev0/fpacker/`).  
Then, run `GenerateProject.bat` to generate the visual studio project files. Premake is included with the repo.