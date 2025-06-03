# send image and result to 192.168.12.2, port 5100
import socket
import time
import threading
import cv2
import numpy as np
import facing as fc

def send_image_to_server(image, server_ip, server_port):
    # Convert the image to bytes
    _, buffer = cv2.imencode('.jpg', image)
    image_bytes = buffer.tobytes()

    # Create a socket connection to the server
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        try:
            sock.connect((server_ip, server_port))
            print(f"Connected to server at {server_ip}:{server_port}")

            # Send the image data
            sock.sendall(image_bytes)
            print("Image sent successfully, with size " + str(len(image_bytes)) + " bytes")
        except Exception as e:
            print(f"Error sending image: {e}")

def test_compare_face_with_stored():
    image_path = "D:\\university\\ky8_last\\DATN\\src\\images\\facesForTest\\f2.jpg"
    image = cv2.imread(image_path)
    if image is None:
        print("Error: Unable to read image.")
        return
    # Resize the image to 800x600
    image = cv2.resize(image, (800, 600))
    print("Image loaded and resized to 800x600.")

    # Get face embeddings from the image
    face_embedding = fc.get_face_embeddings_from_image(image)
    if face_embedding is not None:
        # Compare with stored embeddings
        results = fc.compare_face_with_stored_faces(face_embedding)
        print(f"Face recognition results: {results}")
    else:
        print("No face detected in the image.")
    
def show_current_face_embeddings():
    hello = fc.get_all_stored_face_embeddings()
    for keys, values in hello.items():
        print(f"keys {keys}, type of em {len(values.tolist())}, shape {values.shape}")

def show_face_embedding_by_key(key):
    hello = fc.get_all_stored_face_embeddings()
    print(hello[key])

def test_add_face_embedding():
    image_path = "D:\\university\\ky8_last\\DATN\\src\\images\\facesForTest\\f2.jpg"
    image = cv2.imread(image_path)
    if image is None:
        print("Error: Unable to read image.")
        return
    # Resize the image to 800x600
    image = cv2.resize(image, (800, 600))
    print("Image loaded and resized to 800x600.")

    # Get face embeddings from the image
    face_embedding = fc.get_face_embeddings_from_image(image)
    if face_embedding is not None:
        # Add the face embedding to storage
        fc.extend_face_embedding([face_embedding], "f2")
        print("Face embedding added successfully.")
    else:
        print("No face detected in the image.")

if __name__ == "__main__":
    #send 800x600 image each 5 seconds
    
    # get image from explorer then fix to 800x600
    # image_path = "D:\\university\\ky8_last\\DATN\\prjCsharp\\imageSaveTemp\\temp.jpg"
    # image = cv2.imread(image_path)
    # if image is None:
    #     print("Error: Unable to read image.")
    #     exit(1)
    # image = cv2.resize(image, (800, 600))
    # print("Image loaded and resized to 800x600.")
    # try:
    #     while(True):
    #         send_image_to_server(image, "192.168.12.2", 5100)
    #         time.sleep(5)
    # except KeyboardInterrupt:
    #     print("Program interrupted by user.")
    # except Exception as e:
    #     print(f"An error occurred: {e}")
    fc.remove_all_face_embeddings()
    # show_current_face_embeddings()
    # show_face_embedding_by_key("duy")
    # duyImage = fc.get_face_embeddings_from_image(cv2.imread("D:\\university\\ky7_zz\\doAnDoLuong\\code_main\\imageSaveTemp\\temp.jpg"))
    # fc.extend_face_embedding([duyImage], "duy")
    # sampleFace = fc.get_face_embeddings_from_image(cv2.imread("D:\\university\\ky8_last\\DATN\\src\\images\\facesForTest\\f3.jpg"))
    # result = fc.compare_face_with_stored_faces(sampleFace)
    # print(f"result of comparing {result}")