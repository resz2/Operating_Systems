#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define numINodes 4
#define numDataBlocks 10
#define numDataPointers 4
#define dataBlockSize 40
#define bufferSize 100

char inputBuffer[bufferSize];
char outputBuffer[bufferSize];

struct superBlock {
	int numberINodes;
	int numberDataBlocks;
	int startingINodeIndex;
};

struct iNode
{
	int iNumber;
	int fileSize;
	int nPointers;
	int DataPointers[numDataPointers];
	char fileHumanName[20];

};

struct dataBlock
{
	int ID;
	char data[40];

};

int InodeBitmap[numINodes];
int DataBitmap[numDataBlocks];

void MakeSuperNode(FILE *diskPtr)
{
	struct superBlock *sB1=malloc(sizeof(struct superBlock));
	sB1->numberINodes=numINodes;
	sB1->numberDataBlocks=numDataBlocks;
	sB1->startingINodeIndex=sizeof(struct superBlock)+sizeof(InodeBitmap)+sizeof( DataBitmap);

	fwrite(sB1,sizeof(struct superBlock), 1, diskPtr);
}

void MakeInodeBitmap(FILE *diskPtr)
{
	memset(InodeBitmap,0,numINodes);
	InodeBitmap[0]=1; 												//for root directory
	fwrite(InodeBitmap, sizeof(int), numINodes, diskPtr);
}

void MakeDataBitmap(FILE *diskPtr)
{
	memset(DataBitmap,0,numDataBlocks);
		DataBitmap[0]=1; 											//for root 
		fwrite(DataBitmap, sizeof(int), numDataBlocks, diskPtr);
	}

	void MakeInodes(FILE *diskPtr)
	{
		struct iNode *iNodes=( struct iNode*)malloc(sizeof(struct iNode)*numINodes);
		int i=0;		
		for(i=0;i<numINodes;i++)
		{		
			iNodes[i].iNumber=i;
			iNodes[i].fileSize=0;
			iNodes[i].nPointers = 0;
			strcpy(iNodes[i].fileHumanName,"none");
	// iNodes[i].fileHumanName="none";
			memset(iNodes[i].DataPointers,0,numDataPointers);
		}
		fwrite(iNodes, sizeof(struct iNode), numINodes, diskPtr);
	}

	void MakeDataNodes(FILE *diskPtr)
	{
		struct dataBlock *dataBlocks=(struct dataBlock*)malloc(sizeof(struct dataBlock)*numDataBlocks);
		int i=0;	
		for(i=0;i<numDataBlocks;i++)
		{		
			dataBlocks[i].ID=i;
			strcpy(dataBlocks[i].data,"garbage");
	//	memset(dataBlocks[i].data,'0',dataBlockSize);
		}
		fwrite(dataBlocks, sizeof(struct dataBlock), numDataBlocks, diskPtr);
	}


	int MakeDisk()
	{

		FILE *diskPtr = fopen("/home/sanidhya/Documents/FIle System/Disk.txt", "r+");

		if(!diskPtr)
			return 1;

		MakeSuperNode(diskPtr);
		MakeInodeBitmap(diskPtr);
		MakeDataBitmap(diskPtr);
		MakeInodes(diskPtr);
		MakeDataNodes(diskPtr);

		fclose(diskPtr);
		return  0;

	}

	int ReadFromDisk()
	{
		char buf[1000];

		FILE *diskReader =fopen("/home/sanidhya/Documents/FIle System/Disk.txt", "r+");

		if (!diskReader)
			return 1;

		struct superBlock inputSB;
		fread (&inputSB, sizeof(struct superBlock), 1, diskReader);
		printf ("%d %d\n",inputSB.numberINodes, inputSB.numberDataBlocks);

		int inputINodeBitmap[numINodes];
		fread (&inputINodeBitmap, sizeof(int), numINodes, diskReader);
		int i=0;
		for ( i = 0; i < numINodes; ++i)
		{
			printf("%d\n",inputINodeBitmap[i] );
		}

		int inputDNodeBitmap[numDataBlocks];
		fread (&inputDNodeBitmap, sizeof(int), numDataBlocks, diskReader);
		for ( i = 0; i < numDataBlocks; ++i)
		{
			printf("%d\n",inputDNodeBitmap[i] );
		}


		struct iNode inputNode;

		for ( i = 0; i < numINodes; ++i)
		{
			fread (&inputNode, sizeof(struct iNode), 1, diskReader);
			printf (" inode %d\n",inputNode.iNumber);
		}

		struct dataBlock dataNode;

		for ( i = 0; i < numDataBlocks; ++i)
		{
			fread (&dataNode, sizeof(struct dataBlock), 1, diskReader);
			printf ("data : %s\n",dataNode.data);
		}



		fclose(diskReader);
		return 0;

	}

	int isUnique(char * filename,FILE * diskReader,int myStartingINodeIndex)
	{
				//printf("HIllo\n");


		fseek(diskReader,myStartingINodeIndex ,SEEK_SET);

		struct iNode tempINode;
		int i;
		for(i=0;i<numINodes;i++)
		{

			fread (&tempINode, sizeof(struct iNode), 1, diskReader);
	//printf("%s\n",tempINode.fileHumanName );
			if(strcmp(tempINode.fileHumanName,filename)==0)
			{
		//printf("HI\n");
				return 0;
			}
		}
		return 1;


	}

