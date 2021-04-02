cd build
if [ ! -d "out" ]; then
 mkdir -p out
fi
make clean
make
if [ ! -d "../lib/x86" ]; then
 mkdir -p ../lib/x86
fi
cp out/libslog.so ../lib/x86
