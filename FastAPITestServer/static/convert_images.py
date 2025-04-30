# convert_images.py
import sys

from PIL import Image


def resize_and_save(src_path, dst_path, size, mode="RGB", threshold=None):
    img = Image.open(src_path)
    # Convert to requested mode
    img = img.convert(mode)
    # Resize (may distort aspect ratio)
    img = img.resize(size, Image.LANCZOS)
    # If you need a binary (black/white) mask:
    if threshold is not None:
        # Convert to grayscale then threshold
        gray = img.convert("L")
        bw = gray.point(lambda p: 255 if p > threshold else 0, mode="1")
        bw.convert("RGB").save(dst_path, quality=90)
    else:
        img.save(dst_path, quality=90)


if __name__ == "__main__":
    # Example usage:
    # python convert_images.py input.png
    src = sys.argv[1]
    resize_and_save(src, "static/current/qvga.jpg", (320, 240), mode="L")
    resize_and_save(src, "static/current/mask.jpg", (320, 240), mode="L",
                    threshold=128)
    resize_and_save(src, "static/current/uxga.jpg", (1600, 1200), mode="RGB")
    print("Resized QVGA, mask, and UXGA images.")


# # convert_images.py
# from PIL import Image

# name = ['qvga', 'mask', 'uxga']


# # Define your target sizes
# sizes = {'qvga': (320, 240), 'uxga': (1600, 1200)}

# for name, size in sizes.items():
#     img = Image.open(f'static/placeholders/{name}.png')  # your file
#     # .resize takes a (width, height) tuple and a resampling filter
#     resized = img.resize(size, resample=Image.BILINEAR)
#     resized.save(f'static/placeholders/{name}.jpg', 'JPEG', quality=20)
#     print(f'Saved static/placeholders/{name}.jpg at {size}')

# # load a PNG or generate from scratch
# img = Image.open("static/placeholders/uxga.jpg").resize((32, 32))
# img.save("static/favicon.ico", format="ICO")
