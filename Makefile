all: md5 sha1 ripemd gost
md5:
	g++ main_md5.cpp abstractHash.cpp -o md5.out
sha1:
	g++ main_sha1.cpp abstractHash.cpp -o sha1.out
ripemd:
	g++ main_ripemd160.cpp abstractHash.cpp -o ripemd160.out
gost:
	g++ main_gost341194.cpp abstractHash.cpp -o gost341194.out
clear:
	rm *.out
