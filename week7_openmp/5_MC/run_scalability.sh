# #!
# run from the TimeHisto dir
rm -rf speedUpPi.log

NTHREADS=32
STEP=1

i=$STEP
while [ $i -le $NTHREADS ]
do
#   export OMP_NUM_THREADS=$i

  echo $i >> speedUpPi.log
  g++ MonteCarlo.cpp -O3 -fopenmp -o hello.out

  ./hello.out $i 0 >> speedUpPi.log

echo $i

   
  i=`expr $i + $STEP`
done



echo -e "\007"