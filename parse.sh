FOLDER=/pnfs/numix/persistent/mm/2020-03-17
for file in ${FOLDER}/*; do
  ./parse $file /nova/ana/users/asztuc/numi/mm 
done



#./parse /home/artur/WORK/CHIPS/NUMI/DATA/2020-03-19/23_00_00  /home/artur/WORK/CHIPS/NUMI/output/tester_test
#./parse /home/artur/WORK/CHIPS/NUMI/DATA/2020-03-19/23_30_00  /home/artur/WORK/CHIPS/NUMI/output/tester_test
