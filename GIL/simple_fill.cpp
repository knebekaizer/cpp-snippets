/// Create an image of 512x512. Fill it with red pixels. Write to PNG.

#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL
#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/png_dynamic_io.hpp>
using namespace boost::gil;
int main()
{
    rgb8_image_t img(512, 512);
    rgb8_pixel_t red(255, 0, 0);
    fill_pixels(view(img), red);
    png_write_view("redsquare.png", const_view(img));
}