#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct Node{
   int rank;
   char sourceHex[6];
   int sourceValue;
   int targetSize;
   int isLabel;
   int labelValue;
   struct Link *targets;
} NodeStruct;
typedef struct Link{
    int dashed;
    struct NodeStruct *target;
}Link;
void increaseLinkAarray(struct Link **data,int size,int offset)
{
    *data = realloc(*data, sizeof(Link) * (size+offset));
}
void increaseNodeArray(struct NodeStruct **data,int size,int offset)
{
    *data = realloc(*data, sizeof(NodeStruct) * (size+offset));
}
void addTarget(NodeStruct *data,int size,int sourceValue,int target,int dashed){
    NodeStruct *sourceNode=NULL;
    NodeStruct *targetNode;
    for(int i=0; i < size ;i++){
        if((data+i)->sourceValue==sourceValue){
            sourceNode = (data+i);
        }
        if((data+i)->sourceValue==target){
            targetNode = (data+i);
        }
    }
    Link *l = (Link *)malloc(sizeof(Link));
    l->dashed = dashed;
    l->target = targetNode;
    increaseLinkAarray(&(sourceNode->targets),sourceNode->targetSize,1);
    sourceNode->targetSize = sourceNode->targetSize + 1;
    sourceNode->targets[sourceNode->targetSize-1] = *l;
}


