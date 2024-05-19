# Vector + COW(Copy-on-Write)
## Usage
1.Clone the repository to your local machine:

        git clone https://github.com/coldysplash/vector-copy-on-write.git
        cd vector-copy-on-write

2.Update git submodules:

        git submodule init
        git submodule update

3.Generate and bulding project using presets:

        cmake --preset release
        cmake --build --preset release

4.Run tests:

        ctest --preset release