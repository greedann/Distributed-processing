#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <png.h>

// gcc main.c -o main -lpng -pthread -lm
// ./main 4 5 input.png output.png

typedef struct
{
    png_bytep image;
    png_bytep result;
    int width;
    int height;
    int start_row;
    int end_row;
    int sigma;
} ThreadData;

int load_png_image(const char *filename, png_bytep *image, int *width, int *height)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        printf("Failed to open file\n");
        return 0;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        fclose(fp);
        printf("Failed to create read struct\n");
        return 0;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(fp);
        printf("Failed to create info struct\n");
        return 0;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        printf("Failed to set jmp\n");
        return 0;
    }

    png_init_io(png_ptr, fp);
    png_read_info(png_ptr, info_ptr);

    *width = png_get_image_width(png_ptr, info_ptr);
    *height = png_get_image_height(png_ptr, info_ptr);

    png_byte color_type = png_get_color_type(png_ptr, info_ptr);
    png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    if (bit_depth == 16)
        png_set_strip_16(png_ptr);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);

    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png_ptr);

    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);

    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    *image = (png_bytep)malloc(sizeof(png_byte) * (*height) * png_get_rowbytes(png_ptr, info_ptr));
    png_bytep *row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * (*height));

    for (int y = 0; y < *height; y++)
    {
        row_pointers[y] = *image + y * png_get_rowbytes(png_ptr, info_ptr);
    }

    png_read_image(png_ptr, row_pointers);

    fclose(fp);
    free(row_pointers);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    return 1;
}

void save_png_image(const char *filename, png_bytep image, int width, int height)
{
    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        fprintf(stderr, "Error: Could not open file %s for writing\n", filename);
        return;
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        fclose(fp);
        fprintf(stderr, "Error: Could not create PNG write structure\n");
        return;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, NULL);
        fprintf(stderr, "Error: Could not create PNG info structure\n");
        return;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fprintf(stderr, "Error: Error during PNG file writing\n");
        return;
    }

    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_bytep *row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * height);
    for (int y = 0; y < height; y++)
    {
        row_pointers[y] = image + y * width * 4;
    }

    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    fclose(fp);
    free(row_pointers);
    png_destroy_write_struct(&png_ptr, &info_ptr);
}

double gaussian(double x, double y, double sigma)
{
    return exp(-(x * x + y * y) / (2.0 * sigma * sigma)) / (2 * M_PI * sigma * sigma);
}

void *apply_gaucian_blur(void *arg)
{
    ThreadData *data = (ThreadData *)arg;
    png_bytep image = data->image;
    png_bytep result = data->result;
    int width = data->width;
    int height = data->height;
    int start_row = data->start_row;
    int end_row = data->end_row;
    double sigma = data->sigma;
    int radius = 5;

    for (int y = start_row; y < end_row; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            double sum_r = 0, sum_g = 0, sum_b = 0;
            double sum_weight = 0;
            for (int i = -radius; i <= radius; ++i)
            {
                for (int j = -radius; j <= radius; ++j)
                {
                    int yy = y + i;
                    int xx = x + j;
                    if (yy >= 0 && yy < height && xx >= 0 && xx < width)
                    {
                        double weight = gaussian(i, j, sigma);
                        sum_r += image[4 * (yy * width + xx) + 0] * weight;
                        sum_g += image[4 * (yy * width + xx) + 1] * weight;
                        sum_b += image[4 * (yy * width + xx) + 2] * weight;
                        sum_weight += weight;
                    }
                }
            }
            result[4 * (y * width + x) + 0] = (png_byte)(sum_r / sum_weight);
            result[4 * (y * width + x) + 1] = (png_byte)(sum_g / sum_weight);
            result[4 * (y * width + x) + 2] = (png_byte)(sum_b / sum_weight);
            result[4 * (y * width + x) + 3] = image[4 * (y * width + x) + 3];
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        fprintf(stderr, "Usage: %s <n_threads> <sigma> <input_file.png> <output_file.png>\n", argv[0]);
        return 1;
    }

    int threads_count = atoi(argv[1]);
    int sigma = atoi(argv[2]);
    const char *input_filename = argv[3];
    const char *output_filename = argv[4];

    png_bytep image;
    int width, height;

    if (!load_png_image(input_filename, &image, &width, &height))
    {
        return 1;
    }

    png_bytep result = (png_bytep)malloc(sizeof(png_byte) * width * height * 4);

    pthread_t threads[threads_count];
    ThreadData thread_data[threads_count];

    time_t start_time = clock();
    int rows_per_thread = height / threads_count;
    for (int i = 0; i < threads_count; i++)
    {
        thread_data[i].image = image;
        thread_data[i].result = result;
        thread_data[i].width = width;
        thread_data[i].height = height;
        thread_data[i].sigma = sigma;
        thread_data[i].start_row = i * rows_per_thread;
        thread_data[i].end_row = (i == threads_count - 1) ? height : (i + 1) * rows_per_thread;

        pthread_create(&threads[i], NULL, apply_gaucian_blur, &thread_data[i]);
    }

    for (int i = 0; i < threads_count; i++)
    {
        pthread_join(threads[i], NULL);
    }

    time_t end_time = clock();
    printf("Execution time: %f\n", (double)(end_time - start_time) / CLOCKS_PER_SEC);

    save_png_image(output_filename, result, width, height);

    free(image);
    free(result);

    return 0;
}
