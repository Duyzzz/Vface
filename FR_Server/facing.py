import openvino as ov
import onnxruntime as ort
from insightface.app import FaceAnalysis
import numpy as np
from scipy.spatial.distance import cosine
import path_str as ps
DATABASE_PATH = ps.EMBEDDING_PATH

appFaceRecognizer = FaceAnalysis(name='buffalo_l', root="models/recognition", allowed_modules=['detection', 'recognition'], providers=['OpenVINOExecutionProvider'])
appFaceRecognizer.prepare(ctx_id=0, det_size=(640, 640))  # Load models here

def get_face_embeddings_from_image(image):
    faces = appFaceRecognizer.get(image)
    if len(faces) > 0:
        face_embedding = faces[0].embedding
        face_embedding = np.array(face_embedding)
        return face_embedding
    else:
        return None

def get_all_stored_face_embeddings(file_path = DATABASE_PATH):
    return dict(np.load(file_path))

def save_face_embeddings_to_storage(face_embeddings, face_ids, file_path = DATABASE_PATH):
    data = {}
    for idx, id in enumerate(face_ids):
        print(f"idx: {idx}, id: {id}")
        data[id] = face_embeddings[idx]
    np.savez(file_path, **data)

def extend_face_embedding(face_embedding, face_id, file_path = DATABASE_PATH):
    try:
        data = dict(np.load(file_path))  # Convert to a dictionary for item assignment
    except EOFError:
        data = {}
    if face_id not in data.keys():
        data[face_id] = []
    else:
        data[face_id] = data[face_id].tolist()  # Convert to list for appending
    data[face_id].extend(face_embedding)  # Add new face embedding
    np.savez(file_path, **data)  # Save updated data back to file

def compare_faces(face_embedding1, face_embedding2):
    # Calculate cosine similarity
    similarity = 1 - cosine(face_embedding1, face_embedding2)
    return similarity

def remove_all_face_embeddings(file_path = DATABASE_PATH):
    data = {}
    np.savez(file_path, **data)

def compare_face_with_stored_faces(face_embedding):
    stored_face_embeddings = get_all_stored_face_embeddings()
    print(f"type of stored_face_embeddings: {type(stored_face_embeddings)}")
    for stored_id, stored_embedding in stored_face_embeddings.items():
        operator = stored_embedding.tolist()
        print(f"type of operator 0: {type(operator[0])}")
        for idx in range(len(operator)):
            similarity = compare_faces(face_embedding, operator[idx])
            print(f"compare with id: {stored_id}, embedding {idx}, similarity {similarity}")
            if(similarity >= 0.6):
                print(f"Similarity: {similarity}")
                return stored_id
    return None

def remove_face_embedding(face_id, file_path = DATABASE_PATH):
    data = dict(np.load(file_path))
    if face_id in data.keys():
        del data[face_id]
        np.savez(file_path, **data)
        return True
    else:
        return False
def get_stored_ids(file_path = DATABASE_PATH):
    data = np.load(file_path)
    return list(data.files)