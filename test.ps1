cmake -S . -B build 
cmake --build build 
Set-Location build
ctest --rerun-failed --output-on-failure 