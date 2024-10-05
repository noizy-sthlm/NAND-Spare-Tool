# NAND Spare Tool

A simple command-line tool written in C for removing spare (Out-Of-Band) areas
from NAND dump files.

## Description

This tool reads a NAND dump file and removes the spare areas, leaving just the
main data. It's useful when you quickly want to extract the data
from a NAND flash dump without the spare areas. But remember that removal without
proper ECC decoding may leave you with uncorrected bit-errors in your image.

## Requirements
- GCC
- Make

## Usage

1. **Clone the repository**
```bash
git clone <repository-url>
cd nand_spare_removal_tool
```

2. **Build**
```bash
make
```
3. **Run the tool**
```bash
$ ./nand_tool [OPTIONS] <file> <pageSize> <spareSize>
```

## Planned extentions
- BCH codes
- Hamming codes

