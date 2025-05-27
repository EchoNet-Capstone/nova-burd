#!/usr/bin/bash
cd "$(dirname "$0")"

ELF_FILE_DIR="${1}"
ELF_FILE="$(realpath $ELF_FILE_DIR/firmware.elf)"

BUILD_NAME="$(basename "$ELF_FILE_DIR")"

OUTDIR="outputs/$BUILD_NAME"

TEXT_DUMP_NAME="$OUTDIR/text_dump.S"
TEXT_DUMP_FILE="$TEXT_DUMP_NAME"

FUNCTION_LIST="function_list.txt"
DISASSEMBLE_OUTDIR="$OUTDIR/disassemblies"

mkdir -p "$OUTDIR"
rm -rf "$OUTDIR"/*

mkdir "$DISASSEMBLE_OUTDIR"

# Build a map from demangled name → address + size + mangled
declare -A addr_map
declare -A size_map
declare -A mangled_map

while read -r addr size type mangled; do
    demangled=$(echo "$mangled" | c++filt)
    addr_map["$demangled"]=$addr
    size_map["$demangled"]=$size
    mangled_map["$demangled"]=$mangled
done < <(arm-none-eabi-nm -S --size-sort "$ELF_FILE" | grep ' [Tt] ')

while read -r line; do
    # Skip empty lines and comments
    [[ -z "${line//[[:space:]]/}" || "$line" =~ ^[[:space:]]*# ]] && continue
    
    funcname="$line"

    found=0
    for demangled in "${!addr_map[@]}"; do
        if echo "$demangled" | grep -Eq "(^|::)${funcname}\("; then
            addr=${addr_map[$demangled]}
            size=${size_map[$demangled]}
            start="0x$addr"
            end=$((0x$addr + 0x$size))
            outfile="$DISASSEMBLE_OUTDIR/${funcname}.S"
            echo "Writing disassembly for $funcname → $outfile"
            arm-none-eabi-objdump -d "$ELF_FILE" \
                --start-address=$start \
                --stop-address=0x$(printf "%x" $end) > "$outfile"
            found=1
            break
        fi
    done
    if [[ $found -eq 0 ]]; then
        echo "Warning: Function '$funcname' not found in ELF"
    fi
done < "$FUNCTION_LIST"

echo "Writing text dump → $TEXT_DUMP_FILE"
arm-none-eabi-objdump -D -Mforce-thumb $ELF_FILE > $TEXT_DUMP_FILE

mv "${BUILD_NAME}_output.map" "$OUTDIR/output.map"