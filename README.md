# Verify Face

## Overview

Verify Face is a facial recognition project designed to authenticate and verify individuals' identities using advanced image processing and machine learning techniques. The project processes facial images, compares them against a database of known faces, and determines whether a match exists. It is built using technologies such as Python, OpenCV, and TensorFlow.

Vface is now running on windows(c#)
## Project Structure

```
src/
├── main/                  # Main esp32 application code
├── models/                # models from Insightface open source
├── FR_server/             # Server-side code for API and database interaction
├── db/                    # Database-related files for embeddings and historical
├── managed_components/    # External components or libraries
├── sdkconfig              # SDK configuration
├── CMakeLists.txt         # Build configuration
└── README.md              # Project documentation
```

## Getting Started

### Prerequisites

- Install [Python 3.x](https://www.python.org/).
- Install [CMake](https://cmake.org/) and [ESP-IDF](https://github.com/espressif/esp-idf)
- Install required Python libraries:
   openvino
   openvino-onnx runtime
   opencv
   threading


### Building the Project

1. Clone the repository:
   ```sh
   git clone <repository-url>
   cd <repository-folder>
   ```

2. Configure the project:
   ```sh
   idf.py menuconfig
   ```

3. Build & flash the project:
   ```sh
   idf.py -p COMx build flash
   ```

### Running the Application

1. Start the server and ui:
   UI/UI
