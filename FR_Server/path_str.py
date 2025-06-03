from pathlib import Path
import os
FR_SERVER_DIR = Path(__file__).resolve()
BASE_DIR = FR_SERVER_DIR.parents[1]
IMAGES_DIRECTORY = f"{BASE_DIR}\\capturedImages"
REFERENCE_IMAGES_DIRECTORY = f"{BASE_DIR}\\referenceImages"
EMBEDDING_PATH = f"{BASE_DIR}\\db\\embedding.npz"
HISTORY_FILE_PATH = f"{BASE_DIR}\\db\\history.csv"
def check_and_create_directory(directory_path):
    """
    Check if a directory exists, and create it if it doesn't.
    """
    if not os.path.exists(directory_path):
        os.makedirs(directory_path)
        print(f"Directory {directory_path} created.")
    else:
        print(f"Directory {directory_path} already exists.")