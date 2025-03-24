<img width="70" height="70" align="left" style="float: left; margin: 0 10px 0 0;" alt="SyncStream logo" src="https://github.com/user-attachments/assets/19054044-1f49-4361-9de0-5759ae73b7ec">

# SyncStream

SyncStream is a real-time, multi-user chat application built for Windows. It allows multiple users to connect to a central server and broadcast text messages in real time.

## Demo

### Client:

https://github.com/user-attachments/assets/3a0f6ce2-82de-49b3-8135-05eaeb52c008

### Server:

![SyncStream Server- Demo](https://github.com/user-attachments/assets/4a6a947b-fc35-4664-88df-9f9cb4dee436)

## Technologies Used

* Modern C++
* Microsoft Foundation Class (MFC) Library
* Winsock API
* Visual Studio 2022
* Logo credits: <a href="https://www.flaticon.com/free-icons/communication" title="communication icons">Communication icons created by Vectors Market - Flaticon</a>

## Prerequisites

* *For Users*: Microsoft Visual C++ Redistributable 2015-2019 [Download here](https://answers.microsoft.com/en-us/windows/forum/all/download-link-for-microsoft-visual-c/6c6a0529-3ecb-4049-b868-f61eccd6143f)
* *For Developers*: Visual Studio (with C++ and MFC support) and Microsoft Visual C++ Redistributable 2015-2019.

## Setup and Installation (for Developers)

1.  **Clone the Repository:**
    ```bash
    git clone https://github.com/yadunand-kamath/SyncStream.git
    ```

2.  **Open the Solution in Visual Studio:**
    * Navigate to the cloned repository folder.
    * You will find two project folders: `ChatClient` and `ChatServer`. Each folder contains a Visual Studio solution file (`.sln`).
    * Open the `ChatClient/ChatClient.sln` and `ChatServer/ChatServer.sln` in Visual Studio.
      
3.  **Build the Projects:**
    * In Visual Studio, select the desired build configuration (e.g., `Debug` or `Release`) and architecture (e.g., `x64` or `Win32`).
    * Build both the `ChatServer` and `ChatClient` projects by going to **Build > Build Solution**.

## How to Use (for Users)

1.  **Run the Server:**
    * Navigate to `ChatServer/x64/Release/ChatServer.exe` or [click here](https://github.com/yadunand-kamath/SyncStream/blob/master/ChatServer/x64/Release/ChatServer.exe) and download the executable.
    * Run the `ChatServer.exe` executable. The server window will open, indicating that it's listening for connections.

2.  **Run the Client(s):**
    * Navigate to `ChatClient/x64/Release/ChatClient.exe` or [click here](https://github.com/yadunand-kamath/SyncStream/blob/master/ChatClient/x64/Release/ChatClient.exe).
    * Run the `ChatClient.exe` executable. The client application window will open.
    * In the client application, enter the IP address of the machine where the server is running (if the server is running on the same machine, you can use `127.0.0.1`).
    * Enter a username and click "OK" to connect to the server.
    * You can run multiple instances of the client on different machines (or the same machine) to simulate a multi-user chat.

3.  **Chatting:**
    * Once connected, you can type messages in the message input box at the bottom of the client window and click "Send" (avoid pressing "Enter" as it will close the application).
    * Messages sent by you and other connected users will appear in the main chat window, along with timestamps.

## Features

* Real-time text-based communication.
* Multi-user support.
* Simple graphical user interface using MFC.
* Username registration upon connecting.
* Message broadcasting from server to all connected clients.
* Timestamps on messages.
* IP Address Control for specifying the server IP.

<!--- 
## Contributing (Optional)

If you'd like to contribute to this project, feel free to fork the repository, make your changes, and submit a pull request.

## License (Optional)

[Specify your project's license here, e.g., MIT License, Apache 2.0, etc. You can also add a LICENSE file to your repository.]

## Contact (Optional)

[Your name or contact information]
--->
---

