#/bin/bash 
./compile.sh
input_filename=$1
filename=${input_filename%.*}
#echo $filename
extension=${input_filename##*.}
#echo $extension
encode_filename=${filename}.dat
decode_filename=${filename}_decode.$extension
echo "input file:"$1
echo "encode output file:"$encode_filename
echo "decode output file:"$decode_filename
echo
./encode  $input_filename  $encode_filename #> stdout_en.txt
echo
./decode  $encode_filename  $decode_filename # > stdout_de.txt
echo 
echo "result:"
./compare $input_filename $decode_filename
#hexdump -C $encode_filename >${filename}_code_hex.txt
#hexdump -C $input_filename > ${filename}_origin_hex.txt
#hexdump -C $decode_filename > ${filename}_decode_hex.txt
