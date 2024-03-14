#!/bin/bash

# Check if a file path has been provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <path/to/image.png>"
    exit 1
fi

# Get the image file specified by the command line argument
IMAGE_PATH="$1"

# Check if the file exists
if [ ! -f "$IMAGE_PATH" ]; then
    echo "File does not exist: $IMAGE_PATH"
    exit 1
fi

# Get the filename without the path
FILENAME=$(basename -- "$IMAGE_PATH")
# Get the base name without the extension
BASENAME="${FILENAME%.*}"

for size in 16 32 64 128 256 512; do
    # Define the name of the output file
    OUTPUT_FILE="${BASENAME}_${size}x${size}.png"
    # Use the convert command to resize the image and save it
    convert "$IMAGE_PATH" -resize ${size}x${size} "$OUTPUT_FILE"
    echo "Generated icon: $OUTPUT_FILE"
done

echo "All icons have been generated."
