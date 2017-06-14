# # #!
# 

NTHREADS=80
NSTAT=1


# FILE1=make_omptime.dat
FILE1="make_omptime"$1".dat"
PREF0=`pwd`
PREF1=`pwd`/..
FILE1=$PREF0/$FILE1

echo -e "$NTHREADS $NSTAT\n" > $FILE1

echo -e " single \n"
echo -e " --- single  ---" >> $FILE1
rm -rf $PREF1/single
cd $PREF1; make single
$PREF1/single >> $FILE1

echo -e " singleVc \n"
echo -e " --- singleVc  ---" >> $FILE1
rm -rf $PREF1/singleVc
cd $PREF1;  make singleVc
$PREF1/singleVc >> $FILE1

echo -e " ompVc \n"
echo -e " --- ompVc  ---" >> $FILE1
rm -rf $PREF1/ompVc;
cd $PREF1; make ompVc
i=1
while [ $i -le $NTHREADS ]
do
  j=1
  while [ $j -le $NSTAT ]
	do
		echo -e " ompVc $i - $j\n"
		echo -e "$i: \t" >> $FILE1
		$PREF1/ompVc $i >> $FILE1
		j=`expr $j + 1`
	done
	i=`expr $i + 1`
done

cd $PREF0


