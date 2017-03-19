cd/media/pi/PPV/uspi-master
cd lib
make $1 $2 || exit
cd ..

cd env/lib
make $1 $2 || exit
cd ../..

cd sample/ethernet
./makeall $1 $2 || exit
cd ..
 cd $HOME/Desktop
mkdir done