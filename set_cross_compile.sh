PATH=/opt/cross-pi-gcc-6.3.0-0/bin:$PATH
LD_LIBRARY_PATH=/opt/cross-pi-gcc-6.3.0-0/lib:$LD_LIBRARY_PATH

export AR="arm-linux-gnueabihf-gcc-ar"
export CC="arm-linux-gnueabihf-gcc"
export CXX="arm-linux-gnueabihf-g++"
export CPP="arm-linux-gnueabihf-cpp"
export FC="arm-linux-gnueabihf-gfortran"
export RANLIB="arm-linux-gnueabihf-gcc-ranlib"
export LD="$CXX"