int main()
{
    NodeStruct *nodesArray=NULL;
    int sizeOfNodes=0;
    int minNodeOffset=999999;
    char *source = NULL;
    FILE *fp = fopen("input", "r");
    if (fp != NULL) {
        if (fseek(fp, 0L, SEEK_END) == 0) {
            long bufsize = ftell(fp);
            if (bufsize == -1) { /* Error */ }
            source = malloc(sizeof(char) * (bufsize + 1));
            if (fseek(fp, 0L, SEEK_SET) != 0) { /* Error */ }
            size_t newLen = fread(source, sizeof(char), bufsize, fp);
            if ( ferror( fp ) != 0 ) {
                fputs("Error reading file", stderr);
            } else {
                source[newLen++] = '\0'; 
            }
        }
        fclose(fp);
    }
    char *iterator = source;
    int lineStart=0;
    int currentRank=-1;
    int sizeOfRanks=0;
    int handlingCostNodes=0;
    int constNodes=0;
    int lineLength=0;
    char *subbuff=NULL;
    printf("digraph g {");
    while(*iterator!='\0'){
         if(*iterator=='\n'){
                subbuff = (char *) malloc(sizeof(char) * (lineLength-1));
                if(subbuff == NULL)
                {
                    printf("Memory allocation failed subbuf");
                    return 1;
                }
                memcpy( subbuff, &source[lineStart+1], sizeof(char) * lineLength-1);
                subbuff[lineLength-1] = '\0';
                if( strstr(subbuff, "{ rank = same; \"") != NULL && strstr(subbuff, "CONST NODES")==NULL ) {
                    if(constNodes!=1){
                        sscanf(subbuff,"%*s rank = same; %*s %d %*s%*s",&currentRank);
                        printf("\n%d [label=\"v%d\"];",currentRank,currentRank );
                        sizeOfRanks++;
                    }
                }
                //We have value for rank
                if( handlingCostNodes==0 && currentRank!=-1 ){
                    //we need to associate rank variables offset to rank number
                    if(strlen(subbuff)==8) {
                            char offsetNumber[6];
                            sscanf(subbuff," \"%[^\"]\"",offsetNumber);
                            int n = (int)strtol(offsetNumber, NULL, 16);
                            if(n < minNodeOffset){
                                minNodeOffset = n;
                            }
                            NodeStruct *d = (NodeStruct *)malloc(sizeof(NodeStruct));
                            d->rank = currentRank;
                            d->sourceValue = n;
                            d->targets = NULL;
                            d->targetSize=0;
                            strcpy(d->sourceHex,offsetNumber);
                            increaseNodeArray(&nodesArray,sizeOfNodes,1);
                            nodesArray[sizeOfNodes++] = *d;
                        }

                }
                if(strstr(subbuff,"{ node [shape = box];")!=NULL && strstr(subbuff,"rank")==NULL ){
                    constNodes = 1;
                    handlingCostNodes = 1;
                    char constNode1[6];
                    sscanf(subbuff,"%*s%*s%*s%*s%*s \"%[^\"]\"",constNode1);
                    int n = (int)strtol(constNode1, NULL, 16);
                    if(n < minNodeOffset){
                        minNodeOffset = n;
                    }
                    NodeStruct *d = (NodeStruct *)malloc(sizeof(NodeStruct));
                    d->rank = sizeOfRanks++;
                    d->sourceValue = n;
                    d->targets = NULL;
                    d->isLabel=1;
                    d->targetSize=0;
                    strcpy(d->sourceHex,constNode1);
                    increaseNodeArray(&nodesArray,sizeOfNodes,1);
                    nodesArray[sizeOfNodes++] = *d;
                }
                else if(constNodes==1){
                    constNodes = 0;
                    char constNode2[6];
                    sscanf(subbuff," \"%[^\"]\"",constNode2);
                    int n = (int)strtol(constNode2, NULL, 16);
                    if(n < minNodeOffset){
                        minNodeOffset = n;
                    }
                    NodeStruct *d = (NodeStruct *)malloc(sizeof(NodeStruct));
                    d->rank = sizeOfRanks++;
                    d->sourceValue = n;
                    d->targets = NULL;
                    d->isLabel=1;
                    d->targetSize=0;
                    strcpy(d->sourceHex,constNode2);
                    increaseNodeArray(&nodesArray,sizeOfNodes,1);
                    nodesArray[sizeOfNodes++] = *d;
                }
                if(strstr(subbuff, "x") != NULL && strstr(subbuff, "->")!=NULL ) {
                    char from[6];
                    char to[6];

                    sscanf(subbuff," \"%[^\"]\" -> \"%[^\"]\"",from,to);
                    if(strstr(subbuff, "F0") != NULL){
                        int toOffset = (int)strtol(to, NULL, 16);

                    }
                    else if(strstr(subbuff, "dashed") != NULL){
                        int f = (int)strtol(from, NULL, 16);
                        int t = (int)strtol(to, NULL, 16);
                        addTarget(nodesArray,sizeOfNodes,f,t,1);

                    }else{
                        int f = (int)strtol(from, NULL, 16);
                        int t = (int)strtol(to, NULL, 16);
                        addTarget(nodesArray,sizeOfNodes,f,t,0);

                    }
                }
                if(strstr(subbuff,"x")!=NULL && strstr(subbuff,"label")!=NULL){
                    char offsetValue[6];
                    char label[6];
                    sscanf(subbuff," \"%[^\"]\" %*s%*s \"%[^\"]\"",offsetValue,label);

                }
                lineStart = lineStart + lineLength;
                lineLength = 0;
                free(subbuff);

         }
         lineLength++;
         iterator++;
    }
    //reduce dupes
    int* countOfNotsInTargets= (int*)malloc(sizeof(sizeOfRanks));
    int* countOfXInTargets=(int*)malloc(sizeof(sizeOfRanks));
    int lastRank=0;
    for(int k=0;k<sizeOfRanks;k++){
        *(countOfNotsInTargets+k) = 0;
        *(countOfXInTargets+k) = 0;
    }
    for(int i=0; i < sizeOfNodes; i++){
            Link *iterator = (nodesArray+i)->targets;
            if(lastRank!=(nodesArray+i)->rank){
                for(int k=0;k<sizeOfRanks;k++){
                        *(countOfNotsInTargets+k) = 0;
                        *(countOfXInTargets+k) = 0;
                }
            }
            for(int j=0;j<(nodesArray+i)->targetSize;j++){
                NodeStruct *internal = (iterator+j)->target;
                int rankToShow=0;
                rankToShow = internal->rank;
                //Still on same rank
                if(rankToShow!=-1){
                    *(countOfXInTargets+rankToShow) = *(countOfXInTargets+rankToShow) +  1 - (iterator+j)->dashed;
                    *(countOfNotsInTargets+rankToShow) = *(countOfNotsInTargets+rankToShow)+  (iterator+j)->dashed;
                }
                if(*(countOfXInTargets+rankToShow)+*(countOfNotsInTargets+rankToShow)>2)
                    continue;
                if((iterator+j)->dashed==0 ){
                    if(*(countOfXInTargets+rankToShow)>1)
                        continue;
                    printf("\n%d -> %d [label=\"x%d\"];",(nodesArray+i)->rank,rankToShow,(nodesArray+i)->rank );
                }else {
                    if(*(countOfNotsInTargets+rankToShow)>1)
                        continue;
                    printf("\n%d -> %d [label=\"n.x%d\"];",(nodesArray+i)->rank,rankToShow,(nodesArray+i)->rank );
                }
            }
            lastRank = (nodesArray+i)->rank;
    }
    printf("\n}\n");
    free(countOfNotsInTargets);
    free(countOfXInTargets);
    free(nodesArray);
    free(source); 
    return 0;
}