int createSFS(char* filename, int nbytes) // – returns a fileSystemId 
{
	FILE *diskReader = fopen("/home/sanidhya/Documents/FIle System/Disk.txt", "r+");
	if (!diskReader) {
		return 1;
	}

	struct superBlock inputSB;
	fread (&inputSB, sizeof(struct superBlock), 1, diskReader);
		// printf ("%d %d %d\n",inputSB.numberINodes, inputSB.numberDataBlocks);
	int myStartingINodeIndex=inputSB.startingINodeIndex;

		//1.assignment of inode to file to be created  : read through bitmap for free inode , then go to inode itself

	if(isUnique(filename,diskReader,myStartingINodeIndex)==0)
	{
		printf("Duplicates not allowed.\n");
		return 0;
	}

	fseek(diskReader,sizeof(struct superBlock) ,SEEK_SET);

	printf("inode part\n");

	// fseek(diskReader,0,SEEK_CUR);
	int whichNode;
	int isINodeUnassigned=1;
	int i=0;
	for (i = 0; i < numINodes; ++i)
	{
		fread (&isINodeUnassigned, sizeof(int), 1, diskReader); 
		printf("%d\n",isINodeUnassigned);
		if (isINodeUnassigned==0)
		{
			whichNode=i;
			printf("Found unassigned %d\n", whichNode);
			fseek(diskReader,-1*sizeof(int),SEEK_CUR);
			isINodeUnassigned = 1;
			fwrite(&isINodeUnassigned, sizeof(int) , 1, diskReader); // node now assigned
			break;

		}
	}

		//seek to inode dept.

	fseek(diskReader,myStartingINodeIndex + whichNode*(sizeof(struct iNode)),SEEK_SET);

	struct iNode tempINode;




	tempINode.iNumber=whichNode;
	tempINode.fileSize=0;
	strcpy(tempINode.fileHumanName,filename);
	memset(tempINode.DataPointers,0,numDataPointers);


	printf("Datablock part\n");
	
	//search for available data blocks on bitmap

	fseek(diskReader,sizeof(struct superBlock) + sizeof(InodeBitmap),SEEK_SET);
	int whichDNode;
	int isDNodeUnassigned=1;

	for (i = 0; i < numDataBlocks; ++i)
	{
		fread (&isDNodeUnassigned, sizeof(int), 1, diskReader); 
		printf("%d\n",isDNodeUnassigned);
		if (isDNodeUnassigned==0)
		{
			whichDNode=i;
			printf("Found unassigned %d\n", whichDNode);
			fseek(diskReader,-1*sizeof(int),SEEK_CUR);
			isDNodeUnassigned = 1;
			fwrite(&isDNodeUnassigned, sizeof(int) , 1, diskReader); // node now assigned
			break;

		}
	}

	//attach data blocks of size nbytes to this inode
	
	fseek(diskReader,myStartingINodeIndex + sizeof(struct iNode)*numINodes + whichDNode*(sizeof(struct dataBlock)),SEEK_SET);

	struct dataBlock tempDNode;
	tempDNode.ID=whichDNode;
	// tempDNode.data = null;
	strcpy(tempDNode.data,"It's Lonely here.");
	// printf("%s\n",tempDNode.data);

	fwrite(&tempDNode, sizeof(struct dataBlock), 1, diskReader);

	// fseek(diskReader,myStartingINodeIndex + sizeof(struct iNode)*numINodes + (whichDNode)*(sizeof(struct dataBlock)),SEEK_SET);

 //    fread (&tempDNode, sizeof(struct dataBlock), 1, diskReader);
 //    printf("%s\n",tempDNode.data );




	//modidy the inode data pointer
	fseek(diskReader,myStartingINodeIndex + whichNode*(sizeof(struct iNode)),SEEK_SET);
	tempINode.DataPointers[tempINode.nPointers++] = tempDNode.ID; 
	printf("%d\n", tempINode.DataPointers[tempINode.nPointers-1]);
	fwrite(&tempINode, sizeof(struct iNode), 1, diskReader);

		//2.assignemnt of data block to file to be created : read through data bitmap for free block

		//update inode of directory containing file to contain human file name
		//and correspodning inode number

		// fseek(diskReader,sizeof(int)*numDataBlocks,SEEK_CUR)

	//FindFreeDataBlock()
		//update inode of root to have the new file fno + inode

	fclose(diskReader);
}


void print_inodeBitmaps() {
	
	FILE *diskReader = fopen("/home/sanidhya/Documents/FIle System/Disk.txt", "r+");

	fseek(diskReader, sizeof(struct superBlock), SEEK_SET);
	int val;
	int i;
	for (i= 0; i<numINodes; i++) {
		fread(&val, sizeof(int), 1, diskReader);
		printf("%d ", val);
	}
	fclose(diskReader);
}

