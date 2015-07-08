#
#	Tema 3 ASC
#
#	Student:	NICULA Bogdan
#	Grupa:		334CB

make clean
make
rm output*
cd ./serial
make clean
make
cp btc ..
cp compare ..
cd ..
for i in 1 2 4 8
do
	for file in *.pgm
	do
		echo >> output_compare
		echo "-----------------------------------" >> output
		echo $file >> output
		t_btc1='temp_btc'
		t_pgm1='temp_pgm'
		t_btc2='temp2_btc'
		t_pgm2='temp2_pgm'
		
		#creez executabil pentru implementare seriala
		echo "---------Serial implementation-----------" >> output
		./btc $file $t_btc2 $t_pgm2 >> output

		#creez executabil pentru implementare pe cell, fara double buffering
		echo "---------Personal implementation-----------" >> output
		./tema3 0 $i $file $t_btc1 $t_pgm1 >>output
		#testez rezultatele
		./compare btc $t_btc1 $t_btc2 >> output_compare
		./compare pgm $t_pgm1 $t_pgm2 >> output_compare
		rm $t_btc1 $t_pgm1
		echo >> output
		echo "-------------" >> output_compare
		#creez executabil pentru implementare pe cell, cu double buffering
		./tema3 1 $i $file $t_btc1 $t_pgm1 >>output
		#testez rezultatele
		./compare btc $t_btc1 $t_btc2 >> output_compare
		./compare pgm $t_pgm1 $t_pgm2 >> output_compare
		echo >> output
		echo "-----------------------------------" >> output
		echo >> output_compare
		rm $t_btc1 $t_btc2 $t_pgm2 $t_pgm1
	done
done
