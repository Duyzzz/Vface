import json
import sys

import face_recognition



def load_face_encodings(image_paths):
    encodings = []
    for path in image_paths:
        # Load the image and encode it
        image = face_recognition.load_image_file(path)
        encoding = face_recognition.face_encodings(image)
        if encoding:
            encodings.append(encoding[0])
        else:
            print(f"Warning: No face found in {path}")
    return encodings

def verify_face(input_image_path, known_image_paths):
    # Load input image
    unknown_image = face_recognition.load_image_file(input_image_path)
    unknown_encodings = face_recognition.face_encodings(unknown_image)

    if not unknown_encodings:
        return json.dumps({"error": "No face found in the input image."})

    unknown_encoding = unknown_encodings[0]

    # Load known face encodings (for multiple reference images)
    known_encodings = load_face_encodings(known_image_paths)

    # Compare the input face against all known faces
    results = face_recognition.compare_faces(known_encodings, unknown_encoding)
    
    # If any result is True, return verified
    is_verified = any(results)
    
    return json.dumps({"verified": is_verified})

if __name__ == "__main__":
    # First argument is the input image path

    input_image_path = "test.jpg"
    
    # Remaining arguments are the known reference images
    known_image_paths = ["knownFace.jpg"] 

    # Perform verification and output results as JSON
    output = verify_face(input_image_path, known_image_paths)
    print(output)
