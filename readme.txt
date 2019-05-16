Place the executables in directories of your choice. Keep them in separate directories otherwise the code will get appended to the original files.

Run ./file_server

and in a different tab or terminal run ./file_client

Type commands into the client server and receive outputs.

Logic
1. The server starts and waits for filename.
2. The client sends a filename.
3. The server receives filename.
   If file is present,
   server starts reading file 
   and continues to send a buffer filled with
   file contents encrypted until file-end is reached.
4. End is marked by EOF.
5. File is received as buffers until EOF is 
received. Then it is decrypted.
6. If Not present, a file not found is sent.
