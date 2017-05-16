# #!

NTHREADS=16
NSTAT=10


# FILE1=make_tbbtime.dat
FILE1="make_tbbtime"$1".dat"
PREF0=`pwd`
PREF1=$PREF0/..
FILE1=$PREF0/$FILE1

echo -e "$NTHREADS $NSTAT\n" > $FILE1

echo -e " single \n"
echo -e " --- single  ---" >> $FILE1
rm $PREF1/single
cd $PREF1; make single
$PREF1/single >> $FILE1

echo -e " singleVc \n"
echo -e " --- singleVc  ---" >> $FILE1
rm $PREF1/singleVc
cd $PREF1;  make singleVc
$PREF1/singleVc >> $FILE1

echo -e " tbbVc \n"
echo -e " --- tbbVc  ---" >> $FILE1
rm $PREF1/tbbVc;
cd $PREF1; make tbbVc
i=1
while [ $i -le $NTHREADS ]
do
	j=1
	while [ $j -le $NSTAT ]
	do
		echo -e " tbbVc $i - $j\n"
		echo -e "$i: \t" >> $FILE1
		cd $PREF1; ./tbbVc $i >> $FILE1
		j=`expr $j + 1`
	done
	i=`expr $i + 1`
done




