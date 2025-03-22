# SyncStream - A Real-Time Multi-User Chat Application

## Overview

This project is a real-time, multi-user chat application built for Windows. It allows multiple users to connect to a central server and broadcast text messages in real-time.

## Technologies Used

* Modern C++
* Microsoft Foundation Class (MFC) Library
* Winsock API
* Visual Studio 2022

## Prerequisites

* *For Developers*: Visual Studio (with C++ and MFC support) installed on your development machine.
* *For Users*: Microsoft Visual C++ Redistributable 2015-2019 [Download here](https://answers.microsoft.com/en-us/windows/forum/all/download-link-for-microsoft-visual-c/6c6a0529-3ecb-4049-b868-f61eccd6143f)

## Setup and Installation

1.  **Clone the Repository:**
    ```bash
    git clone [https://github.com/your-username/mfc-chat-app.git](https://www.google.com/search?q=https://github.com/your-username/mfc-chat-app.git)
    ```
    (Replace `your-username` with your actual GitHub username)

2.  **Open the Solution in Visual Studio:**
    * Navigate to the cloned repository folder.
    * You will find two project folders: `ChatClient` and `ChatServer`. Each folder contains a Visual Studio solution file (`.sln`).
    * Open either `ChatClient/ChatClient.sln` or `ChatServer/ChatServer.sln` in Visual Studio. Visual Studio will automatically load both projects in the solution.

3.  **Build the Projects:**
    * In Visual Studio, select the desired build configuration (e.g., `Debug` or `Release`) and architecture (e.g., `x64` or `Win32`).
    * Build both the `ChatServer` and `ChatClient` projects by going to **Build > Build Solution** (or pressing `Ctrl+Shift+B`).

## How to Use

1.  **Run the Server:**
    * Navigate to the `ChatServer/Debug` or `ChatServer/Release` folder (depending on your build configuration).
    * Run the `ChatServer.exe` executable. The server window will open, indicating that it's listening for connections.

2.  **Run the Client(s):**
    * Navigate to the `ChatClient/Debug` or `ChatClient/Release` folder.
    * Run the `ChatClient.exe` executable. The client application window will open.
    * In the client application, enter the IP address of the machine where the server is running (if the server is running on the same machine, you can use `127.0.0.1`).
    * Enter a username and click "OK" to connect to the server.
    * You can run multiple instances of the client on different machines (or the same machine) to simulate a multi-user chat.

3.  **Chatting:**
    * Once connected, you can type messages in the message input box at the bottom of the client window and click "Send" (or press Enter if you had that working).
    * Messages sent by you and other connected users will appear in the main chat window, along with timestamps.

## Features

* Real-time text-based communication.
* Multi-user support.
* Simple graphical user interface using MFC.
* Username registration upon connecting.
* Message broadcasting from server to all connected clients.
* Timestamps on messages.
* Error handling for network operations.
* Client-side loading indication during connection.
* IP Address Control for specifying the server IP.

## Contributing (Optional)

If you'd like to contribute to this project, feel free to fork the repository, make your changes, and submit a pull request.

## License (Optional)

[Specify your project's license here, e.g., MIT License, Apache 2.0, etc. You can also add a LICENSE file to your repository.]

## Contact (Optional)

[Your name or contact information]

---

**How to use this template:**

* Copy and paste this content into a file named `README.md` in the root directory of your `mfc-chat-app` repository (the directory that contains the `ChatClient` and `ChatServer` folders).
* Replace the bracketed information (e.g., `your-username`, license information, contact) with your own details.
* You can format the README further using Markdown syntax. For example, you can add headings, lists, code blocks (as shown above), and more. GitHub supports standard Markdown.

This README should give anyone visiting your repository a good understanding of your project. Good luck!
```
