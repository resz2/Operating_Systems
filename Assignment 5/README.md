## Creating a Pseudo File System
The goal of the assignment is to implement a simple file system. For the purpose of the disk, you will be using a file and develop your file system on top of the file. You need to first implement following basic APIs which will help you in implementing the system.

File System APIs: 
1. `int createSFS( char* filename, int nbytes)` – returns a fileSystemId 
2. `int readData( int disk, int blockNum, void* block)` – returns the num. of bytes read and read them into a buffer pointed by block. 
3. `int writeData(int disk, int blockNum, void* block)` – writes that data in the block to the disk block blockNum.
4. `int writeFile(int disk, char* filename, void* block)` – write the buffer pointed by block. The same data should be recoverable by the file name. 
5. `int readFile(int disk, char* filename, void* block)` – returns the data stored by the given file name. 

Organization: Use the inode design with bitmaps for your organization. 

Diagnostic APIs:

6. `void print_inodeBitmaps(int fileSystemId)` – prints the bitmaps of inodes 
7. `void print_dataBitmaps(int fileSystemId)` – prints the bitmaps of data block 
8. `void print_FileList(int fileSystemId)` – prints all the files in the file system