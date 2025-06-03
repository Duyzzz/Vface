import server as sv
import threading
import facing as fc
import imaging as img
from network_address import ui_endpoint
from network_address import esp32_endpoint
import storing as st
from datetime import datetime
import path_str as ps
IMAGES_DIRECTORY = ps.IMAGES_DIRECTORY
def imageReceiver():
    receiver = sv.Receiver()
    receiver.start()
    while True:
        data = receiver.get_data()
        if data is not None:
            # Send to UI
            # sv.send_data(data, ui_endpoint)
            # Process the received data
            image = img.get_image_from_uint8(data)
            image = img.rotate_image_180(image)
            imagePath = img.save_image(image, IMAGES_DIRECTORY)
            now = datetime.now()
            time = now.strftime("%H:%M:%S")
            weekday = now.strftime("%A")
            date = now.strftime("%d-%m-%Y")
            name = "Unknown"
            status = False
            if image is not None:
                # Perform face recognition
                face_embedding = fc.get_face_embeddings_from_image(image)
                if face_embedding is not None:
                    # Compare with stored embeddings
                    results = fc.compare_face_with_stored_faces(face_embedding)
                    print(f"Face recognition results: {results}")
                    # combine results(name) + image to send to ui
                    if(results == None):
                        combinedData = "Unknown".encode("utf-8") + "ackc".encode("utf-8") +  data
                        sv.send_data(combinedData, ui_endpoint)
                        sv.send_data(f"Display?Unknown".encode("utf-8"), esp32_endpoint)
                    else:
                        name = results
                        status = True
                        combinedData = results.encode("utf-8") + "ackc".encode("utf-8") + data
                        sv.send_data(combinedData, ui_endpoint)
                        sv.send_data(f"Display?{results}".encode("utf-8"), esp32_endpoint)
                else:
                    combinedData = "nofaceackc".encode("utf-8") + data
                    sv.send_data(combinedData, ui_endpoint)
                    sv.send_data(f"Display?noFace".encode("utf-8"), esp32_endpoint)
                    print("No face detected in the image.")
            else:
                print("Error processing the received data.")
            row = [name, time, weekday, date, imagePath, str(status)]
            st.append(row)
            receiver.reset_data()

def commandReceiver():
    receiver = sv.Receiver(("192.168.12.2", 2300))
    receiver.start()
    while(True):
        pack = receiver.get_data()
        # Print received packet
        if pack is not None:
            # Split the received packet into command, name, and image
            try:
                print("start split pack")
                parts = pack.split(b"?", 2)
                if len(parts) == 3:
                    command = parts[0].decode("utf-8")
                    payload1 = parts[1].decode("utf-8")
                    payload2 = parts[2]
                    print(f"Command: {command}, Name: {payload1}, Image data length: {len(payload2)}")
                    # Process the command, name, and image_data as needed
                    if(command == "Save"):
                        image = img.get_image_from_uint8(payload2)
                        image = img.rotate_image_180(image)
                        embedding = fc.get_face_embeddings_from_image(image)
                        tempPath = img.save_image(image, ps.REFERENCE_IMAGES_DIRECTORY)
                        print(f"tempPath: {tempPath}")
                        fc.extend_face_embedding([embedding], payload1)
                    elif(command == "Export"):
                        print("Execute Export command")
                        st.filDataByDays(payload1, payload2.decode("utf-8"))
                        # st.convertToExcel()
                    elif(command == "Exit"):
                        receiver.stop()
                        break
                else:
                    print("Invalid packet format.")
            except Exception as e:
                print(f"Error processing packet: {e}")
            receiver.reset_data()

if __name__ == "__main__":
    mainThread = threading.Thread(target=imageReceiver)
    commandExecuteThread = threading.Thread(target=commandReceiver)
    try: 
        mainThread.start()
        commandExecuteThread.start()
        print("Main thread started.")
    except Exception as e:
        print(f"Error starting main thread: {e}")
