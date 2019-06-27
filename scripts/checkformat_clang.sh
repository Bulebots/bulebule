#!/bin/bash
output=$(mktemp)
errors=$(mktemp)
find src/ -type f -regex ".*\.\(c\|h\)$" \
	-not -path "src/opencm3/*" \
	-not -path "src/printf/*" | while read fname
do
	clang-format "$fname" > "$output"
	git diff --color "$fname" "$output" >> "$errors"
done
rm -f "$output"
if [ -s "$errors" ]
then
	echo -e "\033[0;31mFORMATTING ERRORS!\033[0m"
	cat "$errors"
	rm -f "$errors"
	exit 1
fi
rm -f "$errors"