void print_dataBitmaps() {
	FILE *diskReader = fopen("/home/sanidhya/Documents/FIle System/Disk.txt", "r+");

	fseek(diskReader, sizeof(struct superBlock) + sizeof(InodeBitmap), SEEK_SET);
	int val;
	int i;
	for (i= 0; i<numDataBlocks; i++) {
		fread(&val, sizeof(int), 1, diskReader);
		printf("%d ", val);
	}
	fclose(diskReader);
}


void print_FileList() {
	FILE *diskReader = fopen("/home/sanidhya/Documents/FIle System/Disk.txt", "r+");

	fseek(diskReader, sizeof(struct superBlock) + sizeof(InodeBitmap) + sizeof(DataBitmap), SEEK_SET);

	struct iNode tempINode;
	int i;
	for ( i=0; i<numINodes; i++) {
		fread(&tempINode, sizeof(struct iNode), 1, diskReader);
		printf("%s\n", tempINode.fileHumanName);
	}
	fclose(diskReader);
}

bool isBufferFull(int bufferType) {	//bufferType = 0 for input buffer, 1 for output buffer
	printf("hi2\n");
	if (bufferType == 0 && strlen(inputBuffer) == bufferSize) {
		return true;
	}
	if (bufferType == 1 && strlen(outputBuffer) == bufferSize) {
		return true;
	}
	return false;
}

void clearBuffer(int bufferType) {
	int i;
	// printf("hi\n");
	if (bufferType == 0) {
		for (i=0; i<bufferSize; i++) {
			printf("%c", inputBuffer[i]);
		}
	}
	else {
		for (i=0; i<bufferSize; i++) {
			printf("%c", outputBuffer[i]);
		}	
	}
}

void readData(int blockNum, char block[]) {
	if (isBufferFull(0) == true) {
		clearBuffer(0);
		int i;
		for (i=0; i<dataBlockSize; i++) {
			inputBuffer[i] = block[i];
		}
	}
	else {
	printf("hi\n");
		if (dataBlockSize + strlen(inputBuffer) > bufferSize) {
			int i, j = 0;
			for (i = strlen(inputBuffer)-1; i<bufferSize; i++) {
				inputBuffer[i] = block[j++];
			}
			clearBuffer(0);
			for (i=0; i<(dataBlockSize - j); i++) {
				inputBuffer[i] = block[j++];
			}
		}
		else {
			strcat(inputBuffer, block);
		}
	}
}

void readFile(char *filename) {
	FILE *diskReader = fopen("/home/sanidhya/Documents/FIle System/Disk.txt", "r+");

	fseek(diskReader, sizeof(struct superBlock) + sizeof(int), SEEK_SET);
	int val = 0, i;
	for (i=1; i<numINodes; i++) {
		fread(&val, sizeof(int), 1, diskReader);
		if (val == 1) {
			break;
		}
	}
	if (val == 0) {
		printf("No file in the FIle System");
		return;
	}

	fseek(diskReader, sizeof(struct superBlock) + sizeof(InodeBitmap) + sizeof(DataBitmap), SEEK_SET);
	struct iNode tempINode;

	for (i=0; i<numINodes; i++) {
		fread(&tempINode, sizeof(struct iNode), 1, diskReader);
		if (strcmp(tempINode.fileHumanName, filename) == 0) {
			break;
		}
	}

	fseek(diskReader, sizeof(struct superBlock) + sizeof(InodeBitmap) + sizeof(DataBitmap) + sizeof(struct iNode)*numINodes + sizeof(struct dataBlock), SEEK_SET);
	
	struct dataBlock tempDNode2;
	
	fread(&tempDNode2, sizeof(struct dataBlock), 1, diskReader);
	// printf("%s\n", tempDNode2.data);
		
	if (tempINode.nPointers > 0) {
		int j, k;
		for (j=0; j<tempINode.nPointers; j++) {
			struct dataBlock tempDNode;
			fseek(diskReader, sizeof(struct superBlock) + sizeof(InodeBitmap) + sizeof(DataBitmap) + sizeof(struct iNode)*numINodes, SEEK_SET);
			for (k=0; k<numDataBlocks; k++) {
				fread(&tempDNode, sizeof(struct dataBlock), 1, diskReader);
				if (tempINode.DataPointers[j] == tempDNode.ID) {
					printf("%s\n", tempDNode.data);
					readData (tempDNode.ID, tempDNode.data);
				}
			}
		}
	}
	else {
		printf("File found but empty");
		return;
	}

	fclose(diskReader);
}


int main()
{
 	// int ret = MakeDisk();
 	// if (ret == 1) {
 	// 	printf("Disk not created\n");
 	// }
	createSFS("myfile1.txt",20);
 	//createSFS("myfile1.txt",20);
	print_FileList();
	print_inodeBitmaps();
	printf("\n");
	print_dataBitmaps();
	printf("\n");
	readFile("myfile1.txt");

// ReadFromDisk();
	


}