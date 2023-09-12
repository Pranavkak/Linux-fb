#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

int main() {
    int fbfd = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;
    char *fbp = 0;
    int x, y, offset;

    // Open the framebuffer device
    fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        return 1;
    }

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        return 2;
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        return 3;
    }

    // Calculate the size of the framebuffer in bytes
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    // Map the framebuffer to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((int)fbp == -1) {
        perror("Error: failed to map framebuffer device to memory");
        return 4;
    }

    // Clear the framebuffer (fill with black)
    memset(fbp, 0, screensize);

    // Draw a red pixel at (100, 100)
    x = 100;
    y = 100;
    offset = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) +
             (y + vinfo.yoffset) * finfo.line_length;
    *((unsigned int *)(fbp + offset)) = 0xFF0000; // Red color

    // Wait for user input before exiting
    printf("Press Enter to exit...");
    getchar();

    // Unmap the framebuffer from memory
    munmap(fbp, screensize);

    // Close the framebuffer device
    close(fbfd);

    return 0;
}
