#!/bin/sh
set -e

tmpdir=`mktemp -d /tmp/orig.XXXX`
sortedfile=$tmpdir/sorted
jsonfile=/home/esjolund/features_fewer_lines.json

sed 's/,$//' $jsonfile | sort  > $sortedfile

for i in @format_list_separated_by_spaces@; do
    dir=`mktemp -d /tmp/run.$i.XXXX`
    $<TARGET_FILE:convert-feature-format> json $jsonfile $i $dir/out
    $<TARGET_FILE:convert-feature-format> $i $dir/out json $dir/out2

    sed -i 's/,$//' $dir/out2
    sort $dir/out2 > $dir/sorted
    if ! diff -q  $dir/sorted $sortedfile; then
	echo $i: sorted files differ
	echo To see the difference run: diff -u $dir/sorted $sortedfile
    fi
done
