set -x
touch 1.txt
ln 1.txt 2.txt
ln -s 1.txt 3.txt
ls -il [1-3].txt
read
echo " 123" >> 2.txt
more 1.txt
ls -il [1-3].txt
