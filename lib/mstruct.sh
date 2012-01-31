#!/bin/bash

TEMPLATE_H="map.h"
TEMPLATE_C="map.c"

if [[ -z $1 ]]; then
	echo "Usage: $0 <structure name>"
	echo "Pastes the necessary information from the definition into the"
	echo "implementation files"
	exit 1
fi

sname="$(echo $1 | sed -r 's/\.def$//')"
if [[ ! -f $sname.def ]] || [[ ! -f $sname.dec ]] || [[ ! -f $sname.imp ]]; then
	echo "mstruct: $sname: file does not exist"
	exit 2
fi

file=""
tmph="$(mktemp)"
tmpc="$(mktemp)"
cp "${TEMPLATE_H}" "$tmph"
cp "${TEMPLATE_C}" "$tmpc"

#fdline="$(grep -n "<<<--- FUNCTION_DECLARATIONS" "$tmpc" | cut -d':' -f1)"

tmp="$(mktemp)"
cat > "$tmp" <<EOF
#!/bin/bash
sed '/<<<--- FUNCTION_DECLARATIONS/ {
	r $sname.dec
	d
}' -i "$tmpc"
EOF
chmod +x "$tmp"
eval "$tmp"

cat > "$tmp" <<EOF
#!/bin/bash
sed '/<<<--- FUNCTION_IMPLEMENTATIONS/ {
	r $sname.imp
	d
}' -i "$tmpc"
EOF
chmod +x "$tmp"
eval "$tmp"

rm "$tmp"

while read var; do
	vname="$(echo -n "$var" | sed -r 's/^\s*(\S+)\s*:=.*/\1/')"
	value="$(echo -n "$var" | sed -r 's/.*:=\s*(.*)$/\1/')"
	if [[ "$vname" == "FILE" ]]; then
		file="$value"
	fi
	sed -i "s/\\\${$vname}/$value/g" "$tmph"
	sed -i "s/\\\${$vname}/$value/g" "$tmpc"
	echo "$vname: $value"
done < <(sed -r '/^\s*$/d' "${sname}.def")

echo File name is: $file
mv "$tmph" "$file.h"
mv "$tmpc" "$file.c"

