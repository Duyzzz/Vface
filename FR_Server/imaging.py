import cv2
import numpy as np
import random
import string

def generate_random_string(length = 18):
    characters = string.ascii_letters + string.digits  # a-zA-Z0-9
    return ''.join(random.choices(characters, k=length))


def save_image(image, dictionary_path):
    path = f"{dictionary_path}\\{generate_random_string()}.jpg"
    cv2.imwrite(path, image)
    return path

def flip_image(image):
    flipped_image = cv2.flip(image, 1)  # Flip the image horizontally
    return flipped_image

def get_image_from_path(image_path):
    image = cv2.imread(image_path)
    if image is None:
        print(f"Error: Unable to read image from {image_path}")
        return None
    return image

def get_image_from_uint8(image_uint8):
    image = np.frombuffer(image_uint8, np.uint8)
    result = cv2.imdecode(image, cv2.IMREAD_COLOR)
    if result is None:
        print("Error: Unable to decode image from uint8 array")
        return None
    return result

def rotate_image_180(image):
    return cv2.rotate(image, cv2.ROTATE_180)

def convertBGRtoRGB(image):
    return cv2.cvtColor(image, cv2.COLOR_BGR2RGB)