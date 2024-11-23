/*----------------------------------------------
 * Author: Tianyun Song
 * Date: 10.12.2024
 * Description: This program reads a hexadecimal number representing an image stored as
 * an unsigned long and prints a representation of that image in a grid format.
 * Each bit of the input is represented as '@' for 1 or a space ' ' for 0,
 * creating a visual grid that helps visualize the bit pattern of the image.
 ---------------------------------------------*/
#include <stdio.h>

int main() {
  unsigned long img;
  scanf(" %lx", &img);
  printf("Image (unsigned long): %lx\n", img);

  // Loop through each bit in the 64-bit unsigned long
  for (int i = 0; i < 64; i++) {
    // Create a mask for the current bit
    unsigned long mask = 0x1ul << (63 - i);
        
    // Apply the mask and output the corresponding character
    if (img & mask) {
      printf("@ ");
    } else {
      printf("  ");
    }

    // Every 8 characters, print a new line
    if (i % 8 == 7) {
      printf("\n");
    }
  }
  return 0;
}