#/bin/bash 
./compile.sh
input_filename=$1
filename=${input_filename%.*}
echo $filename
extension=${input_filename##*.}
echo $extension
encode_filename=${filename}_code.txt
decode_filename=${filename}_decode.$extension
echo $encode_filename
echo $decode_filename
./encode  $input_filename  $encode_filename > stdout_en.txt
./decode  $encode_filename  $decode_filename > stdout_de.txt
./compare $input_filename $decode_filename
hexdump -C $input_filename > ${filename}_origin_hex.txt
hexdump -C $decode_filename > ${filename}_decode_hex.txt
