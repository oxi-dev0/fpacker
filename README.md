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

<h3 align="center"> A small header-only c++ library for packing a directory into a single file </h3>
<h6 align="center"> Please Note: This will not compress the files, it just organises them within a binary file. </h6>
<br>
<h2> Update </h2>
<h3> V2.0.0 </h3>

After learning more C++ on other projects, I decided to revisit FPacker to completely revamp it. The 1.0 version was very hacked together, as I was still quite new to the language. The 2.0 release fixes all of the issues the previous versions had (block sizes were written as strings, not binary numbers, to name an example), and fixes the module interface to be intuitive and more efficient to use.
This new update also improves the speed of the library by around 10x, as it skips all the unnecessary conversions and improper types that I used to create the 1.0 version.

<h3> Speed Comparison (Test directory is not changed) </h3>
<h5> V1.1.1 </h5>
<img src="https://github.com/oxi-dev0/fpacker/blob/v2.0.0/imgs/v1.1.1-benchmark-2146kb-release.png" style="width:50%;">
<h5> V2.0.0 </h5>
<img src="https://github.com/oxi-dev0/fpacker/blob/v2.0.0/imgs/v2.0.0-benchmark-2145kb-release.png" style="width:50%;">

<h3> Future </h3>

It is unlikely that I will be revisiting this library, but I hope it helps someone quickly implement packing files.

<h2> Documentation </h2>
<h3> Usage </h3>

First, download the header file from the [Releases](https://github.com/oxi-dev0/fpacker/releases/) page. Inside your project add the header as an include or as part of the source, it does not matter, then just include it where needed.
See [example/src/main.cpp](https://github.com/oxi-dev0/fpacker/blob/main/example/src/main.cpp) for a quick demonstration on how to use the library.

<h3> Building the example from source </h3>

> Visual Studio 2022 is required to build from source.


Clone the repo (Use github desktop, or run `git clone https://github.com/oxi-dev0/fpacker/`).  
Then, run `example/GenerateProject.bat` to generate the visual studio project files. Premake is included with the repo.
